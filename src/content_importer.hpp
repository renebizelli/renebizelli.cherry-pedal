#pragma once

#include <filesystem>

namespace cherry {

class ContentImportError : public std::runtime_error {
public:
    explicit ContentImportError(const std::string& message) : std::runtime_error(message) {}
};

// Validates a candidate "cherry-pedal-data" folder (source.json + bands/)
// found by MediaScanner and, only if every audio file it references
// actually exists, replaces the app's own source.json and bands/ with it.
// Throws ContentImportError (leaving the destination untouched) if the
// candidate is invalid, so a broken pendrive never corrupts a working
// setup.
class ContentImporter {
public:
    void import_from(const std::filesystem::path& candidate, const std::filesystem::path& project_root);
};

}  // namespace cherry
