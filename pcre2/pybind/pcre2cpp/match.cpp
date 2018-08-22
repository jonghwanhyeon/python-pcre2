#include "match.h"
#include "pattern.h"
#include "matchcontext.h"
#include "utils.h"

namespace pcre2 {
  inline static std::shared_ptr<const Group> make_group(const MatchContext& context, uint32_t index);

  static std::vector<std::shared_ptr<const Group>> build_groups(const MatchContext& context);
  static std::map<std::string, std::shared_ptr<const Group>> build_groupmap(const MatchContext& context, const std::vector<std::shared_ptr<const Group>>& groups);

  Match::Match(const MatchContext& context)
  : groups{build_groups(context)},
    groupmap{build_groupmap(context, this->groups)} {

  }

  std::shared_ptr<const Group> make_group(const MatchContext& context, uint32_t index) {
    size_t start = context.ovector[2 * index];
    size_t end = context.ovector[(2 * index) + 1];

    if (start == PCRE2_UNSET) return nullptr;

    return std::shared_ptr<const Group>(new Group {
      context.pos + start,
      context.pos + end,
      utils::to_string(context.subject + start, end - start),
    });
  }

  std::vector<std::shared_ptr<const Group>> build_groups(const MatchContext& context) {
    uint32_t count = pcre2_get_ovector_count(context.match_data);

    std::vector<std::shared_ptr<const Group>> groups;
    for (uint32_t index = 0; index < count; ++index) {
      groups.push_back(make_group(context, index));
    }

    return groups;
  }

  std::map<std::string, std::shared_ptr<const Group>> build_groupmap(const MatchContext& context, const std::vector<std::shared_ptr<const Group>>& groups) {
    std::map<std::string, std::shared_ptr<const Group>> groupmap;
    for (const auto& pair : context.pattern.groupindex) {
      groupmap[pair.first] = groups[pair.second];
    }

    return groupmap;
  }
}