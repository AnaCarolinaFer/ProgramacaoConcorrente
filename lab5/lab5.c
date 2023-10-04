/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 5 */
/* Codigo: Produtor e Consumidores */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_LIN 50 
#define MAX_BUFFER 10 

int N = 0;
sem_t slotCheio, slotVazio;  //semaforos para sincronizacao por condicao
sem_t mutexGeral; //semaforo geral de sincronizacao por exclusao mutua

char** Buffer; //buffer compartilhado

//funcao para inserir um elemento no buffer
void Insere(char *item) {
    static int in = 0;
    sem_wait(&slotVazio); //aguarda slot vazio para inserir
    sem_wait(&mutexGeral); //exclusão mútua entre produtores
    strcpy(Buffer[in], item); 
    in = (in + 1) % N;
    printf("Produtor inseriu: %s\n", item);
    sem_post(&mutexGeral); 
    sem_post(&slotCheio); //sinaliza um slot cheio
}

//funcao para retirar  um elemento no buffer
void Retira(int id) {
    char* item;
    static int out = 0;
    sem_wait(&slotCheio); //aguarda slot cheio para retirar
    sem_wait(&mutexGeral); //exclusão mútua entre consumidores
    item = Buffer[out];
    printf("Consumidor[%d] retirou: %s\n", id, item);
    strcpy(item, Buffer[out]); 
    Buffer[out][0] = 0; 
    out = (out + 1) % N;
    sem_post(&mutexGeral);
    sem_post(&slotVazio); //sinaliza um slot vazio
}

//função para as threads consumidoras
void *consumidor(void * arg) {
    int id = *(int *)(arg);
    free(arg);

    while(1) {
        Retira(id); //retira o proximo item
        sleep(1);
    }

    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    if(argc != 3){
        printf("Error.\n");
        return 1;
    }
    N = atoi(argv[1]);
    char *arquivo = argv[2];

    char **vetor = NULL;
    vetor = malloc(MAX_BUFFER * sizeof(char*));
    for(int i = 0; i < MAX_BUFFER; i++) {
        vetor[i] = malloc((MAX_LIN) * sizeof(char));
    }
    Buffer = vetor;

    pthread_t tid[N];
    int *id;

    //inicializa os semáforos
    sem_init(&mutexGeral, 0, 1);
    sem_init(&slotCheio, 0, 0);
    sem_init(&slotVazio, 0, N);

    //inicializa as threads consumidoras
    for(int i = 1; i <= N; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&tid[i], NULL, consumidor, (void *) (id));        
    }

    // Abre arquivo para leitura
    FILE *file;
    file = fopen(arquivo, "r");

    char linha[MAX_LIN];

    if(file == NULL) {
        printf("Diretório inválido\n");
        exit(1);
    }

    while(fgets(linha, MAX_LIN, file)) {
        Insere(linha); 
        // Gera delay
        sleep(1);
    }

    // espera todas as threads terminarem
    for(int i = 0; i < N; i++)
    {
        if (pthread_join(tid[i], NULL))
        {
            printf("--ERRO: pthread_join()\n");
            exit(-1);
        }
    }

    pthread_exit(NULL);

    //fecha o arquivo
    fclose(file);


}





