#include "matematica.h"

double potencia(double base, double expoente) {
    double resultado = 1.0;
    int i;
    if (expoente < 0) return 0.0; // simplificação didática
    for (i = 0; i < (int)expoente; i++) {
        resultado *= base;
    }
    return resultado;
}
