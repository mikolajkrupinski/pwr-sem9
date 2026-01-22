#include "elliptic-curve2.hpp"

#include <iostream>

ECPointF2k EllipticCurveF2k::doublePoint(const ECPointF2k& P) const {
  if (P.isInfinity) {
    return P;
  }

  FieldPtr ctx = P.x.getContext();
  F2kElement zero(ctx, 0);

  if (P.x == zero) {
    return ECPointF2k();
  }

  F2kElement lambda = P.x + (P.y / P.x);

  F2kElement lambda_sq = lambda * lambda;
  F2kElement x3 = lambda_sq + lambda + a;

  F2kElement x1_plus_x3 = P.x + x3;
  F2kElement lambda_times_x1_plus_x3 = lambda * x1_plus_x3;
  F2kElement y3 = lambda_times_x1_plus_x3 + x3 + P.y;

  return ECPointF2k(x3, y3);
}

ECPointF2k EllipticCurveF2k::add(const ECPointF2k& P,
                                 const ECPointF2k& Q) const {
  if (P.isInfinity) return Q;
  if (Q.isInfinity) return P;

  if (P == Q) {
    return doublePoint(P);
  }

  FieldPtr ctx = P.x.getContext();
  F2kElement zero(ctx, 0);

  F2kElement x_sum = P.x + Q.x;

  if (x_sum == zero) {
    return ECPointF2k();
  }

  F2kElement numerator = P.y + Q.y;
  F2kElement denominator = x_sum;

  F2kElement lambda = numerator / denominator;

  F2kElement lambda_sq = lambda * lambda;
  F2kElement x3 = lambda_sq + lambda + x_sum + a;

  F2kElement x1_plus_x3 = P.x + x3;
  F2kElement lambda_times_x1_plus_x3 = lambda * x1_plus_x3;
  F2kElement y3 = lambda_times_x1_plus_x3 + x3 + P.y;

  return ECPointF2k(x3, y3);
}

ECPointF2k EllipticCurveF2k::multiply(const ECPointF2k& P, num_t k) const {
  ECPointF2k temp = P;
  ECPointF2k result = ECPointF2k();

  num_t scalar = k;

  if (scalar == 0 || P.isInfinity) {
    return ECPointF2k();
  }

  while (scalar > 0) {
    if (scalar % 2 == 1) {
      result = add(result, temp);
    }
    temp = doublePoint(temp);
    scalar = scalar / 2;
  }
  return result;
}

void ECPointF2k::fromStr(const std::string& str) {
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

void EllipticCurveF2k::fromStr(const std::string& str) {
  size_t delimiter_pos = str.find(';');
  std::string a_str = str.substr(0, delimiter_pos);
  std::string b_str = str.substr(delimiter_pos + 1);

  a.fromStr(a_str);
  b.fromStr(b_str);
}