#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORTA_TCP        8080
#define BUFFER_TAM       256
#define MAX_CLIENTES     5
#define ARQUIVO_LOG      "datalog.csv"

char ultima_medicao[BUFFER_TAM] = "Nenhum dado ainda\n";
pthread_mutex_t mutex_medicao = PTHREAD_MUTEX_INITIALIZER;
int serial_fd = -1;
int servidor_rodando = 1;

int configurar_serial(const char *dispositivo) {
    int fd = open(dispositivo, O_RDONLY | O_NOCTTY | O_SYNC);
    if (fd < 0) { perror("open serial"); return -1; }

    struct termios tty;
    if (tcgetattr(fd, &tty) < 0) { perror("tcgetattr"); close(fd); return -1; }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE; tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) < 0) { perror("tcsetattr"); close(fd); return -1; }
    return fd;
}

void *thread_serial(void *arg) {
    char buffer[BUFFER_TAM];
    int pos = 0;
    while (servidor_rodando) {
        char c;
        int n = read(serial_fd, &c, 1);
        if (n > 0) {
            if (c == '\n' || c == '\r') {
                if (pos > 0) {
                    buffer[pos] = '\0';
                    pthread_mutex_lock(&mutex_medicao);
                    strncpy(ultima_medicao, buffer, BUFFER_TAM-1);
                    ultima_medicao[BUFFER_TAM-1] = '\0';
                    pthread_mutex_unlock(&mutex_medicao);
                    printf("[SERIAL] %s\n", buffer);

                    // Salva no log com timestamp
                    FILE *log = fopen(ARQUIVO_LOG, "a");
                    if (log) {
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        fprintf(log, "%04d-%02d-%02d %02d:%02d:%02d,%s\n",
                                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                                tm.tm_hour, tm.tm_min, tm.tm_sec, buffer);
                        fclose(log);
                    }
                    pos = 0;
                }
            } else if (pos < BUFFER_TAM-1) {
                buffer[pos++] = c;
            }
        }
    }
    return NULL;
}

void enviar_arquivo(int socket_cliente, const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "r");
    if (!f) {
        const char *msg = "Arquivo de log não encontrado.\n";
        send(socket_cliente, msg, strlen(msg), 0);
        return;
    }
    char linha[512];
    while (fgets(linha, sizeof(linha), f)) {
        send(socket_cliente, linha, strlen(linha), 0);
    }
    fclose(f);
}

void *thread_cliente(void *arg) {
    int socket_cliente = *(int*)arg;
    free(arg);
    char buffer_cmd[64];
    printf("[SERVIDOR] Cliente conectado.\n");

    while (1) {
        ssize_t n = recv(socket_cliente, buffer_cmd, sizeof(buffer_cmd)-1, 0);
        if (n <= 0) break;
        buffer_cmd[n] = '\0';
        buffer_cmd[strcspn(buffer_cmd, "\r\n")] = 0;

        if (strcmp(buffer_cmd, "GET") == 0) {
            pthread_mutex_lock(&mutex_medicao);
            send(socket_cliente, ultima_medicao, strlen(ultima_medicao), 0);
            send(socket_cliente, "\n", 1, 0);
            pthread_mutex_unlock(&mutex_medicao);
        }
        else if (strcmp(buffer_cmd, "LOG") == 0) {
            enviar_arquivo(socket_cliente, ARQUIVO_LOG);
        }
        else if (strcmp(buffer_cmd, "QUIT") == 0) {
            const char *msg = "Tchau!\n";
            send(socket_cliente, msg, strlen(msg), 0);
            break;
        }
        else {
            const char *msg = "Comandos: GET, LOG, QUIT\n";
            send(socket_cliente, msg, strlen(msg), 0);
        }
    }
    close(socket_cliente);
    return NULL;
}

void finalizar(int s) {
    printf("\nEncerrando servidor...\n");
    servidor_rodando = 0;
    if (serial_fd >= 0) close(serial_fd);
    exit(0);
}

int main(int argc, char **argv) {
    const char *dispositivo = "/dev/ttyS0";
    if (argc > 1) dispositivo = argv[1];

    signal(SIGINT, finalizar);

    serial_fd = configurar_serial(dispositivo);
    if (serial_fd < 0) return 1;

    pthread_t tid_serial;
    pthread_create(&tid_serial, NULL, thread_serial, NULL);

    int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in endereco;
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA_TCP);

    bind(socket_servidor, (struct sockaddr*)&endereco, sizeof(endereco));
    listen(socket_servidor, MAX_CLIENTES);

    printf("Servidor aguardando conexões na porta %d...\n", PORTA_TCP);

    while (servidor_rodando) {
        struct sockaddr_in cliente_addr;
        socklen_t addr_len = sizeof(cliente_addr);
        int socket_cliente = accept(socket_servidor, (struct sockaddr*)&cliente_addr, &addr_len);
        if (socket_cliente < 0) {
            if (servidor_rodando == 0) break;
            perror("accept");
            continue;
        }
        printf("[SERVIDOR] Conexão de %s\n", inet_ntoa(cliente_addr.sin_addr));
        int *p_socket = malloc(sizeof(int));
        *p_socket = socket_cliente;
        pthread_t tid_cliente;
        pthread_create(&tid_cliente, NULL, thread_cliente, p_socket);
        pthread_detach(tid_cliente);
    }

    close(socket_servidor);
    return 0;
}
