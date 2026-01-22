#ifndef F2K_ELEMENT_HPP
#define F2K_ELEMENT_HPP

#include <iostream>
#include <memory>
#include <stdexcept>

#include "common.hpp"
#include "field.hpp"

class F2kElement;

F2kElement inverse(const F2kElement& fe);
F2kElement power(const F2kElement& fe, const num_t& exp);

class F2kElement {
 private:
  num_t bit_string;
  FieldPtr context;

  static num_t bitPolynomialMultiply(const num_t& a, const num_t& b);
  static num_t bitPolynomialReduce(num_t product, const num_t& modulus);
  static num_t bitPolynomialDivide(num_t dividend, const num_t& divisor,
                                   num_t& quotient);

  void checkContext(const F2kElement& other) const {
    if (context != other.context && *context != *other.context) {
      throw std::invalid_argument(
          "Operation on elements from different fields (F2k).");
    }
  }

 public:
  F2kElement() : bit_string(0), context(nullptr) {}

  F2kElement(FieldPtr ctx, const num_t& initial_value = num_t(0))
      : bit_string(initial_value), context(ctx) {}

  F2kElement(FieldPtr ctx, unsigned long long initial_value)
      : bit_string(num_t(initial_value)), context(ctx) {}

  const num_t& getBitString() const { return bit_string; }
  FieldPtr getContext() const { return context; }

  void setBitString(const num_t& _bit_string) { bit_string = _bit_string; }

  size_t degree() const {
    size_t deg = 0;
    num_t temp = bit_string;
    while (temp > 0) {
      temp >>= 1;
      deg++;
    }
    return deg;
  }

  F2kElement operator+(const F2kElement& fe) const;
  F2kElement operator-() const;
  F2kElement operator-(const F2kElement& fe) const;
  F2kElement operator*(const F2kElement& fe) const;
  F2kElement operator/(const F2kElement& fe) const;

  F2kElement& operator+=(const F2kElement& fe);
  F2kElement& operator-=(const F2kElement& fe);
  F2kElement& operator*=(const F2kElement& fe);
  F2kElement& operator/=(const F2kElement& fe);
  F2kElement& operator=(const F2kElement& fe);

  bool operator==(const F2kElement& fe) const {
    return bit_string == fe.bit_string;
  }
  bool operator!=(const F2kElement& fe) const {
    return bit_string != fe.bit_string;
  }

  friend F2kElement inverse(const F2kElement& fe);
  friend F2kElement power(const F2kElement& fe, const num_t& exp);

  std::string toStr() const {
    return encodeNum(this->bit_string, currentEncoding);
  }
  void fromStr(const std::string& str) {
    this->bit_string = decodeNum(str, currentEncoding);
  }
};

#endif  // F2K_ELEMENT_HPP