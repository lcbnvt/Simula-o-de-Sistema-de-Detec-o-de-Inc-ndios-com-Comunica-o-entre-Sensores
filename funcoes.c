#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "funcoes.h"

// As variáveis globais já estão declaradas em main.c

// Função para inicializar a grade
void inicializar_grade() {
    pthread_mutex_lock(&mutex_grade);
    for (int i = 0; i < TAMANHO_GRADE; i++) {
        for (int j = 0; j < TAMANHO_GRADE; j++) {
            grade[i][j] = 'T'; // Inicializa todas as células com sensores ativos
        }
    }
    pthread_mutex_unlock(&mutex_grade);
}

// Função para imprimir a grade
void imprimir_grade() {
    system("clear"); // Limpa o terminal para exibir a grade atualizada
    for (int i = 0; i < TAMANHO_GRADE; i++) {
        for (int j = 0; j < TAMANHO_GRADE; j++) {
            printf("%c ", grade[i][j]);
        }
        printf("\n");
    }
}

// Função auxiliar para verificar se um nodo está na borda
int eh_nodo_borda(int x, int y) {
    return x == 0 || x == TAMANHO_GRADE - 1 || y == 0 || y == TAMANHO_GRADE - 1;
}

// Função da thread do nodo sensor
void *nodo_sensor(void *arg) {
    NodoSensor *nodo = (NodoSensor *)arg;
    int x = nodo->x;
    int y = nodo->y;
    free(nodo);

    while (1) {
        // Espera por um sinal (incêndio detectado)
        pthread_mutex_lock(&mutex_sensor[x][y]);
        pthread_cond_wait(&cond_sensor[x][y], &mutex_sensor[x][y]);
        pthread_mutex_unlock(&mutex_sensor[x][y]);

        // Verifica se a célula está em chamas
        pthread_mutex_lock(&mutex_grade);
        if (grade[x][y] == '@') {
            printf("Sensor em (%d,%d) detectou incêndio!\n", x, y);
            // Notifica os vizinhos
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if ((dx != 0 || dy != 0) && nx >= 0 && nx < TAMANHO_GRADE && ny >= 0 && ny < TAMANHO_GRADE) {
                        // Sinaliza o sensor vizinho
                        pthread_mutex_lock(&mutex_sensor[nx][ny]);
                        pthread_cond_signal(&cond_sensor[nx][ny]);
                        pthread_mutex_unlock(&mutex_sensor[nx][ny]);
                    }
                }
            }
            // Se for nodo de borda, notifica a central de controle
            if (eh_nodo_borda(x, y)) {
                pthread_mutex_lock(&mutex_central);
                alerta_central = 1;
                pthread_cond_signal(&cond_central);
                pthread_mutex_unlock(&mutex_central);
            }
        }
        pthread_mutex_unlock(&mutex_grade);
    }
    return NULL;
}

// Função da thread geradora de incêndios
void *gerador_incendios(void *arg) {
    while (1) {
        sleep(3); // Espera por 3 segundos
        int x = rand() % TAMANHO_GRADE;
        int y = rand() % TAMANHO_GRADE;

        pthread_mutex_lock(&mutex_grade);
        if (grade[x][y] == 'T') {
            grade[x][y] = '@'; // Incendeia a célula
            printf("Incêndio gerado em (%d,%d)\n", x, y);
            // Sinaliza o sensor correspondente
            pthread_mutex_lock(&mutex_sensor[x][y]);
            pthread_cond_signal(&cond_sensor[x][y]);
            pthread_mutex_unlock(&mutex_sensor[x][y]);
        }
        pthread_mutex_unlock(&mutex_grade);
    }
    return NULL;
}

// Função da thread da central de controle
void *central_controle(void *arg) {
    while (1) {
        // Espera por alerta dos nodos de borda
        pthread_mutex_lock(&mutex_central);
        while (alerta_central == 0) {
            pthread_cond_wait(&cond_central, &mutex_central);
        }
        alerta_central = 0;
        pthread_mutex_unlock(&mutex_central);

        // Simula ação de combate ao incêndio
        printf("Central de controle iniciando medidas de combate ao incêndio.\n");
        sleep(2); // Simula tempo para extinguir o fogo

        // Atualiza as células de '@' para '/'
        pthread_mutex_lock(&mutex_grade);
        for (int i = 0; i < TAMANHO_GRADE; i++) {
            for (int j = 0; j < TAMANHO_GRADE; j++) {
                if (grade[i][j] == '@') {
                    grade[i][j] = '/'; // Célula queimada
                }
            }
        }
        pthread_mutex_unlock(&mutex_grade);

        printf("Combate ao incêndio concluído. Grade atualizada.\n");
    }
    return NULL;
}
