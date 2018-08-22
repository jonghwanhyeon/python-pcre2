#pragma once

#include <string>
#include <pcre2.h>

namespace pcre2 { namespace utils {
  const std::string build_error_message(int code);
  const std::string build_error_message(int code, PCRE2_SIZE offset);

  inline bool is_utf8_code_unit(PCRE2_UCHAR character) { return (character & 0xc0) == 0x80; };
  inline std::string to_string(PCRE2_SPTR text) { return std::string(reinterpret_cast<const char *>(text)); };
  inline std::string to_string(PCRE2_SPTR text, PCRE2_SIZE count) { return std::string(reinterpret_cast<const char *>(text), count); };
}}