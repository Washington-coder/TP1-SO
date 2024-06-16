#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>


#define BUFFER_SIZE 40
#define CONSUMERS 4
#define PRODUCERS 8
#define TEMPO_CONSUMIDOR 2
#define TEMPO_PRODUTOR 1



long buffer[BUFFER_SIZE];
int produtorIndex = 0;
int consumerIndex = 0;
long produtorTotal  = 0;
long consumerTotal = 0;

void produz(int valor){
  buffer[produtorIndex] = valor;
  produtorIndex = (produtorIndex + 1) % BUFFER_SIZE;
}

int consome(){
  int tmp = buffer[consumerIndex];
  consumerIndex = (consumerIndex + 1) % BUFFER_SIZE;
  consumerTotal++;

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
    sleep(1);
  }
}

void* produtor(void *arg){
  int i = 5;
  long tid;
  tid = (long)arg;
  printf("Produtor %ld Iniciado!\n", tid);
  sleep(2);
  long loops = 4*i + 5;
  produtorTotal += loops;

  for (i = 0; i < loops; i++) {
    sleep(TEMPO_PRODUTOR);
    produz(tid); 
    printf("Produtor %ld Escreveu %ld no buffer \n", tid,tid);
 } 
  printf("Produtor %ld terminou!!!\n",tid);
  pthread_exit(NULL);

}

void *consumidor(void *arg) {
  int i, tmp = 0;
  long tid;
  tid = (long)arg;
  printf("Consumidor %ld Iniciado!\n", tid);


  while ((produtorTotal - consumerTotal) != 0) {
    sleep(TEMPO_CONSUMIDOR);
    tmp = consome(); 
    printf("Consumidor %ld leu %d indo para a posição: %d !!!\n", tid, tmp,consumerIndex);
  }
  printf("TODO O BUFFER FOI CONSUMIDO ENCERRANDO O PROGRAMA!!!!!\n\n");
  printf("%ld caracteres escritos %ld lidos",produtorTotal,consumerTotal);
  exit(0);
}

int main(){

  long p = 0;
  long c = 0;

  int rc = 0;
  int i = 0;

  pthread_t consumersT[CONSUMERS];
  pthread_t producersT[PRODUCERS];
  pthread_t printT;


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
  for (c = 0; c < CONSUMERS; c++) {
    rc = pthread_join(consumersT[c], NULL); assert(rc == 0);
  }

 

  return 0;
}


