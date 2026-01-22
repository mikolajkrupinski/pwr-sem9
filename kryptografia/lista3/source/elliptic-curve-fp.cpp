#include "elliptic-curve-fp.hpp"

#include <stdexcept>

ECPointFp EllipticCurveFp::add(const ECPointFp& P, const ECPointFp& Q) const {
  if (P.isInfinity) return Q;
  if (Q.isInfinity) return P;

  if (P == Q) {
    return doublePoint(P);
  }

  if (P.x == Q.x) {
    return ECPointFp();
  }

  FpElement numerator = Q.y - P.y;
  FpElement denominator = Q.x - P.x;

  FpElement lambda = numerator / denominator;

  FpElement x3 = (lambda * lambda) - P.x - Q.x;
  FpElement y3 = (lambda * (P.x - x3)) - P.y;

  return ECPointFp(x3, y3);
}

ECPointFp EllipticCurveFp::doublePoint(const ECPointFp& P) const {
  if (P.isInfinity) {
    return P;
  }

  FieldPtr ctx = P.x.getContext();
  FpElement zero(ctx, 0);

  if (P.y == zero) {
    return ECPointFp();
  }

  FpElement two(ctx, 2);
  FpElement three(ctx, 3);

  FpElement numerator = (three * (P.x * P.x)) + this->a;
  FpElement denominator = two * P.y;

  FpElement lambda = numerator / denominator;

  FpElement x3 = (lambda * lambda) - (two * P.x);
  FpElement y3 = (lambda * (P.x - x3)) - P.y;

  return ECPointFp(x3, y3);
}

ECPointFp EllipticCurveFp::multiply(const ECPointFp& P, num_t k) const {
  if (k == 0 || P.isInfinity) {
    return ECPointFp();
  }

  ECPointFp temp = P;
  ECPointFp result = ECPointFp();

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

void ECPointFp::fromStr(const std::string& str) {
  if (str == "INF;INF") {
    isInfinity = true;
    return;
  }

  size_t delimiter_pos = str.find(';');
  std::string x_str = str.substr(0, delimiter_pos);
  std::string y_str = str.substr(delimiter_pos + 1);

  x.fromStr(x_str);
  y.fromStr(y_str);
  isInfinity = false;
}

void EllipticCurveFp::fromStr(const std::string& str) {
  size_t delimiter_pos = str.find(';');
  std::string a_str = str.substr(0, delimiter_pos);
  std::string b_str = str.substr(delimiter_pos + 1);

  a.fromStr(a_str);
  b.fromStr(b_str);
}