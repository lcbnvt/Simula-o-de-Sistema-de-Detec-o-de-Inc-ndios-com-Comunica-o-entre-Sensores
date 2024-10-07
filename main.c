#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "funcoes.h"

// Variáveis globais definidas em funcoes.h
char grade[TAMANHO_GRADE][TAMANHO_GRADE];
pthread_mutex_t mutex_grade = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_sensor[TAMANHO_GRADE][TAMANHO_GRADE];
pthread_mutex_t mutex_sensor[TAMANHO_GRADE][TAMANHO_GRADE];

pthread_mutex_t mutex_central = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_central = PTHREAD_COND_INITIALIZER;
int alerta_central = 0;

// Identificadores de threads
pthread_t threads_sensores[TAMANHO_GRADE][TAMANHO_GRADE];
pthread_t thread_gerador_incendios;
pthread_t thread_central_controle;

int main() {
    inicializar_grade();

    // Iniciar threads dos sensores
    for (int i = 0; i < TAMANHO_GRADE; i++) {
        for (int j = 0; j < TAMANHO_GRADE; j++) {
            NodoSensor *nodo = malloc(sizeof(NodoSensor));
            nodo->x = i;
            nodo->y = j;
            pthread_mutex_init(&mutex_sensor[i][j], NULL);
            pthread_cond_init(&cond_sensor[i][j], NULL);
            pthread_create(&threads_sensores[i][j], NULL, nodo_sensor, (void *)nodo);
        }
    }

    // Iniciar thread geradora de incêndios
    pthread_create(&thread_gerador_incendios, NULL, gerador_incendios, NULL);

    // Iniciar thread central de controle
    pthread_create(&thread_central_controle, NULL, central_controle, NULL);

    // Loop principal para visualização da grade
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mutex_grade);
        imprimir_grade();
        pthread_mutex_unlock(&mutex_grade);
    }

    // Esperar threads 
    pthread_join(thread_gerador_incendios, NULL);
    pthread_join(thread_central_controle, NULL);
    for (int i = 0; i < TAMANHO_GRADE; i++) {
        for (int j = 0; j < TAMANHO_GRADE; j++) {
            pthread_join(threads_sensores[i][j], NULL);
        }
    }

    return 0;
}
