#ifndef FIELD_HPP
#define FIELD_HPP

#include <vector>

#include "common.hpp"

class Field {
 private:
  static num_t p;                  // prime modulus
  static std::vector<num_t> poly;  // coefficients of the irreducible polynomial

 public:
  Field() = delete;

  static void initialize(const num_t& prime, const std::vector<num_t>& _poly) {
    p = prime;
    poly = _poly;
  }

  static num_t getP() { return p; }
  static const std::vector<num_t>& getPoly() { return poly; }
};

#endif  // FIELD_HPP