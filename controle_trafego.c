#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#define NUM_CRUZAMENTOS 2
#define INTERVALO_CONTAGEM 1000000000ULL 
#define FATORIAL_CALCULO 25 

typedef struct {
    int cruzamento_id;
    unsigned long long veiculos;
    unsigned long long capacidade;
    pthread_mutex_t mutex;
} CruzamentoData;

// Função para cálculo fatorial recursivo 
unsigned long long fatorial_recursivo(int n) {
    if (n <= 1) return 1;
    return n * fatorial_recursivo(n-1);
}

// Thread 1: Contagem de veículos
void* contagem_veiculos(void* arg) {
    CruzamentoData* data = (CruzamentoData*)arg;
    data->veiculos = 0;
    
    
    for(unsigned long long i = 0; i < INTERVALO_CONTAGEM; i++) {
        if(i % 2 == 0) {
            pthread_mutex_lock(&data->mutex);
            data->veiculos++;
            pthread_mutex_unlock(&data->mutex);
            
            
            volatile double x = 0;
            for(int j = 0; j < 100; j++) {
                x += (i * j) / 3.14159;
            }
        }
    }
    
    printf("Cruzamento %d: %llu veículos contados\n", data->cruzamento_id, data->veiculos);
    return NULL;
}

// Thread 2: Verificação de capacidade
void* verificar_capacidade(void* arg) {
    CruzamentoData* data = (CruzamentoData*)arg;
    
    
    data->capacidade = 0;
    for(int i = 1; i <= FATORIAL_CALCULO; i++) {
        pthread_mutex_lock(&data->mutex);
        data->capacidade += fatorial_recursivo(i);
        pthread_mutex_unlock(&data->mutex);
        
        
        volatile double y = 0;
        for(int j = 0; j < 50; j++) {
            y += (i * j) / 2.71828;
        }
    }
    
    printf("Cruzamento %d: Capacidade máxima calculada: %llu\n", data->cruzamento_id, data->capacidade);
    return NULL;
}

void gerenciar_cruzamento(int id) {
    pthread_t thread_contagem, thread_capacidade;
    CruzamentoData data;
    data.cruzamento_id = id;
    pthread_mutex_init(&data.mutex, NULL);

    printf("[Cruzamento %d] Processo iniciado (PID: %d)\n", id, getpid());
    
    if(pthread_create(&thread_contagem, NULL, contagem_veiculos, &data) != 0) {
        perror("Erro ao criar thread de contagem");
        exit(EXIT_FAILURE);
    }
    
    if(pthread_create(&thread_capacidade, NULL, verificar_capacidade, &data) != 0) {
        perror("Erro ao criar thread de capacidade");
        exit(EXIT_FAILURE);
    }
    
    pthread_join(thread_contagem, NULL);
    pthread_join(thread_capacidade, NULL);
    
    pthread_mutex_destroy(&data.mutex);
    printf("[Cruzamento %d] Processo finalizado\n", id);
    exit(EXIT_SUCCESS);
}

int main() {
    pid_t pid[NUM_CRUZAMENTOS];
    
    printf("=== SISTEMA DE CONTROLE DE TRÁFEGO ===\n");
    printf("Iniciando monitoramento de %d cruzamentos...\n", NUM_CRUZAMENTOS);
    
    for(int i = 0; i < NUM_CRUZAMENTOS; i++) {
        pid[i] = fork();
        
        if(pid[i] == 0) {
            gerenciar_cruzamento(i+1);
        } else if(pid[i] < 0) {
            perror("Erro ao criar processo filho");
            exit(EXIT_FAILURE);
        }
    }
    
    for(int i = 0; i < NUM_CRUZAMENTOS; i++) {
        wait(NULL);
    }
    
    printf("=== SISTEMA FINALIZADO ===\n");
    return EXIT_SUCCESS;
}