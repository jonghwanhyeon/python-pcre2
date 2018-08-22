#pragma once

#include <memory>
#include <string>
#include <pcre2.h>

#include "utils.h"

namespace pcre2 {
  class Pattern;
  struct Match;

  class MatchContext {
  public:
    explicit MatchContext(const Pattern& pattern, const std::string& text, size_t pos, size_t endpos, uint32_t flags);
    virtual ~MatchContext();

    MatchContext(const MatchContext& other) = delete;
    MatchContext& operator=(const MatchContext& other) = delete;

    MatchContext(MatchContext&& other) noexcept;
    MatchContext& operator=(MatchContext&& other) = delete;

    const Match to_match();

    void update_offset();
    void advance_offset();
    bool was_empty_match() const;
    bool is_at_end_of_subject() const;

  public:
    const Pattern& pattern;
    const std::string text;

    const size_t pos;
    const size_t endpos;
    const size_t flags;

    const PCRE2_SIZE length;
    PCRE2_SIZE offset;
    PCRE2_SPTR subject;

    pcre2_match_data* match_data;
    PCRE2_SIZE* ovector;
  };
}