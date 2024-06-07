#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#define BUFFER_SIZE 40
#define CONSUMERS 1
#define PRODUCERS 8

int buffer[BUFFER_SIZE];
int produtorIndex = 0;
int consumerIndex = 0;
sem_t cheio;
sem_t vazio;
sem_t escrevendoIndexProductor;

void produz(int valor){
  sem_wait(&escrevendoIndexProductor); 
  buffer[produtorIndex] = valor;
  produtorIndex = (produtorIndex + 1) % BUFFER_SIZE;
  sem_post(&escrevendoIndexProductor);
}

int consome(){
  int tmp = buffer[consumerIndex];
  consumerIndex = (consumerIndex + 1) % BUFFER_SIZE;
  return tmp;
}

void* produtor(void *arg){
  int i = 5;
  long tid;
  tid = (long)arg;
  printf("Produtor %ld Iniciado!\n", tid);
  int loops = 4*i;
  for (i = 0; i < loops; i++) {
    sleep(2);
    sem_wait(&cheio);
    produz(i); 
    printf("Produtor %ld Escreveu %d no buffer!!!\n", tid,i);
    sem_post(&vazio); 
 } 
  pthread_exit(NULL);

}

void *consumidor(void *arg) {
  int i, tmp = 0;
  long tid;
  tid = (long)arg;
  printf("Consumidor %ld Iniciado!\n", tid);


  while (tmp != -1) {
    sem_wait(&vazio); 
    sleep(1);
    tmp = consome(); 
    printf("Consumidor %ld leu %d no buffer na posição: %d !!!\n", tid, tmp,consumerIndex);
    sem_post(&cheio); 
  }
  pthread_exit(NULL);

}

int main(){

  long p = 0;
  long c = 0;
  int rc = 0;
  int i = 0;

  pthread_t consumersT[CONSUMERS];
  pthread_t producersT[PRODUCERS];

  sem_init(&cheio, 0, BUFFER_SIZE);
  sem_init(&vazio, 0, 0);
  sem_init(&escrevendoIndexProductor, 0, 1);

  for (p = 0; p < PRODUCERS; p++) {
    rc = pthread_create(&producersT[p], NULL, produtor, (void*)p);
    rc = 0;
    if (rc) {
      printf("ERORR; return code from pthread_create() is %d\n", rc);
      exit(EXIT_FAILURE);
    }
  }

  for (c = 0; c < CONSUMERS ; c++) {
    rc = pthread_create(&consumersT[c], NULL, consumidor, (void*)c);
    rc = 0;
    if (rc) {
      printf("ERORR; return code from pthread_create() is %d\n", rc);
      exit(EXIT_FAILURE);
    }
  }

  for (p = 0; p < PRODUCERS; p++) {
    rc = pthread_join(producersT[p], NULL); assert(rc == 0);
  }
  for (c = 0; c < CONSUMERS; c++) {
    //rc = pthread_join(consumersT[c], NULL); assert(rc == 0);
  }
  printf("todas as thereats acabaram!!!\n");

  for (i = 0; i < BUFFER_SIZE; i++) {
    printf(" %d ",buffer[i]);
  }
  printf("\n");

  return 0;
}


