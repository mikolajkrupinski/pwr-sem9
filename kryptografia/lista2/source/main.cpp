#include <iostream>

#include "elliptic-curve.hpp"
#include "elliptic-curve2.hpp"
#include "f2k-element.hpp"
#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"

void printPoint(const std::string &name, const ECPointF2k &p)
{
  if (p.isInfinity)
  {
    std::cout << name << ": O (Infinity)" << std::endl;
  }
  else
  {
    std::cout << name << ": (0b" << p.x.getBitString()
              << ", 0b" << p.y.getBitString() << ")" << std::endl;
  }
}

void testEllipticCurveF2m()
{
  std::cout << "--- TEST KRZYWEJ ELIPTYCZNEJ NAD F2^4 ---" << std::endl;
  std::cout << "Krzywa: y^2 + xy = x^3 + x^2 + 1 (a=1, b=1)" << std::endl;
  std::cout << "Moduł m(x): x^4 + x + 1 (0b10011)" << std::endl;
  std::cout << "----------------------------------------" << std::endl;

  // 1. Konfiguracja Krzywej
  F2kElement a(1);
  F2kElement b(1);
  EllipticCurveF2k curve(a, b);

  // 2. Punkt bazowy P(x^2, x^3) -> (4, 8) -> (0b0100, 0b1000)
  F2kElement Px(4);
  F2kElement Py(8);
  ECPointF2k P(Px, Py);
  printPoint("P", P);

  // 3. Sprawdzenie Negacji (P + (-P) = O)
  // W F2m: -P = (x, y + x)
  ECPointF2k negP = curve.negate(P); // Oblicza (x, y + x)
  printPoint("-P", negP);

  ECPointF2k P_plus_negP = curve.add(P, negP);
  printPoint("P + (-P)", P_plus_negP);

  if (P_plus_negP.isInfinity)
  {
    std::cout << "✅ TEST NEGACJI ZALICZONY." << std::endl;
  }
  else
  {
    std::cerr << "❌ BLAD: P + (-P) nie dało Infinity." << std::endl;
    return;
  }

  // 4. Podwajanie (Double) P + P = 2P
  // Weryfikacja: Wynik (0b1101, 0b0010) (dziesiętnie 13, 2)
  ECPointF2k P2_double = curve.doublePoint(P);
  printPoint("2P (double)", P2_double);

  // Sprawdzenie poprawności podwajania
  if (P2_double.x.getBitString() == 13 && P2_double.y.getBitString() == 2)
  {
    std::cout << "✅ TEST DOUBLE ZALICZONY (P -> 2P)." << std::endl;
  }
  else
  {
    std::cerr << "❌ BLAD: DoublePoint dało błędny wynik.\n";
  }

  // 5. Dodawanie 2P + P = 3P
  // Weryfikacja: 3P = (0b0011, 0b1010) (dziesiętnie 3, 10)
  ECPointF2k P3_add = curve.add(P2_double, P);
  printPoint("3P (add)", P3_add);

  if (P3_add.x.getBitString() == 3 && P3_add.y.getBitString() == 10)
  {
    std::cout << "✅ TEST ADD ZALICZONY (2P + P = 3P)." << std::endl;
  }
  else
  {
    std::cerr << "❌ BLAD: Add(2P, P) dało błędny wynik.\n";
  }

  // 6. Mnożenie Skalarne (Multiply) 4P
  // Weryfikacja: 4P = 2P + 2P = (0b1011, 0b0110) (dziesiętnie 11, 6)
  ECPointF2k P4_mult = curve.multiply(P, num_t(4));
  printPoint("4P (mult)", P4_mult);

  if (P4_mult.x.getBitString() == 11 && P4_mult.y.getBitString() == 6)
  {
    std::cout << "✅ TEST MULTIPLY ZALICZONY (4P)." << std::endl;
  }
  else
  {
    std::cerr << "❌ BLAD: Multiply(P, 4) dało błędny wynik.\n";
  }

  std::cout << "\n--- TESTY ZAKOŃCZONE ---" << std::endl;
}

int main()
{
  Field::initialize(2, {1}, 0b10011);

  std::cout << "\n--- TEST ARYTMETYKI F2m ---" << std::endl;
  F2kElement A(8); // y1
  F2kElement B(4); // x1

  // A. Sprawdzenie Inwersji (4^-1)
  F2kElement B_inv = inverse(B);
  std::cout << "DEBUG: 4^-1 (0b0100)^-1 = " << B_inv.getBitString()
            << " (Oczekiwano: 9)" << std::endl;

  // B. Sprawdzenie Mnożenia A * B_inv
  F2kElement result = A * B_inv;
  std::cout << "DEBUG: 8 * 9 = " << result.getBitString()
            << " (Oczekiwano: 15)" << std::endl; // 72 mod 19

  // C. Sprawdzenie Dzielenia
  F2kElement div_result = A / B;
  std::cout << "DEBUG: 8 / 4 = " << div_result.getBitString()
            << " (Oczekiwano: 15)" << std::endl; // Musi być to samo co B

  // try
  // {
  //   testEllipticCurveF2m();
  // }
  // catch (const std::exception &e)
  // {
  //   std::cerr << "\nKRYTYCZNY BŁĄD: " << e.what() << std::endl;
  //   std::cerr << "Sprawdź implementację odwrotności w F2kElement." << std::endl;

  return 0;
}
