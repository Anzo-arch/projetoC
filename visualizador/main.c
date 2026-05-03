#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>

#define MAX_PONTOS 100
#define MAX_NOME 50

// Dados lidos do CSV
char nomes[MAX_PONTOS][MAX_NOME];
float valores[MAX_PONTOS];
int num_pontos = 0;

// Zoom atual e índice da barra em destaque (-1 = nenhum)
float zoom = 1.0f;
int barra_selecionada = -1;

// Dimensões da janela
int largura_janela = 800, altura_janela = 600;

// Margens e dimensões do gráfico
float margem_esq = 80, margem_dir = 40, margem_inf = 80, margem_sup = 40;
float area_largura, area_altura;
float max_valor = 0;

// Protótipos
void ler_csv(const char* arquivo);
void desenhar_texto(float x, float y, const char* texto);
void desenhar_eixos();
void desenhar_barras();
void redesenhar();
void teclado(unsigned char tecla, int x, int y);
void mouse(int botao, int estado, int x, int y);

// Leitura do CSV: espera formato "rotulo,valor" por linha
void ler_csv(const char* arquivo) {
    FILE* f = fopen(arquivo, "r");
    if (!f) {
        fprintf(stderr, "Erro ao abrir %s\n", arquivo);
        exit(1);
    }
    char linha[100];
    num_pontos = 0;
    max_valor = 0;
    while (fgets(linha, sizeof(linha), f) && num_pontos < MAX_PONTOS) {
        char nome[MAX_NOME];
        float val;
        if (sscanf(linha, "%49[^,],%f", nome, &val) == 2) {
            strncpy(nomes[num_pontos], nome, MAX_NOME - 1);
            nomes[num_pontos][MAX_NOME - 1] = '\0';
            valores[num_pontos] = val;
            if (val > max_valor) max_valor = val;
            num_pontos++;
        }
    }
    fclose(f);
    if (max_valor == 0) max_valor = 1; // evitar divisão por zero
}

// Função auxiliar para "escrever" texto usando glutBitmapCharacter
void desenhar_texto(float x, float y, const char* texto) {
    glRasterPos2f(x, y);
    while (*texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *texto);
        texto++;
    }
}

// Desenha os eixos e as legendas
void desenhar_eixos() {
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    // Eixo Y
    glVertex2f(margem_esq, margem_inf);
    glVertex2f(margem_esq, margem_inf + area_altura);
    // Eixo X
    glVertex2f(margem_esq, margem_inf);
    glVertex2f(margem_esq + area_largura, margem_inf);
    glEnd();

    // Linhas horizontais de grade (4 divisões)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINES);
    for (int i = 0; i <= 4; i++) {
        float y = margem_inf + (i / 4.0f) * area_altura;
        glVertex2f(margem_esq, y);
        glVertex2f(margem_esq + area_largura, y);
    }
    glEnd();

    // Rótulos do eixo Y (valores)
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= 4; i++) {
        float y = margem_inf + (i / 4.0f) * area_altura;
        float valor = (i / 4.0f) * max_valor;
        char texto[20];
        sprintf(texto, "%.0f", valor);
        desenhar_texto(margem_esq - 40, y - 5, texto);
    }
}

// Desenha as barras do gráfico
void desenhar_barras() {
    if (num_pontos == 0) return;

    float largura_barra = area_largura / (float)num_pontos * 0.7f;
    float espacamento = area_largura / (float)num_pontos;

    for (int i = 0; i < num_pontos; i++) {
        // Cor: destaque se selecionada, senão azul padrão
        if (i == barra_selecionada)
            glColor3f(1.0f, 0.5f, 0.0f);  // laranja
        else
            glColor3f(0.2f, 0.5f, 0.9f);  // azul

        float altura_barra = (valores[i] / max_valor) * area_altura * zoom;
        float x_esq = margem_esq + i * espacamento + (espacamento - largura_barra) / 2.0f;
        float x_dir = x_esq + largura_barra;

        glBegin(GL_QUADS);
        glVertex2f(x_esq, margem_inf);
        glVertex2f(x_dir, margem_inf);
        glVertex2f(x_dir, margem_inf + altura_barra);
        glVertex2f(x_esq, margem_inf + altura_barra);
        glEnd();

        // Rótulo no eixo X (nome)
        glColor3f(0.0f, 0.0f, 0.0f);
        float largura_texto = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)nomes[i]);
        desenhar_texto(x_esq + largura_barra/2 - largura_texto/2, margem_inf - 20, nomes[i]);

        // Valor acima da barra (se zoom permitir)
        if (zoom > 0.8f) {
            char str_val[20];
            sprintf(str_val, "%.0f", valores[i]);
            float text_x = x_esq + largura_barra/2 - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)str_val)/2;
            desenhar_texto(text_x, margem_inf + altura_barra + 5, str_val);
        }
    }
}

// Callback de redesenho
void redesenhar() {
    glClear(GL_COLOR_BUFFER_BIT);
    area_largura = largura_janela - margem_esq - margem_dir;
    area_altura = altura_janela - margem_inf - margem_sup;

    desenhar_eixos();
    desenhar_barras();

    glutSwapBuffers();
}

// Callback de teclado
void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case '+':
            zoom += 0.1f;
            if (zoom > 3.0f) zoom = 3.0f;
            break;
        case '-':
            zoom -= 0.1f;
            if (zoom < 0.2f) zoom = 0.2f;
            break;
        case 'r':
        case 'R':
            zoom = 1.0f;
            barra_selecionada = -1;
            break;
        case 27: // ESC
        case 'q':
        case 'Q':
            exit(0);
            break;
    }
    glutPostRedisplay();
}

// Callback de mouse: detecta clique nas barras
void mouse(int botao, int estado, int x, int y) {
    if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN) {
        // Converte coordenadas da janela para coordenadas do gráfico
        // A origem do mouse é canto superior esquerdo; do OpenGL é inferior esquerdo.
        float mouse_x = (float)x;
        float mouse_y = altura_janela - (float)y;

        // Verifica se está dentro da área do gráfico
        if (mouse_x >= margem_esq && mouse_x <= margem_esq + area_largura &&
            mouse_y >= margem_inf && mouse_y <= margem_inf + area_altura) {

            float espacamento = area_largura / (float)num_pontos;
            int indice = (int)((mouse_x - margem_esq) / espacamento);
            if (indice >= 0 && indice < num_pontos) {
                // Verifica se o clique está na altura da barra (considerando zoom)
                float altura_barra = (valores[indice] / max_valor) * area_altura * zoom;
                if (mouse_y <= margem_inf + altura_barra) {
                    barra_selecionada = indice;
                    printf("Clicou em: %s = %.1f\n", nomes[indice], valores[indice]);
                } else {
                    barra_selecionada = -1; // clicou acima da barra, deseleciona
                }
            }
        } else {
            barra_selecionada = -1;
        }
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    const char* arquivo_csv = "dados.csv";
    if (argc > 1)
        arquivo_csv = argv[1];  // permite passar arquivo como argumento

    ler_csv(arquivo_csv);
    printf("Arquivo '%s' carregado com %d pontos.\n", arquivo_csv, num_pontos);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(largura_janela, altura_janela);
    glutCreateWindow("Visualizador Grafico de Dados");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, largura_janela, 0, altura_janela);

    glutDisplayFunc(redesenhar);
    glutKeyboardFunc(teclado);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}
