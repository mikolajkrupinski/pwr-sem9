#ifndef FIELD_HPP
#define FIELD_HPP

#include <memory>
#include <vector>

#include "common.hpp"

class Field {
 private:
  num_t p;                  // prime modulus
  std::vector<num_t> poly;  // coefficients of the irreducible polynomial
  num_t f2kPoly;            // irreducible polynomial for F2^k

 public:
  Field(const num_t& prime, const std::vector<num_t>& _poly,
        const num_t& _f2kPoly)
      : p(prime), poly(_poly), f2kPoly(_f2kPoly) {}

  num_t getP() const { return p; }
  const std::vector<num_t>& getPoly() const { return poly; }
  num_t getF2kPoly() const { return f2kPoly; }

  // Sprawdzenie czy dwa ciała są matematycznie identyczne
  bool operator==(const Field& other) const {
    return p == other.p && poly == other.poly && f2kPoly == other.f2kPoly;
  }
  bool operator!=(const Field& other) const { return !(*this == other); }
};

using FieldPtr = std::shared_ptr<Field>;

#endif  // FIELD_HPP