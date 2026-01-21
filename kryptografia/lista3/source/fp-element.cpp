#include "fp-element.hpp"

FpElement FpElement::operator+(const FpElement& fe) const {
  checkContext(fe);
  num_t sum = this->value + fe.value;
  // Modulo jest wykonywane w konstruktorze lub tutaj ręcznie dla optymalizacji,
  // ale bezpieczniej zaufać konstruktorowi lub helperom.
  return FpElement(context, sum);
}

FpElement FpElement::operator-() const {
  if (this->value == 0) {
    return FpElement(context, 0);
  }
  return FpElement(context, context->getP() - this->value);
}

FpElement FpElement::operator-(const FpElement& fe) const {
  checkContext(fe);
  return *this + (-fe);
}

FpElement FpElement::operator*(const FpElement& fe) const {
  checkContext(fe);
  return FpElement(context,
                   this->value * fe.value);  // Konstruktor zrobi modulo
}

FpElement FpElement::operator/(const FpElement& fe) const {
  checkContext(fe);
  return *this * inverse(fe);
}

FpElement& FpElement::operator+=(const FpElement& fe) {
  *this = *this + fe;
  return *this;
}

FpElement& FpElement::operator-=(const FpElement& fe) {
  *this = *this - fe;
  return *this;
}

FpElement& FpElement::operator*=(const FpElement& fe) {
  *this = *this * fe;
  return *this;
}

FpElement& FpElement::operator/=(const FpElement& fe) {
  *this = *this / fe;
  return *this;
}

FpElement& FpElement::operator=(const FpElement& fe) {
  // Przy przypisaniu zazwyczaj chcemy przejąć też kontekst,
  // jeśli nasz jest pusty, lub sprawdzić zgodność.
  if (this == &fe) return *this;

  // Jeśli obiekt nie miał kontekstu, przejmujemy go.
  // Jeśli miał, upewniamy się, że jest zgodny (chyba że pozwalamy na zmianę
  // ciała przy przypisaniu). Tutaj zakładam twardą walidację, chyba że
  // this->context jest nullem.
  if (this->context && fe.context && *this->context != *fe.context) {
    throw std::invalid_argument(
        "Cannot assign element from a different field context.");
  }

  this->context = fe.context;
  this->value = fe.value;
  return *this;
}

bool FpElement::operator<(const FpElement& fe) const {
  checkContext(fe);
  return this->value < fe.value;
}
bool FpElement::operator<=(const FpElement& fe) const {
  checkContext(fe);
  return this->value <= fe.value;
}
bool FpElement::operator>(const FpElement& fe) const {
  checkContext(fe);
  return this->value > fe.value;
}
bool FpElement::operator>=(const FpElement& fe) const {
  checkContext(fe);
  return this->value >= fe.value;
}
bool FpElement::operator==(const FpElement& fe) const {
  if (this->context != fe.context && *this->context != *fe.context)
    return false;
  return this->value == fe.value;
}

FpElement inverse(const FpElement& fe) {
  if (fe.value == 0) {
    throw std::invalid_argument("Division by zero");
  }

  num_t p = fe.getContext()->getP();  // Pobieramy P z kontekstu elementu
  num_t r0 = p;
  num_t r1 = fe.value;
  num_t t0 = 0;
  num_t t1 = 1;

  while (r1 > 0) {
    num_t q = r0 / r1;

    num_t r_next = r0 - q * r1;
    r0 = r1;
    r1 = r_next;

    num_t t_next = t0 - q * t1;
    t0 = t1;
    t1 = t_next;
  }

  if (r0 != 1) {
    throw std::invalid_argument(
        "Error: Inverse does not exist (gcd is not 1).");
  }

  if (t0 < 0) {
    t0 = t0 + p;
  }

  return FpElement(fe.getContext(), t0);
}

FpElement power(const FpElement& fe, num_t exp) {
  FpElement base = fe;
  FpElement result(fe.getContext(), 1);  // Jedynka w tym samym ciele

  while (exp > 0) {
    if (exp % 2 == 1) {
      result *= base;
    }
    base *= base;
    exp /= 2;
  }
  return result;
}