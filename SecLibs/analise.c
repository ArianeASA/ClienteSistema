#include "memoryusade.c"
#include <stdio.h>
#include <stdlib.h>
#define QUANT_POR_MIN 50
#define NIVEL_BOM 0
#define NIVEL_RUIM 1

float *consultarMediaRecursos();
int verificaRecursos();

float *consultarMediaRecursos() {
    printf("Entrouuuu");
    params_embarc *infos = (params_embarc *) malloc(sizeof(params_embarc) * (QUANT_POR_MIN + 1));
    long double somaMedias[3] = {0, 0, 0};
    float *medias = (float *) malloc(sizeof(float) * 3);
    int quantLidos = 0;
    fflush(stdin);
    if ((quantLidos = ler_arquivo(infos)) > 0) {
        // Realiza a média de no maximo 50 registros
        if (quantLidos >= QUANT_POR_MIN) {
            printf("Media para mais de 50 registros\n");
            for (int i = 0; i < QUANT_POR_MIN; i++) {
                somaMedias[0] += infos[i].bateria;
                somaMedias[1] += infos[i].memoria;
                somaMedias[2] += infos[i].cpu;
            }
            medias[0] = (float) (somaMedias[0] / QUANT_POR_MIN);
            medias[1] = (float) (somaMedias[1] / QUANT_POR_MIN);
            medias[2] = (float) (somaMedias[2] / QUANT_POR_MIN);
            return medias;
        } else {// Realiza a média da quantidade de registros lidos
            printf("Media para menos de 50 registros\n");
            for (int i = 0; i < quantLidos; i++) {
                somaMedias[0] += infos[i].bateria;
                somaMedias[1] += infos[i].memoria;
                somaMedias[2] += infos[i].cpu;
            }
            medias[0] = (float) (somaMedias[0] / QUANT_POR_MIN);
            medias[1] = (float) (somaMedias[1] / QUANT_POR_MIN);
            medias[2] = (float) (somaMedias[2] / QUANT_POR_MIN);
            // free(infos);
            return medias;
        }
    }
    // free(infos);
    return NULL;
}

// Verifica 3 tipos de recursos memoria.
int verificaRecursos() {
    printf("Realiza a verificação\n");
    int nivel_recursos = 0;

    params_embarc *infos = (params_embarc *) malloc(sizeof(params_embarc) * (QUANT_POR_MIN + 1));
    long double somaMedias[3] = {0, 0, 0};
    float medias[3];
    int quantLidos = 0;
    fflush(stdin);
    if ((quantLidos = ler_arquivo(infos)) > 0) {
        // Realiza a média de no maximo 50 registros
        if (quantLidos >= QUANT_POR_MIN) {
            printf("Media para mais de 50 registros\n");
            for (int i = 0; i < QUANT_POR_MIN; i++) {
                somaMedias[0] += infos[i].bateria;
                somaMedias[1] += infos[i].memoria;
                somaMedias[2] += infos[i].cpu;
            }
            medias[0] = (float) (somaMedias[0] / QUANT_POR_MIN);
            medias[1] = (float) (somaMedias[1] / QUANT_POR_MIN);
            medias[2] = (float) (somaMedias[2] / QUANT_POR_MIN);
            // return medias;
            // printf("\n Medias %lf %lf %lf",medias[0],medias[1],medias[2]);
        } else {// Realiza a média da quantidade de registros lidos
            printf("Media para menos de 50 registros\n");
            for (int i = 0; i < quantLidos; i++) {
                somaMedias[0] += infos[i].bateria;
                somaMedias[1] += infos[i].memoria;
                somaMedias[2] += infos[i].cpu;
            }
            medias[0] = (float) (somaMedias[0] / QUANT_POR_MIN);
            medias[1] = (float) (somaMedias[1] / QUANT_POR_MIN);
            medias[2] = (float) (somaMedias[2] / QUANT_POR_MIN);
        }
    }

    printf("PAssouuu1");

    double mediaBateria = (double) medias[0];
    double mediaMemoria = (double) medias[1];
    double mediaCpu = (double) medias[2];

    printf("PAssouuu1");
    if (mediaBateria > 25 && mediaBateria <= 75)
        nivel_recursos++;
    else if (mediaBateria < 25)
        nivel_recursos += 3;
    printf("PAssouuu2");
    if (mediaCpu > 50 && mediaCpu <= 75)
        nivel_recursos++;
    else if (mediaCpu > 75)
        nivel_recursos += 2;
    printf("PAssouuu3");

    if (mediaMemoria > 50 && mediaMemoria <= 75)
        nivel_recursos++;
    else if (mediaMemoria > 75)
        nivel_recursos += 2;

    //printf("%lf %lf %lf\n",media[0], media[1], media[2]);
    //Se nivel_recursos for igual a 0 é porque ocorreu algum erro
    if (nivel_recursos > 3 || nivel_recursos == 0) {
        printf("Bateria %lf , Memoria %lf, Cpu %lf\n", mediaBateria, mediaMemoria, mediaCpu);
        sleep(20);
        return NIVEL_RUIM;
    }

    // printf("Status dentro de Verifica: %d Nivel dos Recursos: %d \n", status, nivel_recursos);
    // free(medias);
    fflush(stdout);
    // free(infos);
    return NIVEL_BOM;
    //printf("Recursos bem distribuidos.");
}