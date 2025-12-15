#include <iostream>
#include <vector>

#include "common.hpp"
#include "elliptic-curve-fp.hpp"
#include "elliptic-curve.hpp"
#include "elliptic-curve2.hpp"
#include "f2k-element.hpp"
#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"

std::string generalPramsStr();
void setGeneralParamsFromStr(const std::string& str);

int main() {
  // TEST 1
  // "0;0;3;8;1;1;0;1;1;0;0;0;1;2;1;1;1;0;1;0;1;0;1;0;1;0;1;0;1;0;0;0;/;1;*;\n"
  // BASE10, Fpk, p=3, poly=1,1,0,1,1,0,0,0,1, f2k=<stub> ===
  // === 0;3;8;1,1,0,1,1,0,0,0,1;0
  // 1,1,1,0,1,0,1,0 := a
  // 1,0,1,0,1,0,1,0 := b
  // (a / a) * b

  // std::cout << "Params: ";
  // std::string params_str;
  // std::cin >> params_str;
  // setGeneralParamsFromStr(params_str);

  // FpkElement a, b;
  // std::string a_str, b_str;
  // std::cout << "a: ";
  // std::cin >> a_str;
  // a.fromStr(a_str);
  // std::cout << "b: ";
  // std::cin >> b_str;
  // b.fromStr(b_str);

  // FpkElement result = (a / a) * b;
  // std::cout << "Result: " << result.toStr() << std::endl;

  // TEST 2
  // "0;1;17;2;8;11;0;0;/;1;*;\n"
  // BASE10, Fp, p=17, poly=<stub>, f2k=<stub> ===
  // === 0;17;1;1;0
  // 8 := a
  // 11 := b
  // (a / a) * b

  // std::cout << "Params: ";
  // std::string params_str;
  // std::cin >> params_str;
  // setGeneralParamsFromStr(params_str);

  // FpElement a_fp, b_fp;
  // std::string a_fp_str, b_fp_str;
  // std::cout << "a: ";
  // std::cin >> a_fp_str;
  // a_fp.fromStr(a_fp_str);
  // std::cout << "b: ";
  // std::cin >> b_fp_str;
  // b_fp.fromStr(b_fp_str);

  // FpElement result_fp = (a_fp / a_fp) * b_fp;
  // std::cout << "Result: " << result_fp.toStr() << std::endl;

  // TEST 3
  // "0;2;283;2;87;23;0;1;/;1;*;0;/;\n"
  // BASE10, F2k, p=2, poly=<stub>, f2k=283 ===
  // === 0;2;1;1;283
  // 87 := a
  // 23 := b
  // (a / b) * (b / a)

  // std::cout << "Params: ";
  // std::string params_str;
  // std::cin >> params_str;
  // setGeneralParamsFromStr(params_str);

  // F2kElement a_f2k, b_f2k;
  // std::string a_f2k_str, b_f2k_str;
  // std::cout << "a: ";
  // std::cin >> a_f2k_str;
  // a_f2k.fromStr(a_f2k_str);
  // std::cout << "b: ";
  // std::cin >> b_f2k_str;
  // b_f2k.fromStr(b_f2k_str);

  // F2kElement result_f2k = (a_f2k / b_f2k) * (b_f2k / a_f2k);
  // std::cout << "Result: " << result_f2k.toStr() << std::endl;

  // "0;4;13;1;1;2;1;4;0;12;0;1;+;\n"
  // 0;13;1;1;0
  // a = 1, b = 1
  // P = (1,4), Q = (0,12)
  // P + Q

  // "0;5;11;1;1;2;0;1;2;5;0;1;+;\n"

  std::cout << "Params: ";
  std::string params_str;
  std::cin >> params_str;
  setGeneralParamsFromStr(params_str);

  EllipticCurveFp curve(0, 0);
  ECPointFp P, Q;
  std::string curve_str, P_str, Q_str;
  std::cout << "Elliptic curve: ";
  std::cin >> curve_str;
  curve.fromStr(curve_str);

  std::cout << "P: ";
  std::cin >> P_str;
  P.fromStr(P_str);

  std::cout << "Q: ";
  std::cin >> Q_str;
  Q.fromStr(Q_str);

  ECPointFp R = curve.add(P, Q);
  std::cout << "Result: " << R.toStr() << std::endl;

  return 0;
}

/*
 * General parameters are encoded as:
 * encoding;prime_modulus;degree;poly_coeff_0,...,poly_coeff_n;poly_f2k
 */
std::string generalPramsStr() {
  std::string result = std::to_string(static_cast<int>(currentEncoding)) + ";";

  result += encodeNum(Field::getP(), currentEncoding) + ";";

  result += std::to_string(Field::getPoly().size() - 1) + ";";
  for (const auto& coeff : Field::getPoly()) {
    result += encodeNum(coeff, currentEncoding) + ",";
  }
  result.pop_back();
  result += ";";

  result += encodeNum(Field::getF2kPoly(), currentEncoding);

  return result;
}

/*
 * General parameters are encoded as:
 * encoding;prime_modulus;degree;poly_coeff_0,...,poly_coeff_n;poly_f2k
 */
void setGeneralParamsFromStr(const std::string& str) {
  size_t pos = 0;
  size_t next_pos = str.find(';', pos);
  Encoding encoding =
      static_cast<Encoding>(std::stoi(str.substr(pos, next_pos - pos)));
  currentEncoding = encoding;

  pos = next_pos + 1;
  next_pos = str.find(';', pos);
  num_t prime_modulus =
      decodeNum(str.substr(pos, next_pos - pos), currentEncoding);

  pos = next_pos + 1;
  next_pos = str.find(';', pos);
  // size_t degree = std::stoul(str.substr(pos, next_pos - pos));

  pos = next_pos + 1;
  next_pos = str.find(';', pos);
  std::vector<num_t> poly_coeffs;
  size_t start = pos;
  size_t end = str.find(',', start);
  while (end != std::string::npos && end < next_pos) {
    num_t coeff = decodeNum(str.substr(start, end - start), currentEncoding);
    poly_coeffs.push_back(coeff);
    start = end + 1;
    end = str.find(',', start);
  }
  num_t coeff = decodeNum(str.substr(start, next_pos - start), currentEncoding);
  poly_coeffs.push_back(coeff);

  pos = next_pos + 1;
  num_t poly_f2k = decodeNum(str.substr(pos), currentEncoding);

  Field::initialize(prime_modulus, poly_coeffs, poly_f2k);
}
