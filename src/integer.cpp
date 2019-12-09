#include "integer.hpp"

#include <stdexcept> // std::invalid_argument
#include <iomanip>   // std::setfill, std::setw, std::setprecision


// Static constants

// Numeric representation base
const bnl::ulint bnl::integer::base = static_cast<bnl::ulint>(1) << (bnl::ulint_size << 2);

// Base bit mask
const bnl::ulint bnl::integer::base_mask = bnl::integer::base - 1;

// Zero
const bnl::integer bnl::integer::zero;

// One
const bnl::integer bnl::integer::one("1");


// Private static methods

// Compare and returns -1 if a < b, 0 if a == b, and 1 if a > b
int bnl::integer::cmp(const bnl::integer &a, const bnl::integer &b) {
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
inline void bnl::integer::cpy(bnl::ulint *const dest, const bnl::ulint *const src, const std::size_t &n) {
    // Copy each data block
    for (std::size_t i = 0; i < n; i++)
        dest[i] = src[i];
}

// Returns whether the given character if the point character
inline bool bnl::integer::isexp(const char &c) {
    return (c == 'e') || (c == 'E');
}

// Returns whether the given character if the point character
inline bool bnl::integer::isdigit(const char &c) {
    return (c >= '0') && (c <= '9');
}

// Returns whether the given character if the point character
inline bool bnl::integer::ispoint(const char &c) {
    return c == '.';
}

// Returns whether the given character if a sign character
inline bool bnl::integer::issign(const char &c) {
    return (c == '-') || (c == '+');
}


// Private constructors

// Empty integer
inline bnl::integer::integer(const std::size_t &size, const bool &sign) : data(NULL), size(size), sign(sign) {
    // Check size
    if (!size)
        throw std::invalid_argument("can't build bnl::integer from private constructor: invalid size");

    // Reserve empty memory
    data = static_cast<bnl::ulint *>(std::calloc(size, bnl::ulint_size));
}


// Private methods

// The minimum number precision
std::size_t bnl::integer::precision(bool *twos_pow) const {
    // Reference to the leftmost block
    const bnl::ulint &block = data[size - 1];

    // For each bit
    std::size_t bits = size << 5;
    for (bnl::ulint i = bnl::integer::base >> 1; (i < bnl::integer::base) && !(block & i); i >>= 1)
        bits--;

    // Check if is two's power
    if (twos_pow) {
        // Default value
        *twos_pow = true;

        // Check the last block
        if (block & ~(1 << ((bits & 31) - 1)))
            *twos_pow = false;

        // Check the other blocks
        const std::size_t top = size - 1;
        for (std::size_t i = 0; (i < top) && *twos_pow; i++) {
            // If the block has ones is not two's power
            if (data[i])
                *twos_pow = false;
        }
    }

    // Return the precision
    return bits;
}

// Shrink numeric data
void bnl::integer::shrink() {
    // Update size
    for (std::size_t i = size - 1; i && !data[i]; i--)
        size--;

    // Resize the numeric data
    data = static_cast<bnl::ulint *>(std::realloc(data, size * bnl::ulint_size));
}


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
                if (bnl::integer::isdigit(c)) {state = 3; begin = 1;            break;}
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

// Number presicion
std::size_t bnl::integer::precision(const bnl::integer &n) {
    return n.sign ? n.precision() + 1 : n.precision();
}

// Integer division
const bnl::div_t bnl::integer::div(const bnl::integer &a, const bnl::integer &b) {
    // Zeros
    if (bnl::iszero(a))
        return bnl::div_t(bnl::integer::zero, bnl::integer::zero);

    if (bnl::iszero(a))
        throw std::invalid_argument("can't divide: division by zero");

    // Ones
    if (bnl::isone(b))
        return bnl::div_t(b.sign ? -a : a, bnl::integer::zero);

    // Same number
    if (a == b)
        return bnl::div_t(bnl::integer::one, bnl::integer::zero);

    // Divisor larger than dividend
    if (b > a)
        return bnl::div_t(bnl::integer::zero, a);


    // Operands precision
    bool twos_pow;
    const std::size_t bits_a = a.precision();
    const std::size_t bits_b = b.precision(&twos_pow);

    // Bitwise operations if the divisor is power of two
    if (twos_pow) {
        // Operators and answer variables
        const std::size_t shiftment = bits_b - 1;
        const std::size_t mod_mask = (bits_b << 1) - 1;
        bnl::div_t ans;

        // Positive dividend
        if (!a.sign) {
            ans.quot = a >> static_cast<bnl::ldouble>(shiftment);
            ans.rem = a & static_cast<bnl::ldouble>(mod_mask);
        }

        // Negative dividend
        else {
            const bnl::integer a_pos = -a;
            ans.quot = a_pos >> static_cast<bnl::ldouble>(shiftment);
            ans.rem = a_pos & static_cast<bnl::ldouble>(mod_mask);
        }

        // Quotient sign
        ans.quot.sign = a.sign ^ b.sign;

        // Return the answer
        return ans;
    }

    // Operands and answer variables
    const std::size_t bits_diff = bits_a - bits_b;
    div_t ans;
    ans.rem = a >> static_cast<bnl::ldouble>(bits_diff);
    ans.quot.sign = a.sign ^ b.sign;

    // Bit selector and shiftments offset
    bnl::ulint bit_shiftment = (bits_diff & 31) - 1;
    bnl::ulint bit_selector = static_cast<bnl::ulint>(1) << bit_shiftment;
    bnl::ulint offset;


    // Division main loop
    for (std::size_t i = 0, j = bits_diff >> 5; i < bits_diff; i++, bit_selector >>= 1) {
        // Quotient shiftment
        offset = 0;
        for (std::size_t k = 0; k < ans.quot.size; k++) {
            const bnl::ulint tmp = ans.quot.data[k];
            ans.quot.data[k] = (offset >> 31) | (ans.quot.data[k] << 1);
            offset = tmp;
        }

        // Left overflow
        offset >>= 31;
        if (offset) {
            // Resize and append offset
            ans.quot.size++;
            ans.quot.data = static_cast<bnl::ulint *>(std::realloc(ans.quot.data, ans.quot.size * bnl::ulint_size));
            ans.quot.data[ans.quot.size - 1] = offset;
        }

        // Subtraction
        if (ans.rem >= b) {
            ans.rem = ans.rem - b;
            ans.quot.data[0] |= 1;
        }

        //Remainder shiftment
        offset = 0;
        for (std::size_t k = 0; k < ans.quot.size; k++) {
            ans.rem.data[k] = (offset >> 31) | ((ans.rem.data[k] << 1) & bnl::integer::base_mask);
            offset = ans.rem.data[k];
        }

        // Left overflow
        offset >>= 31;
        if (offset) {
            // Resize and append offset
            ans.rem.size++;
            ans.rem.data = static_cast<bnl::ulint *>(std::realloc(ans.rem.data, ans.rem.size * bnl::ulint_size));
            ans.rem.data[ans.rem.size - 1] = offset;
        }

        // Reset the bit selector
        if (!bit_selector) {
            bit_selector = bnl::integer::base >> 1;
            j--;
        }

        // Append bit from dividend to remanider
        const bnl::ulint bit = a.data[j] & bit_selector;
        if (bit)
            ans.rem.data[0] |= 1;
    }

    // Last subtraction

    // Quotient shiftment
    offset = 0;
    for (std::size_t k = 0; k < ans.quot.size; k++) {
        ans.quot.data[k] = (offset >> 31) | ((ans.quot.data[k] << 1) & bnl::integer::base_mask);
        offset = ans.quot.data[k];
    }

    // Left overflow
    offset >>= 31;
    if (offset) {
        // Resize and append offset
        ans.quot.size++;
        ans.quot.data = static_cast<bnl::ulint *>(std::realloc(ans.quot.data, ans.quot.size * bnl::ulint_size));
        ans.quot.data[ans.quot.size - 1] = offset;
    }

    // Subtraction
    if (ans.rem > b) {
        ans.rem = ans.rem - b;
        ans.quot.data[0] |= 1;
    }


    // Return the answer
    return ans;
}


// Public constructors

// Default constructor
bnl::integer::integer() : data(static_cast<bnl::ulint *>(std::calloc(1, bnl::ulint_size))), size(1), sign(false) {}

// Copy constructor
bnl::integer::integer(const bnl::integer &n) : data(static_cast<bnl::ulint *>(std::malloc(n.size * bnl::ulint_size))), size(n.size), sign(n.sign) {
    bnl::integer::cpy(data, n.data, size);
}

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
                bin_r    = bin[k] & 1;
                bin[k] >>= 1;
                bin[k]  |= bin_l << 7;
                bin_l    = bin_r;

                // Binary block increment
                k -= k & 7 ? 1 : 5;
            }

            // Shift the bcd block
            bcd_r    = bcd[j] & 1;
            bcd[j] >>= 1;
            bcd[j]  |= bcd_l << 3;
            bcd_l    = bcd_r;

            // Condition
            if (bcd[j] >= 8)
                bcd[j] -= 3;
        }
    }


    // Shrink the numeric data
    shrink();
}

// Constructor from const char *
bnl::integer::integer(const char *const str) : data(NULL), size(0), sign(false) {
    *this = bnl::integer(std::string(str));
}

// Constructor from long double
bnl::integer::integer(const bnl::ldouble &n) : data(NULL), size(0), sign(false) {
    // Check if is nan
    if (n != n)
        throw std::invalid_argument("can't build bnl::integer from long double: is +nan or -nan");

    // Check if is infinite positive or negative
    static bnl::ldouble zero = 0.0;
    static const bnl::ldouble inf_p = 1.0 / zero;
    static const bnl::ldouble inf_n = -1.0 / zero;
    if ((n <= inf_n) || (n >= inf_p))
        throw std::invalid_argument("can't build bnl::integer from long double: is +inf or -inf");

    // Cast to std::string and build bnl::integer
    std::stringstream stream;
    stream << std::fixed << std::setprecision(11451) << n;
    *this = bnl::integer(stream.str());
}


// Methods

// Returns the given number raised to the given exponent
const bnl::integer bnl::pow(const bnl::integer &a, const bnl::integer &b) {
    // Square exponent constant
    static const bnl::integer two("2");


    // Base case
    if (bnl::iszero(b))
        return bnl::integer::one;


    // Odd exponent
    if (bnl::isodd(b))
        return a * bnl::pow(a, b - bnl::integer::one);

    // Even exponent
    else {
        // Square exponent
        if ((b.size == 1) && (b.data[0] == 2))
            return a * a;

        // Recursive call
        return bnl::pow(bnl::pow(a, b >> bnl::integer::one), two);
    }
}

// Get the string representation with the given radix
const std::string bnl::str(const bnl::integer &n, const int &radix) {
    // Check the radix
    if (radix != 10)
        throw std::invalid_argument("can't convert bnl::integer to std::string: invalid radix");

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

    // Algorithm auxiliar variables
    const std::size_t digits_top = digits - 1;
    bool carry = false;

    // Main bucle for each bit
    for (std::size_t i = bits - 1; i < bits; i--) {
        bnl::uchar bit_l = 0;
        bnl::uchar bit_r = (n.data[i >> 5] & (static_cast<bnl::ulint>(1) << (i & 31))) > 0;

        // Shift each block
        for (std::size_t j = digits_top; j < digits; j--) {
            // Condition
            if (bcd[j] >= 5)
                bcd[j] += 3;

            // Shift BCD
            bit_l    = bcd[j] >> 3;
            bcd[j] <<= 1;
            bcd[j]  &= 15;
            bcd[j]  |= bit_r;
            bit_r    = bit_l;
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
    str.append(reinterpret_cast<const char *>(bcd + zeros), digits - zeros);
    std::free(bcd);
    return str;
}


// Operators overloading

// Type conversion operators

// To std::string
bnl::integer::operator std::string() const {
    return bnl::str(*this);
}


// Postfix increment and decrement operators

// Postfix increment
const bnl::integer bnl::integer::operator ++ (int) {
    bnl::integer ans = *this;
    *this = *this + bnl::integer::one;
    return ans;
}

// Postfix decrement
const bnl::integer bnl::integer::operator -- (int) {
    bnl::integer ans = *this;
    *this = *this - bnl::integer::one;
    return ans;
}


// Uniary operators

// Additive inverse
const bnl::integer bnl::integer::operator - () const {
    // Return the same number if is zero
    if (bnl::iszero(*this))
        return *this;

    // Return copy with inversed sign
    bnl::integer ans = *this;
    ans.sign = !ans.sign;
    return ans;
}


// Arithmetic operators

// Multiplication
const bnl::integer operator * (const bnl::integer &a, const bnl::integer &b) {
    // Zeros
    if (bnl::iszero(a) || bnl::iszero(b))
        return bnl::integer::zero;

    // Ones
    if (bnl::isone(a))
        return a.sign ? -b : b;

    if (bnl::isone(b))
        return b.sign ? -a : a;


    // Operands and answer variables
    const bnl::integer &m = a.size > b.size ? a : b;
    const bnl::integer &n = &m == &a ? b : a;
    bnl::integer ans;

    // Auxiliar variables
    bnl::integer prod;


    // Multiplication main bucle
    for (std::size_t i = 0; i < n.size; i++) {
        // Resize the intermediate product
        prod.size = m.size + i;
        prod.data = static_cast<bnl::ulint *>(std::realloc(prod.data, prod.size * bnl::ulint_size));

        // Right padding with zeros
        for (std::size_t j = 0; j < i; j++)
            prod.data[j] = 0;

        // Current block and carry
        const bnl::ulint &block = n.data[i];
        bnl::ulint carry = 0;

        // Multiplication
        for (std::size_t j = i, k = 0; k < m.size; j++, k++) {
            // Multiply
            prod.data[j] = m.data[k] * block + carry;

            // Check carry
            if (prod.data[j] >= bnl::integer::base) {
                carry = prod.data[j] / bnl::integer::base;
                prod.data[j] &= bnl::integer::base_mask;
            }

            // Reset the carry
            else carry = 0;
        }

        // Carry
        if (carry) {
            prod.size++;
            prod.data = static_cast<bnl::ulint *>(std::realloc(prod.data, prod.size * bnl::ulint_size));
            prod.data[prod.size - 1] = carry;
        }

        // Accumulate the intermediate product
        ans += prod;
    }


    // Set the answer sign and return it
    ans.sign = a.sign ^ b.sign;
    return ans;
}

// Division
const bnl::integer operator / (const bnl::integer &a, const bnl::integer &b) {
    return bnl::integer::div(a, b).quot;
}

// Modulo
const bnl::integer operator % (const bnl::integer &a, const bnl::integer &b) {
    return bnl::integer::div(a, b).rem;
}

// Addition
const bnl::integer operator + (const bnl::integer &a, const bnl::integer &b) {
    // Zeros
    if (bnl::iszero(a))
        return b;

    if (bnl::iszero(b))
        return a;


    // Different signs
    if (!a.sign && b.sign)
        return a - -b;

    if (a.sign && !b.sign)
        return b - -a;


    // Operands and answer variables
    const bnl::integer &m = a.size >= b.size ? a : b;
    const bnl::integer &n = &m == &a ? b : a;
    bnl::integer ans(m.size, m.sign);

    // Auxiliar variables
    std::size_t i = 0;
    bnl::ulint carry = 0;


    // Addition main bucle
    while (i < n.size) {
        // Add
        ans.data[i] = m.data[i] + n.data[i] + carry;

        // Check carry
        if (ans.data[i] >= bnl::integer::base) {
            carry = 1;
            ans.data[i] -= bnl::integer::base;
        }

        // Reset the carry
        else carry = 0;

        // Index increment
        i++;
    }

    // Extra digits bucle
    while (i < m.size) {
        // Add
        ans.data[i] = m.data[i] + carry;

        // Check carry
        if (ans.data[i] >= bnl::integer::base) {
            carry = 1;
            ans.data[i] -= bnl::integer::base;
        }

        // Reset the carry
        else carry = 0;

        // Index increment
        i++;
    }


    // Carry
    if (carry) {
        ans.size++;
        ans.data = static_cast<bnl::ulint *>(std::realloc(ans.data, ans.size * bnl::ulint_size));
        ans.data[ans.size - 1] = 1;
    }

    // Return the answer
    return ans;
}

// Subtraction
const bnl::integer operator - (const bnl::integer &a, const bnl::integer &b) {
    // Same object
    if (&a == &b)
        return bnl::integer::zero;


    // Different signs
    if (a.sign != b.sign)
        return a + -b;


    // Operands and answer variables
    const bnl::integer &m = bnl::abs(a) >= bnl::abs(b) ? a : b;
    const bnl::integer &n = &m == &a ? b : a;
    bnl::integer ans(m.size, &m == &a ? a.sign : !b.sign);

    // Auxiliar variables
    std::size_t i = 0;
    bnl::ulint carry = 0;


    // Subtraction main bucle
    while (i < n.size) {
        // Subtract
        ans.data[i] = m.data[i] - n.data[i] - carry;

        // Check carry
        if (ans.data[i] >= bnl::integer::base) {
            carry = 1;
            ans.data[i] += bnl::integer::base;
        }

        // Reset the carry
        else carry = 0;

        // Index increment
        i++;
    }

    // Extra digits bucle
    while (i < m.size) {
        // Subtract
        ans.data[i] = m.data[i] - carry;

        // Check carry
        if (ans.data[i] >= bnl::integer::base) {
            carry = 1;
            ans.data[i] += bnl::integer::base;
        }

        // Reset the carry
        else carry = 0;

        // Index increment
        i++;
    }


    // Shrink the numeric data and return
    ans.shrink();
    return ans;
}

// Left shift
const bnl::integer operator << (const bnl::integer &a, const bnl::integer &b) {
    // Zeros
    if (bnl::iszero(a) || bnl::iszero(b))
        return a;

    // Negative shift count
    if (b.sign)
        return a >> -b;


    // Memory limits constants
    static const std::size_t mem_bits = sizeof(void *) << 3;
    static const std::size_t max_blocks = static_cast<std::size_t>(-1) / mem_bits;
    static const std::invalid_argument memory_limit_exception = std::invalid_argument("can't shift left: exceeds theoretical memory limits");


    // Block shift
    const std::size_t block_shift = static_cast<std::size_t>(b.size > 1 ? (b.data[1] << 5) | (b.data[0] >> 5) : b.data[0] >> 5);

    // Check memory limits
    if (((b.size >= 2) && (b.data[1] >> 5)) || (max_blocks - block_shift < a.size))
        throw memory_limit_exception;

    // Bit shift and answer variables
    const std::size_t shift_l = static_cast<std::size_t>(b.data[0] & 31);
    const std::size_t shift_r = 32 - shift_l;
    bnl::ulint offset = 0;
    bnl::integer ans(block_shift + a.size, a.sign);


    // Left shift main bucle
    for (std::size_t i = block_shift, j = 0; j < a.size; i++, j++) {
        ans.data[i] = (offset >> shift_r) | ((a.data[j] << shift_l) & bnl::integer::base_mask);
        offset = a.data[j];
    }

    // Left overflow
    offset >>= shift_r;
    if (offset) {
        // Check memory limits
        ans.size++;
        if (ans.size > max_blocks)
            throw memory_limit_exception;

        // Resize and append offset
        ans.data = static_cast<bnl::ulint *>(std::realloc(ans.data, ans.size * bnl::ulint_size));
        ans.data[ans.size - 1] = offset;
    }


    // Return the answer
    return ans;
}

// Right shift
const bnl::integer operator >> (const bnl::integer &a, const bnl::integer &b) {
    // Zeros
    if (bnl::iszero(a) || bnl::iszero(b))
        return a;

    // Negative shift count
    if (b.sign)
        return a << -b;


    // Block shift
    const std::size_t block_shift = static_cast<std::size_t>(b.size > 1 ? (b.data[1] << 5) | (b.data[0] >> 5) : b.data[0] >> 5);

    // Check underflow
    if (block_shift >= a.size)
        return bnl::integer::zero;

    // Bit shift and answer variables
    const std::size_t shift_r = static_cast<std::size_t>(b.data[0] & 31);
    const std::size_t shift_l = 32 - shift_r;
    const bnl::ulint offset_mask = (static_cast<bnl::ulint>(1) << shift_r) - 1;
    bnl::integer ans(a.size - block_shift, a.sign);


    // Positive sign
    if (!a.sign) {
        // Offset
        bnl::ulint offset = 0;

        // Right shift main bucle
        for (std::size_t i = ans.size - 1, j = a.size - 1; i < ans.size; i--, j--) {
            ans.data[i] = (offset << shift_l) | (a.data[j] >> shift_r);
            offset = a.data[j] & offset_mask;
        }
    }

    // Negative sign
    else {
        // Carries
        bnl::ulint carry = 1;
        bnl::ulint carry_ans = 1;

        // First block
        bnl::ulint block = (a.data[0] ^ bnl::integer::base_mask) + carry;

        // Check carry
        if (block >= bnl::integer::base_mask) {
            carry = 1;
            block -= bnl::integer::base_mask;
        }

        // Reset the carry
        else carry = 0;

        // Offset
        bnl::ulint offset = block >> shift_r;

        // Right shift main bucle
        for (std::size_t i = 0, j = block_shift + 1; j < a.size; i++, j++) {
            // Two's complement
            block = (a.data[j] ^ bnl::integer::base_mask) + carry;

            // Check carry
            if (block >= bnl::integer::base_mask) {
                carry = 1;
                block -= bnl::integer::base_mask;
            }

            // Reset the carry
            else carry = 0;

            // Two's complement of the right shiftment
            ans.data[i] = ((((block & offset_mask) << shift_l) | (offset >> shift_r)) ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base_mask) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base_mask;
            }

            // Reset the carry
            else carry_ans = 0;

            // Offset
            offset = block;
        }

        // Fill the last block and two's complement
        ans.data[ans.size - 1] = (((offset_mask << shift_l) | offset) ^ bnl::integer::base_mask) + carry_ans;

        // Check carry
        if (ans.data[ans.size - 1] >= bnl::integer::base_mask) {
            carry_ans = 1;
            ans.data[ans.size - 1] -= bnl::integer::base_mask;
        }
    }


    // Shrink and return the answer
    ans.shrink();
    return ans;
}


// Input and output

// Standard output
std::ostream &operator << (std::ostream &stream, const bnl::integer &n) {
    stream << std::string(n);
    return stream;
}

// Standard input
std::istream &operator >> (std::istream &stream, bnl::integer &n) {
    // Read the integer
    std::string str;
    stream >> str;
    n = str;

    // Return the stream
    return stream;
}


// Comparison and relational operators

// Greater than
bool operator > (const bnl::integer &a, const bnl::integer &b) {
    // Same object
    if (&a == &b)
        return false;

    // Zeros
    const bool a_is_zero = bnl::iszero(a);
    const bool b_is_zero = bnl::iszero(b);

    if (a_is_zero && b_is_zero)
        return false;

    if (a_is_zero)
        return b.sign;

    if (b_is_zero)
        return !a.sign;


    // Different signs
    if (!a.sign && b.sign)
        return true;

    if (a.sign && !b.sign)
        return false;


    // Positive signs
    if (!a.sign) {
        // Check sizes
        if (a.size != b.size)
            return a.size > b.size;

        // Full comparison
        return bnl::integer::cmp(a, b) == 1;
    }

    // Negative signs

    // Check sizes
    if (a.size != b.size)
        return a.size < b.size;

    // Full comparison
    return bnl::integer::cmp(a, b) == -1;
}

// Less than
bool operator < (const bnl::integer &a, const bnl::integer &b) {
    // Same object
    if (&a == &b)
        return false;

    // Zeros
    const bool a_is_zero = bnl::iszero(a);
    const bool b_is_zero = bnl::iszero(b);

    if (a_is_zero && b_is_zero)
        return false;

    if (a_is_zero)
        return !b.sign;

    if (b_is_zero)
        return a.sign;


    // Different signs
    if (!a.sign && b.sign)
        return false;

    if (a.sign && !b.sign)
        return true;


    // Positive signs
    if (!a.sign) {
        // Check sizes
        if (a.size != b.size)
            return a.size < b.size;

        // Full comparison
        return bnl::integer::cmp(a, b) == -1;
    }

    // Negative signs

    // Check sizes
    if (a.size != b.size)
        return a.size > b.size;

    // Full comparison
    return bnl::integer::cmp(a, b) == 1;
}

// Equals to
bool operator == (const bnl::integer &a, const bnl::integer &b) {
    // Same object
    if (&a == &b)
        return true;

    // Different signs
    if ((a.sign != b.sign) || (a.size != b.size))
        return false;

    // Full comparison
    return !bnl::integer::cmp(a, b);
}


// Bitwise operators

// Bitwise AND
const bnl::integer operator & (const bnl::integer &a, const bnl::integer &b) {
    // Zeros
    if (bnl::iszero(a) || bnl::iszero(b))
        return bnl::integer::zero;


    // Netative ones
    if (a.sign && bnl::isone(a))
        return b;

    if (b.sign && bnl::isone(b))
        return a;


    // Operands and answer variables
    const bnl::integer &m = a.size >= b.size ? a : b;
    const bnl::integer &n = &m == &a ? b : a;
    bnl::integer ans(m.size, m.sign && n.sign);

    // Auxiliar variables
    std::size_t i = 0;


    // Positive signs
    if (!m.sign && !n.sign) {
        // AND main bucle
        while(i < n.size) {
            ans.data[i] = m.data[i] & n.data[i];
            i++;
        }
    }

    // Only m nevative
    else if (m.sign && !n.sign) {
        // Carry
        bnl::ulint carry = 1;

        // AND main bucle
        while(i < n.size) {
            // Two's complement of m
            bnl::ulint mc2 = (m.data[i] ^ bnl::integer::base_mask) + carry;

            // Check carry
            if (mc2 >= bnl::integer::base) {
                carry = 1;
                mc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry = 0;

            // Apply the AND operation
            ans.data[i] = mc2 & n.data[i];
            i++;
        }
    }

    // Only n negative
    else if (!m.sign && n.sign) {
        // Carry
        bnl::ulint carry = 1;

        // AND main bucle
        while(i < n.size) {
            // Two's complement of n
            bnl::ulint nc2 = (n.data[i] ^ bnl::integer::base_mask) + carry;

            // Check carry
            if (nc2 >= bnl::integer::base) {
                carry = 1;
                nc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry = 0;

            // Apply the AND operation
            ans.data[i] = m.data[i] & nc2;
            i++;
        }

        // Extra digits bucle
        while (i < m.size) {
            ans.data[i] = m.data[i];
            i++;
        }
    }

    // Negative signs
    else {
        // Carry
        bnl::ulint carry_m = 1;
        bnl::ulint carry_n = 1;
        bnl::ulint carry_ans = 1;

        // AND main bucle
        while(i < n.size) {
            // Two's complements
            bnl::ulint mc2 = (m.data[i] ^ bnl::integer::base_mask) + carry_m;
            bnl::ulint nc2 = (n.data[i] ^ bnl::integer::base_mask) + carry_n;

            // Check carry
            if (mc2 >= bnl::integer::base) {
                carry_m = 1;
                mc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_m = 0;

            // Check carry
            if (nc2 >= bnl::integer::base) {
                carry_n = 1;
                nc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_n = 0;

            // Two's complement of the AND operation
            ans.data[i] = ((mc2 & nc2) ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_ans = 0;

            // Block increment
            i++;
        }

        // Extra digits bucle
        while (i < m.size) {
            // Two's complement of m
            ans.data[i] = (m.data[i] ^ bnl::integer::base_mask) + carry_m;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_m = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_m = 0;

            // Two's complement of answer
            ans.data[i] = (ans.data[i] ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_ans = 0;

            // Block increment
            i++;
        }
    }


    // Shrink the answer and return it
    ans.shrink();
    return ans;
}

// Bitwise OR (inclusive or)
const bnl::integer operator | (const bnl::integer &a, const bnl::integer &b) {
    // Zeros
    if (bnl::iszero(a))
        return b;

    if (bnl::iszero(b))
        return a;


    // Netative ones
    if (a.sign && bnl::isone(a))
        return a;

    if (b.sign && bnl::isone(b))
        return b;


    // Operands and answer variables
    const bnl::integer &m = a.size >= b.size ? a : b;
    const bnl::integer &n = &m == &a ? b : a;
    bnl::integer ans(m.size, m.sign || n.sign);

    // Auxiliar variables
    std::size_t i = 0;


    // Positive signs
    if (!m.sign && !n.sign) {
        // OR main bucle
        while(i < n.size) {
            ans.data[i] = m.data[i] | n.data[i];
            i++;
        }

        // Extra digits bucle
        while (i < m.size) {
            ans.data[i] = m.data[i];
            i++;
        }
    }

    // Only m nevative
    else if (m.sign && !n.sign) {
        // Carry
        bnl::ulint carry_m = 1;
        bnl::ulint carry_ans = 1;

        // OR main bucle
        while(i < n.size) {
            // Two's complement of m
            bnl::ulint mc2 = (m.data[i] ^ bnl::integer::base_mask) + carry_m;

            // Check carry
            if (mc2 >= bnl::integer::base) {
                carry_m = 1;
                mc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_m = 0;

            // Two's complement of the OR operation
            ans.data[i] = ((mc2 | n.data[i]) ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_ans = 0;

            // Block increment
            i++;
        }

        // Extra digits bucle
        while (i < m.size) {
            ans.data[i] = m.data[i];
            i++;
        }
    }

    // Only n negative
    else if (!m.sign && n.sign) {
        // Carry
        bnl::ulint carry_n = 1;
        bnl::ulint carry_ans = 1;

        // OR main bucle
        while(i < n.size) {
            // Two's complement of n
            bnl::ulint nc2 = (n.data[i] ^ bnl::integer::base_mask) + carry_n;

            // Check carry
            if (nc2 >= bnl::integer::base) {
                carry_n = 1;
                nc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_n = 0;

            // Two's complement of the OR operation
            ans.data[i] = ((m.data[i] | nc2) ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_ans = 0;

            // Block increment
            i++;
        }
    }

    // Negative signs
    else {
        // Carry
        bnl::ulint carry_m = 1;
        bnl::ulint carry_n = 1;
        bnl::ulint carry_ans = 1;

        // OR main bucle
        while(i < n.size) {
            // Two's complements
            bnl::ulint mc2 = (m.data[i] ^ bnl::integer::base_mask) + carry_m;
            bnl::ulint nc2 = (n.data[i] ^ bnl::integer::base_mask) + carry_n;

            // Check carry
            if (mc2 >= bnl::integer::base) {
                carry_m = 1;
                mc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_m = 0;

            // Check carry
            if (nc2 >= bnl::integer::base) {
                carry_n = 1;
                nc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_n = 0;

            // Two's complement of the AND operation
            ans.data[i] = ((mc2 | nc2) ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_ans = 0;

            // Block increment
            i++;
        }
    }


    // Shrink the answer and return it
    ans.shrink();
    return ans;
}

// Bitwise XOR (exclusive or)
const bnl::integer operator ^ (const bnl::integer &a, const bnl::integer &b) {
    // Zeros
    if (bnl::iszero(a))
        return b;

    if (bnl::iszero(b))
        return a;


    // Netative ones
    if (a.sign && bnl::isone(a))
        return ~b;

    if (b.sign && bnl::isone(b))
        return ~a;


    // Operands and answer variables
    const bnl::integer &m = a.size >= b.size ? a : b;
    const bnl::integer &n = &m == &a ? b : a;
    bnl::integer ans(m.size, m.sign ^ n.sign);

    // Auxiliar variables
    std::size_t i = 0;


    // Positive signs
    if (!m.sign && !n.sign) {
        // XOR main bucle
        while(i < n.size) {
            ans.data[i] = m.data[i] ^ n.data[i];
            i++;
        }

        // Extra digits bucle
        while (i < m.size) {
            ans.data[i] = m.data[i];
            i++;
        }
    }

    // Only m nevative
    else if (m.sign && !n.sign) {
        // Carry
        bnl::ulint carry_m = 1;
        bnl::ulint carry_ans = 1;

        // OR main bucle
        while(i < n.size) {
            // Two's complement of m
            bnl::ulint mc2 = (m.data[i] ^ bnl::integer::base_mask) + carry_m;

            // Check carry
            if (mc2 >= bnl::integer::base) {
                carry_m = 1;
                mc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_m = 0;

            // Two's complement of the XOR operation
            ans.data[i] = ((mc2 ^ n.data[i]) ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_ans = 0;

            // Block increment
            i++;
        }

        // Extra digits bucle
        while (i < m.size) {
            // Two's complement of m
            ans.data[i] = (m.data[i] ^ bnl::integer::base_mask) + carry_m;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_m = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_m = 0;

            // Two's complement of answer
            ans.data[i] = (ans.data[i] ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_ans = 0;

            // Block increment
            i++;
        }
    }

    // Only n negative
    else if (!m.sign && n.sign) {
        // Carry
        bnl::ulint carry_n = 1;
        bnl::ulint carry_ans = 1;

        // OR main bucle
        while(i < n.size) {
            // Two's complement of n
            bnl::ulint nc2 = (n.data[i] ^ bnl::integer::base_mask) + carry_n;

            // Check carry
            if (nc2 >= bnl::integer::base) {
                carry_n = 1;
                nc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_n = 0;

            // Two's complement of the XOR operation
            ans.data[i] = ((m.data[i] ^ nc2) ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base;
            }

            // Reset the carry
            else carry_ans = 0;

            // Block increment
            i++;
        }

        // Extra digits bucle
        while (i < m.size) {
            ans.data[i] = m.data[i];
            i++;
        }
    }

    // Negative signs
    else {
        // Carry
        bnl::ulint carry_m = 1;
        bnl::ulint carry_n = 1;

        // OR main bucle
        while(i < n.size) {
            // Two's complements
            bnl::ulint mc2 = (m.data[i] ^ bnl::integer::base_mask) + carry_m;
            bnl::ulint nc2 = (n.data[i] ^ bnl::integer::base_mask) + carry_n;

            // Check carry
            if (mc2 >= bnl::integer::base) {
                carry_m = 1;
                mc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_m = 0;

            // Check carry
            if (nc2 >= bnl::integer::base) {
                carry_n = 1;
                nc2 -= bnl::integer::base;
            }

            // Reset the carry
            else carry_n = 0;

            // XOR operation
            ans.data[i] = mc2 ^ nc2;
            i++;
        }

        // Extra digits bucle
        while (i < m.size) {
            ans.data[i] = m.data[i];
            i++;
        }
    }


    // Shrink the answer and return it
    ans.shrink();
    return ans;
}


// Assignment operators

// Direct assignation
bnl::integer &bnl::integer::operator = (const bnl::integer &n) {
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
