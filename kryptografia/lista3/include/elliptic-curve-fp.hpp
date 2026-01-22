#ifndef ELLIPTIC_CURVE_FP_HPP
#define ELLIPTIC_CURVE_FP_HPP

#include "fp-element.hpp"

class ECPointFp {
 public:
  FpElement x;
  FpElement y;
  bool isInfinity;

  ECPointFp() : isInfinity(true) {}

  ECPointFp(const FpElement& _x, const FpElement& _y)
      : x(_x), y(_y), isInfinity(false) {}

  bool operator==(const ECPointFp& other) const {
    if (isInfinity && other.isInfinity) {
      return true;
    }
    if (isInfinity || other.isInfinity) {
      return false;
    }
    return x == other.x && y == other.y;
  }

  bool operator!=(const ECPointFp& other) const { return !(*this == other); }

  std::string toStr() const {
    if (isInfinity) {
      return "INF;INF";
    }
    return x.toStr() + ";" + y.toStr();
  }

  void fromStr(const std::string& str);
};

class EllipticCurveFp {
 private:
  FpElement a;
  FpElement b;

 public:
  EllipticCurveFp(const FpElement& _a, const FpElement& _b) : a(_a), b(_b) {}

  const FpElement& getA() const { return a; }
  const FpElement& getB() const { return b; }
  FieldPtr getContext() const { return a.getContext(); }

  ECPointFp add(const ECPointFp& P, const ECPointFp& Q) const;
  ECPointFp doublePoint(const ECPointFp& P) const;
  ECPointFp multiply(const ECPointFp& P, num_t k) const;

  std::string toStr() const { return a.toStr() + ";" + b.toStr(); }
  void fromStr(const std::string& str);
};

#endif  // ELLIPTIC_CURVE_FP_HPP