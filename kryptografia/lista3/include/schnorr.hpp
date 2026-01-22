#ifndef SCHNORR_HPP
#define SCHNORR_HPP

#include <openssl/sha.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "common.hpp"
#include "crypto-utils.hpp"
#include "diffie-hellman.hpp"
#include "elliptic-curve-fp.hpp"
#include "elliptic-curve.hpp"
#include "elliptic-curve2.hpp"
#include "f2k-element.hpp"
#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"

struct SchnorrSignature {
  num_t s;
  num_t e;

  std::string toStr() const { return "s=" + s.str() + ", e=" + e.str(); }
};

class SchnorrUtils {
 public:
  static num_t hashSha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(),
           hash);

    num_t result;
    boost::multiprecision::import_bits(result, hash,
                                       hash + SHA256_DIGEST_LENGTH);
    return result;
  }

  static std::string toHex(const num_t& val, size_t hex_len) {
    std::stringstream ss;
    std::string raw_hex = encodeNum(val, Encoding::BASE_16);

    if (raw_hex.length() < hex_len) {
      ss << std::string(hex_len - raw_hex.length(), '0');
    } else if (raw_hex.length() > hex_len) {
      throw std::invalid_argument("Value length exceeds expected hex length.");
    }
    ss << raw_hex;
    return ss.str();
  }

  static size_t getHexLenFp(const num_t& p) {
    size_t bits = msb(p) + 1;
    size_t bytes = (bits + 7) / 8;
    return bytes * 2;
  }

  static size_t getHexLenF2k(size_t degree) {
    size_t bytes = (degree + 7) / 8;
    return bytes * 2;
  }

  static std::string encode(const FpElement& el) {
    size_t len = getHexLenFp(el.getContext()->getP());
    return "\"" + toHex(el.getValue(), len) + "\"";
  }

  static std::string encode(const F2kElement& el) {
    num_t poly = el.getContext()->getF2kPoly();
    size_t m = 0;
    if (poly > 0) m = msb(poly);

    size_t len = getHexLenF2k(m);
    return "\"" + toHex(el.getBitString(), len) + "\"";
  }

  static std::string encode(const FpkElement& el) {
    std::stringstream ss;
    ss << "[";
    const auto& coeffs = el.getCoefficients();
    size_t k = el.getContext()->getPoly().size() - 1;
    size_t len = getHexLenFp(el.getContext()->getP());

    for (size_t i = 0; i < k; ++i) {
      num_t val = 0;
      if (i < coeffs.size()) {
        val = coeffs[i].getValue();
      }
      ss << "\"" << toHex(val, len) << "\"";
      if (i < k - 1) ss << ",";
    }
    ss << "]";
    return ss.str();
  }

  template <typename PointT>
  static std::string encodePoint(const PointT& P) {
    if (P.isInfinity) return "null";
    std::stringstream ss;
    ss << "{\"x\":" << encode(P.x) << ",\"y\":" << encode(P.y) << "}";
    return ss.str();
  }
};

template <typename ElementT>
class SchnorrField {
 private:
  ElementT g;
  num_t q;

 public:
  SchnorrField(const ElementT& gen, const num_t& order) : g(gen), q(order) {}

  DHKeyPair<ElementT> keyGen() {
    num_t x = generateSecureRandom(1, q - 1);
    ElementT y = power(g, x);
    return {x, y};
  }

  SchnorrSignature sign(const std::string& msg, const num_t& secretKey) {
    num_t k = generateSecureRandom(1, q - 1);

    ElementT R = power(g, k);

    std::string encodedR = SchnorrUtils::encode(R);
    std::string input = encodedR + msg;
    num_t e_hash = SchnorrUtils::hashSha256(input);

    num_t e = e_hash % q;

    num_t xe = (secretKey * e) % q;
    num_t s = k;
    if (s < xe) s += q;
    s = (s - xe) % q;

    return {s, e};
  }

  bool verify(const std::string& msg, const ElementT& publicKey,
              const SchnorrSignature& sig) {
    if (sig.s >= q || sig.e >= q) return false;

    ElementT gs = power(g, sig.s);
    ElementT ye = power(publicKey, sig.e);
    ElementT R_prime = gs * ye;

    std::string encodedR = SchnorrUtils::encode(R_prime);
    std::string input = encodedR + msg;
    num_t e_hash_prime = SchnorrUtils::hashSha256(input);
    num_t e_prime = e_hash_prime % q;

    return sig.e == e_prime;
  }
};

template <typename CurveT, typename PointT>
class SchnorrCurve {
 private:
  CurveT curve;
  PointT G;
  num_t q;

 public:
  SchnorrCurve(const CurveT& c, const PointT& gen, const num_t& order)
      : curve(c), G(gen), q(order) {}

  DHKeyPair<PointT> keyGen() {
    num_t x = generateSecureRandom(1, q - 1);
    PointT Y = curve.multiply(G, x);
    return {x, Y};
  }

  SchnorrSignature sign(const std::string& msg, const num_t& secretKey) {
    num_t k = generateSecureRandom(1, q - 1);

    PointT R = curve.multiply(G, k);

    std::string encodedR = SchnorrUtils::encodePoint(R);
    std::string input = encodedR + msg;

    num_t e = SchnorrUtils::hashSha256(input) % q;

    num_t xe = (secretKey * e) % q;
    num_t s = k;
    if (s < xe) s += q;
    s = (s - xe) % q;

    return {s, e};
  }

  bool verify(const std::string& msg, const PointT& publicKey,
              const SchnorrSignature& sig) {
    if (sig.s >= q || sig.e >= q) return false;

    PointT sG = curve.multiply(G, sig.s);
    PointT eY = curve.multiply(publicKey, sig.e);
    PointT R_prime = curve.add(sG, eY);

    std::string encodedR = SchnorrUtils::encodePoint(R_prime);
    std::string input = encodedR + msg;
    num_t e_prime = SchnorrUtils::hashSha256(input) % q;

    return sig.e == e_prime;
  }
};

#endif  // SCHNORR_HPP