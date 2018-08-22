#include <sstream>

#include "utils.h"

namespace pcre2 { namespace utils {
  const std::string build_error_message(int code) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(code, buffer, sizeof(buffer));

    return to_string(buffer);
  }

  const std::string build_error_message(int code, PCRE2_SIZE offset) {
    std::stringstream stream;
    stream << build_error_message(code) << " at offset " << offset;

    return stream.str();
  }
}}