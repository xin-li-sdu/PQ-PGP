/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#pragma once


#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include "./phantom_memory.hpp"
#include "./phantom.hpp"

#include "./aes_ni.hpp"
#include "./aes_sw.hpp"

#if defined(__x86_64__) && __has_include("cpuid.h")
#include <cpuid.h>
#endif


namespace phantom {
namespace crypto {


/**
 * @ingroup symmetric
 * @brief A templated base class for AES
 * 
 * @tparam T Class defining the selected AES functionality
 */
template<class T>
class aes
{
public:
    virtual ~aes() {}

    /// Method indicates if AES-NI is available
    bool has_aes_ni()
    {
#if defined(__x86_64__) && __has_include("cpuid.h")
        // Only 64-bit Intel and AMD CPU's support AES-NI
        static int32_t test = -1;
        if (test < 0) {
            uint32_t a, b, c, d;
            if (!__get_cpuid(1, &a, &b, &c, &d)) {
                test = 0;
            }
            else {
                test = (c & 0x2000000);
            }
        }
        return test != 0;
#else
        return false;
#endif
    }

    /// Create an object derived from aes with the required keylength
    static T* create(aes_keylen_e key_len)
    {
        return T::make(key_len);
    }

    /// A method to configure the AES keyspace - a pure virtual method that
    /// must be implemented by concrete derived classes
    virtual int32_t set_key(const uint8_t *key, aes_keylen_e key_len) = 0;

protected:
    aes_keylen_e m_key_len_max;
};


}  // namespace crypto


/// An AES encryption class that abstracts away the implementation
class aes_encrypt : public symmetric_key_ctx, crypto::aes<aes_encrypt>
{
public:
    static aes_encrypt* make(aes_keylen_e key_len);

    int32_t set_key(const uint8_t *key, aes_keylen_e key_len) override;
    virtual int32_t encrypt(uint8_t out[16], const uint8_t in[16]);

private:
    explicit aes_encrypt(aes_keylen_e key_len);

#if defined(__x86_64__)
    std::unique_ptr<crypto::aes_ni> m_aes_ni;
#endif
    std::unique_ptr<crypto::aes_sw> m_aes_sw;
};

/// An AES decryption class that abstracts away the implementation
class aes_decrypt : public symmetric_key_ctx, crypto::aes<aes_decrypt>
{
public:
    static aes_decrypt* make(aes_keylen_e key_len);

    int32_t set_key(const uint8_t *key, aes_keylen_e key_len) override;
    virtual int32_t decrypt(uint8_t out[16], const uint8_t in[16]);

private:
    explicit aes_decrypt(aes_keylen_e key_len);

#if defined(__x86_64__)
    std::unique_ptr<crypto::aes_ni> m_aes_ni;
#endif
    std::unique_ptr<crypto::aes_sw> m_aes_sw;
};

}  // namespace phantom

