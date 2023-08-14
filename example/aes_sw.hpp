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

#include "./aes_storage.hpp"


namespace phantom {
namespace crypto {


/// A software AES class
class aes_sw : public aes_storage
{
public:
    aes_sw();
    ~aes_sw();

    int32_t set_key(const uint8_t *key, aes_keylen_e key_len);
    int32_t encrypt(uint8_t out[16], const uint8_t in[16]);
    int32_t decrypt(uint8_t out[16], const uint8_t in[16]);

private:
    void key_expansion(const uint8_t* key);
    void add_round_key(uint8_t round, uint8_t *state);
    void sub_bytes(uint8_t *state);
    void shift_rows(uint8_t *state);
    void mix_columns(uint8_t *state);
    void inverse_sub_bytes(uint8_t *state);
    void inverse_shift_rows(uint8_t *state);
    void inverse_mix_columns(uint8_t *state);

    size_t  m_nk;
    size_t  m_nr;
    uint8_t m_round_key[240];

    alignas(DEFAULT_MEM_ALIGNMENT) static const uint8_t fwd_sbox[256];
    alignas(DEFAULT_MEM_ALIGNMENT) static const uint8_t rev_sbox[256];
    alignas(DEFAULT_MEM_ALIGNMENT) static const uint8_t rcon[11];
};

}  // namespace crypto
}  // namespace phantom

