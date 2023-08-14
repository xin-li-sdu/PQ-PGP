#include <openssl/evp.h>
#include <stdint.h>
#include <string.h>
#include "aes_ccm.h"

void handleErrors()
{
    return;
}

int ccm_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext){
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    uint8_t iv[] = "00000000000000";

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the encryption operation. */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ccm(), NULL, NULL, NULL))
        handleErrors();

    /*
     * Setting IV len to 7. Not strictly necessary as this is the default
     * but shown here for the purposes of this example.
     */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_IVLEN, 7, NULL))
        handleErrors();

    /* Set tag length */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_TAG, 14, NULL);

    /* Initialise key and IV */
    if(1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    /* Provide the total plaintext length */
    if(1 != EVP_EncryptUpdate(ctx, NULL, &len, NULL, plaintext_len))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can only be called once for this.
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in CCM mode.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Get the tag */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_GET_TAG, 14, ciphertext + ciphertext_len))
        handleErrors();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int ccm_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *plaintext){
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    int ret;

    uint8_t iv[] = "00000000000000";

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the decryption operation. */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ccm(), NULL, NULL, NULL))
        handleErrors();

    /* Setting IV len to 7. Not strictly necessary as this is the default
     * but shown here for the purposes of this example */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_IVLEN, 7, NULL))
        handleErrors();

    /* Set expected tag value. */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_TAG, 14, ciphertext + ciphertext_len - 14))
        handleErrors();

    /* Initialise key and IV */
    if(1 != EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();


    /* Provide the total ciphertext length */
    if(1 != EVP_DecryptUpdate(ctx, NULL, &len, NULL, ciphertext_len-14))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    ret = EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len-14);

    plaintext_len = len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if(ret > 0) {
        /* Success */
        return plaintext_len;
    } else {
        /* Verify failed */
        return -1;
    }
}