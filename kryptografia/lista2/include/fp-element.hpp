#ifndef FP_ELEMENT_HPP
#define FP_ELEMENT_HPP

#include <iostream>
#include <stdexcept>

#include "common.hpp"

class FpElement;

FpElement inverse(const FpElement& fe);
FpElement power(const FpElement& fe, num_t exp);

class FpElement {
 private:
  num_t value;
  const num_t p;

  bool isCompatible(const FpElement& fe) const { return this->p == fe.p; }

 public:
  FpElement(long long _value, num_t _p) : p(_p) {
    this->value = num_t(_value) % _p;
    if (value < 0) {
      value += _p;
    }
  }

  FpElement(const num_t& _value = 0, num_t _p) : p(_p) {
    this->value = _value % _p;
    if (value < 0) {
      value += _p;
    }
  }

  num_t getValue() const { return value; }

  void setValue(const num_t& _value) {
    this->value = _value % this->p;
    if (this->value < 0) {
      this->value += this->p;
    }
  }

  FpElement operator+(const FpElement& fe) const;
  FpElement operator-() const;
  FpElement operator-(const FpElement& fe) const;
  FpElement operator*(const FpElement& fe) const;
  FpElement operator/(const FpElement& fe) const;

  FpElement& operator+=(const FpElement& fe);
  FpElement& operator-=(const FpElement& fe);
  FpElement& operator*=(const FpElement& fe);
  FpElement& operator/=(const FpElement& fe);
  FpElement& operator=(const FpElement& fe);

  bool operator<(const FpElement& fe) const;
  bool operator<=(const FpElement& fe) const;
  bool operator>(const FpElement& fe) const;
  bool operator>=(const FpElement& fe) const;
  bool operator==(const FpElement& fe) const;

  friend FpElement inverse(const FpElement& fe);
  friend FpElement power(const FpElement& fe, num_t exp);

  std::string toStr() const { return encodeNum(this->value, currentEncoding); }
  void fromStr(const std::string& str) {
    this->value = decodeNum(str, currentEncoding) % this->p;
    if (this->value < 0) {
      this->value += this->p;
    }
  }
};

#endif  // FP_ELEMENT_HPP