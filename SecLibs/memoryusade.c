
#include <stdio.h>
#include <stdlib.h>
#define MAX 100
#define DIRETORIO_BIN "../Recursos/dados.bin"
#define QUANT_POR_MIN 50
typedef struct _infos_embarc {
    float memoria;
    float bateria;
    float cpu;
    char crip[11];
} params_embarc;
typedef struct {
    int socket_desc, set;
    char crip[10];
} thread_arg;

void *captura_infos(void *);
void *escrever_arquivo(void *);
int ler_arquivo(params_embarc *);

void *captura_infos(void *args) {

    while (((thread_arg *) args)->set) {
        FILE *mem = popen("echo `smem -m -p - t -c rss | tail -1`", "r");
        FILE *bat = popen("upower -i /org/freedesktop/UPower/devices/battery_BAT1 | grep 'percentage' | sed '1s/percentage://' | sed 's/ //g' ", "r");
        FILE *pros = popen("NUMCPUS=`grep ^proc /proc/cpuinfo | wc -l`; FIRST=`cat /proc/stat | awk '/^cpu / {print $5}'`; sleep 1; SECOND=`cat /proc/stat | awk '/^cpu / {print $5}'`; USED=`echo 2 k 100 $SECOND $FIRST - $NUMCPUS / - p | dc`; echo ${USED}", "r");
        params_embarc infos = *(params_embarc *) malloc(sizeof(params_embarc));

        if (mem == NULL | bat == NULL | pros == NULL)
            break;

        unsigned memoria;
        unsigned bateria;
        unsigned cpu;

        size_t n;
        char buff[8];

        if ((n = fread(buff, 1, sizeof(buff) - 1, mem)) <= 0)
            break;

        buff[n] = '\0';
        if (sscanf(buff, "%u", &memoria) != 1)
            break;

        pclose(mem);

        if ((n = fread(buff, 1, sizeof(buff) - 1, bat)) <= 0)
            break;

        buff[n] = '\0';
        if (sscanf(buff, "%u", &bateria) != 1)
            break;

        pclose(bat);

        if ((n = fread(buff, 1, sizeof(buff) - 1, pros)) <= 0)
            break;

        buff[n] = '\0';
        if (sscanf(buff, "%u", &cpu) != 1)
            break;

        pclose(pros);

        infos.bateria = bateria;
        infos.cpu = cpu;
        infos.memoria = memoria;
        strcpy(infos.crip, (char *) ((thread_arg *) args)->crip);
        escrever_arquivo(&infos);

        sleep(1);

    }
    ((thread_arg *) args)->set = -1;
    pthread_exit(NULL);
    //     return infos;
}

// função para escrever os elementos de uma struct no arquivo
void *escrever_arquivo(void *infos) {
    const params_embarc *informacoes = (params_embarc *) infos;
    FILE *arq;

    // Acrescenta dados ou cria uma arquivo binário para leitura e escrita.
    arq = fopen(DIRETORIO_BIN, "ab+");

    if (arq != NULL) {
        // escreve cada elemento do vetor no arquivo
        fflush(stdin);
        fwrite(&informacoes[0], sizeof(params_embarc), 1, arq);
        //fecha o arquivo
        fclose(arq);
    } else {
        printf("\nErro ao abrir o arquivo para leitura!\n");
        exit(1);// aborta o programa
    }
    return NULL;
}

// função para ler do arquivo
// recebe a struct que ela irá preencher
// retorna a quantidade de elementos preenchidos
int ler_arquivo(params_embarc *infos) {
    // abre o arquivo para leitura
    FILE *arq = fopen(DIRETORIO_BIN, "rb");
    int quebra = 1;
    // int tamanho = sizeof(infos)/sizeof(params_embarc);
    if (arq != NULL) {
        int indice = 0;
        while (quebra) {
            params_embarc p;

            // fread ler os dados
            // retorna a quantidade de elementos lidos com sucesso
            fflush(stdin);
            size_t r = fread(&p, sizeof(params_embarc), 1, arq);

            // se retorno for menor que o count, então sai do loop
            if (r < 1 || indice > QUANT_POR_MIN)
                quebra = 0;

            if (p.bateria != 0.00 || p.cpu != 0.00 || p.memoria != 0.00 || p.crip != "") {
                infos[indice++] = p;
            }
            //caso a struct encontrada tenha todos os dados iguais a zero,
            //é um erro e ele será ignorado
        }
        // fecha o arquivo
        fclose(arq);
        return indice;
    } else {
        printf("\nErro ao abrir o arquivo para leitura!\n");
        // aborta o programa
        exit(1);
    }
}

// int main(void) {
////      params_embarc *infos = captura_infos(1);
////      if (infos == NULL){
////          fprintf(stderr, "Erro na captura dos dados\n");
////      }else{
////          printf("bateria: %.2f\n", infos->bateria);
////          printf("memoria: %.2f\n", infos->memoria);
////          printf("cpu: %.2f\n", infos->cpu);
//
//          params_embarc infos2[MAX];
//
//          printf("Iniciando a leitura\n");
//          int leu = ler_arquivo(infos2);
//          printf("Quantidade Armazenada: %d\n", leu);
//          while(leu-- > 0){
//            printf("bateria2: %.2f\n", infos2[leu].bateria);
//            printf("memoria2: %.2f\n", infos2[leu].memoria);
//            printf("cpu2: %.2f\n\n", infos2[leu].cpu);
//          }
////      }
//      return 0;
// }

// // Ler arquivo Binário
// int main(void) {
//
//    params_embarc infos2[MAX];
//
//    printf("Iniciando a leitura\n");
//    int leu = ler_arquivo(infos2);
//    printf("Quantidade Armazenada: %d\n", leu);
//    while(leu-- > 0){
//        printf("bateria2: %.2f\n", infos2[leu].bateria);
//        printf("memoria2: %.2f\n", infos2[leu].memoria);
//        printf("cpu2: %.2f\n\n", infos2[leu].cpu);
//    }
//    return 0;
//}