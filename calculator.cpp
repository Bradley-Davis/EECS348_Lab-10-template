#include "calculator.hpp"
#include <string>
#include <iostream>
#include <fstream> // file reading library

double parse_number(const std::string &expression) {
    return std::stod(expression);
}

std::pair<char, char> add(char first, char second, char carry) {
    int num1 = first - '0';
    int num2 = second - '0';
    int carryIn = carry - '0';

    int sum = num1 + num2 + carryIn;
    char result = (sum % 10) + '0';
    char carryOut = (sum / 10) + '0';

    return {result, carryOut};
}

std::pair<char, char> sub(char first, char second, char carry) {
    int num1 = (first - '0');
    int num2 = (second - '0');

    // If carry == '2', that means we borrowed from the previous digit
    if (carry == '2') {
        num1 -= 1;
    }

    char carryOut = '0';
    if (num1 < num2) {
        num1 += 10;
        carryOut = '2';
    }
    int diff = num1 - num2;
    char result = (diff % 10) + '0';

    return {result, carryOut};
}