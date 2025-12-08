#include "f2k-element.hpp"

F2kElement F2kElement::operator+(const F2kElement& fe) const {
  return F2kElement(this->bit_string ^ fe.bit_string);
}

F2kElement F2kElement::operator-() const { return *this; }

F2kElement F2kElement::operator-(const F2kElement& fe) const {
  return *this + fe;
}

F2kElement F2kElement::operator*(const F2kElement& fe) const {
  // TODO: Implement multiplication in F2^k
}

F2kElement F2kElement::operator/(const F2kElement& fe) const {
  return *this * inverse(fe);
}

F2kElement& F2kElement::operator+=(const F2kElement& fe) {
  *this = *this + fe;
  return *this;
}

F2kElement& F2kElement::operator-=(const F2kElement& fe) {
  *this = *this - fe;
  return *this;
}

F2kElement& F2kElement::operator*=(const F2kElement& fe) {
  *this = *this * fe;
  return *this;
}

F2kElement& F2kElement::operator/=(const F2kElement& fe) {
  *this = *this / fe;
  return *this;
}

F2kElement& F2kElement::operator=(const F2kElement& fe) {
  this->bit_string = fe.bit_string;
  return *this;
}

F2kElement inverse(const F2kElement& fe) {
  // TODO: Implement inversion in F2^k
}

F2kElement power(const F2kElement& fe, const num_t& exp) {
  if (exp == 0) {
    return 1;
  }

  if (fe.bit_string == 0) {
    return 0;
  }

  num_t current_exp = exp;
  F2kElement result = 1;
  F2kElement base = fe;

  while (current_exp > 0) {
    if (current_exp % 2 == 1) {
      result = result * base;
    }
    base *= base;
    current_exp *= current_exp;
  }

  return result;
}