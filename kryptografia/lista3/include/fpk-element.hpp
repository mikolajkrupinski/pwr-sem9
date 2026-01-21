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

  // --- Metody pomocnicze do arytmetyki wielomianów (Internal Polynomial
  // Arithmetic) ---

  void checkContext(const FpkElement& other) const {
    if (context != other.context && *context != *other.context) {
      throw std::invalid_argument(
          "Operation on elements from different fields (Fpk).");
    }
  }

  // Usuwa wiodące zera
  static void normalizePoly(std::vector<FpElement>& poly);

  // Sprawdza czy wielomian jest zerowy
  static bool isZeroPoly(const std::vector<FpElement>& poly);

  // Mnożenie dwóch wielomianów (bez redukcji modulo)
  static std::vector<FpElement> polyMultiply(const std::vector<FpElement>& a,
                                             const std::vector<FpElement>& b);

  // Dzielenie wielomianów z resztą: dividend = divisor * quotient + remainder
  static void polyDivide(std::vector<FpElement> dividend,
                         const std::vector<FpElement>& divisor,
                         std::vector<FpElement>& quotient,
                         std::vector<FpElement>& remainder);

  // Rozszerzony algorytm Euklidesa dla wielomianów: a*s + b*t = gcd(a,b)
  static void polyExtendedEuclidean(std::vector<FpElement> a,
                                    std::vector<FpElement> b,
                                    std::vector<FpElement>& s,
                                    std::vector<FpElement>& t);

  // Helper: odejmowanie surowych wielomianów
  static std::vector<FpElement> polySubtract(const std::vector<FpElement>& a,
                                             const std::vector<FpElement>& b);

  // Helper: mnożenie wielomianu przez skalar
  static std::vector<FpElement> polyScalarMultiply(
      const std::vector<FpElement>& poly, const FpElement& scalar);

  // Helper: konwersja num_t coeffs na FpElement coeffs
  static std::vector<FpElement> castNumToFpElements(
      FieldPtr ctx, const std::vector<num_t>& vec);

 public:
  FpkElement() : context(nullptr) {}

  FpkElement(FieldPtr ctx, const std::vector<FpElement>& _coefficients = {})
      : coefficients(_coefficients), context(ctx) {
    if (coefficients.empty()) {
      coefficients.push_back(FpElement(ctx, 0));
    } else {
      normalizePoly(coefficients);  // Upewniamy się, że postać jest
                                    // znormalizowana przy tworzeniu
    }

    // Walidacja kontekstu
    for (const auto& c : coefficients) {
      if (c.getContext() != ctx && *c.getContext() != *ctx) {
        throw std::invalid_argument(
            "Coefficient belongs to different field context");
      }
    }
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
    // Zakładamy, że wektory są zawsze znormalizowane
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