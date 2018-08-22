#pragma once

#include <memory>
#include <map>
#include <string>
#include <pcre2.h>

namespace pcre2 {
  class MatchContext;

  class Pattern {
  public:
    explicit Pattern(const std::string& pattern, uint32_t flags = 0);
    virtual ~Pattern();

    Pattern(const Pattern& other) = delete;
    Pattern& operator=(const Pattern& other) = delete;

    Pattern(Pattern&& other) noexcept;
    Pattern& operator=(Pattern&& other) = delete;

    std::shared_ptr<MatchContext> match(const std::string& text, size_t pos = 0, size_t endpos = std::string::npos, uint32_t flags = 0) const;
    std::shared_ptr<MatchContext> match(std::shared_ptr<MatchContext> context) const;

    template <typename T>
    inline T query_info(uint32_t what) const { T where; pcre2_pattern_info(code, what, &where); return where; }

    inline operator pcre2_code*() const { return code; }

  public:
    const std::string pattern;

  private:
    pcre2_code* code;

  public:
    const uint32_t flags;
    const uint32_t groups;
    const std::map<std::string, uint32_t> groupindex;
  };
}