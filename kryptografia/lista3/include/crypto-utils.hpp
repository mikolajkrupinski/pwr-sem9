#ifndef CRYPTO_UTILS_HPP
#define CRYPTO_UTILS_HPP

#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>
#include <random>

#include "common.hpp"

inline num_t generateSecureRandom(const num_t& min, const num_t& max) {
  if (min >= max) return min;

  std::random_device rd;

  boost::random::mt19937 gen(rd());
  boost::random::uniform_int_distribution<num_t> dist(min, max);

  return dist(gen);
}

inline num_t generateLargePrime(unsigned int bits) {
  std::random_device rd;
  boost::random::mt19937 gen(rd());

  num_t min = num_t(1) << (bits - 1);
  num_t max = (num_t(1) << bits) - 1;

  boost::random::uniform_int_distribution<num_t> dist(min, max);

  while (true) {
    num_t candidate = dist(gen);

    if (candidate % 2 == 0) candidate += 1;
    if (candidate > max) candidate -= 2;

    if (boost::multiprecision::miller_rabin_test(candidate, 25)) {
      return candidate;
    }
  }
}

#endif  // CRYPTO_UTILS_HPP