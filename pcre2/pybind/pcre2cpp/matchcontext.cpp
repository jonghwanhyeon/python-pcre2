#include "matchcontext.h"
#include "pattern.h"
#include "match.h"

#include <utility>

namespace pcre2 {
  MatchContext::MatchContext(const Pattern& pattern, const std::string& text, size_t pos, size_t endpos, uint32_t flags)
  : pattern{pattern},
    text{text},
    pos{pos},
    endpos{endpos},
    flags{flags},
    length{endpos - pos},
    offset{0},
    subject{reinterpret_cast<PCRE2_SPTR>(this->text.data() + pos)},
    match_data{pcre2_match_data_create_from_pattern(this->pattern, NULL)},
    ovector{pcre2_get_ovector_pointer(this->match_data)} {
  }

  MatchContext::~MatchContext() {
    if (match_data != nullptr) {
      pcre2_match_data_free(match_data);
    }
  }

  MatchContext::MatchContext(MatchContext&& other) noexcept
  : pattern{std::move(other.pattern)},
    text{std::move(other.text)},
    pos{std::move(other.pos)},
    endpos{std::move(other.endpos)},
    flags{std::move(other.flags)},
    length{std::move(other.length)},
    offset{std::move(other.offset)},
    subject{std::move(other.subject)},
    match_data{std::move(other.match_data)},
    ovector{std::move(other.ovector)} {
    match_data = nullptr;
    ovector = nullptr;
  }

  const Match MatchContext::to_match() {
    return Match(*this);
  }

  void MatchContext::update_offset() {
    offset = ovector[1];
  }

  void MatchContext::advance_offset() {
    offset += 1;
    while (offset < length) {
      if (!utils::is_utf8_code_unit(subject[offset])) break;
      offset += 1;
    }
  }

  bool MatchContext::was_empty_match() const {
    return ovector[0] == ovector[1];
  }

  bool MatchContext::is_at_end_of_subject() const {
    return ovector[0] == length;
  }

}