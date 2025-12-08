#ifndef FP_ELEMENT_HPP
#define FP_ELEMENT_HPP

#include <iostream>
#include <stdexcept>

#include "common.hpp"
#include "field.hpp"

class FpElement;

FpElement inverse(const FpElement& fe);
FpElement power(const FpElement& fe, num_t exp);

std::ostream& operator<<(std::ostream& os, const FpElement& fe);
std::istream& operator>>(std::istream& is, FpElement& fe);

class FpElement {
 private:
  num_t value;

 public:
  FpElement(long long _value) {
    this->value = num_t(_value) % Field::getP();
    if (value < 0) value += Field::getP();
  }

  FpElement(const num_t& _value = 0) {
    this->value = _value % Field::getP();
    if (value < 0) {
      value += Field::getP();
    }
  }

  num_t getValue() const { return value; }

  void setValue(const num_t& _value) {
    this->value = _value % Field::getP();
    if (this->value < 0) {
      this->value += Field::getP();
    }
  }

  FpElement operator+(const FpElement& fe) const;
  FpElement operator-() const;
  FpElement operator-(const FpElement& fe) const;
  FpElement operator*(const FpElement& fe) const;
  FpElement operator/(const FpElement& fe) const;

  FpElement& operator+=(const FpElement& fe);
  FpElement& operator-=(const FpElement& fe);
  FpElement& operator*=(const FpElement& fe);
  FpElement& operator/=(const FpElement& fe);
  FpElement& operator=(const FpElement& fe);

  bool operator<(const FpElement& fe) const;
  bool operator<=(const FpElement& fe) const;
  bool operator>(const FpElement& fe) const;
  bool operator>=(const FpElement& fe) const;
  bool operator==(const FpElement& fe) const;

  friend FpElement inverse(const FpElement& fe);
  friend FpElement power(const FpElement& fe, num_t exp);

  friend std::ostream& operator<<(std::ostream& os, const FpElement& fe);
  friend std::istream& operator>>(std::istream& is, FpElement& fe);
};

#endif  // FP_ELEMENT_HPP