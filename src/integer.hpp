#ifndef __BNL_INTEGER_HPP_
#define __BNL_INTEGER_HPP_

#include "bnl.hpp" // bnl::ulint, bnl::ulint_size, bnl::ldouble

#include <sstream> // std::stringstream, std::istream, std::ostream
#include <string>  // std::string
#include <cstdlib> // std::malloc, std::calloc, std::realloc, std::free


// Forward declaration of the bnl::integer class and bnl::div_t struct
namespace bnl {
    class integer;
    struct div_t;
}


// Arithmetic operators
const bnl::integer operator * (const bnl::integer &a, const bnl::integer &b);
const bnl::integer operator / (const bnl::integer &a, const bnl::integer &b);
const bnl::integer operator % (const bnl::integer &a, const bnl::integer &b);
const bnl::integer operator + (const bnl::integer &a, const bnl::integer &b);
const bnl::integer operator - (const bnl::integer &a, const bnl::integer &b);
const bnl::integer operator << (const bnl::integer &a, const bnl::integer &b);
const bnl::integer operator >> (const bnl::integer &a, const bnl::integer &b);

// Input and output
std::ostream &operator << (std::ostream &stream, const bnl::integer &n);
std::istream &operator >> (std::istream &stream, bnl::integer &n);

// Comparison and relational operators
bool operator > (const bnl::integer &a, const bnl::integer &b);
bool operator < (const bnl::integer &a, const bnl::integer &b);
bool operator >= (const bnl::integer &a, const bnl::integer &b);
bool operator <= (const bnl::integer &a, const bnl::integer &b);
bool operator == (const bnl::integer &a, const bnl::integer &b);
bool operator != (const bnl::integer &a, const bnl::integer &b);

// Bitwise operators
const bnl::integer operator & (const bnl::integer &a, const bnl::integer &b);
const bnl::integer operator | (const bnl::integer &a, const bnl::integer &b);
const bnl::integer operator ^ (const bnl::integer &a, const bnl::integer &b);

// Logic operators
bool operator && (const bnl::integer &a, const bnl::integer &b);
bool operator || (const bnl::integer &a, const bnl::integer &b);


// Big Numbers Library namespace
namespace bnl {
    // Helpful checker functions
    bool iseven(const bnl::integer &n);
    bool isneg(const bnl::integer &n);
    bool isodd(const bnl::integer &n);
    bool isone(const bnl::integer &n);
    bool ispos(const bnl::integer &n);
    bool iszero(const bnl::integer &n);

    // Helpful arithmetic functions
    const bnl::integer abs(const bnl::integer &n);
    const bnl::integer &max(const bnl::integer &a, const bnl::integer &b);
    const bnl::integer &min(const bnl::integer &a, const bnl::integer &b);
    const bnl::integer pow(const bnl::integer &a, const bnl::integer &b);

    // Other helpful functions
    std::string const str(const bnl::integer &n, const int &radix = 10);


    // Integer class
    class integer {
        private:
            // Attributes

            // Numeric data array
            bnl::ulint *data;

            // Numeric data array size
            std::size_t size;

            // Number sign
            bool sign;


            // Static methods

            // Compare and returns -1 if a < b, 0 if a == b, and 1 if a > b
            static int cmp(const bnl::integer &a, const bnl::integer &b);

            // Copy n data blocks of numeric data from source to destiny
            static void cpy(bnl::ulint *const dest, const bnl::ulint *const src, const std::size_t &n);

            // Returns whether the given character if the point character
            static bool isexp(const char &c);

            // Returns whether the given character if the point character
            static bool isdigit(const char &c);

            // Returns whether the given character if the point character
            static bool ispoint(const char &c);

            // Returns whether the given character if a sign character
            static bool issign(const char &c);


            // Constructors

            // Empty integer
            integer(const std::size_t &size, const bool &sign);


            // Methods

            // The minimum number precision
            std::size_t precision(bool *twos_pow = NULL) const;

            // Shrink numeric data
            void shrink();



        public:
            // Static constants

            // Numeric representation base
            static const bnl::ulint base;

            // Base bit mask
            static const bnl::ulint base_mask;

            // Zero
            static const bnl::integer zero;

            // Positive one
            static const bnl::integer one;


            // Static methods

            // Check if the std::string has a valid decimal representation and process it
            static const std::string checkstr(const std::string &str, bool *const sign = NULL);

            // Number presicion
            static std::size_t precision(const bnl::integer &n);

            // Integer division
            static const bnl::div_t div(const bnl::integer &a, const bnl::integer &b);


            // Constructors

            // Default constructor
            integer();

            // Copy constructor
            integer(const bnl::integer &n);

            // Constructor from std::string
            integer(const std::string &str);

            // Constructor from const char *
            integer(const char *const str);

            // Constructor from long double
            integer(const bnl::ldouble &n);


            // Methods

            // Returns whether the value is even
            friend bool iseven(const bnl::integer &n);

            // Returns whether the value is negative
            friend bool isneg(const bnl::integer &n);

            // Returns whether the value is odd
            friend bool isodd(const bnl::integer &n);

            // Returns whether the value equals to one, positive or negative
            friend bool isone(const bnl::integer &n);

            // Returns whether the value is positive
            friend bool ispos(const bnl::integer &n);

            // Returns whe
            friend bool iszero(const bnl::integer &n);


            // Returns the absolute value of a number
            friend const bnl::integer abs(const bnl::integer &n);

            // Returns largest number between two numbers
            friend const bnl::integer &max(const bnl::integer &a, const bnl::integer &b);

            // Returns smaller number between two numbers
            friend const bnl::integer &min(const bnl::integer &a, const bnl::integer &b);

            // Returns the given number raised to the given exponent
            friend const bnl::integer pow(const bnl::integer &a, const bnl::integer &b);


            // Get the string representation with the given radix
            friend const std::string str(const bnl::integer &n, const int &radix);


            // Operators overloading

            // Type conversion operators

            // To std::string
            operator std::string() const;


            // Increment and decrement operators

            // Postfix increment
            const bnl::integer operator ++ (int);

            // Postfix decrement
            const bnl::integer operator -- (int);

            // Prefix increment
            inline const bnl::integer &operator ++ () {
                return *this = *this + bnl::integer::one;
            }

            // Prefix decrement
            inline const bnl::integer &operator -- () {
                return *this = *this - bnl::integer::one;
            }


            // Unary operators

            // Integer promotion
            inline const bnl::integer &operator + () const {
                return *this;
            }

            // Additive inverse
            const bnl::integer operator - () const;

            // Logical NOT
            inline const bnl::integer operator ! () const {
                return bnl::iszero(*this) ? bnl::integer::one : bnl::integer::zero;
            }

            // Bitwise NOT (one's complement)
            inline const bnl::integer operator ~ () const {
                return -(*this + bnl::integer::one);
            }


            // Arithmetic operators

            // Multiplication
            friend const bnl::integer (::operator *) (const bnl::integer &a, const bnl::integer &b);

            // Division
            friend const bnl::integer (::operator /) (const bnl::integer &a, const bnl::integer &b);

            // Modulo
            friend const bnl::integer (::operator %) (const bnl::integer &a, const bnl::integer &b);

            // Addition
            friend const bnl::integer (::operator +) (const bnl::integer &a, const bnl::integer &b);

            // Subtraction
            friend const bnl::integer (::operator -) (const bnl::integer &a, const bnl::integer &b);

            // Left shift
            friend const bnl::integer (::operator <<) (const bnl::integer &a, const bnl::integer &b);

            // Right shift
            friend const bnl::integer (::operator >>) (const bnl::integer &a, const bnl::integer &b);


            // Input and output

            // Standard output
            friend std::ostream &::operator << (std::ostream &stream, const bnl::integer &n);

            // Standard input
            friend std::istream &::operator >> (std::istream &stream, bnl::integer &n);


            // Comparison and relational operators

            // Greater than
            friend bool ::operator > (const bnl::integer &a, const bnl::integer &b);

            // Less than
            friend bool ::operator < (const bnl::integer &a, const bnl::integer &b);

            // Greater than or equal to
            friend bool ::operator >= (const bnl::integer &a, const bnl::integer &b);

            // Less than or equal to
            friend bool ::operator <= (const bnl::integer &a, const bnl::integer &b);

            // Equal to
            friend bool ::operator == (const bnl::integer &a, const bnl::integer &b);

            // Not equal to
            friend bool ::operator != (const bnl::integer &a, const bnl::integer &b);


            // Bitwise operators

            // Bitwise AND
            friend const bnl::integer (::operator &) (const bnl::integer &a, const bnl::integer &b);

            // Bitwise OR (inclusive or)
            friend const bnl::integer (::operator |) (const bnl::integer &a, const bnl::integer &b);

            // Bitwise XOR (exclusive or)
            friend const bnl::integer (::operator ^) (const bnl::integer &a, const bnl::integer &b);


            // Logic operators

            // Logical AND
            friend bool ::operator && (const bnl::integer &a, const bnl::integer &b);

            // Logical OR
            friend bool ::operator || (const bnl::integer &a, const bnl::integer &b);


            // Assignment operators

            // Direct assignation
            bnl::integer &operator = (const bnl::integer &n);

            // Assignation by addition
            inline bnl::integer &operator += (const bnl::integer &n) {
                return *this = *this + n;
            }

            // Assignation by subtraction
            inline bnl::integer &operator -= (const bnl::integer &n) {
                return *this = *this - n;
            }

            // Assignation by multiplication
            inline bnl::integer &operator *= (const bnl::integer &n) {
                return *this = *this * n;
            }

            // Assignation by division
            inline bnl::integer &operator /= (const bnl::integer &n) {
                return *this = *this / n;
            }

            // Assignation by modulo
            inline bnl::integer &operator %= (const bnl::integer &n) {
                return *this = *this % n;
            }

            // Assignation by left shift
            inline bnl::integer &operator <<= (const bnl::integer &n) {
                return *this = *this << n;
            }

            // Assignation by right shift
            inline bnl::integer &operator >>= (const bnl::integer &n) {
                return *this = *this >> n;
            }

            // Assignation by bitwise AND
            inline bnl::integer &operator &= (const bnl::integer &n) {
                return *this = *this & n;
            }

            // Assignation by bitwise OR
            inline bnl::integer &operator |= (const bnl::integer &n) {
                return *this = *this | n;
            }

            // Assignation by bitwise XOR
            inline bnl::integer &operator ^= (const bnl::integer &n) {
                return *this = *this ^ n;
            }


            // Destructor

            // Integer destructor
            inline ~integer() {
                std::free(data);
                data = NULL;
                size = 0;
                sign = false;
            }
    };


    // Structure returned by bnl::integer::div
    struct div_t {
        // Attributes

        // Quotient of the integral division
        bnl::integer quot;

        // Remainder of the integral division
        bnl::integer rem;


        // Constructors

        // Initializator
        div_t(const bnl::integer &q = bnl::integer::zero, const bnl::integer &r = bnl::integer::zero) : quot(q), rem(r) {}
    };


    // Helpful checker functions
    inline bool iseven(const bnl::integer &n) {
        return !(n.data[0] & 1);
    }

    inline bool isneg(const bnl::integer &n) {
        return n.sign;
    }

    inline bool isodd(const bnl::integer &n) {
        return n.data[0] & 1;
    }

    inline bool isone(const bnl::integer &n) {
        return (n.size == 1) && (n.data[0] == 1);
    }

    inline bool ispos(const bnl::integer &n) {
        return !n.sign;
    }

    inline bool iszero(const bnl::integer &n) {
        return (n.size == 1) && (n.data[0] == 0);
    }

    // Helpful arithmetic functions
    inline const bnl::integer abs(const bnl::integer &n) {
        return n.sign ? -n : n;
    }

    inline const bnl::integer &max(const bnl::integer &a, const bnl::integer &b) {
        return a > b ? a : b;
    }

    inline const bnl::integer &min(const bnl::integer &a, const bnl::integer &b) {
        return a < b ? a : b;
    }
}


// Comparison and relational operators
inline bool operator >= (const bnl::integer &a, const bnl::integer &b) {
    return !(a < b);
}

inline bool operator <= (const bnl::integer &a, const bnl::integer &b) {
    return !(a > b);
}

inline bool operator != (const bnl::integer &a, const bnl::integer &b) {
    return !(a == b);
}


// Logic operators
inline bool operator && (const bnl::integer &a, const bnl::integer &b) {
    return (a != bnl::integer::zero) && (b != bnl::integer::zero);
}

inline bool operator || (const bnl::integer &a, const bnl::integer &b) {
    return (a != bnl::integer::zero) || (b != bnl::integer::zero);
}


#endif // __BNL_INTEGER_HPP_
