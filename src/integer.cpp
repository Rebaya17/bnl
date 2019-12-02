#include "integer.hpp"

#include <stdexcept> // std::invalid_argument
#include <iomanip>   // std::setfill, std::setw, std::setprecision


// Private structs

// Decimal std::string decimal representation information constructor
bnl::integer::decimalinfo::decimalinfo() : valid(false), int_sign(false), begin(0), point(std::string::npos), exp(std::string::npos), end(0) {}


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
    // Decimal information
    bnl::integer::decimalinfo info;
    info.end = str.size();

    // Checker bucle
    bool has_sign = false;
    int state = 0;
    for (std::size_t i = 0; i < info.end; i++) {
        // Current character
        const char c = str[i];

        switch (state) {
            case 3: // Integer part
                if (bnl::integer::isdigit(c)) break; // State 3
                if (bnl::integer::ispoint(c)) {state = 4; info.point = i; break;}
                if (bnl::integer::isexp(c))   {state = 5; info.exp   = i; break;}
                return info;

            case 4: // Decimal part
                if (bnl::integer::isdigit(c)) break; // state = 4
                if (bnl::integer::isexp(c))   {state = 5; info.exp   = i; break;}
                return info;

            case 7: // Exponent part
                if (bnl::integer::isdigit(c)) break; // state = 7
                return info;


            case 0: // Begin
                if (bnl::integer::issign(c))  {state = 1; info.int_sign = c == '-';       break;}
                if (bnl::integer::isdigit(c)) {state = 3; info.begin = 0;                 break;}
                if (bnl::integer::ispoint(c)) {state = 2; info.begin = 1; info.point = 0; break;}
                return info;

            case 1: // Sign found
                has_sign = true;
                if (bnl::integer::isdigit(c)) {state = 3; info.begin = 1;                 break;}
                if (bnl::integer::ispoint(c)) {state = 2; info.begin = 2; info.point = 1; break;}
                return info;

            case 2: // Decimal point found
                if (bnl::integer::isdigit(c)) {state = 4; break;}
                return info;

            case 5: // Exponent found
                if (bnl::integer::isdigit(c)) {state = 7; break;}
                if (bnl::integer::issign(c))  {state = 6; break;}
                return info;

            case 6: // Exponent sign found
                if (bnl::integer::isdigit(c)) {state = 7; break;}
                return info;
        }
    }

    // Check if is valid
    info.valid = (state == 3) || (state == 4) || (state == 7);

    // Fix indices
    if (info.exp   == std::string::npos) info.exp   = info.end;
    if (info.point == std::string::npos) info.point = info.exp;
    if (has_sign)                        info.point--;

    // Return the information
    return info;
}


// Public constructors

// Constructor from std::string
bnl::integer::integer(const std::string &str) : data(NULL), size(0), sign(false) {
    // Validate integer
    const bnl::integer::decimalinfo info = bnl::integer::isdecimal(str);

    // Invalid integer
    if (!info.valid) {
        static const std::string msg = "can't build bnl::integer from std::string: invalid format";
        throw std::invalid_argument(msg);
    }

    // Number without exponent
    std::string num = str.substr(info.begin, info.exp - info.begin);
    const bool has_point = info.point && (info.point < info.exp);
    const std::size_t num_size = has_point ? num.size() - 1 : num.size();

    // Apply the decimal point shiftment
    std::size_t point = info.point;
    if (info.exp < info.end) {
        std::stringstream stream;
        std::size_t shiftment;

        // Cast exponent and shift the decimal point
        stream.str(str.substr(info.exp + 1));
        stream >> shiftment;
        point += shiftment;
    }

    // Apply decimal point shiftment

    // Zero or negative point index. No integer part
    static const std::size_t sign_bit = (sizeof(std::size_t) << 3) - 1;
    if ((point == 0) || (point >> sign_bit))
        num.clear();

    // Point idex less than number size. Remove decimals.
    else if (point < num_size) {
        // Point shifted to right
        if (has_point && (point > info.point)) {
            num.erase(point + 1);
            num.erase(info.point, 1);
        }

        // Without point or point shifted to left
        else num.erase(point);
    }

    // Point index greater than or equal to number size. Append zeros if is
    // necessary
    else {
        // Remove the decimal point if exists
        if (has_point)
            num.erase(info.point, 1);

        // Append zeros
        if (point > num_size)
            num.append(point - num.size(), '0');
    }

    // Set to zero if is empty
    if (num.empty())
        num = "0";
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