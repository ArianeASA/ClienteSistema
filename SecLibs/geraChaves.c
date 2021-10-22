#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _chave_simetrica {
    unsigned char *chave;
    unsigned char *inicializador;
} chave_simetrica;
void geraChavesRSA(char *, char *, int);
void *lerChavePublicaRSA(char *);
void *lerChavePrivadaRSA(char *);
void *geraChaveAleatoria(unsigned int);
void imprimeChaveRandomica(unsigned int);
void imprimeChavePrivadaRSA(char *);
void imprimeChavePublicaRSA(char *);
int to_nid(char *);
int gerarChavesECC(char *, char *, char *);
void *criptografarRSA(char *, unsigned char *);
void *descriptografarRSA(char *, char *);
void *lerChavePublicaECC(char *);
void imprimeChavePublicaECC(char *);
void *lerChavePrivadaECC(char *);
void imprimeChavePrivadaECC(char *);
void *carregarSegredo(const EC_POINT *, EC_KEY *, int *);
void *returnSegredo(char *, char *, int *);


void geraChavesRSA(char *diretorioChavePrivada, char *diretorioChavePublica, int tamanhoChaveRSA) {

    unsigned long e = RSA_F4;
    BIGNUM *bne = BN_new();
    int retorno = BN_set_word(bne, e);

    RSA *rsa = RSA_new();
    retorno = RSA_generate_key_ex(rsa, tamanhoChaveRSA, bne, NULL);
    if (retorno != 1)
        goto err;

    BIO *bio = BIO_new_file(diretorioChavePrivada, "w+");
    retorno = PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);
    if (retorno != 1)
        goto err;

    bio = BIO_new_file(diretorioChavePublica, "w+");
    retorno = PEM_write_bio_RSAPublicKey(bio, rsa);
    if (retorno != 1)
        goto err;
err:
    BIO_free_all(bio);
    RSA_free(rsa);
}

void *lerChavePublicaRSA(char *diretorioChavePublica) {
    FILE *arquivo;
    RSA *chavePublica = RSA_new();
    // static char *fraseChave = "criptosystem";

    if ((arquivo = fopen(diretorioChavePublica, "r+")) == NULL) {
        fprintf(stderr, "Erro: Arquivo da chave publica não encontrada '%s'.\n", diretorioChavePublica);
        return NULL;
    }
    if ((chavePublica = PEM_read_RSAPublicKey(arquivo, (RSA **) NULL, NULL, NULL)) == NULL) {
        fprintf(stderr, "Erro: Falha na leitura da chave publica '%s' file.\n", diretorioChavePublica);
        return NULL;
    }
    fclose(arquivo);
    return (void *) chavePublica;
}

void *lerChavePrivadaRSA(char *diretorioChavePrivada) {
    FILE *arquivo;
    RSA *chavePrivadaRSA = RSA_new();
    // static char *fraseChave = "criptosystem";

    if ((arquivo = fopen(diretorioChavePrivada, "r+")) == NULL) {
        fprintf(stderr, "Erro: Arquivo da chave privada não encontrada '%s'.\n", diretorioChavePrivada);
        return NULL;
    }
    if ((chavePrivadaRSA = PEM_read_RSAPrivateKey(arquivo, &chavePrivadaRSA, NULL, NULL)) == NULL) {
        fprintf(stderr, "Erro: Falha na leitura da chave privada '%s' .\n", diretorioChavePrivada);
        return NULL;
    }
    fclose(arquivo);
    return (void *) chavePrivadaRSA;
}

void *geraChaveAleatoria(unsigned int tamanhoChave) {
    unsigned char *chave = (char *) malloc(sizeof(char) * tamanhoChave);
    unsigned char *inicializador = (char *) malloc(sizeof(char) * tamanhoChave / 2);
    chave_simetrica *chaves = (chave_simetrica *) malloc(sizeof(chave_simetrica));

    if (!RAND_bytes(chave, tamanhoChave))
        ;

    if (!RAND_bytes(inicializador, tamanhoChave / 2))
        ;

    chaves->chave = chave;
    chaves->inicializador = inicializador;

    return chaves;
}

void imprimeChaveRandomica(unsigned int tamanho) {
    chave_simetrica *chaves = (chave_simetrica *) geraChaveAleatoria(tamanho);

    printf("\nChave: ");
    BIO_dump_fp(stdout, (const void *) chaves->chave, tamanho);
    printf("\nInicializador: ");
    BIO_dump_fp(stdout, (const void *) chaves->inicializador, tamanho / 2);
}

void imprimeChavePrivadaRSA(char *diretorioChaveRSA) {
    size_t tamanhoChavePrivada;
    char *chavePrivadaString;

    RSA *chavePrivadaRSA = (RSA *) lerChavePrivadaRSA(diretorioChaveRSA);

    BIO *bio_priv = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(bio_priv, chavePrivadaRSA, NULL, NULL, 0, NULL, NULL);

    tamanhoChavePrivada = BIO_pending(bio_priv);

    chavePrivadaString = malloc(tamanhoChavePrivada + 1);

    BIO_read(bio_priv, chavePrivadaString, tamanhoChavePrivada);

    chavePrivadaString[tamanhoChavePrivada] = '\0';

    printf("\n%s\n", chavePrivadaString);

    RSA_free(chavePrivadaRSA);
    BIO_free_all(bio_priv);
    free(chavePrivadaString);
}

void imprimeChavePublicaRSA(char *diretorioChavePublica) {
    size_t tamanhoChavePublica;
    char *chavePublicaString;

    RSA *chavePublicaRSA = (RSA *) lerChavePublicaRSA(diretorioChavePublica);

    BIO *bio_pub = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPublicKey(bio_pub, chavePublicaRSA);

    tamanhoChavePublica = BIO_pending(bio_pub);

    chavePublicaString = malloc(tamanhoChavePublica + 1);

    BIO_read(bio_pub, chavePublicaString, tamanhoChavePublica);

    chavePublicaString[tamanhoChavePublica] = '\0';

    printf("\n%s\n", chavePublicaString);

    RSA_free(chavePublicaRSA);
    BIO_free_all(bio_pub);
    free(chavePublicaString);
}

int to_nid(char *curvename) {
    if (curvename == "secp256k1") {
        return NID_secp256k1;
    } else if (curvename == "brainpool256r1") {
        return NID_brainpoolP256r1;
    }

    return -1;
}

int gerarChavesECC(char *pubkeyfile, char *privkeyfile, char *curve_name) {
    EC_KEY *keygen;
    int nid = to_nid(curve_name);

    if (nid == -1) {
        return -1;
    }

    keygen = EC_KEY_new_by_curve_name(nid);
    if (!keygen) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    int ret;

    ret = EC_KEY_generate_key(keygen);
    if (ret != 1) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    ret = EC_KEY_check_key(keygen);
    if (ret != 1) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    FILE *fp;

    fp = fopen(pubkeyfile, "w+");
    if (!fp) {
        return -1;
    }

    PEM_write_EC_PUBKEY(fp, keygen);

    fclose(fp);

    fp = fopen(privkeyfile, "w+");
    if (!fp) {
        return -1;
    }

    PEM_write_ECPrivateKey(fp, keygen, NULL, NULL, 0, NULL, NULL);

    fclose(fp);

    EC_KEY_free(keygen);

    printf("keygen success");
    return 0;
}

void *criptografarRSA(char *diretorioChavePrivadaRSA, unsigned char *textoClaro) {

    RSA *chavePrivadaRSA = RSA_new();
    char *textoCifrado = (char *) malloc(sizeof(char) * 1024);

    if ((chavePrivadaRSA = (RSA *) lerChavePrivadaRSA(diretorioChavePrivadaRSA)) != NULL) {
        fprintf(stderr, "Private key read.\n\n");
        int tamanhoTextoCifrado = RSA_private_encrypt(strlen(textoClaro), (unsigned char *) textoClaro, textoCifrado, chavePrivadaRSA, RSA_PKCS1_PADDING);
        BIO_dump_fp(stdout, (const char *) textoCifrado, tamanhoTextoCifrado);
    }

    RSA_free(chavePrivadaRSA);
    return textoCifrado;
}

void *descriptografarRSA(char *diretorioChavePublicaRSA, char *textoCifrado) {

    RSA *chavePublicaRSA = RSA_new();
    char *textoClaro = (char *) malloc(sizeof(char) * 1024);

    if ((chavePublicaRSA = (RSA *) lerChavePublicaRSA(diretorioChavePublicaRSA)) != NULL) {
        RSA_public_decrypt(RSA_size(chavePublicaRSA), textoCifrado, textoClaro, chavePublicaRSA, RSA_PKCS1_PADDING);
        printf("\nTexto claro: %s\n", textoClaro);
    }

    RSA_free(chavePublicaRSA);
    return textoClaro;
}

void *lerChavePublicaECC(char *diretorioChavePublica) {

    EC_KEY *chavePublica = EC_KEY_new();
    EVP_PKEY *verificaChave = EVP_PKEY_new();
    FILE *file;
    int retorno;

    if (!(file = fopen(diretorioChavePublica, "r"))) {
        fclose(file);
        goto erro;
    }

    if (!(chavePublica = PEM_read_EC_PUBKEY(file, NULL, NULL, NULL))) {
        ERR_print_errors_fp(stderr);
        goto erro;
    }

    if (retorno = EVP_PKEY_assign_EC_KEY(verificaChave, chavePublica) != 1) {
        ERR_print_errors_fp(stderr);
        goto erro;
    }

    fclose(file);
    free(verificaChave);
    printf("Chave Publica carregada com SUCESSO.");
    return chavePublica;

erro:
    free(verificaChave);
    return NULL;
}

void imprimeChavePublicaECC(char *diretorioChavePublica) {

    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_EC_PUBKEY(bio, (EC_KEY *) lerChavePublicaECC(diretorioChavePublica));

    size_t tamanhoChavePublica = BIO_pending(bio);

    char *chavePublicaString = malloc(tamanhoChavePublica + 1);

    BIO_read(bio, chavePublicaString, tamanhoChavePublica);

    chavePublicaString[tamanhoChavePublica] = '\0';

    printf("\n%s\n", chavePublicaString);

    BIO_free_all(bio);
    free(chavePublicaString);
}

void *lerChavePrivadaECC(char *diretorioChavePrivada) {
    FILE *file;
    EC_KEY *chavePrivada = EC_KEY_new();
    EVP_PKEY *verificaChave = EVP_PKEY_new();
    int retorno;

    if (!(file = fopen(diretorioChavePrivada, "r"))) {
        goto erro;
    }

    if (!(chavePrivada = PEM_read_ECPrivateKey(file, NULL, NULL, NULL))) {
        ERR_print_errors_fp(stderr);
        goto erro;
    }

    EC_KEY_check_key(chavePrivada);
    if ((retorno = EVP_PKEY_assign_EC_KEY(verificaChave, chavePrivada)) != 1) {
        ERR_print_errors_fp(stderr);
        goto erro;
    }

    fclose(file);
    free(verificaChave);
    printf("Chave Privada Carregada Com SUCESSO.");

    return chavePrivada;
erro:
    free(verificaChave);
    return NULL;
}

void imprimeChavePrivadaECC(char *diretorioChavePrivada) {

    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_ECPrivateKey(bio, (EC_KEY *) lerChavePrivadaECC(diretorioChavePrivada), NULL, NULL, 0, NULL, NULL);

    size_t tamanhoChavePrivada = BIO_pending(bio);

    char *chavePrivadaString = malloc(tamanhoChavePrivada + 1);

    BIO_read(bio, chavePrivadaString, tamanhoChavePrivada);

    chavePrivadaString[tamanhoChavePrivada] = '\0';

    printf("\n%s\n", chavePrivadaString);

    BIO_free_all(bio);
    free(chavePrivadaString);
}

void *carregarSegredo(const EC_POINT *chavePublicaRecebidaECC, EC_KEY *chavePrivadaECC, int *tamanhoSegredo) {

    int tamanho;
    unsigned char *segredo;

    tamanho = EC_GROUP_get_degree(EC_KEY_get0_group(chavePrivadaECC));
    *tamanhoSegredo = ((tamanho + 7) / 8);
    printf("Tamanho : %d ", *tamanhoSegredo);
    if (NULL == (segredo = OPENSSL_malloc(*tamanhoSegredo))) {
        printf("Falha ao armazenar segredo.");
        return NULL;
    }

    *tamanhoSegredo = ECDH_compute_key(segredo, *tamanhoSegredo, chavePublicaRecebidaECC, chavePrivadaECC, NULL);

    if (*tamanhoSegredo <= 0) {
        OPENSSL_free(segredo);
        return NULL;
    }
    return segredo;
}

void *returnSegredo(char *diretorioChavePublicaOutro, char *diretorioMinhaChavePrivada, int *tamanho) {
    EC_KEY *minhaChavePrivadaCliente = (EC_KEY *) lerChavePrivadaECC(diretorioMinhaChavePrivada);
    const EC_POINT *chavePublicaOutro = EC_KEY_get0_public_key((const EC_KEY *) lerChavePublicaECC(diretorioChavePublicaOutro));
    unsigned char *segredoServidor = (unsigned char *) carregarSegredo(chavePublicaOutro, minhaChavePrivadaCliente, tamanho);
    printf("\nServidor :\n");
    BIO_dump_fp(stdout, (const char *) segredoServidor, sizeof segredoServidor);
    return segredoServidor;
}

//
//int main(int argc, char const *argv[]){
//    // int tamanho=0;
//    // unsigned char *segredoServidor = (unsigned char *) returnSegredo("./ChavesCliente/pubECC.pem", "./ChavesServidor/privECC.pem", &tamanho);
//    // BIO_dump_fp(stdout, (const char *)segredoServidor, sizeof segredoServidor);
//    // printf("\nTamanho: %d\n", tamanho);
//
//    // gerarChavesECC("./ChavesCliente/pubECC.pem", "./ChavesCliente/privECC.pem", "brainpool256r1");
//    // gerarChavesECC("./ChavesServidor/pubECC.pem", "./ChavesServidor/privECC.pem", "brainpool256r1");
//
//    // EC_KEY *chavePrivadaCliente = (EC_KEY *) lerChavePrivadaECC("./ChavesCliente/privECC.pem");
//    // EC_KEY *chavePrivadaServidor = (EC_KEY *) lerChavePrivadaECC("./ChavesServidor/privECC.pem");
//
//    // fflush(stdout);
//    // const EC_POINT *chavePublicaCliente = EC_KEY_get0_public_key((const EC_KEY *)lerChavePublicaECC("./ChavesCliente/pubECC.pem"));
//    // const EC_POINT *chavePublicaServidor = EC_KEY_get0_public_key((const EC_KEY *) lerChavePublicaECC("./ChavesServidor/pubECC.pem"));
//
//    // unsigned char *segredoServidor = (unsigned char *)carregarSegredo(chavePublicaCliente, chavePrivadaServidor);
//    // unsigned char *segredoCliente = (unsigned char *)carregarSegredo(chavePublicaServidor, chavePrivadaCliente);
//
//    // printf("\nServidor :\n");
//	// BIO_dump_fp(stdout, (const char *)segredoServidor, sizeof segredoServidor);
//
//    // printf("\nCliente :\n");
//	// BIO_dump_fp(stdout, (const char *)segredoCliente, sizeof segredoCliente);
//
//    // lerChavePrivadaECC("./chave/privECC.pem");
//    // imprimeChavePrivadaECC("./chave/privECC.pem");
//    // lerChavePublicaECC("./chave/pubECC.pem");
//    // imprimeChavePublicaECC("./chave/pubECC.pem");
//    // char *textoCifrado = (char *)criptografarRSA("./chave/priv.pem", "Oi tia");
//    // unsigned char *textoClaro = (unsigned char *)descriptografarRSA("./chave/pub.pem", textoCifrado);
//    // printf("TextoClaro 2: %s\n\n", textoClaro);
//
//    // free(textoCifrado);
//    // free(textoClaro);
//    // gerarChavesECC("./chave/pubECC.pem", "./chave/privECC.pem", "brainpool256r1");
//    // imprimeChaveRandomica(32);
//
//    geraChavesRSA("./ChavesCliente/privRSA128.pem", "./ChavesCliente/pubRSA128.pem", 128);
//    imprimeChavePrivadaRSA("./ChavesCliente/privRSA128.pem");
//    imprimeChavePublicaRSA("./ChavesCliente/pubRSA128.pem");
//
//    // geraChavesRSA("./ChavesServidor/privRSA128.pem", "./ChavesServidor/pubRSA128.pem", 128);
//    // imprimeChavePrivadaRSA("./ChavesServidor/privRSA128.pem");
//    // imprimeChavePublicaRSA("./ChavesServidor/pubRSA128.pem");
//}