#pragma once

#include <memory>
#include <map>
#include <vector>
#include <string>

namespace pcre2 {
  class Pattern;
  class MatchContext;

  struct Group {
    const size_t start;
    const size_t end;
    const std::string text;
  };

  struct Match {
    explicit Match(const MatchContext& context);

    const std::vector<std::shared_ptr<const Group>> groups;
    const std::map<std::string, std::shared_ptr<const Group>> groupmap;
  };
}