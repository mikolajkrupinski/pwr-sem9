#include "ghash.hpp"

#include <iostream>

// Tutaj wklejamy ciała funkcji, które usuniemy z main.cpp

num_t bytesToBlock(const std::string& bytes) {
  num_t block = 0;
  for (unsigned char c : bytes) {
    block = (block << 8) | c;
  }
  size_t shift = (16 - bytes.size()) * 8;
  block = block << shift;
  return block;
}

std::vector<num_t> splitAndPad(const std::string& input) {
  std::vector<num_t> blocks;
  size_t len = input.length();
  if (len == 0) return blocks;
  for (size_t i = 0; i < len; i += 16) {
    std::string chunk = input.substr(i, 16);
    blocks.push_back(bytesToBlock(chunk));
  }
  return blocks;
}

F2kElement calculateGHASH(const F2kElement& H, const std::string& A,
                          const std::string& C) {
  FieldPtr ctx = H.getContext();
  std::vector<num_t> S;

  std::vector<num_t> A_blocks = splitAndPad(A);
  S.insert(S.end(), A_blocks.begin(), A_blocks.end());

  std::vector<num_t> C_blocks = splitAndPad(C);
  S.insert(S.end(), C_blocks.begin(), C_blocks.end());

  num_t lenA_bits = A.length() * 8;
  num_t lenC_bits = C.length() * 8;
  num_t lenBlock = (lenA_bits << 64) | lenC_bits;
  S.push_back(lenBlock);

  F2kElement X(ctx, 0);
  for (const auto& block_val : S) {
    F2kElement Si(ctx, block_val);
    X = X + Si;
    X = X * H;
  }
  return X;
}