#pragma once

#include <filesystem>
#include <string>

namespace cherry {

// Single responsibility: turn config-relative or absolute path strings into
// concrete filesystem paths anchored on the project root.
class PathResolver {
public:
    explicit PathResolver(std::filesystem::path project_root);

    std::filesystem::path resolve(const std::string& path) const;

    std::filesystem::path audio_file_path(
        const std::filesystem::path& bands_root,
        const std::string& band_id,
        const std::string& song_id,
        const std::string& file_name) const;

private:
    std::filesystem::path project_root_;
};

}  // namespace cherry
