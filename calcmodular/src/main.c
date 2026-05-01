#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matematica.h"

void registrar_historico(char operacao, double a, double b, double resultado) {
    FILE *log = fopen("historico.txt", "a");
    if (log == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo de histórico.\n");
        return;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(log, "[%02d/%02d/%04d %02d:%02d:%02d] "
                 "%.2f %c %.2f = %.2f\n",
            tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            a, operacao, b, resultado);
    fclose(log);
}

int main() {
    double a, b, resultado;
    char operacao;
    char continuar;

    printf("=== Calculadora Modular ===\n");

    do {
        printf("\nDigite a operacao (ex: 5 + 3): ");
        fflush(stdout);
        scanf("%lf %c %lf", &a, &operacao, &b);

        switch (operacao) {
            case '+': resultado = somar(a, b); break;
            case '-': resultado = subtrair(a, b); break;
            case '*': resultado = multiplicar(a, b); break;
            case '/': resultado = dividir(a, b); break;
            case '^': resultado = potencia(a, b); break;
            default:
                printf("Operacao invalida.\n");
                continue;
        }

        printf("Resultado: %.2lf\n", resultado);
        registrar_historico(operacao, a, b, resultado);

        printf("Deseja continuar? (s/n): ");
        scanf(" %c", &continuar);
    } while (continuar == 's' || continuar == 'S');

    printf("\nHistorico salvo em historico.txt. Ate logo!\n");
    return 0;
}
