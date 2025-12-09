#ifndef FPK_ELEMENT_HPP
#define FPK_ELEMENT_HPP

#include <iostream>
#include <vector>

#include "common.hpp"
#include "field.hpp"
#include "fp-element.hpp"

class FpkElement;

FpkElement inverse(const FpkElement& fe);
FpkElement power(const FpkElement& fe, num_t exp);

std::ostream& operator<<(std::ostream& os, const FpkElement& fe);

class FpkElement {
 private:
  std::vector<FpElement> coefficients;

 public:
  FpkElement(const std::vector<FpElement>& _coefficients = {})
      : coefficients(_coefficients) {}

  const std::vector<FpElement>& getCoefficients() const { return coefficients; }

  void setCoefficients(const std::vector<FpElement>& _coefficients) {
    coefficients = _coefficients;
  }

  size_t degree() const {
    if (coefficients.empty()) {
      return 0;
    }
    return coefficients.size() - 1;
  }

  FpkElement operator+(const FpkElement& fe) const;
  FpkElement operator-() const;
  FpkElement operator-(const FpkElement& fe) const;
  FpkElement operator*(const FpkElement& fe) const;
  FpkElement operator/(const FpkElement& fe) const;

  FpkElement& operator+=(const FpkElement& fe);
  FpkElement& operator-=(const FpkElement& fe);
  FpkElement& operator*=(const FpkElement& fe);
  FpkElement& operator/=(const FpkElement& fe);
  FpkElement& operator=(const FpkElement& fe);

  bool operator==(const FpkElement& fe) const {
    return coefficients == fe.coefficients;
  }

  bool operator!=(const FpkElement& fe) const {
    return coefficients != fe.coefficients;
  }

  friend FpkElement inverse(const FpkElement& fe);
  friend FpkElement power(const FpkElement& fe, num_t exp);

  friend std::ostream& operator<<(std::ostream& os, const FpkElement& fe);
};

#endif  // FPK_ELEMENT_HPP