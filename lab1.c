/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 2 - Laboratório: 1 */
/* Ana Carolina Ferreira de Figueiredo */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <time.h> 

#define NTHREADS  8 //total de threads a serem criadas
#define N 1000 //tamanho do vetor

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
  int ini, step, vsize;
  float *value;
}t_Args;

//funcao executada pelas threads
void *Square (void *arg) {
  t_Args *args = (t_Args *) arg;
  int i;
    
  for(i=(args->ini); i<(args->vsize); i=i+(args->step)) {
    args->value[i] = (args->value[i])*(args->value[i]); 
  }

  free(arg); //libera a alocacao feita na main

  pthread_exit(NULL);
}

//funcao de verificacao
void test(float* v1, float* v2) {
  int i;
    
  for(i=0; i<N; i++){
    if((v1[i]*v1[i]) != v2[i]){
      printf("--ERRO: Values don't match\n");
    }
  }
  printf("Passed the test\n");
}

//funcao principal do programa
int main() {
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  t_Args *args; //recebera os argumentos para a thread

  int i; 
  float v[N]; //inicializa o vetor
  srand((unsigned)time(NULL)); 
  for(i=0; i<N; i++){
      v[i] = (float)rand()/(float)rand();
  }

  float vtest[N]; //vetor para verificação
  for(i=0; i<N; i++) {  
    vtest[i] = v[i];
  }

  for(i=0; i<NTHREADS; i++){
    args = malloc(sizeof(t_Args));
    if (args == NULL) {
      printf("--ERRO: malloc()\n"); exit(-1);
    }
        
    args->ini = i; 
    args->step = NTHREADS;
    args->vsize = N;
    args->value = v;
         
    if(pthread_create(&tid_sistema[i], NULL, Square, (void*) args)){
      printf("--ERRO: pthread_create()\n");
    }
  }

  //espera todas as threads terminarem
  for (int i=0; i<NTHREADS; i++) {
    if (pthread_join(tid_sistema[i], NULL)) {
      printf("--ERRO: pthread_join() \n"); exit(-1); 
    }
  }

  printf("--Thread principal terminou\n");

  test(vtest, v);

  pthread_exit(NULL);
  return 0;
}