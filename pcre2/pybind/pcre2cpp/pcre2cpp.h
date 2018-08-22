#pragma once

#include <pcre2.h>

#include "pattern.h"
#include "matchcontext.h"
#include "match.h"

namespace pcre2 {
  constexpr uint32_t ANCHORED = PCRE2_ANCHORED;
  constexpr uint32_t ENDANCHORED = PCRE2_ENDANCHORED;

  constexpr uint32_t CASELESS = PCRE2_CASELESS;
  constexpr uint32_t MULTILINE = PCRE2_MULTILINE;
  constexpr uint32_t DOTALL = PCRE2_DOTALL;
  constexpr uint32_t EXTENDED = PCRE2_EXTENDED;
}