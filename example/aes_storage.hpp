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


namespace phantom {

/// An enumerated type for the keylength
enum aes_keylen_e {
    AES_128 = 44,
    AES_192 = 52,
    AES_256 = 60
};

namespace crypto {

/// A helper class used to store the key and associated parameters for AES
struct aes_storage : public aligned_base<DEFAULT_MEM_ALIGNMENT>
{
    /// A union used to access individual bytes within a 32-bit word
    union aes_info_u
    {   uint32_t l;
        uint8_t b[4];
    };

    std::unique_ptr<uint32_t[]> m_ks;
    aes_info_u                  m_info;
};


}  // namespace crypto
}  // namespace phantom

