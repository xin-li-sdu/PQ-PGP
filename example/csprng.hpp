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
#include <memory>
#include <vector>

#include "./phantom_types.hpp"
#include "./aes_ctr_drbg.hpp"

/// A bucket of random bits
#define RANDOM_POOL_SIZE        64

/// A mask used to wrap around the pool index
#define RANDOM_POOL_SIZE_MASK   0x3f

/// The size of the bitpool in bits
#define RANDOM_POOL_MAXBITS     (32 * RANDOM_POOL_SIZE)


namespace phantom {
namespace crypto {

/**
 * @defgroup random Random Number Generation
 * 
 * All classes, types, etc. within this group are focused on the generation of cryptographically
 * secure random numbers suitable for use within the various schemes offered by phantom.
 */

/**
 * @ingroup random
 * @brief CSPRNG buffer.
 * A buffer for random data associated with a CSPRNG.
 */
class csprng_buffer : public aligned_base<DEFAULT_MEM_ALIGNMENT>
{
public:
    explicit csprng_buffer(aes_ctr_drbg* aes_ctr_drbg);
    ~csprng_buffer();
    uint32_t get_u32();
    uint64_t get_u64();
private:
    size_t m_rng_cnt;

    union u {
        uint8_t  b[CSPRNG_BUFFER_SIZE];
        uint32_t w32[CSPRNG_BUFFER_SIZE/sizeof(uint32_t)];
        uint64_t w64[CSPRNG_BUFFER_SIZE/sizeof(uint64_t)];
    };
    alignas(DEFAULT_MEM_ALIGNMENT) u m_rng_buffer;

    aes_ctr_drbg* m_aes_ctr_drbg;
};


}  // namespace crypto
}  // namespace phantom
