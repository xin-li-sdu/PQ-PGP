/*****************************************************************************
 * Copyright (C) Neil Smyth 2020                                             *
 *                                                                           *
 * This file is part of phantom.                                             *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
//#include <cstdlib>
#include <cstring>
#include <memory>
#include<cstdlib>
#include <new>
#include <vector>
#include "./phantom_machine.hpp"
//#include"mall.h"
//#include<malloc.h>
/// Default byte alignment of memory
#define DEFAULT_MEM_ALIGNMENT   16


/// Restricted pointers and references
#if defined(__GNUG__)    // GCC or Clang
#define _RESTRICT_    __restrict__
#else
//#error "Error! restrict keyword is undefined"
#endif

namespace phantom {

/**
 * @brief Align (upwards) the provided 'data' pointer with regards to 'base'
 * so that the offset is a multiple of the size of T
 * 
 * @tparam T Data type used for alignment
 * @param base Base pointer
 * @param data Data pointer that is an offset from base
 * @return T* Aligned version of the data pointer
 */
template<typename T>
static T* align_ptr(void* base, void* data)
{
    uint8_t* cb = reinterpret_cast<uint8_t*>(base);
    uint8_t* cd = reinterpret_cast<uint8_t*>(data);
    size_t   k  = static_cast<size_t>(cd - cb);
    size_t   km = k % sizeof(T);
    if (km) {
        k += (sizeof(T)) - km;
    }
    return reinterpret_cast<T*>(cb + k);
}

/**
 * @brief Given the type and number of elements get the required aligned length
 * 
 * @tparam T Data type
 * @param n Length of array in bytes
 * @return size_t Bytes required at end of n bytes for alignment with T data type
 */
template<typename T>
static size_t align_on_type(size_t n)
{
    return ((n + sizeof(T) - 1) / sizeof(T)) * sizeof(T);
}

/**
 * @brief Dynamically align memory of the specified size
 * 
 * @param n Number of bytes required
 * @param alignment Alignment in bytes required
 * @return void* Aligned memory pointer
 */


static inline void* aligned_malloc(size_t n, size_t alignment = DEFAULT_MEM_ALIGNMENT)
{
    // Obtain aligned memory by over-allocation
//    void* p = aligned_alloc(alignment, n + alignment);
     void* p = _aligned_malloc(alignment + n, alignment);
     //printf("P：%p\n",p);

     //printf("q+16：%p\n",q+alignment);
    if (nullptr == p)
  {

        return nullptr;
 }
//    // Encode the memory alignment size into the start of unaligned memory
    uint32_t* p_u32 = static_cast<uint32_t*>(p);
    p_u32[0] = n + alignment;
    char* q=static_cast<char*>(p)+alignment;
    //printf("q：%p\n",q);
//    // Return a pointer to the aligned memory
   // return static_cast<char*>(p) + alignment;
    //return nullptr;
    return q;
   // return p;
}

/**
 * @brief Free aligned heap memory
 * 
 * @param p Pointer to memory
 * @param alignment Alignment used
 */
static inline void aligned_free(void* p, size_t alignment = DEFAULT_MEM_ALIGNMENT)
{
    // Remove the alignment offset to get the original aligned memory pointer
    void*     p_offset  = static_cast<char*>(p) - alignment;

    // Obtain a 32-bit pointer to memory to get the encoded memory allocation size (bytes)
    uint32_t* p_u32     = static_cast<uint32_t*>(p_offset);
    uint32_t  n         = p_u32[0];

    // Obtain an 8-bit pointer to the allocated memory
    uint8_t*  p_u8      = static_cast<uint8_t*>(p_offset);

    // Fill the memory with zeros to erase any sensitive data.
    // We avoid memset as it can be optimised away as we follow erasure with freeing memory resources.
    // We instead use std::fill as the portable C++ mechanism toachieve erasure.
    std::fill(p_u8, p_u8 + n, 0);
#ifndef NDEBUG
    for (size_t i = 0; i < n; i++) {
        assert(p_u8[i] == 0);
    }
#endif

    // Release the memory resources
    //std::free(p_offset);
    _aligned_free(p_offset);
}

/**
 * @brief A base class that enables alignment of derived classes that are
 * statically and dynamically allocated
 * 
 * @tparam ALIGNMENT Memory alignment
 */
template<size_t ALIGNMENT>
class alignas(ALIGNMENT) aligned_base
{
public:
    static_assert(ALIGNMENT > 0, "ALIGNMENT must be positive");
    static_assert((ALIGNMENT & (ALIGNMENT - 1)) == 0, "ALIGNMENT must be a power of 2)");
    static_assert((ALIGNMENT & sizeof(void*)) == 0, "ALIGNMENT must be a multiple of sizeof(void*)");
    static void* operator new(size_t count) { return aligned_malloc(count, ALIGNMENT); }
    static void* operator new[](size_t count) { return aligned_malloc(count, ALIGNMENT); }
    static void operator delete(void* ptr) { return aligned_free(ptr, ALIGNMENT); }
    static void operator delete[](void* ptr) { return aligned_free(ptr, ALIGNMENT); }
};

/**
 * @brief A memory allocation class used to provide custom allocation for STL template types
 * 
 * @tparam T Data type
 * @tparam ALIGNMENT Memory alignment
 */
template<class T, size_t ALIGNMENT = DEFAULT_MEM_ALIGNMENT>
class aligned_allocator
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    /// Constructor
    aligned_allocator() {}

    /// Copy constructor
    template<class U> aligned_allocator(const aligned_allocator<U>&) {}

    /// Return the memory pointer to the input object passed by reference
    T* address(T& x) const
    {
        return &x;
    }

    /// Return the const memory pointer to the input object passed by const reference
    const T* address(const T& x) const
    {
        return &x;
    }

    /// Obtain the maximum size that can be allocated
    size_t max_size() const
    {
        return static_cast<size_t>(-1) / sizeof(T);
    }

    /// Obtain an aligned memory pointer with sufficient memory for n elements of T objects
    T* allocate(size_t n)
    {
        T* p = reinterpret_cast<T*>(aligned_malloc(n * sizeof(T), ALIGNMENT));
        if (!p) {
            throw std::bad_alloc();
        }
        return p;
    }

    /// Deallocate an aligned memory pointer
    void deallocate(T* p, size_t n)
    {
        aligned_free(p, ALIGNMENT);
    }

    /// A classical rebind member in a default allocator class
    template<class U> struct rebind
    {
        using other = aligned_allocator<U>;
    };
};

template <class T>
bool operator==(const aligned_allocator<T>&, const aligned_allocator<T>&) { return true; }

template <class T>
bool operator!=(const aligned_allocator<T>&, const aligned_allocator<T>&) { return false; }

/**
 * @brief A vector class that erases memory when destroyed
 * 
 * @tparam T Data type
 */
template <class T>
using phantom_vector = std::vector<T, aligned_allocator<T>>;

}  // namespace phantom
