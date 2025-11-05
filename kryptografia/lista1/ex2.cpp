#include <iomanip>
#include <iostream>

#include "collision-check.hpp"

#define LE32(x)                                                              \
  (char)((x) & 0xff), (char)(((x) >> 8) & 0xff), (char)(((x) >> 16) & 0xff), \
      (char)(((x) >> 24) & 0xff)

// First part of table 2
constexpr char M0_0_bytes[64] = {
    LE32(0x02dd31d1), LE32(0xc4eee6c5), LE32(0x069a3d69), LE32(0x5cf9af98),
    LE32(0x87b5ca2f), LE32(0xab7e4612), LE32(0x3e580440), LE32(0x897ffbb8),
    LE32(0x0634ad55), LE32(0x02b3f409), LE32(0x8388e483), LE32(0x5a417125),
    LE32(0xe8255108), LE32(0x9fc9cdf7), LE32(0xf2bd1dd9), LE32(0x5b3c3780)};
constexpr char M1_0_bytes[64] = {
    LE32(0xd11d0b96), LE32(0x9c7b41dc), LE32(0xf497d8e4), LE32(0xd555655a),
    LE32(0xc79a7335), LE32(0x0cfdebf0), LE32(0x66f12930), LE32(0x8fb109d1),
    LE32(0x797f2775), LE32(0xeb5cd530), LE32(0xbaade822), LE32(0x5c15cc79),
    LE32(0xddcb74ed), LE32(0x6dd3c55f), LE32(0xd80a9bb1), LE32(0xe3a7cc35)};
constexpr char M0_1_bytes[64] = {
    LE32(0x02dd31d1), LE32(0xc4eee6c5), LE32(0x069a3d69), LE32(0x5cf9af98),
    LE32(0x07b5ca2f), LE32(0xab7e4612), LE32(0x3e580440), LE32(0x897ffbb8),
    LE32(0x0634ad55), LE32(0x02b3f409), LE32(0x8388e483), LE32(0x5a41f125),
    LE32(0xe8255108), LE32(0x9fc9cdf7), LE32(0x72bd1dd9), LE32(0x5b3c3780)};
constexpr char M1_1_bytes[64] = {
    LE32(0xd11d0b96), LE32(0x9c7b41dc), LE32(0xf497d8e4), LE32(0xd555655a),
    LE32(0x479a7335), LE32(0x0cfdebf0), LE32(0x66f12930), LE32(0x8fb109d1),
    LE32(0x797f2775), LE32(0xeb5cd530), LE32(0xbaade822), LE32(0x5c154c79),
    LE32(0xddcb74ed), LE32(0x6dd3c55f), LE32(0x580a9bb1), LE32(0xe3a7cc35)};

// Second part of table 2
constexpr char M0_2_bytes[64] = {
    LE32(0x02dd31d1), LE32(0xc4eee6c5), LE32(0x069a3d69), LE32(0x5cf9af98),
    LE32(0x87b5ca2f), LE32(0xab7e4612), LE32(0x3e580440), LE32(0x897ffbb8),
    LE32(0x0634ad55), LE32(0x02b3f409), LE32(0x8388e483), LE32(0x5a417125),
    LE32(0xe8255108), LE32(0x9fc9cdf7), LE32(0xf2bd1dd9), LE32(0x5b3c3780)};
constexpr char M1_2_bytes[64] = {
    LE32(0x313e82d8), LE32(0x5b8f3456), LE32(0xd4ac6dae), LE32(0xc619c936),
    LE32(0xb4e253dd), LE32(0xfd03da87), LE32(0x06633902), LE32(0xa0cd48d2),
    LE32(0x42339fe9), LE32(0xe87e570f), LE32(0x70b654ce), LE32(0x1e0da880),
    LE32(0xbc2198c6), LE32(0x9383a8b6), LE32(0x2b65f996), LE32(0x702af76f)};
constexpr char M0_3_bytes[64] = {
    LE32(0x02dd31d1), LE32(0xc4eee6c5), LE32(0x069a3d69), LE32(0x5cf9af98),
    LE32(0x07b5ca2f), LE32(0xab7e4612), LE32(0x3e580440), LE32(0x897ffbb8),
    LE32(0x0634ad55), LE32(0x02b3f409), LE32(0x8388e483), LE32(0x5a41f125),
    LE32(0xe8255108), LE32(0x9fc9cdf7), LE32(0x72bd1dd9), LE32(0x5b3c3780)};
constexpr char M1_3_bytes[64] = {
    LE32(0x313e82d8), LE32(0x5b8f3456), LE32(0xd4ac6dae), LE32(0xc619c936),
    LE32(0x34e253dd), LE32(0xfd03da87), LE32(0x06633902), LE32(0xa0cd48d2),
    LE32(0x42339fe9), LE32(0xe87e570f), LE32(0x70b654ce), LE32(0x1e0d2880),
    LE32(0xbc2198c6), LE32(0x9383a8b6), LE32(0xab65f996), LE32(0x702af76f)};

#undef LE32

int main() {
  bool collision_verified;

  collision_verified =
      verify_hash_collision(M0_0_bytes, M1_0_bytes, M0_1_bytes, M1_1_bytes);
  std::cout << "Collision detected on the first part of table 2: ";
  std::cout << (collision_verified ? "True" : "False") << std::endl;

  collision_verified =
      verify_hash_collision(M0_2_bytes, M1_2_bytes, M0_3_bytes, M1_3_bytes);
  std::cout << "Collision detected on the second part of table 2: ";
  std::cout << (collision_verified ? "True" : "False") << std::endl;

  return 0;
}
