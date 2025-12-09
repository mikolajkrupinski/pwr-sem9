#ifndef FIELD_HPP
#define FIELD_HPP

#include <vector>

#include "common.hpp"

class Field {
 private:
  static num_t p;                  // prime modulus
  static std::vector<num_t> poly;  // coefficients of the irreducible polynomial
  static num_t f2kPoly;            // irreducible polynomial for F2^k

 public:
  Field() = delete;

  static void initialize(const num_t& prime, const std::vector<num_t>& _poly,
                         const num_t& _f2kPoly) {
    p = prime;
    poly = _poly;
    f2kPoly = _f2kPoly;
  }

  static num_t getP() { return p; }
  static const std::vector<num_t>& getPoly() { return poly; }
  static num_t getF2kPoly() { return f2kPoly; }
};

#endif  // FIELD_HPP