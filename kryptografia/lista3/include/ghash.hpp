#ifndef GHASH_HPP
#define GHASH_HPP

#include <string>
#include <vector>

#include "common.hpp"
#include "f2k-element.hpp"

num_t bytesToBlock(const std::string& bytes);
std::vector<num_t> splitAndPad(const std::string& input);
F2kElement calculateGHASH(const F2kElement& H, const std::string& A,
                          const std::string& C);

#endif  // GHASH_HPP