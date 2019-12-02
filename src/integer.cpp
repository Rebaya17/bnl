#include "integer.hpp"

#include <stdexcept> // std::invalid_argument
#include <iomanip>   // std::setfill, std::setw, std::setprecision


// Private static constants

// Numeric representation base
const bnl::ulint bnl::integer::BASE = 0x0100000000;


// Public static constants

// Zero
const bnl::integer zero;

// One
const bnl::integer one = "1";


// Public static methods

// Check if the std::string is a valid decimal representation and process it
const std::string bnl::integer::checkstr(const std::string &str, bool *const sign) {
    // Decimal information
    const std::size_t end = str.size();
    std::size_t begin = 0;
    std::size_t point = std::string::npos;
    std::size_t exp = std::string::npos;
    bool has_point = false;
    bool has_sign = false;
    bool is_neg = false;
    bool valid = true;
    int state = 0;

    // Checker bucle
    for (std::size_t i = 0; valid && (i < end); i++) {
        // Current character
        const char c = str[i];

        switch (state) {
            case 3: // Integer part
                if (bnl::integer::isdigit(c)) break; // State 3
                if (bnl::integer::ispoint(c)) {state = 4; point = i; break;}
                if (bnl::integer::isexp(c))   {state = 5; exp   = i; break;}
                valid = false; break;

            case 4: // Decimal part
                has_point = static_cast<bool>(point);
                if (bnl::integer::isdigit(c)) break; // state = 4
                if (bnl::integer::isexp(c))   {state = 5; exp = i; break;}
                valid = false; break;

            case 7: // Exponent part
                if (bnl::integer::isdigit(c)) break; // state = 7
                valid = false; break;


            case 0: // Begin
                if (bnl::integer::issign(c))  {state = 1; is_neg = c == '-';    break;}
                if (bnl::integer::isdigit(c)) {state = 3;                       break;}
                if (bnl::integer::ispoint(c)) {state = 2; begin = 1; point = 0; break;}
                valid = false;
                break;

            case 1: // Sign found
                has_sign = true;
                if (bnl::integer::isdigit(c)) {state = 3; begin = 1;                 break;}
                if (bnl::integer::ispoint(c)) {state = 2; begin = 2; point = 1; break;}
                valid = false; break;

            case 2: // Decimal point found
                if (bnl::integer::isdigit(c)) {state = 4; break;}
                valid = false; break;

            case 5: // Exponent found
                if (bnl::integer::isdigit(c)) {state = 7; break;}
                if (bnl::integer::issign(c))  {state = 6; break;}
                valid = false; break;

            case 6: // Exponent sign found
                if (bnl::integer::isdigit(c)) {state = 7; break;}
                valid = false;
        }
    }

    // Set sign if variable is given
    if (sign != NULL)
        *sign = is_neg;

    // Invalid integer
    if (!valid && (state != 3) && (state != 4) && (state != 7))
        return std::string();


    // Fix indices
    if (exp   == std::string::npos) exp   = end;
    if (point == std::string::npos) point = exp;
    if (has_sign)                   point--;

    // Number without exponent
    std::string num = str.substr(begin, exp - begin);
    const std::size_t num_size = has_point ? num.size() - 1 : num.size();

    // Apply the decimal point shiftment
    std::size_t shifted_point = point;
    if (exp < end) {
        std::stringstream stream;
        std::size_t shiftment;

        // Cast exponent and shift the decimal point
        stream.str(str.substr(exp + 1));
        stream >> shiftment;
        shifted_point += shiftment;
    }

    // Zero or negative point index. No integer part
    static const std::size_t sign_bit = (sizeof(std::size_t) << 3) - 1;
    if (!shifted_point || (shifted_point >> sign_bit))
        num.clear();

    // Point idex less than number size. Remove decimals.
    else if (shifted_point < num_size) {
        // Point shifted to right
        if (has_point && (shifted_point > point)) {
            num.erase(shifted_point + 1);
            num.erase(point, 1);
        }

        // Without point or point shifted to left
        else num.erase(shifted_point);
    }

    // Point index greater than or equal to number size. Append zeros if is
    // necessary
    else {
        // Remove the decimal point if exists
        if (has_point)
            num.erase(point, 1);

        // Append zeros
        if (shifted_point > num_size)
            num.append(shifted_point - num_size, '0');
    }

    // Return the processed number
    return num.empty() ? "0" : num;
}


// Public constructors

// Constructor from std::string
bnl::integer::integer(const std::string &str) : data(NULL), size(0), sign(false) {
    // Process integer
    const std::string num = bnl::integer::checkstr(str, &sign);

    // Check if is valid
    if (num.empty()) {
        static const std::string msg = "can't build bnl::integer from std::string: invalid format";
        throw std::invalid_argument(msg);
    }


    // Dabble-Double, inverse of Double-Dabble to convert BDC to binary
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