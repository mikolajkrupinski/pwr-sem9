#include <iostream>

#include "finite-field.hpp"

int main()
{
  num_t large_prime("2147483647");
  FiniteField::init(large_prime);

  FiniteField a(100);
  FiniteField b(500);

  FiniteField c = a + b;
  FiniteField d = a / b;

  std::cout << "Sum: " << c << std::endl;
  std::cout << "Division: " << d << std::endl;

  return 0;
}
