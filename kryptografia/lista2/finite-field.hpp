#ifndef FINITE_FIELD_HPP
#define FINITE_FIELD_HPP

#include <iostream>
#include <stdexcept>

#include "common.hpp"

class FiniteField;

FiniteField inverse(const FiniteField &ff);
FiniteField expotential(const FiniteField &ff, size_t exp);

std::ostream &operator<<(std::ostream &os, const FiniteField &ff);
std::istream &operator>>(std::istream &is, FiniteField &ff);

class FiniteField
{
private:
    num_t value;
    static num_t MOD;

public:
    static void init(const num_t &modulus)
    {
        if (modulus <= 0)
            throw std::invalid_argument("Modulus must be positive");
        MOD = modulus;
    }

    static num_t getModulus() { return MOD; }

    FiniteField(long long _value)
    {
        if (MOD == 0)
            throw std::runtime_error("FiniteField not initialized!");
        value = num_t(_value) % MOD;
        if (value < 0)
            value += MOD;
    }

    FiniteField(const num_t &_value = 0)
    {
        if (MOD == 0)
            throw std::runtime_error("FiniteField not initialized! Call FiniteField::init(p) first.");
        value = _value % MOD;
        if (value < 0)
            value += MOD;
    }

    num_t getValue() const { return value; }
    void setValue(const num_t &_value)
    {
        this->value = _value % MOD;
        if (this->value < 0)
            this->value += MOD;
    }

    FiniteField operator+(const FiniteField &ff) const;
    FiniteField operator-() const;
    FiniteField operator-(const FiniteField &ff) const;
    FiniteField operator*(const FiniteField &ff) const;
    FiniteField operator/(const FiniteField &ff) const;

    FiniteField &operator+=(const FiniteField &ff);
    FiniteField &operator-=(const FiniteField &ff);
    FiniteField &operator*=(const FiniteField &ff);
    FiniteField &operator/=(const FiniteField &ff);
    FiniteField &operator=(const FiniteField &ff);

    bool operator<(const FiniteField &ff) const;
    bool operator<=(const FiniteField &ff) const;
    bool operator>(const FiniteField &ff) const;
    bool operator>=(const FiniteField &ff) const;
    bool operator==(const FiniteField &ff) const;

    friend FiniteField inverse(const FiniteField &ff);
    friend FiniteField expotential(const FiniteField &ff, size_t exp);

    friend std::ostream &operator<<(std::ostream &os, const FiniteField &ff);
    friend std::istream &operator>>(std::istream &is, FiniteField &ff);
};

inline num_t FiniteField::MOD = 0;

#endif // FINITE_FIELD_HPP