/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#include "./csprng.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>

namespace phantom {
namespace crypto {

csprng_buffer::csprng_buffer(aes_ctr_drbg* aes_ctr_drbg)
{
    m_rng_cnt      = (CSPRNG_BUFFER_SIZE/sizeof(uint32_t));
    m_aes_ctr_drbg = aes_ctr_drbg;
}

csprng_buffer::~csprng_buffer()
{
}

uint32_t csprng_buffer::get_u32()
{
    if ((CSPRNG_BUFFER_SIZE/sizeof(uint32_t)) == m_rng_cnt) {
        m_rng_cnt = 0;
        m_aes_ctr_drbg->update(m_rng_buffer.b);
    }

    return m_rng_buffer.w32[m_rng_cnt++];
}

uint64_t csprng_buffer::get_u64()
{
    if ((CSPRNG_BUFFER_SIZE/sizeof(uint64_t)) == m_rng_cnt) {
        m_rng_cnt = 0;
        m_aes_ctr_drbg->update(m_rng_buffer.b);
    }

    return m_rng_buffer.w64[m_rng_cnt++];
}

}  // namespace crypto


csprng::csprng(size_t seed_period, csprng_entropy_cb cb)
{
    m_aes_ctr_drbg = std::unique_ptr<crypto::aes_ctr_drbg>(new crypto::aes_ctr_drbg(seed_period, cb));
    m_buffer       = std::unique_ptr<crypto::csprng_buffer>(new crypto::csprng_buffer(m_aes_ctr_drbg.get()));
    m_seed_period  = seed_period;
    m_bits         = 0;
    m_wr_idx       = 0;
    m_rd_idx       = 0;
    m_var_bits     = 0;

    update_pool();
}

csprng::~csprng()
{
}

csprng* csprng::make(size_t seed_period, csprng_entropy_cb cb)
{
    try {
        auto obj = new csprng(seed_period, cb);
        return obj;
    }
    catch (std::runtime_error const&) {
        return nullptr;
    }
}

void csprng::update_pool()
{
    if (0 == m_bits) {
#if 0
        size_t num_reads = RANDOM_POOL_MAXBITS >> 6;
        size_t new_bits = RANDOM_POOL_MAXBITS;
        m_rd_idx = 0;
        m_wr_idx = 0;
        while (num_reads--) {
            uint64_t data = m_buffer->get_u64();
            m_random_pool[m_wr_idx++] = (uint32_t)(data >> 32);
            m_random_pool[m_wr_idx++] = (uint32_t)(data & 0xffffffff);
        }
#else
        size_t num_reads = RANDOM_POOL_MAXBITS >> 5;
        size_t new_bits = RANDOM_POOL_MAXBITS;
        m_rd_idx = 0;
        m_wr_idx = 0;
        while (num_reads--) {
            m_random_pool[m_wr_idx++] = m_buffer->get_u32();
        }
#endif
        m_bits = new_bits;
    }
}

void csprng::decrease_pool_bits()
{
    m_bits -= 32;
    m_rd_idx++;
    if (m_rd_idx >= RANDOM_POOL_SIZE) {
        m_rd_idx = 0;
    }
}

bool csprng::get_bit()
{
    return get_bits(1);
}

uint32_t csprng::get_bits(size_t n)
{
    uint32_t retval, mask;

    // To prevent shifting out of bounds ...
    if (n >= 32) {
        n = 32;
        mask = 0xFFFFFFFF;
    }
    else {
        mask = (1 << n) - 1;
    }

    // If too few bits are available, re-populate the
    // variable buffer to obtain the outstanding data
    retval = m_var_buf;
    if (m_var_bits < n) {
        size_t bits = n - m_var_bits;
        retval <<= bits;
        m_var_buf = get_u32();
        retval |= m_var_buf & ((1 << bits) - 1);
        m_var_buf >>= bits;
        m_var_bits = 32 - bits;
    }
    else {
        m_var_buf >>= n;
        m_var_bits -= n;
    }

    return retval & mask;
}

uint8_t csprng::get_u8()
{
    return get_bits(8);
}

uint16_t csprng::get_u16()
{
    return get_bits(16);
}

uint32_t csprng::get_u32()
{
    uint32_t u32;
    update_pool();
    u32 = m_random_pool[m_rd_idx];
    decrease_pool_bits();
    return u32;
}

uint64_t csprng::get_u64()
{
    uint64_t u64;
    update_pool();
    u64  = static_cast<uint64_t>(m_random_pool[m_rd_idx]) << 32;
    decrease_pool_bits();
    update_pool();
    u64 |= m_random_pool[m_rd_idx];
    decrease_pool_bits();
    return u64;
}

float csprng::get_float()
{
    return static_cast<float>(get_u32()) / UINT32_MAX;
}

double csprng::get_double()
{
    // Constants used to convert 53 random bits into a random double
    static const double rnd_dbl_den_scale = 1.11022302462516e-16;   // 1.0/9007199254740992.0
    static const double rnd_dbl_num_scale = 67108864.0;

    uint32_t a = get_bits(27);
    uint32_t b = get_bits(26);
    return (a * rnd_dbl_num_scale + b) * rnd_dbl_den_scale;
}

void csprng::get_mem(uint8_t* destination, size_t len)
{
    uint8_t *p = destination;

    if (len < 64) {
        while (len--) {
            *p++ = get_u8();
        }
        return;
    }

    union alignas(DEFAULT_MEM_ALIGNMENT) u {
        uint64_t u64[8];
        uint32_t u32[16];
        uint8_t  u8[64];
    };

    int32_t num_blocks = (len + 63) >> 6;
    union u data;
    while (num_blocks--) {
        data.u32[0]  = get_u32();
        data.u32[1]  = get_u32();
        data.u32[2]  = get_u32();
        data.u32[3]  = get_u32();
        data.u32[4]  = get_u32();
        data.u32[5]  = get_u32();
        data.u32[6]  = get_u32();
        data.u32[7]  = get_u32();
        data.u32[8]  = get_u32();
        data.u32[9]  = get_u32();
        data.u32[10] = get_u32();
        data.u32[11] = get_u32();
        data.u32[12] = get_u32();
        data.u32[13] = get_u32();
        data.u32[14] = get_u32();
        data.u32[15] = get_u32();
        std::copy(&data.u8[0], &data.u8[(len >= 64)? 64 : len], p);
        len -= 64;
        p   += 64;
    }
}

}  // namespace phantom
