#ifndef __BNL_HPP_
#define __BNL_HPP_

#include <cstdint> // std::uint8_t, std::uint32_t, std::uint64_t
#include <cstddef> // std::size_t


// Big Numbers Library namespace
namespace bnl {
    // Type definitions

    // Unsigned integer of 1 bytes wide
    typedef std::uint8_t uchar;

    // Unsigned integer of 4 bytes wide
    typedef std::uint32_t uint;

    // Unsigned integer of 8 bytes wide
    typedef std::uint64_t ulint;

    // Long double assumed to be 16 bytes wide
    typedef long double ldouble;


    // Type sizes

    // Unsigned long int size
    const std::size_t uchar_size = sizeof(bnl::uchar);

    // Unsigned int size
    const std::size_t uint_size = sizeof(bnl::uint);

    // Unsigned long int size
    const std::size_t ulint_size = sizeof(bnl::ulint);

    // Unsigned long int size
    const std::size_t ldouble_size = sizeof(bnl::ldouble);
}

#endif // __BNL_HPP_