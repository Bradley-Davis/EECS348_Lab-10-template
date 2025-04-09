#include "calculator.hpp"
#include <iostream>
#include <fstream>
#include <string>

// Checks if a character is a digit (without <cctype>)
bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

// Manual validation of a double-format string
bool isValidDouble(const std::string& str) {
    if (str.empty()) return false;

    size_t i = 0;
    bool hasDot = false;
    bool hasDigitsBefore = false;
    bool hasDigitsAfter = false;

    // Optional sign
    if (str[i] == '+' || str[i] == '-') {
        i++;
        if (i >= str.size()) return false; // sign only is invalid
    }

    // Digits before dot
    while (i < str.size() && isDigit(str[i])) {
        hasDigitsBefore = true;
        i++;
    }

    // Decimal point
    if (i < str.size() && str[i] == '.') {
        hasDot = true;
        i++;
        while (i < str.size() && isDigit(str[i])) {
            hasDigitsAfter = true;
            i++;
        }

        // If there's a dot, we need digits either before or after the dot
        if (!hasDigitsAfter && !hasDigitsBefore) return false;
    }

    // No extra characters allowed
    if (i != str.size()) return false;

    // Must have digits either before or after decimal
    return hasDigitsBefore || hasDigitsAfter;
}

// Pads and aligns two double strings
void normalizeDoubles(std::string& s1, std::string& s2) {
    auto splitParts = [](const std::string& s) {
        std::string sign;
        std::string intPart;
        std::string fracPart;
        size_t i = 0;

        // If the string starts with '+' or '-', extract it; otherwise, default to '+'
        if (s[0] == '+' || s[0] == '-') {
            sign = s.substr(0, 1);
            i = 1;
        } else {
            sign = "+";
        }

        size_t dotPos = s.find('.', i);
        if (dotPos != std::string::npos) {
            intPart = s.substr(i, dotPos - i);
            fracPart = s.substr(dotPos + 1);
        } else {
            intPart = s.substr(i);
        }

        return std::make_tuple(sign, intPart, fracPart);
    };

    auto [sign1, int1, frac1] = splitParts(s1);
    auto [sign2, int2, frac2] = splitParts(s2);

    // Determine maximum lengths for integer and fractional parts.
    size_t maxIntLen = (int1.length() > int2.length()) ? int1.length() : int2.length();
    size_t maxFracLen = (frac1.length() > frac2.length()) ? frac1.length() : frac2.length();

    // Pad integer parts with leading zeros.
    int1 = std::string(maxIntLen - int1.length(), '0') + int1;
    int2 = std::string(maxIntLen - int2.length(), '0') + int2;

    // Pad fractional parts with trailing zeros.
    frac1 += std::string(maxFracLen - frac1.length(), '0');
    frac2 += std::string(maxFracLen - frac2.length(), '0');

    // Reconstruct the numbers ensuring that both strings have the same sign, integer, and fractional lengths.
    s1 = sign1 + int1 + (maxFracLen > 0 ? "." + frac1 : "");
    s2 = sign2 + int2 + (maxFracLen > 0 ? "." + frac2 : "");
}

// Convert string to double using std::stod (for a “correct” numeric check)
double expected(const std::string& start) {
    double number = parse_number(start);
    double test = -123.456;   // We are effectively adding -123.456
    return number + test;
}

int main() {
    std::ifstream file("input.txt");
    std::string line;
    std::string reference = "-123.456";  // We will do: line + (-123.456)

    if (!file) {
        std::cout << "Failed to open input.txt\n";
        return 1;
    }

    while (std::getline(file, line)) {
        // Remove trailing carriage return if present (for Windows files)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (isValidDouble(line)) {
            // Make copies to manipulate
            std::string refCopy = reference;
            std::string lineCopy = line;

            // Align the two strings to have the same number of integer/fraction digits
            normalizeDoubles(lineCopy, refCopy);

            // Identify sign of each
            bool lineNegative = (lineCopy[0] == '-');
            bool refNegative  = (refCopy[0] == '-');

            // Remove the leading sign symbol so that digit-by-digit add/sub is simpler
            if (lineNegative || lineCopy[0] == '+') {
                lineCopy[0] = '0';
            }
            if (refNegative || refCopy[0] == '+') {
                refCopy[0] = '0';
            }

            // We'll build our result in `result`
            std::string result(lineCopy.size(), '0');
            char carry = '0';

            // Case 1: same sign => add absolute values
            if (lineNegative == refNegative) {
                for (int i = (int)lineCopy.size() - 1; i >= 0; --i) {
                    if (lineCopy[i] == '.') {
                        result[i] = '.';
                        continue;
                    }
                    auto temp = add(lineCopy[i], refCopy[i], carry);
                    result[i] = temp.first;
                    carry     = temp.second;
                }
                if (carry == '1') {
                    // Insert the leftover carry at front
                    result.insert(result.begin(), '1');
                }
                // Re-insert the sign
                if (lineNegative) {
                    result.insert(result.begin(), '-');
                } 
                // (Or insert '+', if you want explicit sign for positives)

            } else {
                // Case 2: different signs => subtract absolute values
                // Compare absolute values to see which is bigger
                bool lineIsLarger = false;

                for (size_t i = 0; i < lineCopy.size(); i++) {
                    if (lineCopy[i] == '.') continue;
                    if (refCopy[i] == '.') continue;

                    if (lineCopy[i] > refCopy[i]) {
                        lineIsLarger = true;
                        break;
                    } else if (lineCopy[i] < refCopy[i]) {
                        break;
                    }
                }

                const std::string &big   = (lineIsLarger ? lineCopy : refCopy);
                const std::string &small = (lineIsLarger ? refCopy : lineCopy);

                for (int i = (int)big.size() - 1; i >= 0; --i) {
                    if (big[i] == '.') {
                        result[i] = '.';
                        continue;
                    }
                    auto temp = sub(big[i], small[i], carry);
                    result[i] = temp.first;
                    carry     = temp.second;
                }

                // Final sign matches the absolute bigger
                bool finalNegative;
                if (lineIsLarger) {
                    finalNegative = lineNegative; 
                } else {
                    finalNegative = refNegative;
                }
                if (finalNegative) {
                    result.insert(result.begin(), '-');
                }
            }

            // Print results
            std::cout << "Raw input line:    " << line << "\n";
            std::cout << "String operation:  " << result << "\n";
            std::cout << "Double arithmetic: " << expected(line) << "\n\n";
        } else {
            std::cout << "Invalid double: " << line << "\n\n";
        }
    }

    return 0;
}