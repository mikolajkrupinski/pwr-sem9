#include "f2k-element.hpp"

// Metody pomocnicze teraz mogą być statyczne, ale muszą przyjmować parametry,
// lub korzystamy z tych wewnątrz metod klasy.
// Tutaj dostosowałem implementację do nowej struktury.

num_t F2kElement::bitPolynomialMultiply(const num_t& a, const num_t& b) {
  if (a == 0 || b == 0) {
    return 0;
  }

  num_t result = 0;
  // Tworzymy tymczasowy obiekt tylko po to, by wywołać degree(),
  // w wersji zoptymalizowanej degree() powinno być funkcją statyczną/pomocniczą
  // na num_t. Zakładam, że degree() działa na bit_string.
  size_t deg_b = 0;
  num_t temp_b = b;
  while (temp_b > 0) {
    temp_b >>= 1;
    deg_b++;
  }
  if (deg_b > 0) deg_b--;

  for (size_t i = 0; i <= deg_b; i++) {
    if ((b >> i) & 1) {
      result = result ^ (a << i);
    }
  }
  return result;
}

num_t F2kElement::bitPolynomialDivide(num_t dividend, const num_t& divisor,
                                      num_t& quotient) {
  if (divisor == 0) {
    throw std::runtime_error("Dzielnik nie może być zerowym wielomianem.");
  }

  quotient = 0;
  if (dividend == 0) {
    return 0;
  }

  num_t one(1);

  // Helper lambda do stopnia wielomianu
  auto get_degree = [](num_t n) -> ssize_t {
    ssize_t d = 0;
    if (n == 0) return 0;
    while (n > 0) {
      n >>= 1;
      d++;
    }
    return d - 1;
  };

  ssize_t deg_dividend = get_degree(dividend);
  ssize_t deg_divisor = get_degree(divisor);

  while (dividend != 0 && deg_dividend >= deg_divisor) {
    size_t shift = deg_dividend - deg_divisor;

    quotient = quotient ^ (one << shift);
    dividend = dividend ^ (divisor << shift);

    if (dividend != 0) {
      deg_dividend = get_degree(dividend);
    } else {
      break;
    }
  }

  return dividend;
}

num_t F2kElement::bitPolynomialReduce(num_t product, const num_t& modulus) {
  if (modulus == 0) {
    throw std::runtime_error("Moduł (m(x)) nie może być zerowy.");
  }

  auto get_degree = [](num_t n) -> size_t {
    size_t d = 0;
    if (n == 0) return 0;
    while (n > 0) {
      n >>= 1;
      d++;
    }
    return (d > 0) ? d - 1 : 0;
  };

  size_t deg_mod = get_degree(modulus);
  size_t deg_prod = get_degree(product);

  while (deg_prod >= deg_mod &&
         product != 0) {  // added product != 0 check safety
    size_t shift = deg_prod - deg_mod;
    product = product ^ (modulus << shift);
    deg_prod = get_degree(product);
  }

  return product;
}

F2kElement F2kElement::operator+(const F2kElement& fe) const {
  checkContext(fe);
  return F2kElement(context, this->bit_string ^ fe.bit_string);
}

F2kElement F2kElement::operator-() const { return *this; }

F2kElement F2kElement::operator-(const F2kElement& fe) const {
  checkContext(fe);
  return *this + (-fe);
}

F2kElement F2kElement::operator*(const F2kElement& fe) const {
  checkContext(fe);
  num_t product_bits = bitPolynomialMultiply(this->bit_string, fe.bit_string);
  // Pobieramy f2kPoly z kontekstu!
  num_t result_bits = bitPolynomialReduce(product_bits, context->getF2kPoly());
  return F2kElement(context, result_bits);
}

F2kElement F2kElement::operator/(const F2kElement& fe) const {
  checkContext(fe);
  return *this * inverse(fe);
}

F2kElement& F2kElement::operator+=(const F2kElement& fe) {
  *this = *this + fe;
  return *this;
}

F2kElement& F2kElement::operator-=(const F2kElement& fe) {
  *this = *this - fe;
  return *this;
}

F2kElement& F2kElement::operator*=(const F2kElement& fe) {
  *this = *this * fe;
  return *this;
}

F2kElement& F2kElement::operator/=(const F2kElement& fe) {
  *this = *this / fe;
  return *this;
}

F2kElement& F2kElement::operator=(const F2kElement& fe) {
  if (this == &fe) return *this;
  // Logika przejmowania kontekstu (jak w FpElement)
  if (this->context && fe.context && *this->context != *fe.context) {
    throw std::invalid_argument(
        "Cannot assign F2kElement from different field.");
  }
  this->context = fe.context;
  this->bit_string = fe.bit_string;
  return *this;
}

F2kElement inverse(const F2kElement& fe) {
  if (fe.bit_string == 0) {
    throw std::runtime_error(
        "Nie można obliczyć odwrotności elementu zerowego w F2k.");
  }

  num_t a = fe.bit_string;
  // Pobieramy wielomian z kontekstu
  num_t r0 = fe.getContext()->getF2kPoly();
  num_t r1 = a;
  num_t s0 = 0;
  num_t s1 = 1;
  num_t quotient;

  while (r1 != 0) {
    // Uwaga: bitPolynomialDivide jest teraz prywatna lub statyczna,
    // trzeba ją wywołać poprawnie. Jeśli jest prywatna, inverse musi być
    // friendem (jest). Jeśli zrobiliśmy ją instancyjną, to problem. W mojej
    // propozycji F2kElement z headerem, te funkcje są static private, więc
    // friend może je wywołać jako F2kElement::bitPolynomialDivide.
    num_t r_new = F2kElement::bitPolynomialDivide(r0, r1, quotient);

    num_t q_times_s1 = F2kElement::bitPolynomialMultiply(quotient, s1);

    num_t s_new = s0 ^ q_times_s1;

    r0 = r1;
    r1 = r_new;

    s0 = s1;
    s1 = s_new;
  }

  if (r0 != 1) {
    throw std::runtime_error("Element nie jest odwracalny: GCD != 1.");
  }

  return F2kElement(fe.getContext(), s0);
}

F2kElement power(const F2kElement& fe, const num_t& exp) {
  if (exp == 0) {
    return F2kElement(fe.getContext(), 1);
  }

  if (fe.bit_string == 0) {
    return F2kElement(fe.getContext(), 0);
  }

  num_t current_exp = exp;
  F2kElement result(fe.getContext(), 1);
  F2kElement base = fe;

  while (current_exp > 0) {
    if (current_exp % 2 == 1) {
      result = result * base;
    }
    base *= base;
    current_exp /= 2;  // Fixed from /= current_exp which was infinite loop or 1
  }

  return result;
}