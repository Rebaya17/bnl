#include "integer.hpp"

#include <stdexcept> // std::invalid_argument
#include <iomanip>   // std::setfill, std::setw, std::setprecision


// Static constants

// Numeric representation base
const bnl::ulint bnl::integer::base = 0x0100000000;

// Base bit mask
const bnl::ulint bnl::integer::base_mask = bnl::integer::base - 1;

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
        for (std::size_t j = digits_top; j < digits; j--) {
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

}

// Modulo
const bnl::integer operator % (const bnl::integer &a, const bnl::integer &b) {

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
        ans.data[i] = (offset >> shift_r) | (a.data[j] << shift_l);
        offset = a.data[j];
    }

    // Left overflow
    offset = a.data[a.size - 1] >> shift_r;
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
    const bnl::ulint offset_mask = (1 << shift_r) - 1;
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
            ans.data[i] = ((((block & offset_mask) << shift_l) | offset) ^ bnl::integer::base_mask) + carry_ans;

            // Check carry
            if (ans.data[i] >= bnl::integer::base_mask) {
                carry_ans = 1;
                ans.data[i] -= bnl::integer::base_mask;
            }

            // Reset the carry
            else carry_ans = 0;

            // Offset
            offset = block >> shift_r;
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
