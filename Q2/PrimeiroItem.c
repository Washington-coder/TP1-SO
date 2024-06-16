#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>


#define BUFFER_SIZE 40
#define CONSUMERS 1
#define PRODUCERS 8
#define TEMPO_CONSUMIDOR 1
#define TEMPO_PRODUTOR 2

long buffer[BUFFER_SIZE];
int produtorIndex = 0;
int consumerIndex = 0;
long produtorTotal  = 0;
long consumerTotal = 0;

sem_t cheio;
sem_t vazio;
sem_t escrevendoIndexProductor;
sem_t escrevendoProdutorTotal;
sem_t lendoDiferença;
sem_t printingBuffer;

void produz(int valor){
  sem_wait(&escrevendoIndexProductor); 
  buffer[produtorIndex] = valor;
  produtorIndex = (produtorIndex + 1) % BUFFER_SIZE;
  sem_post(&escrevendoIndexProductor);
}

int consome(){
  int tmp = buffer[consumerIndex];
  consumerTotal++;
  consumerIndex = (consumerIndex + 1) % BUFFER_SIZE;
  return tmp;
}

void* printBuffer(void *arg){
  while(1){

    for (int i = 0; i < BUFFER_SIZE; i++) {
      printf("%3ld",buffer[i]);
    } 
    printf("\n");
    for (int x = 0;x < BUFFER_SIZE;x++){
      if (x==consumerIndex){
        printf("  ^");
      }else{
        printf("  -");
      }
    }
    printf("\n");
    sleep(2);
  }
}

void* produtor(void *arg){
  int i = 5;
  long tid;
  tid = (long)arg;
  printf("Produtor %ld Iniciado!\n", tid);
  sleep(2);
  long loops = 4*i + 5;

  sem_wait(&escrevendoProdutorTotal);
  produtorTotal += loops;
  sem_post(&lendoDiferença);
  sem_post(&escrevendoProdutorTotal);

  for (i = 0; i < loops; i++) {
    sem_wait(&cheio);
    sleep(TEMPO_PRODUTOR);
    produz(tid); 
    printf("Produtor %ld Escreveu %ld no buffer indo para posição: %d\n", tid,tid,produtorIndex);
    sem_post(&vazio); 
 } 
  pthread_exit(NULL);

}

void *consumidor(void *arg) {
  int i, tmp = 0;
  long tid;
  tid = (long)arg;
  printf("Consumidor %ld Iniciado!\n", tid);

  sem_wait(&lendoDiferença);
  while ((produtorTotal - consumerTotal) != 0) {
    sem_wait(&vazio); 
    sleep(TEMPO_CONSUMIDOR);
    tmp = consome(); 
    printf("Consumidor %ld leu %d indo para a posição: %d !!!\n", tid, tmp,consumerIndex);
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
  pthread_t printT;

  sem_init(&cheio, 0, BUFFER_SIZE);
  sem_init(&vazio, 0, 0);
  sem_init(&escrevendoIndexProductor, 0, 1);
  sem_init(&escrevendoProdutorTotal,0,1);
  sem_init(&lendoDiferença,0,0);

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

  rc =  pthread_create(&printT, NULL, printBuffer,NULL);

  for (p = 0; p < PRODUCERS; p++) {
    rc = pthread_join(producersT[p], NULL); assert(rc == 0);
  }

  printf("todos os produtores terminaram!!!\n\n");


  for (c = 0; c < CONSUMERS; c++) {
    rc = pthread_join(consumersT[c], NULL); assert(rc == 0);
  }


  printf("TODO O BUFFER FOI CONSUMIDO ENCERRANDO O PROGRAMA!!!!!\n\n");
 

  return 0;
}


