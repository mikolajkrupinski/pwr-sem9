#ifndef COMMON_H
#define COMMON_H

#include <boost/multiprecision/cpp_int.hpp>
#include <string>

using num_t = boost::multiprecision::cpp_int;

enum class Encoding { BASE_10, BASE_16, BASE_64 };

extern Encoding currentEncoding;

std::string encodeNum(const num_t& number, Encoding encoding);
num_t decodeNum(const std::string& str, Encoding encoding);

#endif  // COMMON_H