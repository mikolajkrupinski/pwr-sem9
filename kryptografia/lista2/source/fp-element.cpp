#include "fp-element.hpp"

FpElement FpElement::operator+(const FpElement& fe) const {
  return FpElement((this->value + fe.value) % Field::getP());
}

FpElement FpElement::operator-() const {
  if (this->value == 0) {
    return FpElement(0);
  }
  return FpElement(Field::getP() - this->value);
}

FpElement FpElement::operator-(const FpElement& fe) const {
  return *this + (-fe);
}

FpElement FpElement::operator*(const FpElement& fe) const {
  return FpElement((this->value * fe.value) % Field::getP());
}

FpElement FpElement::operator/(const FpElement& fe) const {
  return *this * inverse(fe);
}

FpElement& FpElement::operator+=(const FpElement& fe) {
  *this = *this + fe;
  return *this;
}

FpElement& FpElement::operator-=(const FpElement& fe) {
  *this = *this - fe;
  return *this;
}

FpElement& FpElement::operator*=(const FpElement& fe) {
  *this = *this * fe;
  return *this;
}

FpElement& FpElement::operator/=(const FpElement& fe) {
  *this = *this / fe;
  return *this;
}

FpElement& FpElement::operator=(const FpElement& fe) {
  this->value = fe.value;
  return *this;
}

bool FpElement::operator<(const FpElement& fe) const {
  return this->value < fe.value;
}
bool FpElement::operator<=(const FpElement& fe) const {
  return this->value <= fe.value;
}
bool FpElement::operator>(const FpElement& fe) const {
  return this->value > fe.value;
}
bool FpElement::operator>=(const FpElement& fe) const {
  return this->value >= fe.value;
}
bool FpElement::operator==(const FpElement& fe) const {
  return this->value == fe.value;
}

FpElement inverse(const FpElement& fe) {
  if (fe.value == 0) {
    throw std::invalid_argument("Division by zero");
  }

  num_t r0 = Field::getP();
  num_t r1 = fe.value;
  num_t t0 = 0;
  num_t t1 = 1;

  while (r1 > 0) {
    num_t q = r0 / r1;

    num_t r_next = r0 - q * r1;
    r0 = r1;
    r1 = r_next;

    num_t t_next = t0 - q * t1;
    t0 = t1;
    t1 = t_next;
  }

  if (r0 != 1) {
    throw std::invalid_argument(
        "Error: Inverse does not exist (gcd is not 1).");
  }

  if (t0 < 0) {
    t0 = t0 + Field::getP();
  }

  return FpElement(t0);
}

FpElement power(const FpElement& fe, num_t exp) {
  FpElement base = fe;
  FpElement result(1);

  while (exp > 0) {
    if (exp % 2 == 1) {
      result *= base;
    }
    base *= base;
    exp /= 2;
  }
  return result;
}
