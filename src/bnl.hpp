#ifndef __BNL_HPP_
#define __BNL_HPP_

#include <cstddef> // std::size_t


// Big Numbers Library namespace
namespace bnl {
    // Type definitions

    // Unsigned char assumed to be 1 bytes wide
    typedef unsigned char uchar;

    // Unsigned long int assumed to be 8 bytes wide
    typedef unsigned long int ulint;

    // Long double assumed to be 16 bytes wide
    typedef long double ldouble;


    // Type sizes

    // Unsigned long int size
    const std::size_t uchar_size = sizeof(bnl::uchar);

    // Unsigned long int size
    const std::size_t ulint_size = sizeof(bnl::ulint);

    // Unsigned long int size
    const std::size_t ldouble_size = sizeof(bnl::ldouble);
}

#endif // __BNL_HPP_