#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

// Configura a porta serial (8N1, 9600 baud, sem controle de fluxo)
int configurar_serial(const char *dispositivo) {
    int fd = open(dispositivo, O_RDONLY | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("Erro ao abrir porta serial");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) < 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD);    // Ignora controles de modem
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 // 8 bits de dados
    tty.c_cflag &= ~PARENB;             // Sem paridade
    tty.c_cflag &= ~CSTOPB;             // 1 bit de parada
    tty.c_cflag &= ~CRTSCTS;            // Sem controle de fluxo por hardware
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Sem controle de fluxo por software
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Modo raw
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 1;   // Lê pelo menos 1 caractere
    tty.c_cc[VTIME] = 5;   // Timeout 0.5s entre caracteres

    if (tcsetattr(fd, TCSANOW, &tty) < 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }
    return fd;
}

void salvar_dados(const char *linha) {
    FILE *log = fopen("datalog.csv", "a");
    if (log == NULL) {
        fprintf(stderr, "Erro ao abrir datalog.csv\n");
        return;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(log, "%04d-%02d-%02d %02d:%02d:%02d,%s",
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, linha);
    fclose(log);
}

int main(int argc, char **argv) {
    const char *dispositivo = "/dev/ttyS0";   // Padrão
    if (argc > 1)
        dispositivo = argv[1];

    printf("Abrindo porta serial: %s\n", dispositivo);
    int fd = configurar_serial(dispositivo);
    if (fd < 0)
        return 1;

    printf("Lendo dados... Pressione Ctrl+C para sair.\n\n");

    char buffer[256];
    int pos = 0;
    while (1) {
        char c;
        int n = read(fd, &c, 1);
        if (n > 0) {
            if (c == '\n' || c == '\r') {
                if (pos > 0) {
                    buffer[pos] = '\0';
                    printf("%s\n", buffer);
                    salvar_dados(buffer);
                    pos = 0;
                }
            } else if (pos < (int)sizeof(buffer)-1) {
                buffer[pos++] = c;
            }
        }
    }

    close(fd);
    return 0;
}
