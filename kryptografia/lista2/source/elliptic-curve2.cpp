#include "elliptic-curve2.hpp"

#include <iostream>

ECPointF2k EllipticCurveF2k::doublePoint(const ECPointF2k &P) const
{
    if (P.isInfinity)
    {
        return P;
    }

    F2kElement zero(0);
    if (P.x == zero)
    {
        return ECPointF2k();
    }

    F2kElement lambda = P.x + (P.y / P.x);

    F2kElement lambda_sq = lambda * lambda;
    F2kElement x3 = lambda_sq + lambda + a;

    F2kElement x1_plus_x3 = P.x + x3;
    F2kElement lambda_times_x1_plus_x3 = lambda * x1_plus_x3;
    F2kElement y3 = lambda_times_x1_plus_x3 + x3 + P.y;

    // Wewnątrz doublePoint(P)
    std::cout << "DEBUG: Double P(" << P.x.getBitString() << ", " << P.y.getBitString() << ")" << std::endl;

    // 1. Sprawdź dzielenie
    F2kElement ratio = P.y / P.x;
    std::cout << "DEBUG: y1/x1 = " << ratio.getBitString() << std::endl; // Oczekiwano 15 (0b1111)

    // 2. Sprawdź lambda
    lambda = P.x + ratio;
    std::cout << "DEBUG: Lambda = " << lambda.getBitString() << std::endl; // Oczekiwano 0 (0b0000)

    // 3. Sprawdź x3
    lambda_sq = lambda * lambda;
    x3 = lambda_sq + lambda + a;
    std::cout << "DEBUG: x3 = " << x3.getBitString() << std::endl; // Oczekiwano 1

    return ECPointF2k(x3, y3);
}

ECPointF2k EllipticCurveF2k::add(const ECPointF2k &P, const ECPointF2k &Q) const
{
    if (P.isInfinity)
        return Q;
    if (Q.isInfinity)
        return P;

    if (P == Q)
    {
        return doublePoint(P);
    }

    F2kElement zero(0);
    F2kElement x_sum = P.x + Q.x;

    if (x_sum == zero)
    {
        return ECPointF2k();
    }

    F2kElement numerator = P.y + Q.y;
    F2kElement denominator = x_sum;

    F2kElement lambda = numerator / denominator;

    F2kElement lambda_sq = lambda * lambda;
    F2kElement x3 = lambda_sq + lambda + x_sum + a;

    F2kElement x1_plus_x3 = P.x + x3;
    F2kElement lambda_times_x1_plus_x3 = lambda * x1_plus_x3;
    F2kElement y3 = lambda_times_x1_plus_x3 + x3 + P.y;

    return ECPointF2k(x3, y3);
}

ECPointF2k EllipticCurveF2k::multiply(const ECPointF2k &P, num_t k) const
{
    ECPointF2k temp = P;

    ECPointF2k result = ECPointF2k();

    num_t scalar = k;

    if (scalar == 0 || P.isInfinity)
    {
        return ECPointF2k();
    }

    while (scalar > 0)
    {
        if (scalar % 2 == 1)
        {
            result = add(result, temp);
        }

        temp = doublePoint(temp);

        scalar = scalar / 2;
    }

    return result;
}
