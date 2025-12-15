#ifndef ELLIPTIC_CURVE_F2K_HPP
#define ELLIPTIC_CURVE_F2K_HPP

#include "f2k-element.hpp"

class ECPointF2k {
 public:
  F2kElement x;
  F2kElement y;
  bool isInfinity;

  ECPointF2k() : isInfinity(true) {}
  ECPointF2k(const F2kElement& _x, const F2kElement& _y)
      : x(_x), y(_y), isInfinity(false) {}

  bool operator==(const ECPointF2k& other) const {
    if (isInfinity && other.isInfinity) {
      return true;
    }
    if (isInfinity || other.isInfinity) {
      return false;
    }
    return x == other.x && y == other.y;
  }

  bool operator!=(const ECPointF2k& other) const { return !(*this == other); }

  std::string toStr() const {
    if (isInfinity) {
      return "INF;INF";
    }
    return x.toStr() + ";" + y.toStr();
  }

  void fromStr(const std::string& str);
};

class EllipticCurveF2k {
 private:
  F2kElement a;
  F2kElement b;

 public:
  EllipticCurveF2k(const F2kElement& _a, const F2kElement& _b) : a(_a), b(_b) {}

  const F2kElement& getA() const { return a; }
  const F2kElement& getB() const { return b; }

  ECPointF2k negate(const ECPointF2k& P) const {
    if (P.isInfinity) return P;
    F2kElement new_y = P.y + P.x;
    return ECPointF2k(P.x, new_y);
  }

  ECPointF2k add(const ECPointF2k& P, const ECPointF2k& Q) const;
  ECPointF2k doublePoint(const ECPointF2k& P) const;
  ECPointF2k multiply(const ECPointF2k& P, num_t k) const;

  std::string toStr() const { return a.toStr() + ";" + b.toStr(); }
  void fromStr(const std::string& str);
};

#endif  // ELLIPTIC_CURVE_F2K_HPP