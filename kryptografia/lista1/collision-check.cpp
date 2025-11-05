#include "collision-check.hpp"

#include "md5.cpp"

bool verify_hash_collision(const char M0_0[64], const char M1_0[64],
                           const char M0_1[64], const char M1_1[64]) {
  auto M0_0_hash = ConstexprHashes::md5(M0_0, ConstexprHashes::initial_buffer);
  auto M1_0_hash = ConstexprHashes::md5(
      M1_0, std::array<uint32_t, 4>{
                {M0_0_hash[0], M0_0_hash[1], M0_0_hash[2], M0_0_hash[3]}});

  auto M0_1_hash = ConstexprHashes::md5(M0_1, ConstexprHashes::initial_buffer);
  auto M1_1_hash = ConstexprHashes::md5(
      M1_1, std::array<uint32_t, 4>{
                {M0_1_hash[0], M0_1_hash[1], M0_1_hash[2], M0_1_hash[3]}});

  for (size_t i = 0; i < 4; i++) {
    if (M1_0_hash[i] != M1_1_hash[i]) {
      return false;
    }
  }
  return true;
}
