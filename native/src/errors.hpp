#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace cherry {

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class AudioNotFoundError : public std::runtime_error {
public:
    explicit AudioNotFoundError(const std::string& message) : std::runtime_error(message) {}
};

class MissingAudioFilesError : public std::runtime_error {
public:
    explicit MissingAudioFilesError(std::vector<std::filesystem::path> missing_files)
        : std::runtime_error(build_message(missing_files)),
          missing_files_(std::move(missing_files)) {}

    const std::vector<std::filesystem::path>& missing_files() const { return missing_files_; }

private:
    static std::string build_message(const std::vector<std::filesystem::path>& missing_files) {
        std::string message = "Audio files not found:\n";
        for (const auto& file : missing_files) {
            message += file.string() + "\n";
        }
        return message;
    }

    std::vector<std::filesystem::path> missing_files_;
};

}  // namespace cherry
