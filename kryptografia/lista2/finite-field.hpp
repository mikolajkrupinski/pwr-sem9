#ifndef FINITE_FIELD_HPP
#define FINITE_FIELD_HPP

#include <iostream>

#include "common.hpp"

// [TODO] Implement check if p is prime / expotential of prime (static_assert?)
template <unum_t p>
class FiniteField {
 private:
  unum_t value;

 public:
  FiniteField(unum_t _value) : value(_value % p) {}

  unum_t getValue() const { return value; }
  void setValue(unum_t _value) { this->value = _value % p; }

  FiniteField operator+(const FiniteField& ff) const;
  FiniteField operator-() const;
  FiniteField operator-(const FiniteField& ff) const;
  FiniteField operator*(const FiniteField& ff) const;
  FiniteField operator/(const FiniteField& ff) const;

  FiniteField& operator+=(const FiniteField& ff) const;
  FiniteField& operator-=(const FiniteField& ff) const;
  FiniteField& operator*=(const FiniteField& ff) const;
  FiniteField& operator/=(const FiniteField& ff) const;
  FiniteField operator=(const FiniteField& ff) const;

  bool operator<(const FiniteField& ff) const;
  bool operator<=(const FiniteField& ff) const;
  bool operator>(const FiniteField& ff) const;
  bool operator>=(const FiniteField& ff) const;
  bool operator==(const FiniteField& ff) const;

  friend FiniteField<p> inverse(const FiniteField<p>& ff);
  friend FiniteField<p> expotential(const FiniteField<p>& ff, size_t exp);

  friend std::ostream& operator<<(std::ostream& os, const FiniteField<p>& ff);
  friend std::istream& operator>>(std::istream& is, const FiniteField<p>& ff);
};

template <unum_t p>
FiniteField<p> FiniteField<p>::operator+(const FiniteField<p>& ff) const {
  return FiniteField<p>((this->value + ff.getValue()) % p);
}

template <unum_t p>
FiniteField<p> FiniteField<p>::operator-() const {
  return FiniteField<p>(p - this->value);
}

template <unum_t p>
FiniteField<p> FiniteField<p>::operator-(const FiniteField<p>& ff) const {
  return *this + (-ff);
}

template <unum_t p>
FiniteField<p> FiniteField<p>::operator*(const FiniteField<p>& ff) const {
  return FiniteField<p>((this->value * ff.getValue()) % p);
}

template <unum_t p>
FiniteField<p> FiniteField<p>::operator/(const FiniteField<p>& ff) const {
  return *this * inverse<7>(ff);
}

template <unum_t p>
FiniteField<p>& FiniteField<p>::operator+=(const FiniteField& ff) const {
  this->value = *this + ff;
  return *this;
}

template <unum_t p>
FiniteField<p>& FiniteField<p>::operator-=(const FiniteField& ff) const {
  this->value = *this - ff;
  return *this;
}

template <unum_t p>
FiniteField<p>& FiniteField<p>::operator*=(const FiniteField& ff) const {
  this->value = *this * ff;
  return *this;
}

template <unum_t p>
FiniteField<p>& FiniteField<p>::operator/=(const FiniteField& ff) const {
  this->value = *this / ff;
  return *this;
}

template <unum_t p>
FiniteField<p> FiniteField<p>::operator=(const FiniteField& ff) const {
  this->value = ff.getValue();
  return *this;
}

template <unum_t p>
bool FiniteField<p>::operator<(const FiniteField& ff) const {
  return this->value < ff.getValue();
}

template <unum_t p>
bool FiniteField<p>::operator<=(const FiniteField& ff) const {
  return this->value <= ff.getValue();
}

template <unum_t p>
bool FiniteField<p>::operator>(const FiniteField& ff) const {
  return this->value > ff.getValue();
}

template <unum_t p>
bool FiniteField<p>::operator>=(const FiniteField& ff) const {
  return this->value >= ff.getValue();
}

template <unum_t p>
bool FiniteField<p>::operator==(const FiniteField& ff) const {
  return this->value == ff.getValue();
}

template <unum_t p>
FiniteField<p> inverse(const FiniteField<p>& ff) {
  if (ff == FiniteField<p>(0)) {
    throw std::invalid_argument("Division by zero");
  }

  unum_t r0 = p;
  unum_t r1 = ff.value;
  unum_t t0 = 0;
  unum_t t1 = 1;

  while (r1 > 0) {
    unum_t q = r0 / r1;

    unum_t r_next = r0 - q * r1;
    r0 = r1;
    r1 = r_next;

    unum_t t_next = t0 - q * t1;
    t0 = t1;
    t1 = t_next;
  }

  if (r0 != 1) {
    throw std::invalid_argument(
        "Error: Inverse does not exist (gcd is not 1).");
  }

  // [TODO] Is there a possibility that t0 < 0? What then?

  return FiniteField<p>(t0);
}

template <unum_t p>
FiniteField<p> expotential(const FiniteField<p>& ff, size_t exp) {
  FiniteField<p> base = ff;
  FiniteField<p> result(1);

  while (exp > 0) {
    if (exp % 2 == 1) {
      result *= base;
    }
    base *= base;
    exp /= 2;
  }

  return result;
}

template <unum_t p>
std::ostream& operator<<(std::ostream& os, const FiniteField<p>& ff) {
  os << ff.getValue();
  return os;
}

template <unum_t p>
std::istream& operator>>(std::istream& is, const FiniteField<p>& ff) {
  is >> ff.value;
  ff.value %= p;
  return is;
}

#endif  // FINITE_FIELD_HPP
