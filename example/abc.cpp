#include "abc.h"
double add(int x)
{
    ZZ a;
   // ZZX f;
    a=x*5;
    //cout<<a;
    return 2*x;
   // mpz_t t;
   //mpz_init(t);
    /*  mpz_ui_pow_ui(t,2,100);*/
    //gmp_printf("%d",t);
    Vec< Pair< ZZX, long > > factors;
    //ZZ c;
    //factor(c, factors, f);

}
QString pkword()
{

    return "我成功了";
}



void handleErrors()
{
    return;
}

int ccm_encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *aad, int aad_len,
                unsigned char *key,
                unsigned char *iv,
                unsigned char *ciphertext,
                unsigned char *tag)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;


    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the encryption operation. */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ccm(), NULL, NULL, NULL)) //改成128位
        handleErrors();

    /*
     * Setting IV len to 7. Not strictly necessary as this is the default
     * but shown here for the purposes of this example.
     */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_IVLEN, 13, NULL)) //随机数长度改为13
        handleErrors();

    /* Set tag length */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_TAG, 8, NULL); //认证消息长度改为8

    /* Initialise key and IV */
    if(1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    /* Provide the total plaintext length */
    if(1 != EVP_EncryptUpdate(ctx, NULL, &len, NULL, plaintext_len))
        handleErrors();

    /* Provide any AAD data. This can be called zero or one times as required */
    if(1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
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
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_GET_TAG, 8, tag)) //认证消息长度改为8
        handleErrors();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int ccm_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *aad, int aad_len,
                unsigned char *tag,
                unsigned char *key,
                unsigned char *iv,
                unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    int ret;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the decryption operation. */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ccm(), NULL, NULL, NULL)) //改成128位
        handleErrors();

    /* Setting IV len to 7. Not strictly necessary as this is the default
     * but shown here for the purposes of this example */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_IVLEN, 13, NULL)) //随机数长度改为13
        handleErrors();

    /* Set expected tag value. */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_TAG, 8, tag)) //认证消息长度改为8
        handleErrors();

    /* Initialise key and IV */
    if(1 != EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();


    /* Provide the total ciphertext length */
    if(1 != EVP_DecryptUpdate(ctx, NULL, &len, NULL, ciphertext_len))
        handleErrors();

    /* Provide any AAD data. This can be called zero or more times as required */
    if(1 != EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    ret = EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);

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

 int aes111()
{
    uint8_t key[] = "0123456789abcdef";
    uint8_t nonce[] = "0123456789abc";
    uint8_t plain[] = "0123456789abcdef";
    uint8_t add[] = "0123456789abcdef";
    uint8_t crypt[128] = {0};
    uint8_t auth[128] = {0};
    uint8_t decrypt[128] = {0};
    int i, result;

    ccm_encrypt(plain, strlen((const char*)plain), add,  strlen((const char*)add), key, nonce, crypt, auth);
    printf("crypt:");
    for (i = 0; i < 16; i++)
        printf("%02x", crypt[i]);
    printf("\n");
    printf("auth:");
    for (i = 0; i < 16; i++)
        printf("%02x",  auth[i]);
    printf("\n");

    result = ccm_decrypt(crypt, strlen((const char*)crypt), add, strlen((const char*)add), auth, key, nonce, decrypt);
    printf("decrypt:");
    for (i = 0; i < 16; i++)
        printf("%02x", decrypt[i]);
    printf("\n");
    printf("result:%d\n", result);

    return result;
}
