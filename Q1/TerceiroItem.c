#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ESCRITORES 5

int saldo = 1000;

void* escritor(void* arg) {
    int id = *((int*)arg);
    while (1) {
        int operacao = rand() % 2;
        int valor = rand() % 100;
        
        if (operacao == 0) {
            int novo_saldo = saldo + valor;
            printf("Escritor %d: Depósito de %d, Saldo atualizado é %d\n", id, valor, novo_saldo);
            saldo = novo_saldo;
        } else {
            if (saldo >= valor) {
                int novo_saldo = saldo - valor;
                printf("Escritor %d: Saque de %d, Saldo atualizado é %d\n", id, valor, novo_saldo);
                saldo = novo_saldo;
            } else {
                printf("Escritor %d: Tentativa de saque de %d falhou, Saldo insuficiente (%d)\n", id, valor, saldo);
            }
        }
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t escritores[NUM_ESCRITORES];
    int ids_escritores[NUM_ESCRITORES];

    for (int i = 0; i < NUM_ESCRITORES; i++) {
        ids_escritores[i] = i + 1;
        if (pthread_create(&escritores[i], NULL, escritor, &ids_escritores[i]) != 0) {
            perror("Erro ao criar thread escritora");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_ESCRITORES; i++) {
        pthread_join(escritores[i], NULL);
    }

    return 0;
}
