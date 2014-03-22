#include <cstdint>                      // for int64_t
#include <fstream>                      // for operator<<, basic_ostream, etc
#include <memory>                       // for unique_ptr
#include <sstream>                      // for stringstream
#include <stdexcept>                    // for runtime_error

#include "SimulationConfiguration.h"
#include "WarpedDebug.h"                // for debugout
#include "json/json.h"                  // for Value, Reader

class SimulationConfiguration::impl {
public:
    impl(std::istream& input) {
        Json::Reader reader;
        auto success = reader.parse(input, root);

        if (!success) {
            throw std::runtime_error(std::string("Failed to parse configuration\n") +
                                     reader.getFormattedErrorMessages());
        }

        if (!root.isObject()) {
            throw std::runtime_error("Configuration file must be a JSON object.");
        }
    }

    Json::Value get_value(std::initializer_list<std::string> list) {
        Json::Value value = root;

        for (auto elem : list) {
            value = value[elem];
            // XXX: Value::operator! is the same as Value::isNull(). If this class is
            // extended to support null values in the configuration file, this
            // check would need to change.
            if (!value) {
                std::stringstream ss;
                ss << "Key {";
                for (auto elem2 : list) {
                    ss << '"' << elem2 << "\" ";
                }
                ss << "} does not exist";

                debug::debugout << ss.str() << '\n';
                throw std::runtime_error(ss.str());
            }
        }

        return value;
    }

    Json::Value root;
};

SimulationConfiguration::SimulationConfiguration(const std::string& config_file_name) {
    std::ifstream input(config_file_name);
    if (!input.is_open()) {
        throw std::runtime_error(std::string("Could not open cofiguration file ") + config_file_name);
    }

    try {
        pimpl = std::unique_ptr<impl>(new impl(input));
    } catch (...) {
        input.close();
        throw;
    }
    input.close();
}

SimulationConfiguration::SimulationConfiguration(std::istream& input) {
    pimpl = std::unique_ptr<impl>(new impl(input));
}

SimulationConfiguration::~SimulationConfiguration() {}

std::string SimulationConfiguration::as_string(std::initializer_list<std::string> list) {
    return pimpl->get_value(list).asString();
}

int SimulationConfiguration::as_int(std::initializer_list<std::string> list) {
    return pimpl->get_value(list).asInt();
}

std::int64_t SimulationConfiguration::as_int64(std::initializer_list<std::string> list) {
    return pimpl->get_value(list).asInt64();
}

bool SimulationConfiguration::as_bool(std::initializer_list<std::string> list) {
    return pimpl->get_value(list).asBool();
}

double SimulationConfiguration::as_double(std::initializer_list<std::string> list) {
    return pimpl->get_value(list).asDouble();
}

std::string SimulationConfiguration::get_string(std::initializer_list<std::string> list,
                                                const std::string& default_value) noexcept {
    try {
        return as_string(list);
    } catch (...) {
        return default_value;
    }
}

int SimulationConfiguration::get_int(std::initializer_list<std::string> list,
                                     int default_value) noexcept {
    try {
        return as_int(list);
    } catch (...) {
        return default_value;
    }
}

std::int64_t SimulationConfiguration::get_int64(std::initializer_list<std::string> list,
                                                int64_t default_value) {
    try {
        return as_int64(list);
    } catch (...) {
        return default_value;
    }
}

bool SimulationConfiguration::get_bool(std::initializer_list<std::string> list,
                                       bool default_value) noexcept {
    try {
        return as_bool(list);
    } catch (...) {
        return default_value;
    }
}

double SimulationConfiguration::get_double(std::initializer_list<std::string> list,
                                           double default_value) noexcept {
    try {
        return as_double(list);
    } catch (...) {
        return default_value;
    }
}
