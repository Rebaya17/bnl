#include "integer.hpp"

#include <stdexcept> // std::invalid_argument
#include <iomanip>   // std::setfill, std::setw, std::setprecision


// Public static constants

// Numeric representation base
const bnl::ulint bnl::integer::base = 0x0100000000;

// Zero
const bnl::integer bnl::integer::zero;

// One
const bnl::integer bnl::integer::one("1");


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
                valid = false; break;

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
    if (num.empty())
        throw std::invalid_argument("can't build bnl::integer from std::string: invalid format");


    // Reverse Double-Dabble to convert BDC to binary

    // Numeric data size
    const std::size_t num_size = num.size();
    size = ((num_size - 1) >> 3) + 1;

    // Algorithm sizes
    const std::size_t nibbles = size << 3;
    const std::size_t bits = size << 5;

    // Algorithm memory
    data = static_cast<bnl::ulint *>(std::calloc(size << 1, bnl::ulint_size));
    bnl::uchar *const bin = reinterpret_cast<bnl::uchar *>(data);
    bnl::uchar *const bcd = bin + nibbles;

    const bnl::uchar &bcd_top = bcd[num_size - 1];
    const std::size_t bin_top = ((size - 1) << 3) + 3;

    // Copy the number data
    for (std::size_t i = 0; i < num_size; i++)
        bcd[i] = num[i] - '0';


    // Main bucle for each bit
    for (std::size_t i = 0; i < bits; i++) {
        bnl::uchar bcd_r = 0;
        bnl::uchar bcd_l = 0;
        bnl::uchar bin_r = 0;
        bnl::uchar bin_l = bcd_top & 1;

        // Shift each block
        for (std::size_t j = 0, k = bin_top; j < nibbles; j++) {
            // Shift the binary block
            if ((j & 7) < 4) {
                bnl::uchar &block = bin[k];
                bin_r   = block & 1;
                block >>= 1;
                block  |= bin_l << 7;
                bin_l   = bin_r;

                // Binary block increment
                k -= k & 7 ? 1 : 5;
            }

            // Shift the bcd block
            bnl::uchar &block = bin[k];
            bcd_r   = block & 1;
            block >>= 1;
            block  |= bcd_l << 3;
            bcd_l   = bcd_r;

            // Condition
            if (block >= 8)
                block -= 3;
        }
    }


    // Fix the numeric data size
    for (std::size_t i = size - 1; i && !data[i]; i--)
        size--;

    // Release the algorithm memory
    data = static_cast<bnl::ulint *>(std::realloc(data, size * bnl::ulint_size));
}

// Constructor from long double
bnl::integer::integer(const bnl::ldouble &n) : data(NULL), size(0), sign(false) {
    // Check if is nan
    if (n != n)
        throw std::invalid_argument("can't build bnl::integer from long double: is nan");

    // Check if is positive infinite
    static const bnl::ldouble inf_pos = 1.0L / 0.0L;
    if (n == inf_pos)
        throw std::invalid_argument("can't build bnl::integer from long double: is inf");

    // Check if is negative infinite
    static const bnl::ldouble inf_neg = -1.0L / 0.0L;
    if (n < inf_neg)
        throw std::invalid_argument("can't build bnl::integer from long double: is -inf");

    // Cast to std::string and build bnl::integer
    std::stringstream stream;
    stream << std::fixed << std::setprecision(11451) << n;
    *this = bnl::integer(stream.str());
}


// Methods

// Get the string representation with the given radix
const std::string bnl::str(const bnl::integer &n, const int &radix) {
    // Check the radix
    if (radix != 10) {
        std::stringstream stream;
        stream << radix;
        throw std::invalid_argument("can't convert bnl::integer to std::string: invalid radix " + stream.str());
    }

    // Well known integers
    static const std::string zero("0");
    static const std::string one_pos("1");
    static const std::string one_neg("-1");

    // Zero or one
    if (bnl::iszero(n)) return zero;
    if (bnl::isone(n))  return n.sign ? one_neg : one_pos;


    // Double-Dabble to convert binary to BCD

    // Algorithm sizes
    const std::size_t digits = n.size * 10;
    const std::size_t bits = n.size << 5;

    // Algorithm memory
    bnl::uchar *const bcd = static_cast<bnl::uchar *>(std::calloc(digits, bnl::uchar_size));

    const std::size_t digits_top = digits - 1;
    const std::size_t bits_top = bits - 1;

    // Algorithm auxiliar variables
    static const std::size_t bit_mask = static_cast<std::size_t>(-1);
    bool carry = false;

    // Main bucle for each bit
    for (std::size_t i = bits_top; i < bits; i--) {
        bnl::uchar bit_l = 0;
        bnl::uchar bit_r = (n.data[i >> 5] & (1 << (i & bit_mask))) > 0;

        // Shift each block
        for (bnl::uchar j = digits_top; j < digits; j--) {
            // Block
            bnl::uchar &block = bcd[j];

            // Condition
            if (block >= 5)
                block += 3;

            // Shift BCD
            bit_l   = block >> 3;
            block <<= 1;
            block  &= 15;
            block  |= bit_r;
            bit_r   = bit_l;
        }

        // Update carry
        carry = static_cast<bool>(bit_l);
    }


    // Count zeros
    std::size_t zeros = 0;
    for (std::size_t i = 0; i < digits && !bcd[i]; i++)
        zeros++;

    // Convert digits to character
    for (std::size_t i = zeros; i < digits; i++)
        bcd[i] += '0';

    // String sign
    std::string str;
    if (n.sign)
        str.append("-");

    // String carry
    if (carry)
        str.append("1");

    // Append the BCD data and return it
    str.append(reinterpret_cast<const char *>(bcd + zeros));
    std::free(bcd);
    return str;
}


// Operators overloading

// Type conversion operators

// To std::string
bnl::integer::operator std::string() const {
    return bnl::str(*this);
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