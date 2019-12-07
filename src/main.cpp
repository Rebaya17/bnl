#include "integer.hpp"
#include <iostream>
#include <string>

int main() {
    // Constructors and output
    std::cout << "Copy constructor:             " << bnl::integer(bnl::integer("123456")) << std::endl;
    std::cout << "From string constructor:      " << bnl::integer("9999999999") << std::endl;
    std::cout << "                              " << bnl::integer("12348.1155") << std::endl;
    std::cout << "                              " << bnl::integer("-123456789") << std::endl;
    std::cout << "                              " << bnl::integer("-987654.44") << std::endl;
    std::cout << "                              " << bnl::integer("12348E11") << std::endl;
    std::cout << "                              " << bnl::integer("1.2348E11") << std::endl;
    std::cout << "                              " << bnl::integer("12348E-3") << std::endl;
    std::cout << "                              " << bnl::integer("98765.12E-3") << std::endl;
    std::cout << "                              " << bnl::integer(".12E2") << std::endl;
    std::cout << "                              " << bnl::integer("-.15") << std::endl;
    std::cout << "                              " << bnl::integer("345.") << std::endl;
    std::cout << "From string constructor:      " << bnl::integer(98765.12E-3) << std::endl << std::endl;

    // Common operators
    bnl::integer a = "465978445001246";
    bnl::integer b = "17803599121421";

    std::cout << "Negation:              " << -a << std::endl;
    std::cout << "Not:                   " << !a << std::endl;
    std::cout << "Not (bitwise):         " << ~a << std::endl;
    std::cout << "Adition:               " << a + b << std::endl;
    std::cout << "Subtraction:           " << a - b << std::endl;
    std::cout << "Multiplication:        " << a * b << std::endl;
    std::cout << "                       " << a * -b << std::endl;
    std::cout << "                       " << -a * -b << std::endl;
    std::cout << "Division:              " << a / b << std::endl;
    std::cout << "                       " << -a / b << std::endl;
    std::cout << "                       " << b / a << std::endl;
    std::cout << "Division:              " << a / b << std::endl;
    std::cout << "Left shift:            " << (a << bnl::integer("24")) << std::endl;
    std::cout << "                       " << (-a << bnl::integer("24")) << std::endl;
    std::cout << "Right shift:           " << (a >> bnl::integer("24")) << std::endl;
    std::cout << "                       " << (-a >> bnl::integer("24")) << std::endl;
    std::cout << "Greater than:          " << (a > b) << std::endl;
    std::cout << "Less than:             " << (a < b) << std::endl;
    std::cout << "Greater or equal than: " << (a >= b) << std::endl;
    std::cout << "Less or equal than:    " << (a <= b) << std::endl;
    std::cout << "Equal to:              " << (a == b) << std::endl;
    std::cout << "Not equal to:          " << (a != b) << std::endl;
    std::cout << "AND (bitwise):         " << (a & b) << std::endl;
    std::cout << "OR (bitwise):          " << (a | b) << std::endl;
    std::cout << "XOR (bitwise):         " << (a ^ b) << std::endl;
    std::cout << "AND:                   " << (a && b) << std::endl;
    std::cout << "OR:                    " << (a || b) << std::endl;
    std::cout << "Try with others numbers, operator combination and assignations..." << std::endl << std::endl;

    // Helpful functions
    std::cout << "Absolute value: " << bnl::abs(a) << std::endl;
    std::cout << "                " << bnl::abs(-a) << std::endl;
    std::cout << "Maximum:        " << bnl::max(a, b) << std::endl;
    std::cout << "Minimum:        " << bnl::min(a, b) << std::endl;
    std::cout << "Power:          " << bnl::pow(a, bnl::integer("8")) << std::endl;

    return 0;
}
