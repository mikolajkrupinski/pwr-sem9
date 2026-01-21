#ifndef GHASH_HPP
#define GHASH_HPP

#include "common.hpp"
#include "f2k-element.hpp"

F2kElement calculateGHASH(const F2kElement& H, const std::string& A,
                          const std::string& C);

#endif  // GHASH_HPP