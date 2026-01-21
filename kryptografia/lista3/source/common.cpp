#include "common.hpp"

Encoding currentEncoding = Encoding::BASE_10;

std::string encodeNum(const num_t& number, Encoding encoding) {
  if (number == 0) {
    return encoding == Encoding::BASE_64 ? "A" : "0";
  }

  std::string result = "";

  if (encoding == Encoding::BASE_10) {
    num_t num = number;
    while (num > 0) {
      result = static_cast<char>('0' + (num % 10).convert_to<int>()) + result;
      num /= 10;
    }
  } else if (encoding == Encoding::BASE_16) {
    num_t num = number;
    while (num > 0) {
      uint16_t remainder = static_cast<uint16_t>(num % 16);
      result = static_cast<char>(remainder < 10 ? '0' + remainder
                                                : 'A' + (remainder - 10)) +
               result;
      num /= 16;
    }
  } else {  // encoding == Encoding::BASE_64
    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    num_t num = number;
    while (num > 0) {
      uint16_t remainder = static_cast<uint16_t>(num % 64);
      result = base64_chars[remainder] + result;
      num /= 64;
    }
  }

  return result;
}

num_t decodeNum(const std::string& str, Encoding encoding) {
  num_t result = 0;

  if (encoding == Encoding::BASE_10) {
    for (char c : str) {
      result = result * 10 + (c - '0');
    }
  } else if (encoding == Encoding::BASE_16) {
    for (char c : str) {
      result = result * 16;
      if (c >= '0' && c <= '9') {
        result += (c - '0');
      } else if (c >= 'A' && c <= 'F') {
        result += (c - 'A' + 10);
      }
    }
  } else {  // encoding == Encoding::BASE_64
    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (char c : str) {
      result = result * 64;
      size_t index = base64_chars.find(c);
      if (index != std::string::npos) {
        result += index;
      }
    }
  }

  return result;
}
