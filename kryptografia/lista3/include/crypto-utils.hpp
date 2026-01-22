#ifndef CRYPTO_UTILS_HPP
#define CRYPTO_UTILS_HPP

#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>
#include <random>  // Dodano dla std::random_device

#include "common.hpp"

// 1. Bezpieczne generowanie liczby losowej z zakresu [min, max]
inline num_t generateSecureRandom(const num_t& min, const num_t& max) {
  if (min >= max) return min;

  // ZMIANA: Używamy standardowego std::random_device
  std::random_device rd;

  // Generator Boost zasilany ziarnem ze standardowego urządzenia losowego
  boost::random::mt19937 gen(rd());
  boost::random::uniform_int_distribution<num_t> dist(min, max);

  return dist(gen);
}

// 2. Generowanie bezpiecznej liczby pierwszej o zadanej liczbie bitów
inline num_t generateLargePrime(unsigned int bits) {
  // ZMIANA: Używamy standardowego std::random_device
  std::random_device rd;
  boost::random::mt19937 gen(rd());

  // Generujemy liczbę z zakresu [2^(bits-1), 2^bits - 1]
  num_t min = num_t(1) << (bits - 1);
  num_t max = (num_t(1) << bits) - 1;

  boost::random::uniform_int_distribution<num_t> dist(min, max);

  while (true) {
    num_t candidate = dist(gen);

    // Musi być nieparzysta
    if (candidate % 2 == 0) candidate += 1;
    if (candidate > max) candidate -= 2;

    // Test Millera-Rabina (25 powtórzeń)
    if (boost::multiprecision::miller_rabin_test(candidate, 25)) {
      return candidate;
    }
  }
}

#endif  // CRYPTO_UTILS_HPP