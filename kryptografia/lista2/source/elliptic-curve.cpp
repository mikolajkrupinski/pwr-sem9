#include "elliptic-curve.hpp"

#include <stdexcept>

#include "fpk-element.hpp"

ECPoint EllipticCurve::add(const ECPoint& P, const ECPoint& Q) const {
  if (P.isInfinity) return Q;
  if (Q.isInfinity) return P;

  if (P == Q) {
    return doublePoint(P);
  }

  if (P.x == Q.x) {
    return ECPoint();
  }

  FpkElement numerator = Q.y - P.y;
  FpkElement denominator = Q.x - P.x;

  FpkElement lambda = numerator / denominator;

  FpkElement x3 = (lambda * lambda) - P.x - Q.x;

  FpkElement y3 = (lambda * (P.x - x3)) - P.y;

  return ECPoint(x3, y3);
}

ECPoint EllipticCurve::doublePoint(const ECPoint& P) const {
  if (P.isInfinity) {
    return P;
  }

  FpkElement zero({0});
  if (P.y == zero) {
    return ECPoint();
  }

  FpkElement two({2});
  FpkElement three({3});

  FpkElement numerator = (three * (P.x * P.x)) + a;

  FpkElement denominator = two * P.y;

  FpkElement lambda = numerator / denominator;

  FpkElement x3 = (lambda * lambda) - (two * P.x);

  FpkElement y3 = (lambda * (P.x - x3)) - P.y;

  return ECPoint(x3, y3);
}

ECPoint EllipticCurve::multiply(const ECPoint& P, num_t k) const {
  if (k == 0 || P.isInfinity) {
    return ECPoint();
  }

  ECPoint temp = P;

  ECPoint result = ECPoint();

  num_t scalar = k;

  while (scalar > 0) {
    if (scalar % 2 == 1) {
      result = add(result, temp);
    }

    temp = doublePoint(temp);

    scalar = scalar / 2;
  }

  return result;
}

std::ostream& operator<<(std::ostream& os, const EllipticCurve& ec) {
  os << "(" << ec.a << ", " << ec.b << ")";
  return os;
}

std::istream& operator>>(std::istream& is, EllipticCurve& ec) {
  std::string temp;
  is >> temp;

  if (temp.front() != '(' || temp.back() != ')') {
    throw std::runtime_error("Invalid FpkElement format");
  }
  temp = temp.substr(1, temp.size() - 2);

  size_t start = 0;
  size_t mid = temp.find(',') + 1;
  size_t end = temp.size() - 1;

  std::string x_str = temp.substr(start, mid - 1);
  std::istringstream x_stream(x_str);
  FpkElement x;
  x_stream >> x;

  std::string y_str = temp.substr(mid, end - mid + 1);
  std::istringstream y_stream(y_str);
  FpkElement y;
  y_stream >> y;

  ec.x = x;
  ec.y = y;

  return is;
}
