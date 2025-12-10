#include "f2k-element.hpp"

num_t F2kElement::bitPolynomialMultiply(const num_t& a, const num_t& b) {
  if (a == 0 || b == 0) {
    return 0;
  }

  num_t result = 0;
  size_t deg_b = F2kElement(b).degree();

  for (size_t i = 0; i <= deg_b; i++) {
    if ((b >> i) & 1) {
      result = result ^ (a << i);
    }
  }
  return result;
}

num_t F2kElement::bitPolynomialDivide(num_t dividend, const num_t& divisor,
                                      num_t& quotient) {
  if (divisor == 0) {
    throw std::runtime_error("Dzielnik nie może być zerowym wielomianem.");
  }

  quotient = 0;
  if (dividend == 0) {
    return 0;
  }

  num_t one(1);
  ssize_t deg_dividend = F2kElement(dividend).degree();
  ssize_t deg_divisor = F2kElement(divisor).degree();

  while (dividend != 0 && deg_dividend >= deg_divisor) {
    size_t shift = deg_dividend - deg_divisor;

    quotient = quotient ^ (one << shift);

    dividend = dividend ^ (divisor << shift);

    if (dividend != 0) {
      deg_dividend = static_cast<ssize_t>(F2kElement(dividend).degree());
    } else {
      break;
    }
  }

  return dividend;
}

num_t F2kElement::bitPolynomialReduce(num_t product, const num_t& modulus) {
  if (modulus == 0) {
    throw std::runtime_error("Moduł (m(x)) nie może być zerowy.");
  }

  size_t deg_mod = F2kElement(modulus).degree();
  size_t deg_prod = F2kElement(product).degree();

  while (deg_prod >= deg_mod) {
    size_t shift = deg_prod - deg_mod;
    product = product ^ (modulus << shift);
    deg_prod = F2kElement(product).degree();
  }

  return product;
}

F2kElement F2kElement::operator+(const F2kElement& fe) const {
  return F2kElement(this->bit_string ^ fe.bit_string);
}

F2kElement F2kElement::operator-() const { return *this; }

F2kElement F2kElement::operator-(const F2kElement& fe) const {
  return *this + fe;
}

F2kElement F2kElement::operator*(const F2kElement& fe) const {
  num_t product_bits = bitPolynomialMultiply(this->bit_string, fe.bit_string);
  num_t result_bits = bitPolynomialReduce(product_bits, Field::getF2kPoly());
  return F2kElement(result_bits);
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
  if (fe.bit_string == 0) {
    throw std::runtime_error(
        "Nie można obliczyć odwrotności elementu zerowego w F2k.");
  }

  num_t a = fe.bit_string;

  num_t r0 = Field::getF2kPoly();
  num_t r1 = a;
  num_t s0 = 0;
  num_t s1 = 1;
  num_t quotient;

  while (r1 != 0) {
    num_t r_new = F2kElement::bitPolynomialDivide(r0, r1, quotient);

    num_t q_times_s1 = F2kElement::bitPolynomialMultiply(quotient, s1);

    num_t s_new = s0 ^ q_times_s1;

    r0 = r1;
    r1 = r_new;

    s0 = s1;
    s1 = s_new;
  }

  if (r0 != 1) {
    throw std::runtime_error("Element nie jest odwracalny: GCD != 1.");
  }

  return F2kElement(s0);
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
    current_exp /= current_exp;
  }

  return result;
}

std::ostream& operator<<(std::ostream& os, const F2kElement& fe) {
  os << encodeNum(fe.bit_string, currentEncoding);
  return os;
}

std::istream& operator>>(std::istream& is, F2kElement& fe) {
  std::string temp;
  is >> temp;
  fe.setBitString(decodeNum(temp, currentEncoding));
  return is;
}