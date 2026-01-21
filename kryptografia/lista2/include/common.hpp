#ifndef COMMON_H
#define COMMON_H

#include <boost/multiprecision/cpp_int.hpp>
#include <string>

using num_t = boost::multiprecision::cpp_int;

enum class Encoding { BASE_10 = 0, BASE_16 = 1, BASE_64 = 2 };

enum class FieldType { FPK = 0, FP = 1, F2K = 2, EFPK = 3, EFP = 4, EF2K = 5 };

extern Encoding currentEncoding;

std::string encodeNum(const num_t& number, Encoding encoding);
num_t decodeNum(const std::string& str, Encoding encoding);

#endif  // COMMON_H