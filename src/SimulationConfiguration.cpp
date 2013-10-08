#include "SimulationConfiguration.h"

#include <cstdint>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "json/json.h"
#include "WarpedDebug.h"

void SimulationConfiguration::init(std::istream& input) {
    Json::Reader reader;
    auto success = reader.parse(input, root_);

    if (!success) {
        throw std::runtime_error(std::string("Failed to parse configuration\n") +
                                 reader.getFormattedErrorMessages());
    }

    if (!root_.isObject()) {
        throw std::runtime_error("Configuration file must be a JSON object.");
    }
}

SimulationConfiguration::SimulationConfiguration(const std::string& config_file_name) {
    std::ifstream input(config_file_name);
    if (!input.is_open()) {
        throw std::runtime_error(std::string("Could not open cofiguration file ") + config_file_name);
    }

    try {
        init(input);
    } catch (...) {
        input.close();
        throw;
    }
    input.close();
}

SimulationConfiguration::SimulationConfiguration(std::istream& input) {
    init(input);
}

SimulationConfiguration::~SimulationConfiguration() {}

Json::Value SimulationConfiguration::get_value(std::initializer_list<std::string> list) {
    Json::Value value = root_;

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

std::string SimulationConfiguration::as_string(std::initializer_list<std::string> list) {
    return get_value(list).asString();
}

int SimulationConfiguration::as_int(std::initializer_list<std::string> list) {
    return get_value(list).asInt();
}

std::int64_t SimulationConfiguration::as_int64(std::initializer_list<std::string> list) {
    return get_value(list).asInt64();
}


bool SimulationConfiguration::as_bool(std::initializer_list<std::string> list) {
    return get_value(list).asBool();
}

double SimulationConfiguration::as_double(std::initializer_list<std::string> list) {
    return get_value(list).asDouble();
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