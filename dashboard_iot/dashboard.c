#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <GL/glut.h>
#include <pthread.h>

#define SERVIDOR_IP   "127.0.0.1"
#define PORTA         8080
#define MAX_PONTOS    100

// Dados do gráfico
float temp_hist[MAX_PONTOS] = {0};
float umid_hist[MAX_PONTOS] = {0};
int num_pontos = 0;
int pausado = 0;
float zoom = 1.0f;

// Socket
int sock;
pthread_mutex_t dados_mutex = PTHREAD_MUTEX_INITIALIZER;

// Dimensões
int larg = 900, alt = 600;
float margem_esq = 80, margem_dir = 40, margem_inf = 60, margem_sup = 40;

// Protótipos OpenGL
void desenhar_texto(float x, float y, const char *texto);
void desenhar_eixos();
void desenhar_grafico();
void display();
void teclado(unsigned char tecla, int x, int y);
void timer(int valor);
void *thread_rede(void *arg);

void desenhar_texto(float x, float y, const char *texto) {
    glRasterPos2f(x, y);
    while (*texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *texto);
        texto++;
    }
}

void desenhar_eixos() {
    float area_larg = larg - margem_esq - margem_dir;
    float area_alt = alt - margem_inf - margem_sup;

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(margem_esq, margem_inf);
    glVertex2f(margem_esq, margem_inf + area_alt);
    glVertex2f(margem_esq, margem_inf);
    glVertex2f(margem_esq + area_larg, margem_inf);
    glEnd();

    // Grade e rótulos
    glColor3f(0.8f, 0.8f, 0.8f);
    for (int i = 0; i <= 5; i++) {
        float y = margem_inf + (i / 5.0f) * area_alt;
        glBegin(GL_LINES);
        glVertex2f(margem_esq, y);
        glVertex2f(margem_esq + area_larg, y);
        glEnd();
        char buf[10];
        int valor = (int)(100 - i * 20);  // 0..100%
        sprintf(buf, "%d", valor);
        desenhar_texto(margem_esq - 30, y - 5, buf);
    }
    // Rótulo fixo
    desenhar_texto(margem_esq - 70, margem_inf + area_alt/2 - 10, "Temp/Umidade (%)");
}

void desenhar_grafico() {
    if (num_pontos < 2) return;

    float area_larg = larg - margem_esq - margem_dir;
    float area_alt = alt - margem_inf - margem_sup;

    // Temperatura (linha vermelha)
    glColor3f(1.0f, 0.3f, 0.3f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < num_pontos; i++) {
        float x = margem_esq + (i / (float)(MAX_PONTOS-1)) * area_larg;
        float y = margem_inf + (temp_hist[i] / 100.0f) * area_alt * zoom;
        glVertex2f(x, y);
    }
    glEnd();

    // Ícones nos pontos de temperatura
    glPointSize(4);
    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < num_pontos; i++) {
        float x = margem_esq + (i / (float)(MAX_PONTOS-1)) * area_larg;
        float y = margem_inf + (temp_hist[i] / 100.0f) * area_alt * zoom;
        glVertex2f(x, y);
    }
    glEnd();

    // Umidade (linha azul)
    glColor3f(0.3f, 0.3f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < num_pontos; i++) {
        float x = margem_esq + (i / (float)(MAX_PONTOS-1)) * area_larg;
        float y = margem_inf + (umid_hist[i] / 100.0f) * area_alt * zoom;
        glVertex2f(x, y);
    }
    glEnd();

    glPointSize(4);
    glColor3f(0.0f, 0.0f, 0.8f);
    glBegin(GL_POINTS);
    for (int i = 0; i < num_pontos; i++) {
        float x = margem_esq + (i / (float)(MAX_PONTOS-1)) * area_larg;
        float y = margem_inf + (umid_hist[i] / 100.0f) * area_alt * zoom;
        glVertex2f(x, y);
    }
    glEnd();

    // Legendas
    glColor3f(0.8f, 0.0f, 0.0f);
    desenhar_texto(larg - 150, alt - 30, "Temperatura");
    glColor3f(0.0f, 0.0f, 0.8f);
    desenhar_texto(larg - 150, alt - 50, "Umidade");
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    desenhar_eixos();
    desenhar_grafico();

    // Status
    glColor3f(0.0f, 0.0f, 0.0f);
    char status[64];
    sprintf(status, "Pontos: %d  Zoom: %.1fx  %s", num_pontos, zoom, pausado ? "(PAUSADO)" : "");
    desenhar_texto(10, alt - 20, status);

    glutSwapBuffers();
}

void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case '+': zoom += 0.1f; if (zoom > 3.0f) zoom = 3.0f; break;
        case '-': zoom -= 0.1f; if (zoom < 0.2f) zoom = 0.2f; break;
        case 'p': case 'P': pausado = !pausado; break;
        case 'r': case 'R': num_pontos = 0; zoom = 1.0f; break;
        case 27: exit(0);
    }
    glutPostRedisplay();
}

void timer(int valor) {
    glutPostRedisplay();
    glutTimerFunc(100, timer, 0);  // 10 fps
}

void *thread_rede(void *arg) {
    char buffer[64];
    while (1) {
        if (!pausado) {
            send(sock, "GET", 3, 0);
            int n = recv(sock, buffer, sizeof(buffer)-1, 0);
            if (n > 0) {
                buffer[n] = '\0';
                float temp, umid;
                if (sscanf(buffer, "T:%f,U:%f", &temp, &umid) == 2) {
                    pthread_mutex_lock(&dados_mutex);
                    if (num_pontos < MAX_PONTOS) {
                        temp_hist[num_pontos] = temp;
                        umid_hist[num_pontos] = umid;
                        num_pontos++;
                    } else {
                        // shift left: desloca array para manter os últimos MAX_PONTOS
                        memmove(temp_hist, temp_hist+1, (MAX_PONTOS-1)*sizeof(float));
                        memmove(umid_hist, umid_hist+1, (MAX_PONTOS-1)*sizeof(float));
                        temp_hist[MAX_PONTOS-1] = temp;
                        umid_hist[MAX_PONTOS-1] = umid;
                        // num_pontos já é MAX_PONTOS
                    }
                    pthread_mutex_unlock(&dados_mutex);
                }
            }
        }
        usleep(800000); // ~1.25 Hz para acompanhar o sensor (1s)
    }
    return NULL;
}

int main(int argc, char **argv) {
    // Conectar ao servidor
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTA);
    inet_pton(AF_INET, SERVIDOR_IP, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return 1;
    }
    printf("Conectado ao servidor.\n");

    // Thread de rede
    pthread_t tid;
    pthread_create(&tid, NULL, thread_rede, NULL);

    // OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(larg, alt);
    glutCreateWindow("Dashboard IoT - Temperatura e Umidade");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, larg, 0, alt);

    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();

    close(sock);
    return 0;
}
