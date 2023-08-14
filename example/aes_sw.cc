/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#include "./aes_sw.hpp"


namespace phantom {
namespace crypto {

/// Forward S-boxes
alignas(DEFAULT_MEM_ALIGNMENT) const uint8_t aes_sw::fwd_sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/// Reverse S-boxes
alignas(DEFAULT_MEM_ALIGNMENT) const uint8_t aes_sw::rev_sbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

/// x^(i-1) mod 0x11b
alignas(DEFAULT_MEM_ALIGNMENT) const uint8_t aes_sw::rcon[11] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};


#define XMULT(x)        ((x << 1) ^ (((x >> 7) & 1) * 0x1b))
#define MULT(x, y)                                      \
     ((((y>>0) & 1) * x) ^                              \
      (((y>>1) & 1) * XMULT(x)) ^                       \
      (((y>>2) & 1) * XMULT(XMULT(x))) ^                \
      (((y>>3) & 1) * XMULT(XMULT(XMULT(x)))) ^         \
      (((y>>4) & 1) * XMULT(XMULT(XMULT(XMULT(x))))))


aes_sw::aes_sw()
{
}

aes_sw::~aes_sw()
{
}

int32_t aes_sw::set_key(const uint8_t *key, aes_keylen_e key_len)
{
    switch (key_len)
    {
        case AES_128: m_nk = 4; m_nr = 10; key_expansion(key); break;
        case AES_192: m_nk = 6; m_nr = 12; key_expansion(key); break;
        case AES_256: m_nk = 8; m_nr = 14; key_expansion(key); break;
        default:      {};
    }

    return EXIT_SUCCESS;
}

int32_t aes_sw::encrypt(uint8_t out[16], const uint8_t in[16])
{
    if (in != out) {
        for (size_t i = 0; i < 16; i++) {
            out[i] = in[i];
        }
    }

    // The initial round adds the round key to the state
    add_round_key(0, out);

    // The Nr - 1 normal rounds
    for (size_t i = 1; i < m_nr; i++) {
        sub_bytes(out);
        shift_rows(out);
        mix_columns(out);
        add_round_key(i, out);
    }

    // The last round omits the MixColumns() function
    sub_bytes(out);
    shift_rows(out);
    add_round_key(m_nr, out);

    return EXIT_SUCCESS;
}

int32_t aes_sw::decrypt(uint8_t out[16], const uint8_t in[16])
{
    if (in != out) {
        for (size_t i = 0; i < 16; i++) {
            out[i] = in[i];
        }
    }

    // The initial round adds the round key to the state
    add_round_key(m_nr, out);

    // The Nr - 1 normal rounds
    for (size_t i = m_nr - 1; i > 0; i--) {
        inverse_shift_rows(out);
        inverse_sub_bytes(out);
        add_round_key(i, out);
        inverse_mix_columns(out);
    }

    // The last round omits the MixColumns() function
    inverse_shift_rows(out);
    inverse_sub_bytes(out);
    add_round_key(0, out);

    return EXIT_SUCCESS;
}

void aes_sw::key_expansion(const uint8_t* key)
{
    for (size_t i = 0; i < m_nk; i++) {
        m_round_key[4*i + 0] = key[4*i + 0];
        m_round_key[4*i + 1] = key[4*i + 1];
        m_round_key[4*i + 2] = key[4*i + 2];
        m_round_key[4*i + 3] = key[4*i + 3];
    }

    for (size_t i = m_nk; i < 4 * (m_nr + 1); i++) {
        uint8_t temp[4];

        size_t k = (i - 1) * 4;
        temp[0] = m_round_key[k + 0];
        temp[1] = m_round_key[k + 1];
        temp[2] = m_round_key[k + 2];
        temp[3] = m_round_key[k + 3];

        if (0 == (i % m_nk)) {
            // RotWord()
            uint8_t t;
            t       = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;

            // Subword()
            temp[0] = fwd_sbox[temp[0]];
            temp[1] = fwd_sbox[temp[1]];
            temp[2] = fwd_sbox[temp[2]];
            temp[3] = fwd_sbox[temp[3]];

            temp[0] = temp[0] ^ rcon[i/m_nk];
        }

        if (8 == m_nk && 4 == (i % m_nk)) {
            // Subword()
            temp[0] = fwd_sbox[temp[0]];
            temp[1] = fwd_sbox[temp[1]];
            temp[2] = fwd_sbox[temp[2]];
            temp[3] = fwd_sbox[temp[3]];
        }

        k = (i - m_nk) * 4;
        m_round_key[4*i + 0] = m_round_key[k + 0] ^ temp[0];
        m_round_key[4*i + 1] = m_round_key[k + 1] ^ temp[1];
        m_round_key[4*i + 2] = m_round_key[k + 2] ^ temp[2];
        m_round_key[4*i + 3] = m_round_key[k + 3] ^ temp[3];
    }
}

void aes_sw::add_round_key(uint8_t round, uint8_t *state)
{
    state[ 0] ^= m_round_key[(round * 4 * 4) + (0 * 4) + 0];
    state[ 1] ^= m_round_key[(round * 4 * 4) + (0 * 4) + 1];
    state[ 2] ^= m_round_key[(round * 4 * 4) + (0 * 4) + 2];
    state[ 3] ^= m_round_key[(round * 4 * 4) + (0 * 4) + 3];
    state[ 4] ^= m_round_key[(round * 4 * 4) + (1 * 4) + 0];
    state[ 5] ^= m_round_key[(round * 4 * 4) + (1 * 4) + 1];
    state[ 6] ^= m_round_key[(round * 4 * 4) + (1 * 4) + 2];
    state[ 7] ^= m_round_key[(round * 4 * 4) + (1 * 4) + 3];
    state[ 8] ^= m_round_key[(round * 4 * 4) + (2 * 4) + 0];
    state[ 9] ^= m_round_key[(round * 4 * 4) + (2 * 4) + 1];
    state[10] ^= m_round_key[(round * 4 * 4) + (2 * 4) + 2];
    state[11] ^= m_round_key[(round * 4 * 4) + (2 * 4) + 3];
    state[12] ^= m_round_key[(round * 4 * 4) + (3 * 4) + 0];
    state[13] ^= m_round_key[(round * 4 * 4) + (3 * 4) + 1];
    state[14] ^= m_round_key[(round * 4 * 4) + (3 * 4) + 2];
    state[15] ^= m_round_key[(round * 4 * 4) + (3 * 4) + 3];
}

void aes_sw::sub_bytes(uint8_t *state)
{
    state[ 0] = fwd_sbox[state[ 0]];
    state[ 1] = fwd_sbox[state[ 1]];
    state[ 2] = fwd_sbox[state[ 2]];
    state[ 3] = fwd_sbox[state[ 3]];
    state[ 4] = fwd_sbox[state[ 4]];
    state[ 5] = fwd_sbox[state[ 5]];
    state[ 6] = fwd_sbox[state[ 6]];
    state[ 7] = fwd_sbox[state[ 7]];
    state[ 8] = fwd_sbox[state[ 8]];
    state[ 9] = fwd_sbox[state[ 9]];
    state[10] = fwd_sbox[state[10]];
    state[11] = fwd_sbox[state[11]];
    state[12] = fwd_sbox[state[12]];
    state[13] = fwd_sbox[state[13]];
    state[14] = fwd_sbox[state[14]];
    state[15] = fwd_sbox[state[15]];
}

void aes_sw::shift_rows(uint8_t *state)
{
    uint8_t t;

    t         = state[1];
    state[1]  = state[5];
    state[5]  = state[9];
    state[9]  = state[13];
    state[13] = t;

    t         = state[2];
    state[2]  = state[10];
    state[10] = t;
    t         = state[6];
    state[6]  = state[14];
    state[14] = t;

    t         = state[3];
    state[3]  = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7]  = t;
}

void aes_sw::mix_columns(uint8_t *state)
{
    uint8_t a, b, t;

    t = state[0];
    a = state[0] ^ state[1] ^ state[2] ^ state[3];
    b = state[0] ^ state[1];
    b = XMULT(b);
    state[0] ^= b ^ a;
    b = state[1] ^ state[2];
    b = XMULT(b);
    state[1] ^= b ^ a;
    b = state[2] ^ state[3];
    b = XMULT(b);
    state[2] ^= b ^ a;
    b = state[3] ^ t;
    b = XMULT(b);
    state[3] ^= b ^ a;

    t = state[4];
    a = state[4] ^ state[5] ^ state[6] ^ state[7];
    b = state[4] ^ state[5];
    b = XMULT(b);
    state[4] ^= b ^ a;
    b = state[5] ^ state[6];
    b = XMULT(b);
    state[5] ^= b ^ a;
    b = state[6] ^ state[7];
    b = XMULT(b);
    state[6] ^= b ^ a;
    b = state[7] ^ t;
    b = XMULT(b);
    state[7] ^= b ^ a;

    t = state[8];
    a = state[8] ^ state[9] ^ state[10] ^ state[11];
    b = state[8] ^ state[9];
    b = XMULT(b);
    state[8] ^= b ^ a;
    b = state[9] ^ state[10];
    b = XMULT(b);
    state[9] ^= b ^ a;
    b = state[10] ^ state[11];
    b = XMULT(b);
    state[10] ^= b ^ a;
    b = state[11] ^ t;
    b = XMULT(b);
    state[11] ^= b ^ a;

    t = state[12];
    a = state[12] ^ state[13] ^ state[14] ^ state[15];
    b = state[12] ^ state[13];
    b = XMULT(b);
    state[12] ^= b ^ a;
    b = state[13] ^ state[14];
    b = XMULT(b);
    state[13] ^= b ^ a;
    b = state[14] ^ state[15];
    b = XMULT(b);
    state[14] ^= b ^ a;
    b = state[15] ^ t;
    b = XMULT(b);
    state[15] ^= b ^ a;
}

void aes_sw::inverse_sub_bytes(uint8_t *state)
{
    state[ 0] = rev_sbox[state[ 0]];
    state[ 1] = rev_sbox[state[ 1]];
    state[ 2] = rev_sbox[state[ 2]];
    state[ 3] = rev_sbox[state[ 3]];
    state[ 4] = rev_sbox[state[ 4]];
    state[ 5] = rev_sbox[state[ 5]];
    state[ 6] = rev_sbox[state[ 6]];
    state[ 7] = rev_sbox[state[ 7]];
    state[ 8] = rev_sbox[state[ 8]];
    state[ 9] = rev_sbox[state[ 9]];
    state[10] = rev_sbox[state[10]];
    state[11] = rev_sbox[state[11]];
    state[12] = rev_sbox[state[12]];
    state[13] = rev_sbox[state[13]];
    state[14] = rev_sbox[state[14]];
    state[15] = rev_sbox[state[15]];
}

void aes_sw::inverse_shift_rows(uint8_t *state)
{
    uint8_t t;

    t         = state[13];
    state[13] = state[9];
    state[9]  = state[5];
    state[5]  = state[1];
    state[1]  = t;

    t         = state[2];
    state[2]  = state[10];
    state[10] = t;
    t         = state[6];
    state[6]  = state[14];
    state[14] = t;

    t         = state[3];
    state[3]  = state[7];
    state[7]  = state[11];
    state[11] = state[15];
    state[15] = t;
}

void aes_sw::inverse_mix_columns(uint8_t *state)
{
    uint8_t a, b, c, d;

    a = state[ 0];
    b = state[ 1];
    c = state[ 2];
    d = state[ 3];
    state[ 0] = MULT(a, 0x0e) ^ MULT(b, 0x0b) ^ MULT(c, 0x0d) ^ MULT(d, 0x09);
    state[ 1] = MULT(a, 0x09) ^ MULT(b, 0x0e) ^ MULT(c, 0x0b) ^ MULT(d, 0x0d);
    state[ 2] = MULT(a, 0x0d) ^ MULT(b, 0x09) ^ MULT(c, 0x0e) ^ MULT(d, 0x0b);
    state[ 3] = MULT(a, 0x0b) ^ MULT(b, 0x0d) ^ MULT(c, 0x09) ^ MULT(d, 0x0e);
    a = state[ 4];
    b = state[ 5];
    c = state[ 6];
    d = state[ 7];
    state[ 4] = MULT(a, 0x0e) ^ MULT(b, 0x0b) ^ MULT(c, 0x0d) ^ MULT(d, 0x09);
    state[ 5] = MULT(a, 0x09) ^ MULT(b, 0x0e) ^ MULT(c, 0x0b) ^ MULT(d, 0x0d);
    state[ 6] = MULT(a, 0x0d) ^ MULT(b, 0x09) ^ MULT(c, 0x0e) ^ MULT(d, 0x0b);
    state[ 7] = MULT(a, 0x0b) ^ MULT(b, 0x0d) ^ MULT(c, 0x09) ^ MULT(d, 0x0e);
    a = state[ 8];
    b = state[ 9];
    c = state[10];
    d = state[11];
    state[ 8] = MULT(a, 0x0e) ^ MULT(b, 0x0b) ^ MULT(c, 0x0d) ^ MULT(d, 0x09);
    state[ 9] = MULT(a, 0x09) ^ MULT(b, 0x0e) ^ MULT(c, 0x0b) ^ MULT(d, 0x0d);
    state[10] = MULT(a, 0x0d) ^ MULT(b, 0x09) ^ MULT(c, 0x0e) ^ MULT(d, 0x0b);
    state[11] = MULT(a, 0x0b) ^ MULT(b, 0x0d) ^ MULT(c, 0x09) ^ MULT(d, 0x0e);
    a = state[12];
    b = state[13];
    c = state[14];
    d = state[15];
    state[12] = MULT(a, 0x0e) ^ MULT(b, 0x0b) ^ MULT(c, 0x0d) ^ MULT(d, 0x09);
    state[13] = MULT(a, 0x09) ^ MULT(b, 0x0e) ^ MULT(c, 0x0b) ^ MULT(d, 0x0d);
    state[14] = MULT(a, 0x0d) ^ MULT(b, 0x09) ^ MULT(c, 0x0e) ^ MULT(d, 0x0b);
    state[15] = MULT(a, 0x0b) ^ MULT(b, 0x0d) ^ MULT(c, 0x09) ^ MULT(d, 0x0e);
}


}  // namespace crypto
}  // namespace phantom
