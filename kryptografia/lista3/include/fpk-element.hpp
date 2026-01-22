#ifndef FPK_ELEMENT_HPP
#define FPK_ELEMENT_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "common.hpp"
#include "field.hpp"
#include "fp-element.hpp"

class FpkElement;

FpkElement inverse(const FpkElement& fe);
FpkElement power(const FpkElement& fe, num_t exp);

class FpkElement {
 private:
  std::vector<FpElement> coefficients;
  FieldPtr context;

  void checkContext(const FpkElement& other) const {
    if (context != other.context && *context != *other.context) {
      throw std::invalid_argument(
          "Operation on elements from different fields (Fpk).");
    }
  }

  static void normalizePoly(std::vector<FpElement>& poly);

  static bool isZeroPoly(const std::vector<FpElement>& poly);

  static std::vector<FpElement> polyMultiply(const std::vector<FpElement>& a,
                                             const std::vector<FpElement>& b);

  static void polyDivide(std::vector<FpElement> dividend,
                         const std::vector<FpElement>& divisor,
                         std::vector<FpElement>& quotient,
                         std::vector<FpElement>& remainder);

  static void polyExtendedEuclidean(std::vector<FpElement> a,
                                    std::vector<FpElement> b,
                                    std::vector<FpElement>& s,
                                    std::vector<FpElement>& t);

  static std::vector<FpElement> polySubtract(const std::vector<FpElement>& a,
                                             const std::vector<FpElement>& b);

  static std::vector<FpElement> polyScalarMultiply(
      const std::vector<FpElement>& poly, const FpElement& scalar);

  static std::vector<FpElement> castNumToFpElements(
      FieldPtr ctx, const std::vector<num_t>& vec);

 public:
  FpkElement() : context(nullptr) {}

  FpkElement(FieldPtr ctx, const std::vector<FpElement>& _coefficients = {})
      : coefficients(_coefficients), context(ctx) {
    if (coefficients.empty()) {
      coefficients.push_back(FpElement(ctx, 0));
    } else {
      normalizePoly(coefficients);
    }

    for (const auto& c : coefficients) {
      if (c.getContext() != ctx && *c.getContext() != *ctx) {
        throw std::invalid_argument(
            "Coefficient belongs to different field context");
      }
    }
  }

  FpkElement(FieldPtr ctx, long long val) : context(ctx) {
    coefficients = {FpElement(ctx, val)};
    normalizePoly(coefficients);
  }

  FpkElement(FieldPtr ctx, const num_t& val) : context(ctx) {
    coefficients = {FpElement(ctx, val)};
    normalizePoly(coefficients);
  }

  const std::vector<FpElement>& getCoefficients() const { return coefficients; }
  FieldPtr getContext() const { return context; }

  void setCoefficients(const std::vector<FpElement>& _coefficients) {
    coefficients = _coefficients;
    normalizePoly(coefficients);
  }

  size_t degree() const {
    if (coefficients.empty()) return 0;
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

  std::string toStr() const;
  void fromStr(const std::string& str);
};

#endif  // FPK_ELEMENT_HPP