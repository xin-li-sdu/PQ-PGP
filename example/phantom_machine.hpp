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


#ifndef IS_BIG_ENDIAN
#define IS_BIG_ENDIAN           1    ///> byte 0 is most significant
#define IS_LITTLE_ENDIAN        0
#endif
#ifndef IS_LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN        0    ///> byte 0 is least significant
#define IS_BIG_ENDIAN           0
#endif

#if IS_BIG_ENDIAN == 1 && IS_LITTLE_ENDIAN == 1
#error "Error! System is defined as both little- and big-endian"
#endif
#if IS_BIG_ENDIAN == 0 && IS_LITTLE_ENDIAN == 0
#error "Error! System is defined as neither little- or big-endian"
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#define PLATFORM_BYTE_ORDER IS_BIG_ENDIAN
#else
#define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN
#endif



#define GET_UINT32_BE(n, b, i) {                          \
    (n) = (static_cast<uint32_t>((b)[(i)    ]) << 24) |   \
          (static_cast<uint32_t>((b)[(i) + 1]) << 16) |   \
          (static_cast<uint32_t>((b)[(i) + 2]) <<  8) |   \
          (static_cast<uint32_t>((b)[(i) + 3])); }

#define GET_UINT64_BE(n, b, i) {                          \
    (n) = (static_cast<uint64_t>((b)[(i)    ]) << 56) |   \
          (static_cast<uint64_t>((b)[(i) + 1]) << 48) |   \
          (static_cast<uint64_t>((b)[(i) + 2]) << 40) |   \
          (static_cast<uint64_t>((b)[(i) + 3]) << 32) |   \
          (static_cast<uint64_t>((b)[(i) + 4]) << 24) |   \
          (static_cast<uint64_t>((b)[(i) + 5]) << 16) |   \
          (static_cast<uint64_t>((b)[(i) + 6]) <<  8) |   \
          (static_cast<uint64_t>((b)[(i) + 7])); }

#define PUT_UINT32_BE(n, b, i) {                          \
    (b)[(i)    ] = static_cast<uint8_t>((n) >> 24);       \
    (b)[(i) + 1] = static_cast<uint8_t>((n) >> 16);       \
    (b)[(i) + 2] = static_cast<uint8_t>((n) >>  8);       \
    (b)[(i) + 3] = static_cast<uint8_t>((n)); }

#define PUT_UINT64_BE(n, b, i) {                          \
    (b)[(i)    ] = static_cast<uint8_t>((n) >> 56);       \
    (b)[(i) + 1] = static_cast<uint8_t>((n) >> 48);       \
    (b)[(i) + 2] = static_cast<uint8_t>((n) >> 40);       \
    (b)[(i) + 3] = static_cast<uint8_t>((n) >> 32);       \
    (b)[(i) + 4] = static_cast<uint8_t>((n) >> 24);       \
    (b)[(i) + 5] = static_cast<uint8_t>((n) >> 16);       \
    (b)[(i) + 6] = static_cast<uint8_t>((n) >>  8);       \
    (b)[(i) + 7] = static_cast<uint8_t>((n)); }


namespace phantom {

/**
 * @brief Static methods that return machine endianness and word size
 */
class machine
{
public:
    static const bool is_big_endian()
    {
        return PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN;
    }

    static const bool is_little_endian()
    {
        return PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN;
    }

    static const bool byte_order()
    {
        return PLATFORM_BYTE_ORDER;
    }

    static const bool bits_per_word()
    {
#if defined (__x86_64)
        return 64;
#else
        return 32;
#endif
    }

    static const bool log2_bits_per_word()
    {
#if defined (__x86_64)
        return 6;
#else
        return 5;
#endif
    }
};

}  // namespace phantom

