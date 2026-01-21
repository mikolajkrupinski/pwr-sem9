#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common.hpp"
#include "elliptic-curve-fp.hpp"
#include "elliptic-curve.hpp"
#include "elliptic-curve2.hpp"
#include "f2k-element.hpp"
#include "field.hpp"
#include "fp-element.hpp"
#include "fpk-element.hpp"

// Funkcja pomocnicza do parsowania stringa i tworzenia obiektu Field
std::shared_ptr<Field> createFieldFromStr(const std::string& str);

int main() {
  std::cout << "Params: ";
  std::string params_str;
  std::cin >> params_str;

  FieldPtr field = createFieldFromStr(params_str);

  // TEST 1
  // "0;0;3;8;1;1;0;1;1;0;0;0;1;2;1;1;1;0;1;0;1;0;1;0;1;0;1;0;1;0;0;0;/;1;*;\n"
  // BASE10, Fpk, p=3, poly=1,1,0,1,1,0,0,0,1, f2k=<stub> ===
  // === 0;3;8;1,1,0,1,1,0,0,0,1;0
  // 1,1,1,0,1,0,1,0 := a
  // 1,0,1,0,1,0,1,0 := b
  // (a / a) * b

  FpkElement a(field);
  FpkElement b(field);

  std::string a_str, b_str;

  std::cout << "a: ";
  std::cin >> a_str;
  a.fromStr(
      a_str);  // Teraz fromStr bezpiecznie korzysta z 'field' wewnątrz 'a'

  std::cout << "b: ";
  std::cin >> b_str;
  b.fromStr(b_str);

  // Operatory automatycznie sprawdzą, czy 'a' i 'b' mają ten sam kontekst
  FpkElement result = (a / a) * b;
  std::cout << "Result: " << result.toStr() << std::endl;

  return 0;
}

std::shared_ptr<Field> createFieldFromStr(const std::string& str) {
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
  // size_t degree_stub = std::stoul(str.substr(pos, next_pos - pos)); // degree
  // (nieużywane bezpośrednio, wynika z wektora)

  pos = next_pos + 1;
  next_pos = str.find(';', pos);

  std::vector<num_t> poly_coeffs;
  size_t start = pos;
  size_t end = str.find(',', start);

  // Parsowanie współczynników wielomianu
  while (end != std::string::npos && end < next_pos) {
    num_t coeff = decodeNum(str.substr(start, end - start), currentEncoding);
    poly_coeffs.push_back(coeff);
    start = end + 1;
    end = str.find(',', start);
  }
  num_t coeff = decodeNum(str.substr(start, next_pos - start), currentEncoding);
  poly_coeffs.push_back(coeff);

  pos = next_pos + 1;
  // Szukamy końca stringa lub kolejnego średnika
  size_t final_pos = str.find(';', pos);
  if (final_pos == std::string::npos) final_pos = str.length();

  num_t poly_f2k = decodeNum(str.substr(pos, final_pos - pos), currentEncoding);

  // Tworzymy i zwracamy wskaźnik do nowego obiektu Field
  return std::make_shared<Field>(prime_modulus, poly_coeffs, poly_f2k);
}