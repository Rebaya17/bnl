# Another Big Numbers Library

**WARNING: This library has been created for educational and research**
**purposes. It is not recommended for use in production environments.**

Another Big Numbers Library is developed to explore low-level features of the
language and operator overloading, providing high performance objects with an
easy to use interface, which each class have the same behavior of the built-in
data types, or at least the most of its features are simulated.

In theory, the precision limit is defined by the size of the system memory.
Despite all this, not enough tests have been performed, so it may contain
several unknown critic errors. If important calculations need to be performed
with arbitrary precision, the use of revised and stable libraries such as [GMP]
is recommended.

See [main.cpp] for example of use.



# Features

 - Written in C++11
 - Dependencies free
 - All the classes and functions are defined inside the `bnl` namespace
 - Arbitrary precision integer class with all operators overloaded
 - Licensed under the [MIT license]


## The `bnl::integer` class

The `bnl::integer` class provides an arbitrary precision integer implementation.
The numerical data is stored in raw binary format by a self-managed dynamic
array of `std::uint64_t`, ordered from the least significant to the most
significant data block. Each block of data can store values in the interval
[0, 2<sup>32</sup>) simulating a base 2<sup>32</sup> - 1 representation.

The numeric data is converting from raw binary to `std::string` and from
`std::string` to raw binary with the Double-Dabble and Reverse Double-Dabble
algorithm respectively.

All methods are own implementations and optimal performance is not guaranteed.


### Helpful checker functions

| Function      | Description                                                   |
| ------------- | ------------------------------------------------------------- |
| `bnl::iseven` | Returns whether the value is even                             |
| `bnl::isneg`  | Returns whether the value is negative                         |
| `bnl::isodd`  | Returns whether the value is odd                              |
| `bnl::isone`  | Returns whether the value equals to one, positive or negative |
| `bnl::ispos`  | Returns whether the value is positive                         |
| `bnl::iszero` | Returns whether the value equals to zero                      |


### Helpful arithmetic functions

| Function            | Description                                             |
| ------------------- | ------------------------------------------------------- |
| `bnl::abs`          | Returns the absolute value of a number                  |
| `bnl::max`          | Returns largest number between two numbers              |
| `bnl::min`          | Returns smaller number between two numbers              |
| `bnl::pow`          | Returns the given number raised to the given exponent   |
| `bnl::integer::div` | Returns the quotient and remainder of integer division* |

*See [`bnl::div_t`] for more details.


### Miscelaneous functions

| Function                  | Description                                                         |
| ------------------------- | ------------------------------------------------------------------- |
| `bnl::integer::checkstr`  | Check if a string has a valid decimal representation and process it |
| `bnl::integer::precision` | Returns the number presicion                                        |


### Constructors

| Constructor         | Description                                                 |
| ------------------- | ----------------------------------------------------------- |
| Default             | Creates a new instance initialized to zero                  |
| Copy                | Creates a new instance from already existing `bnl::integer` |
| From `std::string`  | Creates a new instance from `std::string`                   |
| From `const char *` | Creates a new instance from `const char *`                  |
| From `long double`  | Creates a new instance from `long double`                   |


### Operators

The operator are listing by descending precedence order.


#### Type conversion operators

| Operator      | Description                 |
| ------------- | --------------------------- |
| `std::string` | Conversion to `std::string` |


#### Increment and decrement operators

| Operator      | Description                  |
| ------------- | ---------------------------- |
| `++`          | Prefix and postfix increment |
| `--`          | Prefix and postfix decrement |


#### Unary operators

| Operator      | Description                    |
| ------------- | ------------------------------ |
| `+`           | Integer promotion              |
| `-`           | Additive inverse               |
| `!`           | Logical NOT                    |
| `~`           | Bitwise NOT (one's complement) |


#### Arithmetic operators

| Operator      | Description                    |
| ------------- | ------------------------------ |
| `*`           | Multiplication                 |
| `/`           | Division                       |
| `%`           | Modulo                         |
| `+`           | Addition                       |
| `-`           | Subtraction                    |
| `<<`          | Left shift, also used for I/O  |
| `>>`          | Right shift, also used for I/O |


#### Comparison and relational operators

| Operator      | Description              |
| ------------- | ------------------------ |
| `>`           | Greater than             |
| `<`           | Less than                |
| `>=`          | Greater than or equal to |
| `<=`          | Less than or equal to    |
| `==`          | Equal to                 |
| `!=`          | Not equal to             |


#### Bitwise operators

| Operator      | Description                |
| ------------- | -------------------------- |
| `&`           | Bitwise AND                |
| `\|`          | Bitwise OR (inclusive or)  |
| `^`           | Bitwise XOR (exclusive or) |


#### Logic operators

| Operator      | Description |
| ------------- | ----------- |
| `&&`          | Logical AND |
| `\|\|`        | Logical OR  |


#### Assignment operators

| Operator      | Description                   |
| ------------- | ----------------------------- |
| `=`           | Direct assignation            |
| `+=`          | Assignation by addition       |
| `-=`          | Assignation by subtraction    |
| `*=`          | Assignation by multiplication |
| `/=`          | Assignation by division       |
| `%=`          | Assignation by modulo         |
| `<<=`         | Assignation by left shift     |
| `>>=`         | Assignation by right shift    |
| `&=`          | Assignation by bitwise AND    |
| `\|=`         | Assignation by bitwise OR     |
| `^=`          | Assignation by bitwise XOR    |


## The `bnl::div_t` struct

Similar to `std::div_t` defined in `cmath`, represents the result value of an
integral division performed by function `bnl::integer::div`.

It has two int members: `quot` and `rem`.

| Member | Description                                       |
| ------ | ------------------------------------------------- |
| `quot` | Represents the quotient of the integral division  |
| `rem`  | Represents the remainder of the integral division |



# Requirements and considerations

Since the code is written in C++11 standard and is dependencies free, can be
compiled by most compilers. To ensure the portability, the integer data types of
fixed width ptovided by `cstdint` have been used, but other data types sizes
like `long double` have been assumed. The assumed data types sizes are in to the
next table.

| Data type           | Size in bytes |
| ------------------- | ------------: |
| `std::uint8_t`      |             1 |
| `std::uint32_t`     |             4 |
| `std::uint64_t`     |             8 |
| `long double`       |            16 |


[//]: # "Links references"

[GMP]: https://gmplib.org/
[main.cpp]: src/main.cpp
[MIT license]: LICENSE
[`bnl::div_t`]: #the-bnldiv_t-struct
