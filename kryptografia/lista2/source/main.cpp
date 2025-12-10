#include <iostream>

#include "elliptic-curve.hpp"
#include "elliptic-curve2.hpp"
#include "f2k-element.hpp"
#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"

int main() {
  Field::initialize(100, {1}, 0b10011);

  currentEncoding = Encoding::BASE_64;

  FpElement a(16);

  std::cout << a << std::endl;

  FpkElement poly({FpElement(1), FpElement(0), FpElement(1)});  // x^2 + 1

  std::cin >> poly;

  std::cout << poly << std::endl;

  F2kElement f2kElem1(0b1101);  // x^3 + x^2 + 1

  std::cout << f2kElem1 << std::endl;

  return 0;
}
