#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <string.h>

#define MAX_MSGB 1024

typedef struct _parametros_cifra {
    unsigned char *chave;
    unsigned char *iv;
    const EVP_CIPHER *tipo_cifra;
} params_cifra;

int encrypt(unsigned char *, int, params_cifra *, unsigned char *);
int decrypt(unsigned char *, int, params_cifra *, unsigned char *);
void handleErrors(void);


void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    // abort();
}


int encrypt(unsigned char *plaintext, int plaintext_len, params_cifra *params, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Criando o contexto da criptografia*/
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
    Iniciando o processo criptográfico onde temos 
        EVP_bf_cbc()
        EVP_aes_128_cbc()
        EVP_aes_192_cbc()
        EVP_aes_256_cbc()

     */
    if (1 != EVP_EncryptInit_ex(ctx, params->tipo_cifra, NULL, params->chave, params->iv))
        handleErrors();


    memset(ciphertext, '\0', MAX_MSGB);
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;


    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}


int decrypt(unsigned char *ciphertext, int ciphertext_len, params_cifra *params, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    // printf("Contexto criado\n");

    if (1 != EVP_DecryptInit_ex(ctx, params->tipo_cifra, NULL, params->chave, params->iv))
        handleErrors();

    // printf("Inicio da decrip\n");

    memset(plaintext, '\0', MAX_MSGB);
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    // printf("Quase lá EVP_DecryptUpdate\n");

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        handleErrors();
        return -1;
    }
    plaintext_len += len;
    // printf("Finalizando EVP_DecryptFinal_ex\n");

    EVP_CIPHER_CTX_free(ctx);
    // printf("Finalizado e return tamanho do texto\n");
    return plaintext_len;
}
