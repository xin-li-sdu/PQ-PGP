/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#include "./aes.hpp"
#include "./aes_ni.hpp"
#include "./aes_sw.hpp"

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


aes_encrypt::aes_encrypt(aes_keylen_e key_len)
{
    m_key_len_max = key_len;
#if defined(__x86_64__)
    if (has_aes_ni()) {
        m_aes_ni = std::unique_ptr<crypto::aes_ni>(new crypto::aes_ni(key_len));
    }
    else
#endif
    {
        m_aes_sw = std::unique_ptr<crypto::aes_sw>(new crypto::aes_sw());
    }
}

aes_encrypt* aes_encrypt::make(aes_keylen_e key_len)
{
    return new aes_encrypt(key_len);
}

int32_t aes_encrypt::set_key(const uint8_t *key, aes_keylen_e key_len)
{
    if (key_len > m_key_len_max || 0 == key) {
        return EXIT_FAILURE;
    }

#if defined(__x86_64__)
    if (m_aes_ni.get()) {
        if (AES_128 == key_len) {
            m_aes_ni->encrypt_key_128(key);
        }
        else if (AES_192 == key_len) {
            m_aes_ni->encrypt_key_192(key);
        }
        else if (AES_256 == key_len) {
            m_aes_ni->encrypt_key_256(key);
        }
    }
    else
#endif
    {
        m_aes_sw->set_key(key, key_len);
    }

    return EXIT_SUCCESS;
}

int32_t aes_encrypt::encrypt(uint8_t out[16], const uint8_t in[16])
{
#if defined(__x86_64__)
    if (m_aes_ni.get()) {
        return m_aes_ni->encrypt(out, in);
    }
    else
#endif
    {
        return m_aes_sw->encrypt(out, in);
    }
    return EXIT_FAILURE;
}


aes_decrypt::aes_decrypt(aes_keylen_e key_len)
{
    m_key_len_max = key_len;
#if defined(__x86_64__)
    if (has_aes_ni()) {
        m_aes_ni = std::unique_ptr<crypto::aes_ni>(new crypto::aes_ni(key_len));
    }
    else
#endif
    {
        m_aes_sw = std::unique_ptr<crypto::aes_sw>(new crypto::aes_sw());
    }
}

aes_decrypt* aes_decrypt::make(aes_keylen_e key_len)
{
    return new aes_decrypt(key_len);
}

int32_t aes_decrypt::set_key(const uint8_t *key, aes_keylen_e key_len)
{
    if (key_len > m_key_len_max || 0 == key) {
        return EXIT_FAILURE;
    }

#if defined(__x86_64__)
    if (m_aes_ni.get()) {
        if (AES_128 == key_len) {
            m_aes_ni->decrypt_key_128(key);
        }
        else if (AES_192 == key_len) {
            m_aes_ni->decrypt_key_192(key);
        }
        else if (AES_256 == key_len) {
            m_aes_ni->decrypt_key_256(key);
        }
    }
    else
#endif
    {
        m_aes_sw->set_key(key, key_len);
    }

    return EXIT_SUCCESS;
}

int32_t aes_decrypt::decrypt(uint8_t out[16], const uint8_t in[16])
{
#if defined(__x86_64__)
    if (m_aes_ni.get()) {
        return m_aes_ni->decrypt(out, in);
    }
    else
#endif
    {
        return m_aes_sw->decrypt(out, in);
    }
    return EXIT_FAILURE;
}

}  // namespace phantom


#ifdef __clang__
#pragma clang attribute pop
#else
#pragma GCC reset_options
#endif
