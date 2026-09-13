#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

namespace cherry {

// Single responsibility: read a file from disk and parse it as JSON.
// Knows nothing about the domain shape of the data it reads.
class JsonFileReader {
public:
    nlohmann::json read(const std::filesystem::path& file) const;
};

}  // namespace cherry
