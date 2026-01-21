#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common.hpp"
#include "diffie-hellman.hpp"
#include "elliptic-curve-fp.hpp"
#include "elliptic-curve.hpp"
#include "elliptic-curve2.hpp"
#include "f2k-element.hpp"
#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"
#include "ghash.hpp"

std::shared_ptr<Field> createFieldFromStr(const std::string& str);

void testGHash();

void testDH_Fp(FieldPtr ctx);
void testDH_F2k(FieldPtr ctx);
void testDH_Fpk(FieldPtr ctx);

template <typename CurveT, typename PointT, typename ElementT>
void runCurveDH(FieldPtr ctx, const std::string& typeName);

int main() {
  // testGHash();

  srand(time(NULL));  // Seed dla RNG

  std::cout << "Choose structure:\n";
  std::cout << "1. Classic Fp\n";
  std::cout << "2. Characteristic 2 field F2k\n";
  std::cout << "3. Extended prime field Fpk\n";
  std::cout << "4. EC over Fp\n";
  std::cout << "5. EC over F2k\n";
  std::cout << "6. EC over Fpk\n";

  int choice;
  std::cin >> choice;

  std::cout << "Enter Field Params (encoding;p;deg;poly_coeffs;f2k_poly): ";
  std::string params_str;
  std::cin >> params_str;
  FieldPtr field = createFieldFromStr(params_str);

  try {
    switch (choice) {
      case 1:
        testDH_Fp(field);
        break;
      case 2:
        testDH_F2k(field);
        break;
      case 3:
        testDH_Fpk(field);
        break;
      case 4:
        runCurveDH<EllipticCurveFp, ECPointFp, FpElement>(field, "Fp");
        break;
      case 5:
        runCurveDH<EllipticCurveF2k, ECPointF2k, F2kElement>(field, "F2k");
        break;
      case 6:
        runCurveDH<EllipticCurve, ECPoint, FpkElement>(field, "Fpk");
        break;
      default:
        std::cout << "Invalid choice\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}

std::shared_ptr<Field> createFieldFromStr(const std::string& str) {
  size_t pos = 0;
  size_t next_pos = str.find(';', pos);

  Encoding encoding =
      static_cast<Encoding>(std::stoi(str.substr(pos, next_pos - pos)));
  currentEncoding = encoding;

  pos = next_pos + 1;
  next_pos = str.find(';', pos);
  num_t prime_modulus =
      decodeNum(str.substr(pos, next_pos - pos), currentEncoding);

  pos = next_pos + 1;
  next_pos = str.find(';', pos);
  // size_t degree_stub = std::stoul(str.substr(pos, next_pos - pos)); // degree
  // (nieużywane bezpośrednio, wynika z wektora)

  pos = next_pos + 1;
  next_pos = str.find(';', pos);

  std::vector<num_t> poly_coeffs;
  size_t start = pos;
  size_t end = str.find(',', start);

  // Parsowanie współczynników wielomianu
  while (end != std::string::npos && end < next_pos) {
    num_t coeff = decodeNum(str.substr(start, end - start), currentEncoding);
    poly_coeffs.push_back(coeff);
    start = end + 1;
    end = str.find(',', start);
  }
  num_t coeff = decodeNum(str.substr(start, next_pos - start), currentEncoding);
  poly_coeffs.push_back(coeff);

  pos = next_pos + 1;
  // Szukamy końca stringa lub kolejnego średnika
  size_t final_pos = str.find(';', pos);
  if (final_pos == std::string::npos) final_pos = str.length();

  num_t poly_f2k = decodeNum(str.substr(pos, final_pos - pos), currentEncoding);

  // Tworzymy i zwracamy wskaźnik do nowego obiektu Field
  return std::make_shared<Field>(prime_modulus, poly_coeffs, poly_f2k);
}

void testGHash() {
  num_t p(2);
  std::vector<num_t> empty_poly;  // Nieistotne dla F2k

  // Budowanie wielomianu x^128 + x^7 + x^2 + x + 1
  num_t ghash_poly("0x100000000000000000000000000000087");

  // Inicjalizacja kontekstu
  FieldPtr ghash_field = std::make_shared<Field>(p, empty_poly, ghash_poly);

  std::cout << "--- GHASH Test ---\n";

  // Klucz H (Auth Key) - przykładowa wartość (np. 42)
  // W prawdziwym GCM H = E_K(0^128)
  F2kElement H(ghash_field, 42);

  // Dane wejściowe
  std::string AAD = "HeaderData";            // A
  std::string Ciphertext = "SecretMessage";  // C

  std::cout << "H: " << H.toStr() << std::endl;
  std::cout << "AAD: \"" << AAD << "\"" << std::endl;
  std::cout << "Ciphertext: \"" << Ciphertext << "\"" << std::endl;

  F2kElement tag = calculateGHASH(H, AAD, Ciphertext);

  std::cout << "GHASH Output (Tag): " << tag.toStr() << std::endl;

  // Wyświetlenie w HEX dla czytelności (wymaga ustawienia encodingu na BASE_16
  // wcześniej) currentEncoding = Encoding::BASE_16; std::cout << "GHASH Hex: "
  // << tag.toStr() << std::endl;
}

void testDH_Fp(FieldPtr ctx) {
  std::cout << "\n=== Diffie-Hellman on Fp ===" << std::endl;
  // Parametry domeny: g, q
  std::string g_str, q_str;
  std::cout << "Generator g: ";
  std::cin >> g_str;
  std::cout << "Order q: ";
  std::cin >> q_str;

  FpElement g(ctx);
  g.fromStr(g_str);
  num_t q = decodeNum(q_str, currentEncoding);

  // Inicjalizacja protokołu
  DHProtocolField<FpElement> dh(g, q);

  // Alice
  auto aliceKeys = dh.generateKeys();
  std::cout << "Alice Priv: " << aliceKeys.secretKey
            << ", Pub: " << aliceKeys.publicKey.toStr() << std::endl;

  // Bob
  auto bobKeys = dh.generateKeys();
  std::cout << "Bob   Priv: " << bobKeys.secretKey
            << ", Pub: " << bobKeys.publicKey.toStr() << std::endl;

  // Uzgadnianie
  FpElement aliceShared = dh.agree(aliceKeys.secretKey, bobKeys.publicKey);
  FpElement bobShared = dh.agree(bobKeys.secretKey, aliceKeys.publicKey);

  std::cout << "Shared Secrets match? "
            << (aliceShared == bobShared ? "YES" : "NO") << std::endl;
  std::cout << "Shared Secret: " << aliceShared.toStr() << std::endl;
}

void testDH_F2k(FieldPtr ctx) {
  std::cout << "\n=== Diffie-Hellman on F2k ===" << std::endl;
  std::string g_str, q_str;
  std::cout << "Generator g (poly): ";
  std::cin >> g_str;
  std::cout << "Order q: ";
  std::cin >> q_str;

  F2kElement g(ctx);
  g.fromStr(g_str);
  num_t q = decodeNum(q_str, currentEncoding);

  DHProtocolField<F2kElement> dh(g, q);

  auto aliceKeys = dh.generateKeys();
  auto bobKeys = dh.generateKeys();

  F2kElement aliceShared = dh.agree(aliceKeys.secretKey, bobKeys.publicKey);
  F2kElement bobShared = dh.agree(bobKeys.secretKey, aliceKeys.publicKey);

  std::cout << "Shared Secrets match? "
            << (aliceShared == bobShared ? "YES" : "NO") << std::endl;
  std::cout << "Value: " << aliceShared.toStr() << std::endl;
}

void testDH_Fpk(FieldPtr ctx) {
  std::cout << "\n=== Diffie-Hellman on Fpk ===" << std::endl;
  std::string g_str, q_str;
  std::cout << "Generator g (vector): ";
  std::cin >> g_str;
  std::cout << "Order q: ";
  std::cin >> q_str;

  FpkElement g(ctx);
  g.fromStr(g_str);
  num_t q = decodeNum(q_str, currentEncoding);

  DHProtocolField<FpkElement> dh(g, q);

  auto aliceKeys = dh.generateKeys();
  auto bobKeys = dh.generateKeys();

  FpkElement aliceShared = dh.agree(aliceKeys.secretKey, bobKeys.publicKey);
  FpkElement bobShared = dh.agree(bobKeys.secretKey, aliceKeys.publicKey);

  std::cout << "Shared Secrets match? "
            << (aliceShared == bobShared ? "YES" : "NO") << std::endl;
  std::cout << "Value: " << aliceShared.toStr() << std::endl;
}

// Funkcja szablonowa dla krzywych (działa dla Fp, F2k i Fpk dzięki spójnemu
// interfejsowi)
template <typename CurveT, typename PointT, typename ElementT>
void runCurveDH(FieldPtr ctx, const std::string& typeName) {
  std::cout << "\n=== Diffie-Hellman on Elliptic Curve over " << typeName
            << " ===" << std::endl;

  // Wczytanie parametrów krzywej
  ElementT a(ctx), b(ctx);
  std::string curve_str;
  std::cout << "Curve params (a;b): ";
  std::cin >> curve_str;
  // Uwaga: tutaj zakładamy prosty parsing, w praktyce trzeba by użyć fromStr na
  // obiekcie curve ale klasy Curve nie mają metody ustawiającej a i b
  // bezpośrednio z zewnątrz bez obiektu. Zrobimy obejście - stworzymy obiekt
  // tymczasowo.

  // Parsowanie a i b ręcznie, bo Curve::fromStr działa na wewnętrznych polach
  size_t del = curve_str.find(';');
  a.fromStr(curve_str.substr(0, del));
  b.fromStr(curve_str.substr(del + 1));

  CurveT curve(a, b);

  // Wczytanie generatora G i rzędu q
  std::string G_str, q_str;
  PointT G(ElementT(ctx, 0), ElementT(ctx, 0));  // dummy init

  std::cout << "Generator G (x;y): ";
  std::cin >> G_str;
  G.fromStr(G_str);

  std::cout << "Order q: ";
  std::cin >> q_str;
  num_t q = decodeNum(q_str, currentEncoding);

  // Protokół EC-DH
  DHProtocolCurve<CurveT, PointT> dh(curve, G, q);

  auto aliceKeys = dh.generateKeys();
  std::cout << "Alice Priv: " << aliceKeys.secretKey
            << "\nAlice Pub: " << aliceKeys.publicKey.toStr() << std::endl;

  auto bobKeys = dh.generateKeys();
  std::cout << "Bob Priv: " << bobKeys.secretKey
            << "\nBob Pub: " << bobKeys.publicKey.toStr() << std::endl;

  PointT aliceShared = dh.agree(aliceKeys.secretKey, bobKeys.publicKey);
  PointT bobShared = dh.agree(bobKeys.secretKey, aliceKeys.publicKey);

  std::cout << "Shared Secrets match? "
            << (aliceShared == bobShared ? "YES" : "NO") << std::endl;
  std::cout << "Shared Point: " << aliceShared.toStr() << std::endl;
}