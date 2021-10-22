#include "SecLibs/evpTestes.c"
#include "SecLibs/geraChaves.c"
#include <arpa/inet.h>
#include <linux/kernel.h>
#include <netinet/in.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
// #include "/usr/include/proc/readproc.h"
#include "SecLibs/analise.c"
// #include "memoryusade.c"
#include <time.h>

#define MAX_MSG 1024
#define MAX_MSG128 16
#define DIR_TEMPOS "../Tempos/temposExecucao.csv"
#define MAX_SIZE_chave128 16
#define MAX_SIZE_chave192 24
#define MAX_SIZE_chave256 32
#define QUANT_CRIP 6

void *trataConversa(void *);
void trocaCrip(char *);
int timerUltimaTroca(time_t, time_t *);
void tempoExecucaoUmaMensagem(clock_t, char *);
// void gravaTemposComunicacao(time_t , char * );
//char *vetCrip[QUANT_CRIP] = {"##AES256##", "##AES192##",
//                             "##AES128##", "##BF128##",
//                             "##BF192##", "##BF256##"};
char *vetCrip[3] = {"##AES256##", "##AES192##", "##AES128##"};

int timerUltimaTroca(time_t troca, time_t *fim) {
    time_t now, gasto;
    time(&now);

    gasto = now - troca;
    // printf("Timer: %ld/ inicio: %ld/ agora: %ld\n", gasto, troca, now);
    //Verificação a cada 1 min = 60 segundos
    if (gasto > 300) {
        fflush(stdin);
        time(fim);
        printf("Necessário a Realização da Troca\n");
        printf("Timer: %ld/ inicio: %ld/ agora: %ld\n", gasto, troca, now);
        fflush(stdout);
        return 1;
    }
    return 0;
}
int imprimeCabecalho = 0;
void tempoExecucaoUmaMensagem(clock_t inicio, char cripAtual[]) {

    clock_t fim;
    fim = clock();
    double tempo_gasto;
    tempo_gasto = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
    char *arquivo = DIR_TEMPOS;
    printf("Tempo gasto: %lf s\n", tempo_gasto);

    FILE *arq;
    // Acrescenta dados ou cria uma arquivo leitura e escrita.
    arq = fopen(arquivo, "a+");

    if (imprimeCabecalho == 0) {
        fprintf(arq, "\n %s ;", cripAtual);
        imprimeCabecalho++;
    }

    if (arq != NULL) {
        // escreve cada elemento do vetor no arquivo
        fflush(stdin);
        fprintf(arq, " %lf ;", tempo_gasto);

        //fecha o arquivo
        fclose(arq);
    } else {
        printf("\nErro ao abrir o arquivo para leitura!\n");
        exit(1);// aborta o programa
    }
}


params_cifra *params;
void trocaCrip(char *crip) {
    params_cifra *parametros = (params_cifra *) malloc(sizeof(params_cifra));
    params_cifra *varlimpar = params;
    int tamanho = 0;
    if (strcmp(crip, "##AES256##") == 0) {
        /* A 256 bit chave */
        parametros->chave = (unsigned char *) returnSegredo("../ChavesClientes/pubECC.pem", "../ChavesServidor/privECC.pem", &tamanho);
        // "01234567890123456789012345678901";
        parametros->iv = (unsigned char *) "UTF3456789012345";
        parametros->tipo_cifra = EVP_aes_256_cbc();
    } else if (strcmp(crip, "##AES192##") == 0) {
        /* A 192 bit chave */
        parametros->chave = (unsigned char *) "KKKKKKKK888985888582525T";
        parametros->iv = (unsigned char *) "BRA3456789012345";
        parametros->tipo_cifra = EVP_aes_192_cbc();
    } else if (strcmp(crip, "##AES128##") == 0) {
        /* A 128 bit chave */
        parametros->chave = (unsigned char *) "ARIAEFGHIJ123456";
        parametros->iv = (unsigned char *) "VOA3456789012345";
        parametros->tipo_cifra = EVP_aes_128_cbc();
    }
    //    else if (strcmp(crip, "##BF128##") == 0)
    //    {
    //        /* A 128 bit chave */
    //        parametros->chave = (unsigned char *)"PARAEFGHIJ123456";
    //        parametros->iv = (unsigned char *)"ARIANE25";
    //        parametros->tipo_cifra = EVP_bf_cbc();
    //    }
    //    else if (strcmp(crip, "##BF192##") == 0)
    //    {
    //        /* A 192 bit chave */
    //        parametros->chave = (unsigned char *)"KKKKKKKK888985888582525T";
    //        parametros->iv = (unsigned char *)"BABA4538";
    //        parametros->tipo_cifra = EVP_bf_cbc();
    //    }
    //    else if (strcmp(crip, "##BF256##") == 0)
    //    {
    //        /* A 256 bit chave */
    //        parametros->chave = (unsigned char *)"01234567890123456789012345678901";
    //        parametros->iv = (unsigned char *)"TIO9K2K2";
    //        parametros->tipo_cifra = EVP_bf_cbc();
    //    }

    if (!params) {
        params = parametros;
        free(varlimpar);
    } else {
        params = parametros;
    }

    printf("Troca Realizada\n");
}

int decrip(unsigned char *textocifrado, unsigned char *textoclaro, int tamanho) {

    int textoclaro_tamanho;

    /* Descifrando textocifrado */
    textoclaro_tamanho = decrypt(textocifrado, tamanho, params,
                                 textoclaro);

    textoclaro[textoclaro_tamanho] = '\0';

    /* Mostrando o texto claro */
    printf("Texto Claro:\n");
    // printf("%s\n", textoclaro);

    return 0;
}

int crip(unsigned char *textoclaro, unsigned char *textocifrado) {

    int textocifrado_tamanho;

    /* Criptografar o textoclaro */
    textocifrado_tamanho = encrypt(textoclaro, strlen((char *) textoclaro), params,
                                   textocifrado);

    /* Imprimindo texto cifrado */
    printf("Texto Cifrado :\n");
    BIO_dump_fp(stdout, (const char *) textocifrado, textocifrado_tamanho);

    return 0;
}

int main(int argc, char *argv[]) {
    // variaveis
    int socket_desc;
    struct sockaddr_in servidor;

    /*****************************************/
    /* Criando um socket */
    // AF_INET = ARPA INTERNET PROTOCOLS
    // SOCK_STREAM = TCP
    // 0 = protocolo IP
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1) {
        printf("Nao foi possivel criar socket\n");
        return -1;
    }

    /* Informacoes para conectar no servidor */
    // IP do servidor
    // familia ARPANET
    // Porta - hton = host to network short (2bytes)
    //    servidor.sin_addr.s_addr = inet_addr("189.103.147.15");//Servidor prof
    servidor.sin_addr.s_addr = inet_addr("127.0.0.1");//Servidor local
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(1234);

    //Conectando no servidor remoto
    if (connect(socket_desc, (struct sockaddr *) &servidor, sizeof(servidor)) < 0) {
        printf("Nao foi possivel conectar\n");
        return -1;
    }
    printf("Conectado no servidor\n");

    pthread_t infosCliente[2];
    thread_arg *args = (thread_arg *) malloc(sizeof(thread_arg));
    // printf("Socket %d\n", socket_desc);
    args->socket_desc = socket_desc;
    // printf("Socket depois da passgem: %d\n", args->socket_desc);
    args->set = 1;
    strcpy(args->crip, "##init##");

    //    strcpy(args->crip ,);
    /**** Criando thread para coleta de dados durante a realização das tarefas do cliente******/
    // if (pthread_create(&infosCliente[0], NULL, inserindoDados, &(args->set)) < 0)
    if (pthread_create(&infosCliente[0], NULL, captura_infos, (void *) args) < 0) {
        perror("nao foi possivel criar thread: ");
        return -1;
    }
    {
        printf("Criou thread 1\n");
    }
    /**** Criando thread para comunicação com o servidor******/
    if (pthread_create(&infosCliente[1], NULL, trataConversa, (void *) args) < 0) {
        perror("nao foi possivel criar thread: ");
        return -1;
    }
    {
        printf("Criou thread 2\n");
    }

    while (args->set >= 0) {
    }

    //Inicia a leitura do arquivo binário criado
    //    params_embarc *infos = (params_embarc *)malloc(sizeof(params_embarc));

    // printf("Iniciando a leitura\n");
    // int leu = ler_arquivo(infos);
    // // printf("Quantidade Armazenada: %d\n", leu);
    // while(leu-- > 0){
    //     // printf("bateria2: %.2f\n", infos[leu].bateria);
    //     // printf("memoria2: %.2f\n", infos[leu].memoria);
    //     // printf("cpu2: %.2f\n\n", infos[leu].cpu);
    // }

    close(socket_desc);
    return 0;
}

// int socket_desc, int *set
void *trataConversa(void *args) {
    /*******COMUNICAO - TROCA DE MENSAGENS **************/
    // ptr_thread_arg arg = (thread_arg*) args;
    // printf("Socket %d\n", ((thread_arg*) args)->socket_desc );
    // printf("SET : %d\n", ((thread_arg*) args)->set);
    char *mensagem = (char *) malloc(MAX_MSG * sizeof(char));
    char resposta_servidor[MAX_MSG];
    char *msg = (char *) malloc(MAX_MSG * sizeof(char));
    // tempo_cifra * tempoAndTextoCifrado = malloc(sizeof(tempo_cifra));
    int tamanho;
    int indiceAtual = 0;
    time_t troca;
    clock_t tempoIncioEnvio;
    time(&troca);
    strcpy(((thread_arg *) args)->crip, vetCrip[indiceAtual]);

    //Recebe mensagem de boas vindas
    if ((tamanho = recv(((thread_arg *) args)->socket_desc, resposta_servidor, MAX_MSG, 0)) < 0) {
        printf("Falha ao receber resposta\n");
        return -1;
    }
    resposta_servidor[tamanho] = '\0';

    puts(resposta_servidor);
    //Inicializando com o tipo de criptografia
    trocaCrip(vetCrip[indiceAtual]);

    while (1) {

        //Envia mensagem para o servidor
        //AES 128bits o bloco
        tempoIncioEnvio = clock();
        strcpy(msg, "Harry James Potter (Godric's Hollow, 31 de julho de 1980) simplesmente Harry Potter é um personagem fictício protagonista da série homônima de livros (e das respectivas adaptações para o cinema) da autora britânica J. K. Rowling. Vai Harry!");
        crip(msg, mensagem);

        printf("Enviando: ... \n");
        fflush(stdout);
        if (send(((thread_arg *) args)->socket_desc, mensagem, strlen(mensagem), 0) < 0) {
            printf("Erro ao enviar mensagem\n");
            return -1;
        }

        fflush(stdin);
        while (recv(((thread_arg *) args)->socket_desc, resposta_servidor, MAX_MSG128, 0) < 0) {
            if (strcmp(resposta_servidor, "##OK##") == 0) {
                trocaCrip(msg);
                printf("OK! \n");
            } else {
                fflush(stdout);
                if (send(((thread_arg *) args)->socket_desc, mensagem, strlen(mensagem), 0) < 0) {
                    printf("Erro ao enviar mensagem\n");
                    // return -1;
                }
            }
        }
        if (strcmp(resposta_servidor, "##OK##") == 0) {
            tempoExecucaoUmaMensagem(tempoIncioEnvio, vetCrip[indiceAtual]);
        }

        //		if (timerUltimaTroca(troca, &troca) && verificaRecursos() == 0)
        //		if (timerUltimaTroca(troca, &troca))
        if (indiceAtual == 0 || timerUltimaTroca(troca, &troca)) {

            indiceAtual++;
            //            if (indiceAtual >= QUANT_CRIP)
            if (indiceAtual >= 3) {
                //			    indiceAtual = 0;

                break;
            } else {
                time(&troca);
                strcpy(msg, vetCrip[indiceAtual]);
                msg[strlen(msg)] = '\0';
                memset(mensagem, '\0', MAX_MSG);
                puts(msg);

                printf("\033[1;42m Iniciando o processo de Troca de Criptografia \033[0m\n");
                crip(msg, mensagem);

                printf("Enviando: ... \n");
                fflush(stdout);
                if (send(((thread_arg *) args)->socket_desc, mensagem, strlen(mensagem), 0) < 0) {
                    printf("\031[1;42m Erro ao enviar mensagem. \031[0m\n");
                    // return -1;
                } else {

                    //Recebendo resposta do servidor (echo)
                    fflush(stdin);
                    if ((tamanho = recv(((thread_arg *) args)->socket_desc, resposta_servidor, MAX_MSG128, 0)) < 0) {
                        printf("\031[1;42m Falha ao receber resposta \031[0m\n");
                        return -1;
                    }
                    printf("Resposta: ");
                    resposta_servidor[tamanho] = '\0';
                    puts(resposta_servidor);
                    if (strcmp(resposta_servidor, "##TROCAOK##") == 0) {
                        trocaCrip(msg);
                        printf("\033[1;42m Processo de Troca finalizada com SUCESSO! \033[0m\n");
                        imprimeCabecalho = 0;
                        strcpy(((thread_arg *) args)->crip, vetCrip[indiceAtual]);

                    } else {
                        printf("\031[1;42m Processo de Troca NÃO finalizada com SUCESSO! \031[0m\n");
                    }
                }
            }
        }
        //limpa as variaveis
        memset(mensagem, 0, MAX_MSG);
        memset(resposta_servidor, 0, MAX_MSG);
        memset(msg, '\0', sizeof(msg));
    }
    printf("Finalizada a conexão");
    ((thread_arg *) args)->set = 0;
    pthread_exit(NULL);
}