#ifndef F2K_ELEMENT_HPP
#define F2K_ELEMENT_HPP

#include "common.hpp"
#include "field.hpp"

class F2kElement;

F2kElement inverse(const F2kElement& fe);
F2kElement power(const F2kElement& fe, const num_t& exp);

class F2kElement {
 private:
  num_t bit_string;

 public:
  F2kElement(const num_t& initial_value = num_t(0))
      : bit_string(initial_value) {}

  F2kElement(unsigned long long initial_value)
      : bit_string(num_t(initial_value)) {}

  const num_t& getBitString() const { return bit_string; }

  void setBitString(const num_t& _bit_string) { bit_string = _bit_string; }

  F2kElement operator+(const F2kElement& fe) const;
  F2kElement operator-() const;
  F2kElement operator-(const F2kElement& fe) const;
  F2kElement operator*(const F2kElement& fe) const;
  F2kElement operator/(const F2kElement& fe) const;

  F2kElement& operator+=(const F2kElement& fe);
  F2kElement& operator-=(const F2kElement& fe);
  F2kElement& operator*=(const F2kElement& fe);
  F2kElement& operator/=(const F2kElement& fe);
  F2kElement& operator=(const F2kElement& fe);

  bool operator==(const F2kElement& fe) const {
    return bit_string == fe.bit_string;
  }
  bool operator!=(const F2kElement& fe) const {
    return bit_string != fe.bit_string;
  }

  friend F2kElement inverse(const F2kElement& fe);
  friend F2kElement power(const F2kElement& fe, const num_t& exp);
};

#endif  // F2K_ELEMENT_HPP