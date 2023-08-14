/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#pragma once

#include <memory>
#include "./aes.hpp"


// The maximum threshold at which the CTR-DRBG is reseeded (2^31 blocks)
#define CTR_DRBG_MAX_RESEED  0x80000000

// The minimum threshold at which the CTR-DRBG is reseeded
// (i.e. 2^19 bits or 2^12 blocks)
#define CTR_DRBG_MIN_RESEED  0x00010000

// The size of the (CS)PRNG transfer buffer (MUST BE a factor of 64
// and at least 64)
#define CSPRNG_BUFFER_SIZE   64


namespace phantom {
namespace crypto {

/**
 * @ingroup random
 * @brief AES CTR-DRBG CSPRNG, a deterministic random bit generator defined by NIST SP 800-90A
 */
class aes_ctr_drbg : public aligned_base<DEFAULT_MEM_ALIGNMENT>
{
public:
    aes_ctr_drbg(uint32_t seed_period, csprng_entropy_cb cb);
    ~aes_ctr_drbg();

    /// Reset the CTR-DRBG to its initial state
    void reset();

    /// Reseed the CTR-DRBG, this is done automatically every seed_period calls
    /// to aes_prng_update()
    void reseed();

    /// Generate another 64 bytes of random data, reseeding if necessary
    bool update(uint8_t *bytes);

private:
    void internal_update();

    uint32_t        m_reseed_ctr;
    uint32_t        m_seed_period;
    uint32_t        m_counter;
    std::unique_ptr<aes_encrypt> m_ctx_aes;

    csprng_entropy_cb m_seed_cb;

    uint8_t m_key[32];
};

}  // namespace crypto
}  // namespace phantom
