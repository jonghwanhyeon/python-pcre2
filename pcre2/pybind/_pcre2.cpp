#include <memory>
#include <string>
#include <vector>
#include <tuple>

#pragma gcc diagnostic push
#pragma gcc diagnostic ignored "-Wdeprecated-declarations"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#pragma gcc diagnostic pop

#include "pcre2cpp/pcre2cpp.h"

namespace py = pybind11;
namespace re = pcre2;

PYBIND11_MODULE(_pcre2, m) {
  m.attr("ANCHORED") = re::ANCHORED;
  m.attr("ENDANCHORED") = re::ENDANCHORED;
  m.attr("CASELESS") = re::CASELESS;
  m.attr("DOTALL") = re::DOTALL;
  m.attr("MULTILINE") = re::MULTILINE;
  m.attr("EXTENDED") = re::EXTENDED;

  py::class_<re::Pattern>(m, "Pattern")
    .def(py::init<const std::string&, uint32_t>(), py::arg("pattern"), py::arg("flags") = 0)

    .def("match", [](const re::Pattern& self, const std::string& text, size_t pos, size_t endpos, uint32_t flags) {
      return self.match(text, pos, endpos, flags);
    }, py::arg("text"), py::arg("pos") = 0, py::arg("endpos") = std::string::npos, py::arg("flags") = 0,
       py::keep_alive<0, 1>()/* Pattern should be kept alive until MatchContext is freed */)

    .def("match", [](const re::Pattern& self, std::shared_ptr<re::MatchContext> context) {
      return self.match(context);
    }, py::keep_alive<0, 1>()/* Pattern should be kept alive until MatchContext is freed */)

    .def_readonly("flags", &re::Pattern::flags)
    .def_readonly("groups", &re::Pattern::groups)
    .def_readonly("groupindex", &re::Pattern::groupindex)
    .def_readonly("pattern", &re::Pattern::pattern)
  ;

  py::class_<re::MatchContext, std::shared_ptr<re::MatchContext>>(m, "MatchContext")
    .def("to_match", &re::MatchContext::to_match)
    .def_readonly("text", &re::MatchContext::text)
    .def_readonly("pos", &re::MatchContext::pos)
    .def_readonly("endpos", &re::MatchContext::endpos)
    .def_readonly("flags", &re::MatchContext::flags)
    .def_property_readonly("pattern", [](const re::MatchContext& self) -> const re::Pattern& {
      return self.pattern;
    })
  ;

  py::class_<re::Group, std::shared_ptr<re::Group>>(m, "Group")
    .def_readonly("start", &re::Group::start)
    .def_readonly("end", &re::Group::end)
    .def_readonly("text", &re::Group::text)
  ;

  py::class_<re::Match>(m, "Match")
    .def_readonly("groups", &re::Match::groups)
    .def_readonly("groupmap", &re::Match::groupmap)
  ;
}