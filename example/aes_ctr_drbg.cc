/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#include "./aes_ctr_drbg.hpp"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace phantom {
namespace crypto {

/// Create and initialise a CTR-DRBG state context
aes_ctr_drbg::aes_ctr_drbg(uint32_t seed_period, csprng_entropy_cb cb)
{
    if (nullptr == cb) {
        throw std::runtime_error("AES CTR-DRBG is using a NULL callback");
    }
    m_seed_cb = cb;

    // The seed_period relates to bytes, so convert to a reseed
    // block period and check the bounds
    seed_period >>= 4;
    if (seed_period > CTR_DRBG_MAX_RESEED) {
        seed_period = CTR_DRBG_MAX_RESEED;
    }
    else if (seed_period < CTR_DRBG_MIN_RESEED) {
        seed_period = CTR_DRBG_MIN_RESEED;
    }
    m_seed_period = (uint32_t) seed_period;

    // Reset the counter to zero
    m_counter = 0;

    // Generate new initial keys
    m_seed_cb(32, m_key);
    m_ctx_aes = std::unique_ptr<aes_encrypt>(crypto::aes<aes_encrypt>::create(AES_256));
    m_ctx_aes->set_key(m_key, AES_256);

    // Reseed the context
    reseed();
}

aes_ctr_drbg::~aes_ctr_drbg()
{
}

void aes_ctr_drbg::reset()
{
    // Reset the key and counter to zero
    m_counter = 0;
    memset(m_key, 0, 32);

    // Generate new initial keys
    m_ctx_aes->set_key(m_key, AES_256);

    // Reseed the context
    reseed();
}

void aes_ctr_drbg::internal_update()
{
    size_t seedlen = 4 + 32;
    size_t num_blocks = 3;
    uint8_t bytes[48];
    union u {
        uint8_t b[4];
        uint32_t w;
    };
    union u ctr;

    uint8_t data[16];

    ctr.w = ++m_counter;
    data[0]  = ctr.b[0];
    data[1]  = ctr.b[1];
    data[2]  = ctr.b[2];
    data[3]  = ctr.b[3];
    data[4]  = data[0];
    data[5]  = data[1];
    data[6]  = data[2];
    data[7]  = data[3];
    data[8]  = data[0];
    data[9]  = data[1];
    data[10] = data[2];
    data[11] = data[3];
    data[12] = data[0];
    data[13] = data[1];
    data[14] = data[2];
    data[15] = data[3];
    m_ctx_aes->encrypt(bytes + 32, data);
    ctr.w = ++m_counter;
    data[0]  = ctr.b[0];
    data[1]  = ctr.b[1];
    data[2]  = ctr.b[2];
    data[3]  = ctr.b[3];
    data[4]  = data[0];
    data[5]  = data[1];
    data[6]  = data[2];
    data[7]  = data[3];
    data[8]  = data[0];
    data[9]  = data[1];
    data[10] = data[2];
    data[11] = data[3];
    data[12] = data[0];
    data[13] = data[1];
    data[14] = data[2];
    data[15] = data[3];
    m_ctx_aes->encrypt(bytes + 16, data);
    ctr.w = ++m_counter;
    data[0]  = ctr.b[0];
    data[1]  = ctr.b[1];
    data[2]  = ctr.b[2];
    data[3]  = ctr.b[3];
    data[4]  = data[0];
    data[5]  = data[1];
    data[6]  = data[2];
    data[7]  = data[3];
    data[8]  = data[0];
    data[9]  = data[1];
    data[10] = data[2];
    data[11] = data[3];
    data[12] = data[0];
    data[13] = data[1];
    data[14] = data[2];
    data[15] = data[3];
    m_ctx_aes->encrypt(bytes, data);

    // XOR the counter and key with entropy if required
    m_seed_cb(4, ctr.b);
    m_seed_cb(32, m_key);
    m_key[ 0] ^= bytes[12];
    m_key[ 1] ^= bytes[13];
    m_key[ 2] ^= bytes[14];
    m_key[ 3] ^= bytes[15];
    m_key[ 4] ^= bytes[16];
    m_key[ 5] ^= bytes[17];
    m_key[ 6] ^= bytes[18];
    m_key[ 7] ^= bytes[19];
    m_key[ 8] ^= bytes[20];
    m_key[ 9] ^= bytes[21];
    m_key[10] ^= bytes[22];
    m_key[11] ^= bytes[23];
    m_key[12] ^= bytes[24];
    m_key[13] ^= bytes[25];
    m_key[14] ^= bytes[26];
    m_key[15] ^= bytes[27];
    m_key[16] ^= bytes[28];
    m_key[17] ^= bytes[29];
    m_key[18] ^= bytes[30];
    m_key[19] ^= bytes[31];
    m_key[20] ^= bytes[32];
    m_key[21] ^= bytes[33];
    m_key[22] ^= bytes[34];
    m_key[23] ^= bytes[35];
    m_key[24] ^= bytes[36];
    m_key[25] ^= bytes[37];
    m_key[26] ^= bytes[38];
    m_key[27] ^= bytes[39];
    m_key[28] ^= bytes[40];
    m_key[29] ^= bytes[41];
    m_key[30] ^= bytes[42];
    m_key[31] ^= bytes[43];
    m_counter ^= ctr.w;

    // Generate new keys
    m_ctx_aes->set_key(m_key, AES_256);
}

void aes_ctr_drbg::reseed()
{
    m_reseed_ctr = 0;
    internal_update();
}

bool aes_ctr_drbg::update(uint8_t *bytes)
{
    if (nullptr == bytes) {
        return false;
    }

    // Encrypt the counter using the specified key
    union u {
        uint8_t  b[16];
        uint32_t w[4];
    };
    for (size_t i = 0; i < CSPRNG_BUFFER_SIZE; i+=16) {
        // Increment the counter
        union u data;
        data.w[0] = data.w[1] = data.w[2] = data.w[3] = m_counter++;

        // Encrypt the block
        m_ctx_aes->encrypt(bytes + i, data.b);
    }

    // Increment the reseed counter
    m_reseed_ctr++;
    if (m_reseed_ctr >= m_seed_period) {
        reseed();
    }

    return true;
}

}  // namespace crypto
}  // namespace phantom
