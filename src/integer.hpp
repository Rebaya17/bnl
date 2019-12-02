#ifndef __BNL_INTEGER_HPP_
#define __BNL_INTEGER_HPP_

#include "bnl.hpp" // bnl::ulint, bnl::ulint_size, bnl::ldouble

#include <sstream> // std::stringstream, std::istream, std::ostream
#include <string>  // std::string
#include <cstdlib> // std::malloc, std::calloc, std::realloc, std::free


// Forward declaration of the bnl::integer class
namespace bnl {
    class integer;
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
    const bnl::integer max(const bnl::integer &a, const bnl::integer &b);
    const bnl::integer min(const bnl::integer &a, const bnl::integer &b);
    const bnl::integer pow(const bnl::integer &a, const bnl::integer &b);


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


            // Static inline functions

            // Compare and returns -1 if a < b, 0 if a == b, and 1 if a > b
            static inline int cmp(const bnl::integer &a, const bnl::integer &b) {
                // Compare each data block from the most significative to the
                // least significative
                for (std::size_t i = a.size - 1; i < a.size; i--)
                    if (a.data[i] != b.data[i])
                        // Returns 1 if a < b, or -1 if a > b
                        return a.data[i] < b.data[i] ? -1 : 1;

                // Return 0 if the numbers are equal
                return 0;
            }

            // Copy n data blocks of numeric data from source to destiny
            static inline void cpy(bnl::ulint *const dest, const bnl::ulint *const src, const std::size_t &n) {
                // Copy each data block
                for (std::size_t i = 0; i < n; i++)
                    dest[i] = src[i];
            }

            // Returns whether the given character if the point character
            static inline bool isexp(const char &c) {
                return (c == 'e') || (c == 'E');
            }

            // Returns whether the given character if the point character
            static inline bool isdigit(const char &c) {
                return (c >= '0') && (c <= '9');
            }

            // Returns whether the given character if the point character
            static inline bool ispoint(const char &c) {
                return c == '.';
            }

            // Returns whether the given character if a sign character
            static inline bool issign(const char &c) {
                return (c == '-') || (c == '+');
            }



        public:
            // Static constants

            // Numeric representation base
            static const bnl::ulint base;

            // Zero
            static const bnl::integer zero;

            // Positive one
            static const bnl::integer one;


            // Static methods

            // Check if the std::string is a valid decimal representation and process it
            static const std::string checkstr(const std::string &str, bool *const sign = NULL);


            // Constructors

            // Default constructor
            inline integer() : data(static_cast<bnl::ulint *>(std::calloc(1, bnl::ulint_size))), size(1), sign(false) {}

            // Copy constructor
            inline integer(const bnl::integer &n) : data(static_cast<bnl::ulint *>(std::malloc(n.size * bnl::ulint_size))), size(n.size), sign(n.sign) {
                bnl::integer::cpy(data, n.data, size);
            }

            // Constructor from std::string
            integer(const std::string &str);

            // Constructor from const char *
            inline integer(const char *const str) : data(NULL), size(0), sign(false) {
                *this = bnl::integer(std::string(str));
            }

            // Constructor from long double
            integer(const bnl::ldouble &n);


            // Methods

            // Get the string representation with the given radix
            std::string const str(const int &radix = 10) const;


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
            friend const bnl::integer max(const bnl::integer &a, const bnl::integer &b);

            // Returns smaller number between two numbers
            friend const bnl::integer min(const bnl::integer &a, const bnl::integer &b);

            // Returns the given number raised to the given exponent
            friend const bnl::integer pow(const bnl::integer &a, const bnl::integer &b);


            // Operators overloading

            // Type conversion operators

            // To std::string
            operator std::string() const;


            // Increment and decrement operators

            // Postfix increment
            inline const bnl::integer operator ++ (int) {
                bnl::integer ans = *this;
                *this = *this + bnl::integer::one;
                return ans;
            }

            // Postfix decrement
            inline const bnl::integer operator -- (int) {
                bnl::integer ans = *this;
                *this = *this - bnl::integer::one;
                return ans;
            }

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
            inline const bnl::integer operator - () const {
                // Return the same number if is zero
                if (bnl::iszero(*this))
                    return *this;

                // Return copy with inversed sign
                bnl::integer ans = *this;
                ans.sign = !ans.sign;
                return ans;
            }

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
            inline bnl::integer &operator = (const bnl::integer &n) {
                // Copy attributes if is not the same number
                if ((this != &n) && (*this != n)) {
                    size = n.size;
                    sign = n.sign;
                    data = static_cast<bnl::ulint *>(std::realloc(data, size * bnl::ulint_size));
                    bnl::integer::cpy(data, n.data, size);
                }

                // Return the number
                return *this;
            }

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
                size = 0;
                sign = false;
            }
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

    inline const bnl::integer max(const bnl::integer &a, const bnl::integer &b) {
        return bnl::integer::cmp(a, b) == 1 ? b : a;
    }

    inline const bnl::integer min(const bnl::integer &a, const bnl::integer &b) {
        return bnl::integer::cmp(a, b) == 1 ? a : b;
    }

    inline const bnl::integer pow(const bnl::integer &a, const bnl::integer &b) {
        // Base case
        bnl::integer ans = bnl::integer::one;

        // Accumulate products
        for (bnl::integer i = bnl::integer::zero; i < b; i++)
            ans *= a;

        // Return the power
        return ans;
    }
}


// Input and output
inline std::ostream &operator << (std::ostream &stream, const bnl::integer &n) {
    stream << std::string(n);
    return stream;
}

inline std::istream &operator >> (std::istream &stream, bnl::integer &n) {
    // Read the integer
    std::string str;
    stream >> str;
    n = str;

    // Return the stream
    return stream;
}


// Comparison and relational operators
inline bool operator >= (const bnl::integer &a, const bnl::integer &b) {
    return !(a > b);
}

inline bool operator <= (const bnl::integer &a, const bnl::integer &b) {
    return !(a < b);
}

inline bool operator == (const bnl::integer &a, const bnl::integer &b) {
    // Same object
    if (&a == &b)
        return true;

    // Different signs
    if ((a.sign != b.sign) || (a.size != b.size))
        return false;

    // Full comparison
    return bnl::integer::cmp(a, b) == 0;
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
