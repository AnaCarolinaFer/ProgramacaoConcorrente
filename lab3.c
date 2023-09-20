/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 3 */
/* Codigo: Uso de variáveis de condição e suas operações básicas para sincronização por condição */

/***** Condicao logica da aplicacao: a thread B so pode imprimir "Bye" depois que duas threads A imprimirem  "Hello"  ****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


/* Variaveis globais */
int contador = 0;
int nthreads;
pthread_mutex_t mutex;
pthread_cond_t cond_bar;

/*funcao barreira */
void barreira(int nthreads) {
    pthread_mutex_lock(&mutex);
    if (contador < nthreads - 1) {
        contador++;
        pthread_cond_wait(&cond_bar, &mutex);
    } else {
        contador=0;
        pthread_cond_broadcast(&cond_bar);
    }
    pthread_mutex_unlock(&mutex);
}

/* funcao que sera executada pelas threads */
void *mensagens (void *args) {
    int id = *(int*) args;

    printf("Ola da thread  %d\n", id+1);
    barreira(nthreads);
    printf("Que dia bonito %d\n", id+1);
    barreira(nthreads);
    printf("Ate breve da thread %d\n", id+1);

    pthread_exit(NULL);
}


/* Funcao principal */
int main(int argc, char *argv[]) {
    pthread_t *tid; //identificadores das threads no sistema
    int *id; //identificadores das threads para passar como argumento na funcao

    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&cond_bar, NULL);

    if(argc<2) {
        printf("Digite: %s  <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}

    /* Cria as threads */
    for(int i=0; i<nthreads; i++) {
        id = malloc(sizeof(int));
        if(id==NULL) {puts("ERRO--malloc"); return 2;}
        *id=i;
        if(pthread_create(tid+i, NULL, mensagens, (void *)id)){     
            puts("ERRO--pthread_create"); return 3;
        }
    } 
    /* Espera todas as threads completarem */
    for (int i = 0; i < nthreads; i++) {
        pthread_join(tid[i], NULL);
    }

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_bar);

  return 0;
}