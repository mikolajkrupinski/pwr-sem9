#ifndef DIFFIE_HELLMAN_HPP
#define DIFFIE_HELLMAN_HPP

#include <cstdlib>  // dla rand()
#include <iostream>

#include "common.hpp"

// Prosty generator losowy dla num_t (do celów edukacyjnych używamy rand)
// W produkcji należałoby użyć generatora kryptograficznego
inline num_t generatePrivateKey(const num_t& q) {
  // Generujemy liczbę z zakresu [1, q-1]
  // Uwaga: dla bardzo dużych q, rand() nie pokryje całego zakresu,
  // ale wystarczy do demonstracji mechanizmu.
  return (num_t(rand()) % (q - 1)) + 1;
}

// Struktura przechowująca parę kluczy
template <typename T>
struct DHKeyPair {
  num_t secretKey;
  T publicKey;
};

// --- IMPLEMENTACJA DLA CIAŁ SKOŃCZONYCH (Fp, F2k, Fpk) ---
// Wymaga, aby typ T miał zdefiniowaną funkcję power(base, exp)

template <typename ElementType>
class DHProtocolField {
 private:
  ElementType g;  // Generator
  num_t q;        // Rząd grupy

 public:
  DHProtocolField(const ElementType& generator, const num_t& order)
      : g(generator), q(order) {}

  // Faza 1: Generowanie kluczy (sk, pk = g^sk)
  DHKeyPair<ElementType> generateKeys() {
    num_t sk = generatePrivateKey(q);
    ElementType pk = power(g, sk);
    return {sk, pk};
  }

  // Faza 2: Uzgadnianie sekretu (ss = other_pk^sk)
  ElementType agree(const num_t& mySecretKey,
                    const ElementType& otherPublicKey) {
    return power(otherPublicKey, mySecretKey);
  }
};

// --- IMPLEMENTACJA DLA KRZYWYCH ELIPTYCZNYCH ---
// Wymaga obiektu Curve do wykonania mnożenia

template <typename CurveType, typename PointType>
class DHProtocolCurve {
 private:
  CurveType curve;
  PointType G;  // Generator point
  num_t q;      // Rząd punktu G

 public:
  DHProtocolCurve(const CurveType& _curve, const PointType& _generator,
                  const num_t& _order)
      : curve(_curve), G(_generator), q(_order) {}

  // Faza 1: Generowanie kluczy (sk, PK = [sk]G)
  DHKeyPair<PointType> generateKeys() {
    num_t sk = generatePrivateKey(q);
    PointType pk = curve.multiply(G, sk);
    return {sk, pk};
  }

  // Faza 2: Uzgadnianie sekretu (SS = [sk]OtherPK)
  PointType agree(const num_t& mySecretKey, const PointType& otherPublicKey) {
    return curve.multiply(otherPublicKey, mySecretKey);
  }
};

#endif  // DIFFIE_HELLMAN_HPP