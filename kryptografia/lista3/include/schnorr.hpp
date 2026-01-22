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
#include "diffie-hellman.hpp"  // Do generowania losowych nonce (k)
#include "elliptic-curve-fp.hpp"
#include "elliptic-curve.hpp"
#include "elliptic-curve2.hpp"
#include "f2k-element.hpp"
#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"

// Struktura podpisu
struct SchnorrSignature {
  num_t s;
  num_t e;

  std::string toStr() const { return "s=" + s.str() + ", e=" + e.str(); }
};

class SchnorrUtils {
 public:
  // Pomocnicza funkcja do SHA256
  static num_t hashSha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(),
           hash);

    // Konwersja bajtów na num_t
    num_t result = 0;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
      result = (result << 8) | hash[i];
    }
    return result;
  }

  // Pomocnicza funkcja do formatowania hex z paddingiem
  static std::string toHex(const num_t& val, size_t hex_len) {
    std::stringstream ss;
    // Konwersja num_t na hex string (bez '0x')
    // Boost cpp_int nie ma prostego manipulatora do fixed width hex, robimy to
    // ręcznie
    std::string raw_hex = encodeNum(val, Encoding::BASE_16);

    if (raw_hex.length() < hex_len) {
      ss << std::string(hex_len - raw_hex.length(), '0');
    } else if (raw_hex.length() > hex_len) {
      // Teoretycznie nie powinno się zdarzyć przy poprawnym modulo, ale dla
      // bezpieczeństwa (chyba że liczba ma wiodące zera które encodeNum ucina)
    }
    ss << raw_hex;
    return ss.str();
  }

  // Obliczanie długości hex dla Fp
  static size_t getHexLenFp(const num_t& p) {
    // Liczba bitów
    size_t bits = msb(p) + 1;
    // Zaokrąglenie do pełnych bajtów -> *2 dla hex
    size_t bytes = (bits + 7) / 8;
    return bytes * 2;
  }

  // Obliczanie długości hex dla F2k
  static size_t getHexLenF2k(size_t degree) {
    size_t bytes = (degree + 7) / 8;
    return bytes * 2;
  }

  // --- ENCODING RULES (a, b, c, d, e) ---

  // (a) Fp encoding
  static std::string encode(const FpElement& el) {
    size_t len = getHexLenFp(el.getContext()->getP());
    return "\"" + toHex(el.getValue(), len) + "\"";
  }

  // (b) F2k encoding
  static std::string encode(const F2kElement& el) {
    // Stopień wielomianu ciała to degree F2kPoly (najwyższy bit)
    num_t poly = el.getContext()->getF2kPoly();
    size_t m = 0;
    if (poly > 0) m = msb(poly);

    size_t len = getHexLenF2k(m);
    return "\"" + toHex(el.getBitString(), len) + "\"";
  }

  // (c) Fpk encoding
  static std::string encode(const FpkElement& el) {
    std::stringstream ss;
    ss << "[";
    const auto& coeffs = el.getCoefficients();
    // Musimy zebrać k elementów, uzupełniając zerami do stopnia ciała
    // Stopień rozszerzenia k to rozmiar wektora poly w Field - 1
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

  // (d) EC Point encoding (generic template)
  template <typename PointT>
  static std::string encodePoint(const PointT& P) {
    if (P.isInfinity) return "null";  // Edge case
    std::stringstream ss;
    ss << "{\"x\":" << encode(P.x) << ",\"y\":" << encode(P.y) << "}";
    return ss.str();
  }
};

// --- SCHEMAT SCHNORRA DLA CIAŁ (Fp, F2k, Fpk) ---
template <typename ElementT>
class SchnorrField {
 private:
  ElementT g;
  num_t q;

 public:
  SchnorrField(const ElementT& gen, const num_t& order) : g(gen), q(order) {}

  // KeyGen
  DHKeyPair<ElementT> keyGen() {
    num_t x = generateSecureRandom(1, q - 1);
    ElementT y = power(g, x);
    return {x, y};
  }

  // Sign
  SchnorrSignature sign(const std::string& msg, const num_t& secretKey) {
    // 1. k = random [1, q-1]
    num_t k = generateSecureRandom(1, q - 1);

    // 2. R = g^k
    ElementT R = power(g, k);

    // 3. e = H(Encode(R) || m)
    std::string encodedR = SchnorrUtils::encode(R);
    std::string input = encodedR + msg;
    num_t e_hash = SchnorrUtils::hashSha256(input);

    // e = e_hash mod q
    num_t e = e_hash % q;

    // 4. s = (k - x*e) mod q
    num_t xe = (secretKey * e) % q;
    num_t s = k;
    if (s < xe) s += q;  // Obsługa odejmowania modulo
    s = (s - xe) % q;

    return {s, e};
  }

  // Verify
  bool verify(const std::string& msg, const ElementT& publicKey,
              const SchnorrSignature& sig) {
    if (sig.s >= q || sig.e >= q) return false;

    // 1. R' = g^s * y^e
    ElementT gs = power(g, sig.s);
    ElementT ye = power(publicKey, sig.e);
    ElementT R_prime = gs * ye;

    // 2. e' = H(Encode(R') || m)
    std::string encodedR = SchnorrUtils::encode(R_prime);
    std::string input = encodedR + msg;
    num_t e_hash_prime = SchnorrUtils::hashSha256(input);
    num_t e_prime = e_hash_prime % q;

    return sig.e == e_prime;
  }
};

// --- SCHEMAT SCHNORRA DLA KRZYWYCH ELIPTYCZNYCH ---
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

    // R = [k]G
    PointT R = curve.multiply(G, k);

    // Encode EC point uses special overload
    std::string encodedR = SchnorrUtils::encodePoint(R);
    std::string input = encodedR + msg;

    num_t e = SchnorrUtils::hashSha256(input) % q;

    // s = (k - x*e) mod q
    num_t xe = (secretKey * e) % q;
    num_t s = k;
    if (s < xe) s += q;
    s = (s - xe) % q;

    return {s, e};
  }

  bool verify(const std::string& msg, const PointT& publicKey,
              const SchnorrSignature& sig) {
    if (sig.s >= q || sig.e >= q) return false;

    // R' = [s]G + [e]Y
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