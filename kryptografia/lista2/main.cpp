#include <iostream>

#include "finite-field.hpp"

int main() {
  FiniteField<7> a(5);
  FiniteField<7> b(4);

  std::cout << a + b << std::endl;
  std::cout << -a << std::endl;
  std::cout << a / b << std::endl;
  std::cout << expotential<7>(a, 3) << std::endl;

  return 0;
}
