#include <iostream>

#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"

int main() {
  Field::initialize(3, {1, 0, 1});

  std::cout << "--- Arytmetyka w ciele F3^2 (mod x^2 + 1) ---" << std::endl;
  std::cout << "Moduł p: 3, Stopień k: 2, Moduł wielomianowy: x^2 + 1"
            << std::endl;
  std::cout << "------------------------------------------------" << std::endl;

  // Element a: x + 1. Współczynniki: [1 (a0), 1 (a1)]
  FpkElement a({FpElement(1), FpElement(1)});
  // Element b: 2x + 2. Współczynniki: [2 (a0), 2 (a1)]
  FpkElement b({FpElement(2), FpElement(2)});

  std::cout << "a = " << a << std::endl;
  std::cout << "b = " << b << std::endl;
  std::cout << "---" << std::endl;

  // 1. Dodawanie: a + b
  // (x + 1) + (2x + 2) = 3x + 3 = 0x + 0 = 0 (mod 3)
  FpkElement sum = a + b;
  std::cout << "1. Dodawanie: a + b = " << sum << " (oczekiwano: 0)"
            << std::endl;

  // 2. Odejmowanie: a - b
  // (x + 1) - (2x + 2) = -x - 1 = 2x + 2 (mod 3)
  FpkElement diff = a - b;
  std::cout << "2. Odejmowanie: a - b = " << diff << " (oczekiwano: 2x + 2)"
            << std::endl;

  // 3. Mnożenie: a * a
  // (x + 1) * (x + 1) = x^2 + 2x + 1
  // Redukcja mod (x^2 + 1): x^2 -> -1 mod 3 = 2
  // Wynik: (2) + 2x + 1 = 2x + 3 = 2x + 0 (mod 3)
  FpkElement prod = a * a;
  std::cout << "3. Mnożenie: a * a = " << prod << " (oczekiwano: 2x)"
            << std::endl;

  // 4. Dzielenie: a / a
  // a / a = 1
  FpkElement quotient = a / a;
  std::cout << "4. Dzielenie: a / a = " << quotient << " (oczekiwano: 1)"
            << std::endl;

  // 5. Inwersja i Mnożenie (Test poprawności inwersji)
  // a * a^(-1) = 1
  FpkElement inv = inverse(a);
  FpkElement unit_test = a * inv;
  std::cout << "5. Test Inwersji: a * a^(-1) = " << unit_test
            << " (oczekiwano: 1)" << std::endl;

  return 0;

  return 0;
}
