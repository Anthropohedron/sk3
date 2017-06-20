#include <iostream>
#include <yaml-cpp/yaml.h>
#include "../sk3.hpp"
#include "representation.hpp"
#include "instantiation.hpp"
#include "convert.hpp"

namespace SK3 {

using Instantiate::Factory;

namespace Config {

using namespace std;

System *read(const string &filename) {
  const Config cfg = YAML::LoadFile(filename).as<Config>();
  const int errors = Validate::countErrors(cfg);
  if (errors > 0) {
    cerr << endl << "ERROR COUNT: " << errors << endl;
    return NULL;
  }
  Factory factory(cfg.variants);
  //TODO: instantiation
  return NULL;
}

} // namespace SK3::Config
} // namespace SK3

