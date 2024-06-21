#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_LEITORES 5
#define NUM_ESCRITORES 3

int saldo = 50;

void* leitor(void* arg) {
    int id = *((int*)arg);
    while (1) {
        int saldo_atual = saldo;
        printf("Leitor %d: Saldo atual é %d\n", id, saldo_atual);
        sleep(1);
    }
    return NULL;
}

void* escritor(void* arg) {
    int id = *((int*)arg);
    while (1) {
        int operacao = rand() % 2;
        int valor = 1;
        
        if (operacao == 0) {
            saldo += valor;
            printf("Escritor %d: Depósito de %d, Saldo atualizado é %d\n", id, valor, saldo);
        } else {
            if (saldo >= valor) {
                saldo -= valor;
                printf("Escritor %d: Saque de %d, Saldo atualizado é %d\n", id, valor, saldo);
            } else {
                printf("Escritor %d: Tentativa de saque de %d falhou, Saldo insuficiente (%d)\n", id, valor, saldo);
            }
        }
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t leitores[NUM_LEITORES];
    pthread_t escritores[NUM_ESCRITORES];
    int ids_leitores[NUM_LEITORES];
    int ids_escritores[NUM_ESCRITORES];

    for (int i = 0; i < NUM_LEITORES; i++) {
        ids_leitores[i] = i + 1;
        if (pthread_create(&leitores[i], NULL, leitor, &ids_leitores[i]) != 0) {
            perror("Erro ao criar thread leitora");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_ESCRITORES; i++) {
        ids_escritores[i] = i + 1;
        if (pthread_create(&escritores[i], NULL, escritor, &ids_escritores[i]) != 0) {
            perror("Erro ao criar thread escritora");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_LEITORES; i++) {
        pthread_join(leitores[i], NULL);
    }
    for (int i = 0; i < NUM_ESCRITORES; i++) {
        pthread_join(escritores[i], NULL);
    }

    return 0;
}
