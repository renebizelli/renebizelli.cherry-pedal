#include "json_file_reader.hpp"

#include <fstream>

#include "errors.hpp"

namespace cherry {

nlohmann::json JsonFileReader::read(const std::filesystem::path& file) const {
    std::ifstream stream(file);

    if (!stream.is_open()) {
        throw ConfigError("Config file not found: " + file.string());
    }

    nlohmann::json data;

    try {
        stream >> data;
    } catch (const nlohmann::json::parse_error& error) {
        throw ConfigError("Invalid JSON in " + file.string() + ": " + error.what());
    }

    return data;
}

}  // namespace cherry
