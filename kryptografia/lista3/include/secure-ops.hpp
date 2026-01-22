#ifndef SECURE_OPS_HPP
#define SECURE_OPS_HPP

#include <boost/multiprecision/cpp_int.hpp>

#include "common.hpp"
#include "field.hpp"

inline bool getBit(const num_t& n, size_t pos) {
  return boost::multiprecision::bit_test(n, pos);
}

template <typename T>
T powerSecure(T base, num_t exp) {
  T r0(base.getContext(), 1);
  T r1 = base;

  long bits = msb(exp);

  for (long i = bits; i >= 0; --i) {
    bool bit = getBit(exp, i);

    if (!bit) {
      // Case 0: R1 = R0 * R1, R0 = R0^2
      r1 = r0 * r1;
      r0 = r0 * r0;
    } else {
      // Case 1: R0 = R0 * R1, R1 = R1^2
      r0 = r0 * r1;
      r1 = r1 * r1;
    }
  }
  return r0;
}

template <typename CurveT, typename PointT>
PointT multiplySecure(CurveT& curve, PointT P, num_t scalar) {
  PointT r0 = P;
  r0.isInfinity = true;
  PointT r1 = P;

  long bits = msb(scalar);

  for (long i = bits; i >= 0; --i) {
    bool bit = getBit(scalar, i);

    if (!bit) {
      // Case 0: R1 = R0 + R1, R0 = 2*R0
      r1 = curve.add(r0, r1);
      r0 = curve.add(r0, r0);
    } else {
      // Case 1: R0 = R0 + R1, R1 = 2*R1
      r0 = curve.add(r0, r1);
      r1 = curve.add(r1, r1);
    }
  }
  return r0;
}

#endif  // SECURE_OPS_HPP