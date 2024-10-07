#ifndef FUNCOES_H
#define FUNCOES_H

#define TAMANHO_GRADE 30  // Definição do tamanho da grade

#include <pthread.h>

// Estrutura para o nodo sensor
typedef struct {
    int x;
    int y;
} NodoSensor;

// Protótipos de funções
void inicializar_grade();
void imprimir_grade();
int eh_nodo_borda(int x, int y);

void *nodo_sensor(void *arg);
void *gerador_incendios(void *arg);
void *central_controle(void *arg);

// Variáveis globais compartilhadas
extern char grade[TAMANHO_GRADE][TAMANHO_GRADE];
extern pthread_mutex_t mutex_grade;

extern pthread_cond_t cond_sensor[TAMANHO_GRADE][TAMANHO_GRADE];
extern pthread_mutex_t mutex_sensor[TAMANHO_GRADE][TAMANHO_GRADE];

extern pthread_mutex_t mutex_central;
extern pthread_cond_t cond_central;
extern int alerta_central;

#endif // FUNCOES_H
