#ifndef SIMULATION_CONFIGURATION_H
#define SIMULATION_CONFIGURATION_H

#include <cstdint>                      // for int64_t
#include <initializer_list>             // for initializer_list
#include <iosfwd>                       // for istream
#include <string>                       // for string

#include "json/json.h"                  // for Value

// This class holds configuration settings read in from a configuration file
//
// The current implementation requires the settings to be held in a json
// object (map, in c++ terms). Objects can be nested arbitrarily.
//
// To access a configuration setting, call the as_* or get_* function on the
// appropriate type. The as_* functions throw a std::runtime_error if the
// requested key isn't in the configuration, or if the value can't be
// converted to the requested type. The get_* functions return the provided
// default value instead of throwing an exception.
//
// Because objects can be nested, the as_* and get_* functions take an
// initializer list of arbitrary length that specifies the path to the setting
// in the configuration. For example, if the configuration file looks like the
// following:
//
// {
//     "root": true,
//     "nest": {
//         "nested": "value"
//     }
// }
//
// The "root" key would be accesed with:
//
//      configuration.as_bool({"root"});
//
// And the "nested" key would be accesed with:
//
//      configuration.as_string({"nest", "nested"});
//
// The previous two examples would throw an exception if the keys didn't exist
// in the dictionary. To return a default value instead, do the following:
//
//      configuration.as_string({"nest", "nested"}, "default value");

class SimulationConfiguration {
public:
    SimulationConfiguration(const std::string& config_file_name);
    SimulationConfiguration(std::istream& input);
    ~SimulationConfiguration();

    // These functions raise a std::runtime_error if the given key isn't
    // present or the value can't be converted into the required type.
    std::string as_string(std::initializer_list<std::string> list);
    int as_int(std::initializer_list<std::string> list);
    std::int64_t as_int64(std::initializer_list<std::string> list);
    bool as_bool(std::initializer_list<std::string> list);
    double as_double(std::initializer_list<std::string> list);

    // These versions return the given default instead of throwing an error
    std::string get_string(std::initializer_list<std::string> list,
                           const std::string& default_value) noexcept;
    int get_int(std::initializer_list<std::string> list, int default_value) noexcept;
    std::int64_t get_int64(std::initializer_list<std::string> list, int64_t default_value);
    bool get_bool(std::initializer_list<std::string> list, bool default_value) noexcept;
    double get_double(std::initializer_list<std::string> list, double default_value) noexcept;

private:
    void init(std::istream& input);
    Json::Value get_value(std::initializer_list<std::string> list);
    Json::Value root_;
};

#endif
