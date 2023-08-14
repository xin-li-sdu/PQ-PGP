/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>
#include <new>
#include <string>
#include <vector>

#include "./phantom_types.hpp"

/**
 * @brief Phantom namespace for library-wide scope
 * 
 * This namespace defines all public APIs for the phantom library.
 */
namespace phantom {

namespace crypto {

class aes_ctr_drbg;
class csprng_buffer;

}  // namespace crypto

/// @}

/**
 * @ingroup random
 * @brief CSPRNG implementation
 * 
 * An AES-CTR-DRBG based cryptographically secure PRNG.
 */
class csprng : public aligned_base<DEFAULT_MEM_ALIGNMENT>
{
public:
    ~csprng();

    /// @brief Create a CSPRNG object based on the user's selected options
    /// @param seed_period The number of random bytes generated between re-seeding
    /// @param cb A callback used to periodically re-seed the CSPRNG
    /// @return The FPE object
    static csprng* make(size_t seed_period, csprng_entropy_cb cb);

    /// @brief Generate 0 to 32 bits
    /// @param m Number of bits
    /// @return A 32-bit random word, most significant bits padded with 0
    uint32_t get_bits(size_t n);

    /// @brief Generate a specified byte array
    /// @param destination Array pointer to write the random bytes to
    /// @param len Number of bytes to write
    void get_mem(uint8_t* destination, size_t len);

    /// @brief Generate a random boolean - templated
    /// @return A random integer, most significant bits padded with 0
    template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type* = nullptr>
    T get() { return get_bit(); }

    /// @brief Generate a random double value - templated
    /// @return A random floting point number
    template<typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type* = nullptr>
    T get() { return get_double(); }

    /// @brief Generate a random boolean
    bool get_bit();

    /// @brief Generate a random 8-bit integer value
    uint8_t get_u8();

    /// @brief Generate a random 16-bit integer value
    uint16_t get_u16();

    /// @brief Generate a random 32-bit integer value
    uint32_t get_u32();

    /// @brief Generate a random 64-bit integer value
    uint64_t get_u64();

    /// @brief Generate a random float value
    float get_float();

    /// @brief Generate a random double value
    double get_double();

private:
    csprng(size_t seed_period, csprng_entropy_cb cb);

    /// Update the buffered random pool
    void update_pool();

    /// Remove 32 bits from the pool and increment the pool read index
    void decrease_pool_bits();

    // A buffer used to maintain random bits for output
    alignas(DEFAULT_MEM_ALIGNMENT) uint32_t  m_random_pool[RANDOM_POOL_SIZE];
    int32_t   m_bits;
    int32_t   m_wr_idx;
    int32_t   m_rd_idx;

    // A buffer used to store bits for the prng_var function
    uint32_t  m_var_buf;
    size_t    m_var_bits;

    // The number of random bytes to be produced before the CSPRNG
    // is reseeded
    size_t    m_seed_period;

    // The underlying CSPRNG - AES-CTR-DRBG
    std::unique_ptr<crypto::aes_ctr_drbg> m_aes_ctr_drbg;

    /// A buffer used to store the AES-CTR-DRBG output
    std::unique_ptr<crypto::csprng_buffer> m_buffer;
};

/// Template specialization for the get methods of csprng
/// @{
template <>
inline uint8_t csprng::get<uint8_t>() { return get_u8(); }
template <>
inline uint16_t csprng::get<uint16_t>() { return get_u16(); }
template <>
inline uint32_t csprng::get<uint32_t>() { return get_u32(); }
template <>
inline uint64_t csprng::get<uint64_t>() { return get_u64(); }
template <>
inline float csprng::get<float>() { return get_float(); }
/// @}


/** 
 * @brief Symmetric Key context
 */
class symmetric_key_ctx
{
public:
    virtual ~symmetric_key_ctx() {}

    /// Setter method to define the cipher type and key length
    void set_keylen(symmetric_key_type_e type)
    {
        m_type = type;
    }

    /// Getter method to retrieve the cipher type and key length
    symmetric_key_type_e get_keylen()
    {
        return m_type;
    }

protected:
    symmetric_key_type_e m_type;
};

}  // namespace phantom
