#include "elliptic-curve2.hpp"

#include <iostream>

ECPointF2k EllipticCurveF2k::doublePoint(const ECPointF2k& P) const {
  if (P.isInfinity) {
    return P;
  }

  F2kElement zero(0);
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

  F2kElement zero(0);
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

std::ostream& operator<<(std::ostream& os, const EllipticCurveF2k& ec) {
  os << "(" << ec.a << ", " << ec.b << ")";
  return os;
}

std::istream& operator>>(std::istream& is, EllipticCurveF2k& ec) {
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
