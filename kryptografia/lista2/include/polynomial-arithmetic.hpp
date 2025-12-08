#ifndef POLYNOMIAL_ARITHMETIC_HPP
#define POLYNOMIAL_ARITHMETIC_HPP

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "fp-element.hpp"

class PolynomialArithmetic {
 private:
  static void normalize(std::vector<FpElement>& poly) {
    while (poly.size() > 1 && poly.back() == 0) {
      poly.pop_back();
    }
  }

 public:
  static std::vector<FpElement> castNumToFpElements(
      const std::vector<num_t>& vec) {
    std::vector<FpElement> result;
    result.reserve(vec.size());
    for (const auto& val : vec) {
      result.emplace_back(FpElement(val));
    }
    return result;
  }

  static bool isZero(const std::vector<FpElement>& poly) {
    if (poly.empty()) return true;
    return poly.size() == 1 && poly[0] == 0;
  }

  static std::vector<FpElement> multiply(const std::vector<FpElement>& a,
                                         const std::vector<FpElement>& b) {
    if (isZero(a) || isZero(b)) {
      return {FpElement(0)};
    }

    size_t deg_a = a.size();
    size_t deg_b = b.size();
    size_t result_size = deg_a + deg_b - 1;

    std::vector<FpElement> result(result_size, FpElement(0));

    for (size_t i = 0; i < deg_a; ++i) {
      for (size_t j = 0; j < deg_b; ++j) {
        result[i + j] += a[i] * b[j];
      }
    }

    return result;
  }

  static void divide(std::vector<FpElement> dividend,
                     const std::vector<FpElement>& divisor,
                     std::vector<FpElement>& quotient,
                     std::vector<FpElement>& remainder) {
    std::vector<FpElement> div_copy = divisor;
    normalize(div_copy);

    if (isZero(div_copy)) {
      throw std::runtime_error("Division by zero polynomial");
    }

    int deg_divisor = div_copy.size() - 1;
    const FpElement& lc_divisor = div_copy.back();

    FpElement lc_divisor_inv = inverse(lc_divisor);

    normalize(dividend);

    int deg_dividend = dividend.size() - 1;
    if (deg_dividend < deg_divisor) {
      quotient = {FpElement(0)};
      remainder = dividend;
      return;
    }

    quotient.resize(deg_dividend - deg_divisor + 1, FpElement(0));

    for (int i = deg_dividend; i >= deg_divisor; --i) {
      const FpElement& lc_dividend = dividend.back();

      FpElement q_coeff = lc_dividend * lc_divisor_inv;

      int q_pos = i - deg_divisor;
      quotient[q_pos] = q_coeff;

      for (int j = 0; j <= deg_divisor; ++j) {
        dividend[j + q_pos] = dividend[j + q_pos] - (q_coeff * div_copy[j]);
      }

      dividend.pop_back();
      normalize(dividend);
    }

    remainder = dividend;
  }

  static void extendedEuclidean(std::vector<FpElement> a,
                                std::vector<FpElement> b,
                                std::vector<FpElement>& s,
                                std::vector<FpElement>& t) {
    std::vector<FpElement> r0 = a;
    std::vector<FpElement> r1 = b;
    std::vector<FpElement> s0 = {FpElement(1)};
    std::vector<FpElement> s1 = {FpElement(0)};

    std::vector<FpElement> q;

    while (!isZero(r1)) {
      std::vector<FpElement> remainder;

      divide(r0, r1, q, remainder);

      std::vector<FpElement> q_times_s1 = multiply(q, s1);
      std::vector<FpElement> s_new = polynomialSubtraction(s0, q_times_s1);

      r0 = r1;
      r1 = remainder;

      s0 = s1;
      s1 = s_new;
    }

    const FpElement& gcd_constant = r0[0];
    if (gcd_constant == 0) {
      throw std::runtime_error(
          "GCD is zero polynomial, inverse does not exist.");
    }

    FpElement gcd_inv = inverse(gcd_constant);

    s = scalarMultiply(s0, gcd_inv);
    t = {};
  }

 private:
  static std::vector<FpElement> polynomialSubtraction(
      const std::vector<FpElement>& a, const std::vector<FpElement>& b) {
    size_t max_size = std::max(a.size(), b.size());
    std::vector<FpElement> result(max_size);

    for (size_t i = 0; i < max_size; ++i) {
      const FpElement& coeff_a = (i < a.size()) ? a[i] : FpElement(0);
      const FpElement& coeff_b = (i < b.size()) ? b[i] : FpElement(0);
      result[i] = coeff_a - coeff_b;
    }

    normalize(result);
    return result;
  }

  static std::vector<FpElement> scalarMultiply(
      const std::vector<FpElement>& poly, const FpElement& scalar) {
    if (scalar == 0) {
      return {FpElement(0)};
    }

    std::vector<FpElement> result = poly;
    for (FpElement& coeff : result) {
      coeff = coeff * scalar;
    }
    return result;
  }
};

#endif  // POLYNOMIAL_ARITHMETIC_HPP