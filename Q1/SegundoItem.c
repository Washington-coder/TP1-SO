#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_LEITORES 5
#define NUM_ESCRITORES 3

int saldo = 50;
int leitores = 0;
int escritores = 0;

sem_t sem_escritor;  // Semáforo para controlar o acesso dos escritores
sem_t sem_leitor;  // Semáforo para controlar o acesso dos leitores

void* leitor(void* arg) {
    int id = *((int*)arg);
    while (1) {
        sem_wait(&sem_leitor);
        leitores++;
        
        if (leitores == 1){
            sem_wait(&sem_escritor);
        }

        if (escritores == 0){
            sem_post(&sem_leitor);
        }
        
        printf("Leitor %d: Saldo atual é %d\n", id, saldo);
        
        sem_wait(&sem_leitor);
        
        leitores--;
        if (leitores == 0){
            sem_post(&sem_escritor);
        }
        
        sem_post(&sem_leitor);
        
        sleep(1);
    }
    return NULL;
}

void* escritor(void* arg) {
    int id = *((int*)arg);
    while (1) {
        sem_wait(&sem_escritor);
        escritores++;
        
        int operacao = rand() % 2; // 0 para depósito, 1 para saque
        int valor = 1;
        
        if (operacao == 0) {
            // Deposito
            saldo += valor;
            printf("Escritor %d: Depósito de %d, Saldo atualizado é %d\n", id, valor, saldo);
        } else {
            // Saque
            if (saldo >= valor) {
                saldo -= valor;
                printf("Escritor %d: Saque de %d, Saldo atualizado é %d\n", id, valor, saldo);
            } else {
                printf("Escritor %d: Tentativa de saque de %d falhou, Saldo insuficiente (%d)\n", id, valor, saldo);
            }
        }
        
        escritores--;
        sem_post(&sem_escritor);
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t leitores[NUM_LEITORES];
    pthread_t escritores[NUM_ESCRITORES];
    int ids_leitores[NUM_LEITORES];
    int ids_escritores[NUM_ESCRITORES];

    sem_init(&sem_escritor, 0, 1);
    sem_init(&sem_leitor, 0, 1);

    // Criação das threads leitoras
    for (int i = 0; i < NUM_LEITORES; i++) {
        ids_leitores[i] = i + 1;
        if (pthread_create(&leitores[i], NULL, leitor, &ids_leitores[i]) != 0) {
            perror("Erro ao criar thread leitora");
            exit(1);
        }
    }

    // Criação das threads escritoras
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

    sem_destroy(&sem_escritor);
    sem_destroy(&sem_leitor);

    return 0;
}
