#include "matematica.h"
#include <stdio.h>

double dividir(double a, double b) {
    if (b == 0.0) {
        fprintf(stderr, "Aviso: divisão por zero. Retornando 0.\n");
        return 0.0;
    }
    return a / b;
}
