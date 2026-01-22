#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
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
#include "ghash.hpp"
#include "schnorr.hpp"
#include "secure-ops.hpp"

inline num_t extractValue(const FpElement& e) { return e.getValue(); }

inline num_t extractValue(const F2kElement& e) { return e.getBitString(); }

inline num_t extractValue(const FpkElement& e) { return 0; }

std::string toPaddedHex(const num_t& val, const num_t& reference_val) {
  if (reference_val == 0) return encodeNum(val, Encoding::BASE_16);

  size_t bits = msb(reference_val) + 1;
  size_t bytes = (bits + 7) / 8;
  size_t expected_hex_len = bytes * 2;

  std::string raw = encodeNum(val, Encoding::BASE_16);

  if (raw.length() < expected_hex_len) {
    return std::string(expected_hex_len - raw.length(), '0') + raw;
  }
  return raw;
}

num_t promptNum(const std::string& msg) {
  std::cout << msg;
  std::string s;
  std::cin >> s;
  return decodeNum(s, currentEncoding);
}

std::shared_ptr<Field> createFieldInteractive(int choice) {
  std::cout << "\n--- Field Parameters Setup ---\n";
  std::cout << "Select Encoding [0=DEC, 1=HEX, 2=B64]: ";
  int enc;
  std::cin >> enc;
  currentEncoding = static_cast<Encoding>(enc);

  num_t p(0);
  std::vector<num_t> poly;
  num_t f2kPoly(0);

  if (choice == 1 || choice == 4) {
    p = promptNum("Enter Prime Modulus (p): ");
  } else if (choice == 2 || choice == 5) {
    p = 2;
    f2kPoly =
        promptNum("Enter Irreducible Polynomial (integer representation): ");
  } else if (choice == 3 || choice == 6) {
    p = promptNum("Enter Prime Modulus (p): ");
    std::cout << "Enter Irreducible Poly Coefficients (comma separated): ";
    std::string listStr;
    std::cin >> listStr;
    std::stringstream ss(listStr);
    std::string segment;
    while (std::getline(ss, segment, ',')) {
      poly.push_back(decodeNum(segment, currentEncoding));
    }
  }

  return std::make_shared<Field>(p, poly, f2kPoly);
}

num_t generateWithHammingWeight(size_t bits, bool heavy) {
  if (!heavy) {
    return (num_t(1) << (bits - 1)) | 1;
  } else {
    return (num_t(1) << bits) - 1;
  }
}

template <typename Func>
double measureAvgTime(Func f, int iterations) {
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iterations; ++i) {
    f();
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end - start;
  return elapsed.count() / iterations;
}

template <typename ElementT>
void runTimingTest_Field(FieldPtr ctx, const std::string& name) {
  std::cout << "\n--- Side-Channel Analysis: " << name << " ---\n";
  std::string g_str;
  std::cout << "Enter Generator g: ";
  std::cin >> g_str;
  ElementT g(ctx);
  g.fromStr(g_str);

  size_t bits = 256;
  if (name == "Fp" && ctx->getP() > 0) bits = msb(ctx->getP());

  int iterations = 100;

  num_t expLight = generateWithHammingWeight(bits, false);
  num_t expHeavy = generateWithHammingWeight(bits, true);

  std::cout << "Running benchmarks (" << iterations << " iterations)...\n";
  std::cout << "Exp Length: " << bits << " bits\n";

  double t_insecure_light =
      measureAvgTime([&]() { power(g, expLight); }, iterations);
  double t_insecure_heavy =
      measureAvgTime([&]() { power(g, expHeavy); }, iterations);

  double t_secure_light =
      measureAvgTime([&]() { powerSecure(g, expLight); }, iterations);
  double t_secure_heavy =
      measureAvgTime([&]() { powerSecure(g, expHeavy); }, iterations);

  std::cout << std::fixed << std::setprecision(4);
  std::cout << "\n[Classic Power]\n";
  std::cout << "  Light HW time: " << t_insecure_light << " ms\n";
  std::cout << "  Heavy HW time: " << t_insecure_heavy << " ms\n";
  std::cout << "  Diff Factor:   " << (t_insecure_heavy / t_insecure_light)
            << "x (Leakage!)\n";

  std::cout << "\n[Secure Power (Montgomery)]\n";
  std::cout << "  Light HW time: " << t_secure_light << " ms\n";
  std::cout << "  Heavy HW time: " << t_secure_heavy << " ms\n";
  std::cout << "  Diff Factor:   " << (t_secure_heavy / t_secure_light)
            << "x (Target: ~1.0)\n";
}

template <typename CurveT, typename PointT, typename CoordT>
void runTimingTest_Curve(FieldPtr ctx, const std::string& name) {
  std::cout << "\n--- Side-Channel Analysis: " << name << " ---\n";
  std::string a_str, b_str, G_str;
  std::cout << "Curve a: ";
  std::cin >> a_str;
  std::cout << "Curve b: ";
  std::cin >> b_str;
  CoordT a(ctx);
  a.fromStr(a_str);
  CoordT b(ctx);
  b.fromStr(b_str);
  CurveT curve(a, b);

  PointT G(CoordT(ctx, 0), CoordT(ctx, 0));
  std::cout << "Generator G (x;y): ";
  std::cin >> G_str;
  G.fromStr(G_str);

  size_t bits = 256;
  int iterations = 50;

  num_t scalarLight = generateWithHammingWeight(bits, false);
  num_t scalarHeavy = generateWithHammingWeight(bits, true);

  std::cout << "Running benchmarks (" << iterations << " iterations)...\n";

  double t_insecure_light =
      measureAvgTime([&]() { curve.multiply(G, scalarLight); }, iterations);
  double t_insecure_heavy =
      measureAvgTime([&]() { curve.multiply(G, scalarHeavy); }, iterations);

  double t_secure_light = measureAvgTime(
      [&]() { multiplySecure(curve, G, scalarLight); }, iterations);
  double t_secure_heavy = measureAvgTime(
      [&]() { multiplySecure(curve, G, scalarHeavy); }, iterations);

  std::cout << std::fixed << std::setprecision(4);
  std::cout << "\n[Classic Multiply]\n";
  std::cout << "  Light HW time: " << t_insecure_light << " ms\n";
  std::cout << "  Heavy HW time: " << t_insecure_heavy << " ms\n";
  std::cout << "  Diff Factor:   " << (t_insecure_heavy / t_insecure_light)
            << "x\n";

  std::cout << "\n[Secure Multiply (Montgomery)]\n";
  std::cout << "  Light HW time: " << t_secure_light << " ms\n";
  std::cout << "  Heavy HW time: " << t_secure_heavy << " ms\n";
  std::cout << "  Diff Factor:   " << (t_secure_heavy / t_secure_light)
            << "x\n";
}

void runGHASH_Interactive() {
  std::cout << "\n=== GHASH ADVERSARY TOOL ===\n";
  std::cout << "Select Encoding [0=DEC, 1=HEX, 2=B64]: ";
  int enc;
  std::cin >> enc;
  currentEncoding = static_cast<Encoding>(enc);

  std::string defaultPolyHex = "100000000000000000000000000000087";
  std::cout << "Use standard GHASH polynomial? [y/n]: ";
  char useStd;
  std::cin >> useStd;

  num_t f2kPoly;
  if (useStd == 'y' || useStd == 'Y') {
    f2kPoly = decodeNum(defaultPolyHex, Encoding::BASE_16);
  } else {
    f2kPoly = promptNum("Enter custom F2k Polynomial: ");
  }

  auto field = std::make_shared<Field>(2, std::vector<num_t>{}, f2kPoly);

  std::string h_str, A, C;
  std::cout << "Enter Auth Key H: ";
  std::cin >> h_str;
  F2kElement H(field);
  H.fromStr(h_str);

  std::cin.ignore();
  std::cout << "Enter AAD (ASCII string): ";
  std::getline(std::cin, A);
  std::cout << "Enter Ciphertext (ASCII string): ";
  std::getline(std::cin, C);

  F2kElement tag = calculateGHASH(H, A, C);

  num_t ref128 = (num_t(1) << 128) - 1;

  std::cout << ">>> Calculated TAG: " << toPaddedHex(tag.getBitString(), ref128)
            << std::endl;
}

template <typename ElementT>
void runDH_Interactive_Field(FieldPtr ctx, const std::string& name) {
  std::string g_str;
  std::cout << "--- DH " << name << " Setup ---\n";
  std::cout << "Generator g: ";
  std::cin >> g_str;
  num_t q = promptNum("Group Order q: ");

  ElementT g(ctx);
  g.fromStr(g_str);
  DHProtocolField<ElementT> dh(g, q);

  std::cout << "\n[1] YOUR KEY GENERATION\n";
  num_t sk = promptNum(
      "Enter your Secret Key (integer) OR type '0' to generate random: ");

  if (sk == 0) {
    sk = generateSecureRandom(1, q);
    std::cout << "-> Generated Random Secret Key: " << sk << std::endl;
  }

  ElementT myPK = power(g, sk);

  std::string myPK_hex = toPaddedHex(extractValue(myPK), ctx->getP());

  std::cout << "-> Your Public Key (PADDED - COPY TO JSON & MESSAGE): "
            << std::endl;
  std::cout << myPK_hex << std::endl;

  std::cout << "\n[2] KEY AGREEMENT\n";
  std::string peer_pk_str;
  std::cout << "Enter Peer's Public Key: ";
  std::cin >> peer_pk_str;

  ElementT peerPK(ctx);
  peerPK.fromStr(peer_pk_str);

  ElementT ss = dh.agree(sk, peerPK);

  std::string ss_hex = toPaddedHex(extractValue(ss), ctx->getP());
  std::cout << ">>> SHARED SECRET (PADDED - COPY TO JSON): " << std::endl;
  std::cout << ss_hex << std::endl;
}

template <typename CurveT, typename PointT, typename CoordT>
void runDH_Interactive_Curve(FieldPtr ctx, const std::string& name) {
  std::string a_str, b_str, G_str;
  std::cout << "--- DH EC (" << name << ") Setup ---\n";
  std::cout << "Curve Param a: ";
  std::cin >> a_str;
  std::cout << "Curve Param b: ";
  std::cin >> b_str;

  CoordT a(ctx);
  a.fromStr(a_str);
  CoordT b(ctx);
  b.fromStr(b_str);
  CurveT curve(a, b);

  PointT G(CoordT(ctx, 0), CoordT(ctx, 0));
  std::cout << "Generator G (x;y): ";
  std::cin >> G_str;
  G.fromStr(G_str);

  num_t q = promptNum("Group Order q: ");
  DHProtocolCurve<CurveT, PointT> dh(curve, G, q);

  std::cout << "\n[1] YOUR KEY GENERATION\n";
  num_t sk = promptNum(
      "Enter your Secret Key (integer) OR type '0' to generate random: ");
  if (sk == 0) {
    sk = generateSecureRandom(1, q);
    std::cout << "-> Generated Random Secret Key: " << sk << std::endl;
  }

  PointT myPK = curve.multiply(G, sk);
  std::cout << "-> Your Public Key: " << myPK.toStr() << std::endl;

  std::cout << "\n[2] KEY AGREEMENT\n";
  std::string peer_pk_str;
  std::cout << "Enter Peer's Public Key (x;y): ";
  std::cin >> peer_pk_str;

  PointT peerPK(CoordT(ctx, 0), CoordT(ctx, 0));
  peerPK.fromStr(peer_pk_str);

  PointT ss = dh.agree(sk, peerPK);
  std::cout << ">>> SHARED SECRET POINT: " << ss.toStr() << std::endl;
}

template <typename ElementT>
void runSchnorr_Interactive_Field(FieldPtr ctx, const std::string& name) {
  std::string g_str;
  std::cout << "--- Schnorr " << name << " Setup ---\n";
  std::cout << "Generator g: ";
  std::cin >> g_str;
  num_t q = promptNum("Group Order q: ");

  ElementT g(ctx);
  g.fromStr(g_str);
  SchnorrField<ElementT> schnorr(g, q);

  int action;
  std::cout << "Action: [1] Sign (as Prover), [2] Verify (as Verifier): ";
  std::cin >> action;
  std::cin.ignore();

  if (action == 1) {
    std::string msg;
    num_t sk = promptNum("Enter Secret Key (int): ");
    std::cin.ignore();

    std::cout << "Enter Message (e.g. your padded DH Public Key): ";
    std::getline(std::cin, msg);

    auto sig = schnorr.sign(msg, sk);

    std::string s_hex = toPaddedHex(sig.s, q);
    std::string e_hex = toPaddedHex(sig.e, q);

    std::cout << ">>> SIGNATURE generated (PADDED):\n";
    std::cout << "\"s\": \"" << s_hex << "\"\n";
    std::cout << "\"e\": \"" << e_hex << "\"\n";

  } else {
    std::string pk_str, msg;
    std::cout << "Enter Signer's Public Key: ";
    std::cin >> pk_str;
    ElementT pk(ctx);
    pk.fromStr(pk_str);

    num_t s = promptNum("Enter Signature s: ");
    num_t e = promptNum("Enter Signature e: ");

    std::cin.ignore();
    std::cout << "Enter Message: ";
    std::getline(std::cin, msg);

    SchnorrSignature sig{s, e};
    bool valid = schnorr.verify(msg, pk, sig);
    std::cout << ">>> VERIFICATION RESULT: " << (valid ? "VALID" : "INVALID")
              << std::endl;
  }
}

template <typename CurveT, typename PointT, typename CoordT>
void runSchnorr_Interactive_Curve(FieldPtr ctx, const std::string& name) {
  std::string a_str, b_str, G_str;
  std::cout << "--- Schnorr EC (" << name << ") Setup ---\n";
  std::cout << "Curve a: ";
  std::cin >> a_str;
  std::cout << "Curve b: ";
  std::cin >> b_str;

  CoordT a(ctx);
  a.fromStr(a_str);
  CoordT b(ctx);
  b.fromStr(b_str);
  CurveT curve(a, b);

  PointT G(CoordT(ctx, 0), CoordT(ctx, 0));
  std::cout << "Generator G (x;y): ";
  std::cin >> G_str;
  G.fromStr(G_str);

  num_t q = promptNum("Order q: ");
  SchnorrCurve<CurveT, PointT> schnorr(curve, G, q);

  int action;
  std::cout << "Action: [1] Sign (as Prover), [2] Verify (as Verifier): ";
  std::cin >> action;
  std::cin.ignore();

  if (action == 1) {
    std::string msg;
    num_t sk = promptNum("Enter Secret Key (int): ");
    std::cin.ignore();

    std::cout << "Enter Message: ";
    std::getline(std::cin, msg);

    auto sig = schnorr.sign(msg, sk);

    std::string s_hex = toPaddedHex(sig.s, q);
    std::string e_hex = toPaddedHex(sig.e, q);

    std::cout << ">>> SIGNATURE generated (PADDED):\n";
    std::cout << "\"s\": \"" << s_hex << "\"\n";
    std::cout << "\"e\": \"" << e_hex << "\"\n";

  } else {
    std::string pk_str, msg;
    std::cout << "Enter Signer's Public Key (x;y): ";
    std::cin >> pk_str;
    PointT pk(CoordT(ctx, 0), CoordT(ctx, 0));
    pk.fromStr(pk_str);

    num_t s = promptNum("Enter Signature s: ");
    num_t e = promptNum("Enter Signature e: ");

    std::cin.ignore();
    std::cout << "Enter Message: ";
    std::getline(std::cin, msg);

    SchnorrSignature sig{s, e};

    bool valid = schnorr.verify(msg, pk, sig);
    std::cout << ">>> VERIFICATION RESULT: " << (valid ? "VALID" : "INVALID")
              << std::endl;
  }
}

int main() {
  srand(time(NULL));

  while (true) {
    std::cout << "\n============================================\n";
    std::cout << "      ALGEBRAIC CRYPTO ADVERSARY TOOL       \n";
    std::cout << "============================================\n";
    std::cout << "1. GHASH\n";
    std::cout << "2. Diffie-Hellman Key Exchange\n";
    std::cout << "3. Schnorr Signature Scheme\n";
    std::cout << "4. Side-Channel Timing Analysis\n";
    std::cout << "0. Exit\n";
    std::cout << "Select mode: ";

    int mode;
    std::cin >> mode;
    if (mode == 0) break;
    if (mode == 1) {
      runGHASH_Interactive();
      continue;
    }

    std::cout << "\nSelect Algebraic Structure:\n";
    std::cout << "1. Fp (Classic)\n";
    std::cout << "2. F2k (Char 2)\n";
    std::cout << "3. Fpk (Extension)\n";
    std::cout << "4. Elliptic Curve over Fp\n";
    std::cout << "5. Elliptic Curve over F2k\n";
    std::cout << "6. Elliptic Curve over Fpk\n";
    int struct_choice;
    std::cin >> struct_choice;

    auto field = createFieldInteractive(struct_choice);
    if (!field) continue;

    if (mode == 2) {  // DH
      switch (struct_choice) {
        case 1:
          runDH_Interactive_Field<FpElement>(field, "Fp");
          break;
        case 2:
          runDH_Interactive_Field<F2kElement>(field, "F2k");
          break;
        case 3:
          runDH_Interactive_Field<FpkElement>(field, "Fpk");
          break;
        case 4:
          runDH_Interactive_Curve<EllipticCurveFp, ECPointFp, FpElement>(
              field, "EC-Fp");
          break;
        case 5:
          runDH_Interactive_Curve<EllipticCurveF2k, ECPointF2k, F2kElement>(
              field, "EC-F2k");
          break;
        case 6:
          runDH_Interactive_Curve<EllipticCurve, ECPoint, FpkElement>(field,
                                                                      "EC-Fpk");
          break;
      }
    } else if (mode == 3) {  // Schnorr
      switch (struct_choice) {
        case 1:
          runSchnorr_Interactive_Field<FpElement>(field, "Fp");
          break;
        case 2:
          runSchnorr_Interactive_Field<F2kElement>(field, "F2k");
          break;
        case 3:
          runSchnorr_Interactive_Field<FpkElement>(field, "Fpk");
          break;
        case 4:
          runSchnorr_Interactive_Curve<EllipticCurveFp, ECPointFp, FpElement>(
              field, "EC-Fp");
          break;
        case 5:
          runSchnorr_Interactive_Curve<EllipticCurveF2k, ECPointF2k,
                                       F2kElement>(field, "EC-F2k");
          break;
        case 6:
          runSchnorr_Interactive_Curve<EllipticCurve, ECPoint, FpkElement>(
              field, "EC-Fpk");
          break;
      }
    } else if (mode == 4) {  // Side-Channel Timing
      switch (struct_choice) {
        case 1:
          runTimingTest_Field<FpElement>(field, "Fp");
          break;
        case 2:
          runTimingTest_Field<F2kElement>(field, "F2k");
          break;
        case 3:
          runTimingTest_Field<FpkElement>(field, "Fpk");
          break;
        case 4:
          runTimingTest_Curve<EllipticCurveFp, ECPointFp, FpElement>(field,
                                                                     "EC-Fp");
          break;
        case 5:
          runTimingTest_Curve<EllipticCurveF2k, ECPointF2k, F2kElement>(
              field, "EC-F2k");
          break;
        case 6:
          runTimingTest_Curve<EllipticCurve, ECPoint, FpkElement>(field,
                                                                  "EC-Fpk");
          break;
      }
    }
  }

  return 0;
}