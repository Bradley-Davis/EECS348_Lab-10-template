#ifndef __CALCULATOR_HPP
#define __CALCULATOR_HPP

#include <string>

// this function should expect that `expression` has already been
// validated and thus throw no exceptions
double parse_number(const std::string &expression);

#endif // __CALCULATOR_HPP

std::pair<char, char> add(char first, char second, char carry);
std::pair<char, char> sub(char first, char second, char carry);