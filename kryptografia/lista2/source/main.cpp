#include <iostream>

#include "elliptic-curve.hpp"
#include "f2k-element.hpp"
#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"

void printPoint(const std::string& name, const ECPoint& p) {
  if (p.isInfinity) {
    std::cout << name << ": O (Infinity)" << std::endl;
  } else {
    // Zakładamy, że FpkElement ma metodę print lub operator<< wypisujący
    // wartość Tutaj używam uproszczonego getCoefficients()[0] dla Fp
    std::cout << name << ": (" << p.x.getCoefficients()[0].getValue() << ", "
              << p.y.getCoefficients()[0].getValue() << ")" << std::endl;
  }
}

int main() {
  Field::initialize(17, {1}, 0b0);

  std::cout << "--- TEST KRZYWEJ ELIPTYCZNEJ NAD F17 ---" << std::endl;

  // 2. Definicja Krzywej: y^2 = x^3 + 2x + 2
  FpkElement a({2});
  FpkElement b({2});
  EllipticCurve curve(a, b);

  // 3. Punkt startowy P(5, 1)
  // Sprawdzenie: 5^3 + 2*5 + 2 = 125 + 10 + 2 = 137. 137 mod 17 = 1. y^2 = 1 ->
  // ok.
  ECPoint P(FpkElement({5}), FpkElement({1}));
  printPoint("P", P);

  // --- TEST 1: Podwajanie (Double) ---
  // Oczekiwany wynik: (6, 3)
  ECPoint P2_double = curve.doublePoint(P);
  printPoint("2P (double)", P2_double);

  // --- TEST 2: Dodawanie (Add) P + P ---
  // Powinno dać ten sam wynik co doublePoint
  ECPoint P2_add = curve.add(P, P);
  printPoint("2P (add)", P2_add);

  if (P2_double == P2_add)
    std::cout << ">> ZGODNOSC: Add(P,P) == Double(P)" << std::endl;
  else
    std::cerr << ">> BLAD: Add(P,P) != Double(P)" << std::endl;

  // --- TEST 3: Dodawanie P + 2P = 3P ---
  // Oczekiwany wynik: (10, 6)
  ECPoint P3 = curve.add(P, P2_double);
  printPoint("3P", P3);

  // --- TEST 4: Mnożenie Skalarne (Multiply) ---
  // Sprawdzamy algorytm Double-and-Add dla k=2, k=3, k=4
  ECPoint P2_mult = curve.multiply(P, num_t(2));
  ECPoint P3_mult = curve.multiply(P, num_t(3));
  ECPoint P4_mult = curve.multiply(P, num_t(4));  // Oczekiwane (3, 1)

  printPoint("2P (mult)", P2_mult);
  printPoint("3P (mult)", P3_mult);
  printPoint("4P (mult)", P4_mult);

  // Weryfikacja
  bool test_passed = true;
  if (P2_mult != P2_double) {
    std::cout << "BLAD: Mnożenie *2 niepoprawne\n";
    test_passed = false;
  }
  if (P3_mult != P3) {
    std::cout << "BLAD: Mnożenie *3 niepoprawne\n";
    test_passed = false;
  }

  // --- TEST 5: Element Neutralny ---
  // P + (-P) = Infinity
  // -P to (x, -y) -> (5, -1 mod 17) -> (5, 16)
  ECPoint negP(FpkElement({5}), FpkElement({16}));
  ECPoint inf = curve.add(P, negP);
  printPoint("P + (-P)", inf);

  if (inf.isInfinity)
    std::cout << ">> ZGODNOSC: P + (-P) daje Infinity" << std::endl;
  else {
    std::cout << "BLAD: P + (-P) nie daje Infinity\n";
    test_passed = false;
  }

  // Podsumowanie
  std::cout << "----------------------------------------" << std::endl;
  if (test_passed)
    std::cout << "✅ WSZYSTKIE TESTY ZALICZONE" << std::endl;
  else
    std::cout << "❌ WYSTĄPIŁY BŁĘDY" << std::endl;

  return 0;
}
