/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#include "./aes_ni.hpp"


#if defined(__x86_64__)

#if defined(__clang__)

// Ensure that we enable support for AES-NI
#pragma clang attribute push(__attribute__((target("ssse3,sse4.1,aes"))), apply_to = function)
#define __m128i_u   __m128i

#elif defined(__GNUG__)

// Ensure that we enable support for AES-NI
#pragma GCC target("ssse3")
#pragma GCC target("sse4.1")
#pragma GCC target("aes")

#endif



namespace phantom {
namespace crypto {


aes_ni::aes_ni(aes_keylen_e key_len)
{
    m_ks = std::unique_ptr<uint32_t[]>(new uint32_t[static_cast<size_t>(key_len)]);
}

aes_ni::~aes_ni()
{
}

__m128i aes_ni::assist_128(__m128i t1, __m128i t2)
{
    __m128i t3;
    t2 = _mm_shuffle_epi32(t2, 0xff);
    t3 = _mm_slli_si128(t1, 0x4);
    t1 = _mm_xor_si128(t1, t3);
    t3 = _mm_slli_si128(t3, 0x4);
    t1 = _mm_xor_si128(t1, t3);
    t3 = _mm_slli_si128(t3, 0x4);
    t1 = _mm_xor_si128(t1, t3);
    t1 = _mm_xor_si128(t1, t2);
    return t1;
}

void aes_ni::encrypt_key_128(const uint8_t *key)
{
    __m128i t1, t2;
    __m128i *ks = reinterpret_cast<__m128i*>(m_ks.get());

    t1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key));

    ks[0] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x1);
    t1 = assist_128(t1, t2);
    ks[1] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x2);
    t1 = assist_128(t1, t2);
    ks[2] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x4);
    t1 = assist_128(t1, t2);
    ks[3] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x8);
    t1 = assist_128(t1, t2);
    ks[4] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x10);
    t1 = assist_128(t1, t2);
    ks[5] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x20);
    t1 = assist_128(t1, t2);
    ks[6] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x40);
    t1 = assist_128(t1, t2);
    ks[7] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x80);
    t1 = assist_128(t1, t2);
    ks[8] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x1b);
    t1 = assist_128(t1, t2);
    ks[9] = t1;

    t2 = _mm_aeskeygenassist_si128(t1, 0x36);
    t1 = assist_128(t1, t2);
    ks[10] = t1;

    m_info.l = 0;
    m_info.b[0] = 10 * 16;
}

void aes_ni::assist_192(__m128i* t1, __m128i * t2, __m128i * t3)
{
    __m128i t4;
    *t2 = _mm_shuffle_epi32(*t2, 0x55);
    t4  = _mm_slli_si128(*t1, 0x4);
    *t1 = _mm_xor_si128(*t1, t4);
    t4  = _mm_slli_si128(t4, 0x4);
    *t1 = _mm_xor_si128(*t1, t4);
    t4  = _mm_slli_si128(t4, 0x4);
    *t1 = _mm_xor_si128(*t1, t4);
    *t1 = _mm_xor_si128(*t1, *t2);
    *t2 = _mm_shuffle_epi32(*t1, 0xff);
    t4  = _mm_slli_si128(*t3, 0x4);
    *t3 = _mm_xor_si128(*t3, t4);
    *t3 = _mm_xor_si128(*t3, *t2);
}

void aes_ni::encrypt_key_192(const uint8_t *key)
{
    __m128i t1, t2, t3;
    __m128i *ks = reinterpret_cast<__m128i*>(m_ks.get());

    t1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key));
    t3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key + 16));

    ks[0] = t1;
    ks[1] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x1);
    assist_192(&t1, &t2, &t3);

    ks[1] = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(ks[1]), _mm_castsi128_pd(t1), 0));
    ks[2] = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(t1), _mm_castsi128_pd(t3), 1));

    t2 = _mm_aeskeygenassist_si128(t3, 0x2);
    assist_192(&t1, &t2, &t3);
    ks[3] = t1;
    ks[4] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x4);
    assist_192(&t1, &t2, &t3);
    ks[4] = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(ks[4]), _mm_castsi128_pd(t1), 0));
    ks[5] = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(t1), _mm_castsi128_pd(t3), 1));

    t2 = _mm_aeskeygenassist_si128(t3, 0x8);
    assist_192(&t1, &t2, &t3);
    ks[6] = t1;
    ks[7] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x10);
    assist_192(&t1, &t2, &t3);
    ks[7] = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(ks[7]), _mm_castsi128_pd(t1), 0));
    ks[8] = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(t1), _mm_castsi128_pd(t3), 1));

    t2 = _mm_aeskeygenassist_si128(t3, 0x20);
    assist_192(&t1, &t2, &t3);
    ks[9] = t1;
    ks[10] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x40);
    assist_192(&t1, &t2, &t3);
    ks[10] = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(ks[10]), _mm_castsi128_pd(t1), 0));
    ks[11] = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(t1), _mm_castsi128_pd(t3), 1));

    t2 = _mm_aeskeygenassist_si128(t3, 0x80);
    assist_192(&t1, &t2, &t3);
    ks[12] = t1;

    m_info.l = 0;
    m_info.b[0] = 12 * 16;
}

void aes_ni::assist_256_1(__m128i* t1, __m128i * t2)
{
    __m128i t4;
    *t2 = _mm_shuffle_epi32(*t2, 0xff);
    t4  = _mm_slli_si128(*t1, 0x4);
    *t1 = _mm_xor_si128(*t1, t4);
    t4  = _mm_slli_si128(t4, 0x4);
    *t1 = _mm_xor_si128(*t1, t4);
    t4  = _mm_slli_si128(t4, 0x4);
    *t1 = _mm_xor_si128(*t1, t4);
    *t1 = _mm_xor_si128(*t1, *t2);
}

void aes_ni::assist_256_2(__m128i* t1, __m128i * t3)
{
    __m128i t2, t4;
    t4  = _mm_aeskeygenassist_si128(*t1, 0x0);
    t2  = _mm_shuffle_epi32(t4, 0xaa);
    t4  = _mm_slli_si128(*t3, 0x4);
    *t3 = _mm_xor_si128(*t3, t4);
    t4  = _mm_slli_si128(t4, 0x4);
    *t3 = _mm_xor_si128(*t3, t4);
    t4  = _mm_slli_si128(t4, 0x4);
    *t3 = _mm_xor_si128(*t3, t4);
    *t3 = _mm_xor_si128(*t3, t2);
}

void aes_ni::encrypt_key_256(const uint8_t *key)
{
    __m128i t1, t2, t3;
    __m128i *ks = reinterpret_cast<__m128i*>(m_ks.get());

    t1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key));
    t3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key + 16));

    ks[0] = t1;
    ks[1] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x01);
    assist_256_1(&t1, &t2);
    ks[2] = t1;
    assist_256_2(&t1, &t3);
    ks[3] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x02);
    assist_256_1(&t1, &t2);
    ks[4] = t1;
    assist_256_2(&t1, &t3);
    ks[5] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x04);
    assist_256_1(&t1, &t2);
    ks[6] = t1;
    assist_256_2(&t1, &t3);
    ks[7] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x08);
    assist_256_1(&t1, &t2);
    ks[8] = t1;
    assist_256_2(&t1, &t3);
    ks[9] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x10);
    assist_256_1(&t1, &t2);
    ks[10] = t1;
    assist_256_2(&t1, &t3);
    ks[11] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x20);
    assist_256_1(&t1, &t2);
    ks[12] = t1;
    assist_256_2(&t1, &t3);
    ks[13] = t3;

    t2 = _mm_aeskeygenassist_si128(t3, 0x40);
    assist_256_1(&t1, &t2);
    ks[14] = t1;

    m_info.l = 0;
    m_info.b[0] = 14 * 16;
}

void aes_ni::enc_to_dec()
{
    __m128i *ks = reinterpret_cast<__m128i*>(m_ks.get());

    for (size_t j = 1; j < static_cast<size_t>(m_info.b[0]) >> 4; j++) {
        ks[j] = _mm_aesimc_si128(ks[j]);
    }
}

void aes_ni::decrypt_key_128(const uint8_t *key)
{
    encrypt_key_128(key);
    enc_to_dec();

}

void aes_ni::decrypt_key_192(const uint8_t *key)
{
    encrypt_key_192(key);
    enc_to_dec();
}

void aes_ni::decrypt_key_256(const uint8_t *key)
{
    encrypt_key_256(key);
    enc_to_dec();
}

int32_t aes_ni::encrypt(uint8_t out[16], const uint8_t in[16])
{
    __m128i *key = reinterpret_cast<__m128i*>(m_ks.get()), t;

    if (m_info.b[0] != 10 * 16 && m_info.b[0] != 12 * 16 && m_info.b[0] != 14 * 16) {
        return EXIT_FAILURE;
    }

    t = _mm_xor_si128(_mm_loadu_si128(reinterpret_cast<const __m128i*>(in)), *reinterpret_cast<const __m128i*>(key));

    switch (m_info.b[0])
    {
    case 14 * 16:
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        // DO NOT BREAK!
    case 12 * 16:
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        // DO NOT BREAK!
    case 10 * 16:
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenc_si128(t, *reinterpret_cast<const __m128i*>(++key));
        t = _mm_aesenclast_si128(t, *reinterpret_cast<const __m128i*>(++key));
    }

    __m128i* result = reinterpret_cast<__m128i*>(out);
    _mm_storeu_si128(result, t);
    return EXIT_SUCCESS;
}

int32_t aes_ni::decrypt(uint8_t out[16], const uint8_t in[16])
{
    __m128i *key = reinterpret_cast<__m128i*>(m_ks.get()) + (m_info.b[0] >> 4), t;

    if (m_info.b[0] != 10 * 16 && m_info.b[0] != 12 * 16 && m_info.b[0] != 14 * 16) {
        return EXIT_FAILURE;
    }

    t = _mm_xor_si128(_mm_loadu_si128(reinterpret_cast<const __m128i*>(in)), *reinterpret_cast<const __m128i*>(key));

    switch (m_info.b[0])
    {
    case 14 * 16:
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        // DO NOT BREAK!
    case 12 * 16:
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        // DO NOT BREAK!
    case 10 * 16:
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdec_si128(t, *reinterpret_cast<const __m128i*>(--key));
        t = _mm_aesdeclast_si128(t, *reinterpret_cast<const __m128i*>(--key));
    }

    _mm_storeu_si128(reinterpret_cast<__m128i_u*>(out), t);
    return EXIT_SUCCESS;
}


}  // namespace crypto
}  // namespace phantom


#ifdef __clang__
#pragma clang attribute pop
#else
#pragma GCC reset_options
#endif

#endif
