#include "fpk-element.hpp"

#include "polynomial-arithmetic.hpp"

FpkElement FpkElement::operator+(const FpkElement& fe) const {
  size_t k = std::max(coefficients.size(), fe.coefficients.size());
  std::vector<FpElement> result_coeffs(k);

  for (size_t i = 0; i < k; ++i) {
    const FpElement& a =
        (i < coefficients.size()) ? coefficients[i] : FpElement(0);
    const FpElement& b =
        (i < fe.coefficients.size()) ? fe.coefficients[i] : FpElement(0);
    result_coeffs[i] = a + b;
  }

  return FpkElement(result_coeffs);
}

FpkElement FpkElement::operator-() const {
  std::vector<FpElement> result_coeffs = coefficients;

  for (FpElement& coeff : result_coeffs) {
    coeff = -coeff;
  }
  return FpkElement(result_coeffs);
}

FpkElement FpkElement::operator-(const FpkElement& fe) const {
  return *this + (-fe);
}

FpkElement FpkElement::operator*(const FpkElement& fe) const {
  std::vector<FpElement> product_coeffs =
      PolynomialArithmetic::multiply(this->coefficients, fe.coefficients);

  std::vector<FpElement> quotient_coeffs;
  std::vector<FpElement> remainder_coeffs;
  PolynomialArithmetic::divide(
      product_coeffs,
      PolynomialArithmetic::castNumToFpElements(Field::getPoly()),
      quotient_coeffs, remainder_coeffs);

  return FpkElement(remainder_coeffs);
}

FpkElement FpkElement::operator/(const FpkElement& fe) const {
  return *this * inverse(fe);
}

FpkElement& FpkElement::operator+=(const FpkElement& fe) {
  *this = *this + fe;
  return *this;
}

FpkElement& FpkElement::operator-=(const FpkElement& fe) {
  *this = *this - fe;
  return *this;
}

FpkElement& FpkElement::operator*=(const FpkElement& fe) {
  *this = *this * fe;
  return *this;
}

FpkElement& FpkElement::operator/=(const FpkElement& fe) {
  *this = *this / fe;
  return *this;
}

FpkElement& FpkElement::operator=(const FpkElement& fe) {
  this->coefficients = fe.coefficients;
  return *this;
}

FpkElement inverse(const FpkElement& fe) {
  if (PolynomialArithmetic::isZero(fe.coefficients)) {
    throw std::runtime_error("Division by zero");
  }

  std::vector<FpElement> s, t;

  PolynomialArithmetic::extendedEuclidean(
      fe.coefficients,
      PolynomialArithmetic::castNumToFpElements(Field::getPoly()), s, t);

  return FpkElement(s);
}

FpkElement power(const FpkElement& fe, num_t exp) {
  if (exp == 0) {
    return FpkElement({1});
  }

  if (PolynomialArithmetic::isZero(fe.coefficients)) {
    return fe;
  }

  FpkElement result = FpkElement({1});
  FpkElement base = fe;

  while (exp > 0) {
    if (exp % 2 == 1) {
      result *= base;
    }
    base *= base;
    exp /= 2;
  }

  return result;
}

std::ostream& operator<<(std::ostream& os, const FpkElement& fe) {
  os << "[";
  for (size_t i = 0; i < fe.coefficients.size(); ++i) {
    os << fe.coefficients[i];
    if (i != fe.coefficients.size() - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}
