#ifndef FP_ELEMENT_HPP
#define FP_ELEMENT_HPP

#include <iostream>
#include <memory>
#include <stdexcept>

#include "common.hpp"
#include "field.hpp"

class FpElement;

FpElement inverse(const FpElement& fe);
FpElement power(const FpElement& fe, num_t exp);

class FpElement {
 private:
  num_t value;
  FieldPtr context;

  void checkContext(const FpElement& other) const {
    if (context != other.context && *context != *other.context) {
      throw std::invalid_argument(
          "Operation on elements from different fields (Fp).");
    }
  }

 public:
  // Konstruktor domyślny jest niebezpieczny bez kontekstu,
  // ale czasem potrzebny do vector::resize.
  // W idealnym świecie unikalibyśmy go, ale tu zostawiam pusty ptr (uwaga przy
  // użyciu).
  FpElement() : value(0), context(nullptr) {}

  FpElement(FieldPtr ctx, const num_t& _value = 0) : context(ctx) {
    if (!ctx) throw std::invalid_argument("Field context cannot be null");
    this->value = _value % ctx->getP();
    if (this->value < 0) {
      this->value += ctx->getP();
    }
  }

  // Przeciążenie dla typów prostych (int, long long) dla wygody
  FpElement(FieldPtr ctx, long long _value) : context(ctx) {
    if (!ctx) throw std::invalid_argument("Field context cannot be null");
    this->value = num_t(_value) % ctx->getP();
    if (value < 0) value += ctx->getP();
  }

  num_t getValue() const { return value; }
  FieldPtr getContext() const { return context; }

  void setValue(const num_t& _value) {
    if (!context) throw std::runtime_error("Unitialized context in setValue");
    this->value = _value % context->getP();
    if (this->value < 0) {
      this->value += context->getP();
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

  // Uwaga: fromStr nie może już magicznie ustawić modulo,
  // obiekt musi być już zainicjalizowany poprawnym kontekstem!
  void fromStr(const std::string& str) {
    if (!context)
      throw std::runtime_error("Cannot parse into element with null context");
    this->value = decodeNum(str, currentEncoding) % context->getP();
    if (this->value < 0) {
      this->value += context->getP();
    }
  }
};

#endif  // FP_ELEMENT_HPP