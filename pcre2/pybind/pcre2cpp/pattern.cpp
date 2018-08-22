#include "pattern.h"
#include "matchcontext.h"
#include "utils.h"

#include <utility>

namespace pcre2 {
  static pcre2_code* compile(const std::string& pattern, uint32_t flags);
  static std::map<std::string, uint32_t> build_groupindex(const Pattern& pattern);

  Pattern::Pattern(const std::string& pattern, uint32_t flags)
  : pattern{pattern},
    code{compile(this->pattern, flags)},
    flags{query_info<uint32_t>(PCRE2_INFO_ALLOPTIONS) & (~PCRE2_UTF)},
    groups{query_info<uint32_t>(PCRE2_INFO_CAPTURECOUNT)},
    groupindex{build_groupindex(*this)} {
  }

  Pattern::~Pattern() {
    if (code != nullptr) {
      pcre2_code_free(code);
    }
  }

  Pattern::Pattern(Pattern&& other) noexcept
  : pattern{std::move(other.pattern)},
    code{std::move(other.code)},
    flags{std::move(other.flags)},
    groups{std::move(other.groups)},
    groupindex{std::move(other.groupindex)} {
    other.code = nullptr;
  }

  std::shared_ptr<MatchContext> Pattern::match(const std::string& text, size_t pos, size_t endpos, uint32_t flags) const {
    if (endpos > text.size()) endpos = text.size();
    if (pos > endpos) {
      return nullptr;
    }

    std::shared_ptr<MatchContext> context(new MatchContext(*this, text, pos, endpos, flags));
    int error = pcre2_match(
      context->pattern,
      context->subject,
      context->length,
      0,
      context->flags,
      context->match_data,
      NULL
    );

    if (error < 0) {
      if (error == PCRE2_ERROR_NOMATCH) return nullptr;

      throw std::invalid_argument(utils::build_error_message(error));
    }

    context->update_offset();
    return context;
  }

  std::shared_ptr<MatchContext> Pattern::match(std::shared_ptr<MatchContext> context) const {
    do {
      uint32_t flags = context->flags;

      if (context->was_empty_match()) {
        if (context->is_at_end_of_subject()) {
          return nullptr;
        }

        flags |= PCRE2_NOTEMPTY_ATSTART;
        flags |= PCRE2_ANCHORED;
      }

      int error = pcre2_match(
        context->pattern,
        context->subject,
        context->length,
        context->offset,
        flags,
        context->match_data,
        NULL
      );

      if (error < 0) {
        if (error == PCRE2_ERROR_NOMATCH) {
          if (flags == 0) return nullptr;

          context->advance_offset();
          continue;
        }

        throw std::invalid_argument(utils::build_error_message(error));
      }
    } while (false);

    context->update_offset();
    return context;
  }

  pcre2_code* compile(const std::string& pattern, uint32_t flags) {
    int error_code;
    PCRE2_SIZE error_offset;

    pcre2_code* code = pcre2_compile(
      reinterpret_cast<PCRE2_SPTR>(pattern.data()),
      pattern.size(),
      flags | PCRE2_UTF,
      &error_code,
      &error_offset,
      NULL
    );

    if (code == NULL) {
      throw std::invalid_argument(utils::build_error_message(error_code, error_offset));
    }

    return code;
  }

  std::map<std::string, uint32_t> build_groupindex(const Pattern& pattern) {
    uint32_t count = pattern.query_info<uint32_t>(PCRE2_INFO_NAMECOUNT);
    PCRE2_SPTR table = pattern.query_info<PCRE2_SPTR>(PCRE2_INFO_NAMETABLE);
    uint32_t size_of_entry = pattern.query_info<uint32_t>(PCRE2_INFO_NAMEENTRYSIZE);

    std::map<std::string, uint32_t> groupindex;
    for (uint32_t i = 0; i < count; ++i) {
      uint32_t index = (table[0] << 8) | table[1];
      groupindex[utils::to_string(table + 2)] = index;

      table += size_of_entry;
    }

    return groupindex;
  }
}