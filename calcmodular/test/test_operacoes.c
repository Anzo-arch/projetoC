#include <stdio.h>
#include <assert.h>
#include "../include/matematica.h"

int main() {
    // Testes básicos
    assert(somar(2.0, 3.0) == 5.0);
    assert(subtrair(5.0, 3.0) == 2.0);
    assert(multiplicar(4.0, 2.5) == 10.0);
    assert(dividir(10.0, 2.0) == 5.0);
    assert(potencia(2.0, 3.0) == 8.0);

    // Teste de divisão por zero (deve retornar 0)
    assert(dividir(10.0, 0.0) == 0.0);

    // Teste de potência com expoente 0
    assert(potencia(5.0, 0.0) == 1.0);

    printf("Todos os testes passaram!\n");
    return 0;
}
