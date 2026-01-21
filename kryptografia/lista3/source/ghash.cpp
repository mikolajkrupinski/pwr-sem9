#include "ghash.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "common.hpp"
#include "f2k-element.hpp"
#include "field.hpp"

// Pomocnicza funkcja konwertująca 16 bajtów (lub mniej) na liczbę 128-bitową
// (num_t). Traktujemy input jako Big Endian (pierwszy bajt to najwyższe
// współczynniki wielomianu), co jest standardem w interpretacji napisów jako
// dużych liczb.
num_t bytesToBlock(const std::string& bytes) {
  num_t block = 0;
  for (unsigned char c : bytes) {
    block = (block << 8) | c;
  }
  // Jeśli blok jest krótszy niż 16 bajtów, przesuwamy go w lewo (padding zerami
  // z prawej strony w sensie bitowym, ale w arytmetyce wielomianów GHASH
  // padding jest "dopełnieniem do 128 bitów"). Wzór: padWithZeros appends 0s
  // until full block. Jeśli mamy np. 1 bajt, to jest on na początku bloku.
  size_t shift = (16 - bytes.size()) * 8;
  block = block << shift;
  return block;
}

// Funkcja dzieląca dane na 128-bitowe bloki z paddingiem
std::vector<num_t> splitAndPad(const std::string& input) {
  std::vector<num_t> blocks;
  size_t len = input.length();

  // Pusta wiadomość też może generować bloki w specyficznych implementacjach,
  // ale wg wzoru A i C są dzielone na m i n bloków. Jeśli len=0, m=0.
  if (len == 0) return blocks;

  for (size_t i = 0; i < len; i += 16) {
    std::string chunk = input.substr(i, 16);
    blocks.push_back(bytesToBlock(chunk));
  }
  return blocks;
}

// Implementacja algorytmu GHASH
// H - Klucz uwierzytelniający (element pola GF(2^128))
// A - Additional Authenticated Data (napis)
// C - Ciphertext (napis)
F2kElement calculateGHASH(const F2kElement& H, const std::string& A,
                          const std::string& C) {
  FieldPtr ctx = H.getContext();

  // 1. Podział A i C na bloki Si
  std::vector<num_t> S;

  std::vector<num_t> A_blocks = splitAndPad(A);
  S.insert(S.end(), A_blocks.begin(), A_blocks.end());

  std::vector<num_t> C_blocks = splitAndPad(C);
  S.insert(S.end(), C_blocks.begin(), C_blocks.end());

  // 2. Tworzenie bloku długości: len(A)||len(C) (64 bity każdy)
  // len(A) i len(C) to długość w bitach!
  num_t lenA_bits = A.length() * 8;
  num_t lenC_bits = C.length() * 8;

  num_t lenBlock = (lenA_bits << 64) | lenC_bits;
  S.push_back(lenBlock);

  // 3. Obliczenia: Xi = (Xi-1 + Si) * H
  // X0 = 0
  F2kElement X(ctx, 0);

  for (const auto& block_val : S) {
    F2kElement Si(ctx, block_val);

    // Dodawanie w GF(2^k) to XOR (załatwia to operator+)
    X = X + Si;

    // Mnożenie w GF(2^k) (załatwia to operator* z redukcją wielomianową)
    X = X * H;
  }

  return X;
}