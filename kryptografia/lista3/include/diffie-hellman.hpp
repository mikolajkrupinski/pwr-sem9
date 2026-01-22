#ifndef DIFFIE_HELLMAN_HPP
#define DIFFIE_HELLMAN_HPP

#include <cstdlib>
#include <iostream>

#include "common.hpp"
#include "crypto-utils.hpp"

template <typename T>
struct DHKeyPair {
  num_t secretKey;
  T publicKey;
};

template <typename ElementType>
class DHProtocolField {
 private:
  ElementType g;
  num_t q;

 public:
  DHProtocolField(const ElementType& generator, const num_t& order)
      : g(generator), q(order) {}

  DHKeyPair<ElementType> generateKeys() {
    num_t sk = generateSecureRandom(1, q);
    ElementType pk = power(g, sk);
    return {sk, pk};
  }

  ElementType agree(const num_t& mySecretKey,
                    const ElementType& otherPublicKey) {
    return power(otherPublicKey, mySecretKey);
  }
};

template <typename CurveType, typename PointType>
class DHProtocolCurve {
 private:
  CurveType curve;
  PointType G;
  num_t q;

 public:
  DHProtocolCurve(const CurveType& _curve, const PointType& _generator,
                  const num_t& _order)
      : curve(_curve), G(_generator), q(_order) {}

  DHKeyPair<PointType> generateKeys() {
    num_t sk = generateSecureRandom(1, q);
    PointType pk = curve.multiply(G, sk);
    return {sk, pk};
  }

  PointType agree(const num_t& mySecretKey, const PointType& otherPublicKey) {
    return curve.multiply(otherPublicKey, mySecretKey);
  }
};

#endif  // DIFFIE_HELLMAN_HPP