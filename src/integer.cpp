#include "integer.hpp"

#include <stdexcept> // std::invalid_argument
#include <iomanip>   // std::setfill, std::setw, std::setprecision


// Private structs

// Decimal std::string decimal representation information constructor
bnl::integer::decimalinfo::decimalinfo() : valid(false), int_sign(false), exp_sign(false), begin(0), point(0), exp(0), end(0) {}


// Private static constants

// Numeric representation base
const bnl::ulint bnl::integer::BASE = 0x0100000000;


// Public static constants

// Zero
const bnl::integer zero;

// One
const bnl::integer one = "1";


// Private static functions

// Check if the std::string is a valid decimal representation
const bnl::integer::decimalinfo bnl::integer::isdecimal(const std::string &str) {

}


// Public constructors

// Constructor from std::string
bnl::integer::integer(const std::string &str) : data(NULL), size(0), sign(false) {

}

// Constructor from long double
bnl::integer::integer(const bnl::ldouble &n) : data(NULL), size(0), sign(false) {
    // Check if is nan
    if (n != n) {
        static const char msg[] = "can't build bnl::integer from long double: is nan";
        throw std::invalid_argument(msg);
    }

    // Check if is positive infinite
    static const bnl::ldouble inf_pos = 1.0L / 0.0L;
    if (n == inf_pos) {
        static const char msg[] = "can't build bnl::integer from long double: is inf";
        throw std::invalid_argument(msg);
    }

    // Check if is negative infinite
    static const bnl::ldouble inf_neg = -1.0L / 0.0L;
    if (n < inf_neg) {
        static const char msg[] = "can't build bnl::integer from long double: is -inf";
        throw std::invalid_argument(msg);
    }

    // Cast to std::string and build bnl::integer
    std::stringstream stream;
    stream << std::fixed << std::setprecision(11451) << n;
    *this = bnl::integer(stream.str());
}

// Methods

// Get the string representation with the given radix
std::string const bnl::integer::str(const int &radix) const {

}


// Operators overloading

// Type conversion operators

// To std::string
bnl::integer::operator std::string() const {

}

// Arithmetic operators

// Multiplication
const bnl::integer operator * (const bnl::integer &a, const bnl::integer &b) {

}

// Division
const bnl::integer operator / (const bnl::integer &a, const bnl::integer &b) {

}

// Modulo
const bnl::integer operator % (const bnl::integer &a, const bnl::integer &b) {

}

// Addition
const bnl::integer operator + (const bnl::integer &a, const bnl::integer &b) {

}

// Subtraction
const bnl::integer operator - (const bnl::integer &a, const bnl::integer &b) {

}

// Left shift
const bnl::integer operator << (const bnl::integer &a, const bnl::integer &b) {

}

// Right shift
const bnl::integer operator >> (const bnl::integer &a, const bnl::integer &b) {

}


// Comparison and relational operators

// Greater than
bool operator > (const bnl::integer &a, const bnl::integer &b) {

}

// Less than
bool operator < (const bnl::integer &a, const bnl::integer &b) {

}


// Bitwise operators

// Bitwise AND
const bnl::integer operator & (const bnl::integer &a, const bnl::integer &b) {

}

// Bitwise OR (inclusive or)
const bnl::integer operator | (const bnl::integer &a, const bnl::integer &b) {

}

// Bitwise XOR (exclusive or)
const bnl::integer operator ^ (const bnl::integer &a, const bnl::integer &b) {

}