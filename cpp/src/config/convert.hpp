#ifndef CONFIG__CONVERT_HPP
#define CONFIG__CONVERT_HPP

#include <yaml-cpp/node/convert.h>

namespace SK3 {
namespace Config {

struct Logger;
struct Variants;
struct Task;
struct Machine;
struct Demand;
struct Prereq;
struct Config;

} // namespace SK3::Config
} // namespace SK3

namespace YAML {

// namespace alias
namespace SK3C {
  using namespace SK3::Config;
}

template<> struct convert<SK3C::Logger> {
  static bool decode(const Node &node, SK3C::Logger &rhs);
};

template<> struct convert<SK3C::Variants> {
  static bool decode(const Node &node, SK3C::Variants &rhs);
};

template<> struct convert<SK3C::Task> {
  static bool decode(const Node &node, SK3C::Task &rhs);
};

template<> struct convert<SK3C::Machine> {
  static bool decode(const Node &node, SK3C::Machine &rhs);
};

template<> struct convert<SK3C::Demand> {
  static bool decode(const Node &node, SK3C::Demand &rhs);
};

template<> struct convert<SK3C::Prereq> {
  static bool decode(const Node &node, SK3C::Prereq &rhs);
};

template<> struct convert<SK3C::Config> {
  static bool decode(const Node &node, SK3C::Config &rhs);
};

} // namespace YAML

#endif //CONFIG__CONVERT_HPP

