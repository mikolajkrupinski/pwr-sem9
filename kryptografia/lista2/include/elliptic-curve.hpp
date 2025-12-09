#ifndef EC_POINT_HPP
#define EC_POINT_HPP

#include "fpk-element.hpp"

class ECPoint {
 public:
  FpkElement x;
  FpkElement y;
  bool isInfinity;

  ECPoint() : isInfinity(true) {}
  ECPoint(const FpkElement& _x, const FpkElement& _y)
      : x(_x), y(_y), isInfinity(false) {}

  bool operator==(const ECPoint& other) const {
    if (isInfinity && other.isInfinity) {
      return true;
    }
    if (isInfinity || other.isInfinity) {
      return false;
    }
    return x == other.x && y == other.y;
  }

  bool operator!=(const ECPoint& other) const { return !(*this == other); }
};

class EllipticCurve {
 private:
  FpkElement a;
  FpkElement b;

 public:
  EllipticCurve(const FpkElement& _a, const FpkElement& _b) : a(_a), b(_b) {}

  const FpkElement& getA() const { return a; }
  const FpkElement& getB() const { return b; }

  ECPoint add(const ECPoint& P, const ECPoint& Q) const;
  ECPoint doublePoint(const ECPoint& P) const;
  ECPoint multiply(const ECPoint& P, num_t k) const;
};

#endif  // EC_POINT_HPP