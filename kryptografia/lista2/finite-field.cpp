#include "finite-field.hpp"

FiniteField FiniteField::operator+(const FiniteField &ff) const
{
  return FiniteField((this->value + ff.value) % MOD);
}

FiniteField FiniteField::operator-() const
{
  if (this->value == 0)
  {
    return FiniteField(0);
  }
  return FiniteField(MOD - this->value);
}

FiniteField FiniteField::operator-(const FiniteField &ff) const
{
  return *this + (-ff);
}

FiniteField FiniteField::operator*(const FiniteField &ff) const
{
  return FiniteField((this->value * ff.value) % MOD);
}

FiniteField FiniteField::operator/(const FiniteField &ff) const
{
  return *this * inverse(ff);
}

FiniteField &FiniteField::operator+=(const FiniteField &ff)
{
  *this = *this + ff;
  return *this;
}

FiniteField &FiniteField::operator-=(const FiniteField &ff)
{
  *this = *this - ff;
  return *this;
}

FiniteField &FiniteField::operator*=(const FiniteField &ff)
{
  *this = *this * ff;
  return *this;
}

FiniteField &FiniteField::operator/=(const FiniteField &ff)
{
  *this = *this / ff;
  return *this;
}

FiniteField &FiniteField::operator=(const FiniteField &ff)
{
  this->value = ff.value;
  return *this;
}

bool FiniteField::operator<(const FiniteField &ff) const { return this->value < ff.value; }
bool FiniteField::operator<=(const FiniteField &ff) const { return this->value <= ff.value; }
bool FiniteField::operator>(const FiniteField &ff) const { return this->value > ff.value; }
bool FiniteField::operator>=(const FiniteField &ff) const { return this->value >= ff.value; }
bool FiniteField::operator==(const FiniteField &ff) const { return this->value == ff.value; }

FiniteField inverse(const FiniteField &ff)
{
  if (ff.value == 0)
  {
    throw std::invalid_argument("Division by zero");
  }

  num_t r0 = FiniteField::MOD;
  num_t r1 = ff.value;
  num_t t0 = 0;
  num_t t1 = 1;

  while (r1 > 0)
  {
    num_t q = r0 / r1;

    num_t r_next = r0 - q * r1;
    r0 = r1;
    r1 = r_next;

    num_t t_next = t0 - q * t1;
    t0 = t1;
    t1 = t_next;
  }

  if (r0 != 1)
  {
    throw std::invalid_argument("Error: Inverse does not exist (gcd is not 1).");
  }

  if (t0 < 0)
  {
    t0 = t0 + FiniteField::MOD;
  }

  return FiniteField(t0);
}

FiniteField expotential(const FiniteField &ff, size_t exp)
{
  FiniteField base = ff;
  FiniteField result(1);

  while (exp > 0)
  {
    if (exp % 2 == 1)
    {
      result *= base;
    }
    base *= base;
    exp /= 2;
  }
  return result;
}

std::ostream &operator<<(std::ostream &os, const FiniteField &ff)
{
  os << ff.value;
  return os;
}

std::istream &operator>>(std::istream &is, FiniteField &ff)
{
  num_t temp;
  is >> temp;
  ff.setValue(temp);
  return is;
}