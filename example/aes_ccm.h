#include <stdint.h>

int ccm_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext);

int ccm_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *plaintext);