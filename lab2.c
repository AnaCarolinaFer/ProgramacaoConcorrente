/* Multiplicacao de matriz-vetor (considerando matrizes quadradas) */
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"
#include <math.h>

long long int *vet; //vetor de entrada
int nthreads; //numero de threads
int primos = 0;
pthread_mutex_t mutex;

typedef struct{
   int ini, step;
   long long int vsize; //dimensao da estrutura de entrada
} tArgs;

//funcao que as threads executarao
int ehPrimo(long long int n) {
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
    for (i=3; i<sqrt(n)+1; i+=2){
        if(n%i==0) return 0;
    }
    return 1;
}

void *verifPrimo(void *arg) {
   tArgs *args = (tArgs*) arg;
   for(int i=(args->ini); i<(args->vsize); i=i+(args->step)) {
      if(ehPrimo(vet[i])){
        pthread_mutex_lock(&mutex);
        primos++; //incrementa o contador de primos
        pthread_mutex_unlock(&mutex);
      }
   }

   pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char* argv[]) {
   long long int N; //dimensao do vetor de entrada
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   double delta, inicio, fim;

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);

   //leitura e avaliacao dos parametros de entrada
   if(argc<3) {
      printf("Digite: %s <quantidade de naturais, numero de eleemntos N> <numero de threads>\n", argv[0]);
      return 1;
   }
   N = atoll(argv[1]);
   nthreads = atoi(argv[2]);
   if (nthreads > N) nthreads=N;

   //alocacao de memoria para as estruturas de dados
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   vet = (long long int *) malloc(sizeof(long long int) * N);
   if (vet == NULL) {printf("ERRO--malloc\n"); return 2;}

   //inicializacao da estrutura de dados de entrada 
   for(int i=0; i<N; i++) {
        vet[i] = i;  
   }

   GET_TIME(inicio);
   for(int i=0; i<nthreads; i++){
    args = malloc(sizeof(tArgs));
    if (args == NULL) {
      printf("--ERRO: malloc()\n"); exit(-1);
    }
        
    args->ini = i; 
    args->step = nthreads;
    args->vsize = N;   
      
    if(pthread_create(&tid[i], NULL, verifPrimo, (void*) args)){
      printf("--ERRO: pthread_create()\n");
    }
  }
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }

   pthread_mutex_destroy(&mutex);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Quantidade de primos: %d\n", primos);
   printf("Tempo de execução: %lf", delta);

   //liberacao da memoria
   free(vet);
   free(args);
   free(tid);

   

   return 0;
}