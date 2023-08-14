/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#pragma once

#include "./aes_storage.hpp"

#if defined(__x86_64__)
#include <x86intrin.h>


#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include "./phantom_memory.hpp"
#include "./phantom.hpp"


namespace phantom {
namespace crypto {


/// A hardware AES class - using AES-NI
class aes_ni : public aes_storage
{
public:
    explicit aes_ni(aes_keylen_e key_len);
    ~aes_ni();

    void encrypt_key_128(const uint8_t *key);
    void encrypt_key_192(const uint8_t *key);
    void encrypt_key_256(const uint8_t *key);
    int32_t encrypt(uint8_t out[16], const uint8_t in[16]);

    void decrypt_key_128(const uint8_t *key);
    void decrypt_key_192(const uint8_t *key);
    void decrypt_key_256(const uint8_t *key);
    int32_t decrypt(uint8_t out[16], const uint8_t in[16]);

private:
    __m128i assist_128(__m128i t1, __m128i t2);
    void assist_192(__m128i* t1, __m128i * t2, __m128i * t3);
    void assist_256_1(__m128i* t1, __m128i * t2);
    void assist_256_2(__m128i* t1, __m128i * t3);
    void enc_to_dec();
};


}  // namespace crypto
}  // namespace phantom

#endif
