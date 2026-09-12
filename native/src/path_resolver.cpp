#include "path_resolver.hpp"

namespace cherry {

PathResolver::PathResolver(std::filesystem::path project_root)
    : project_root_(std::move(project_root)) {}

std::filesystem::path PathResolver::resolve(const std::string& path) const {
    std::filesystem::path source_path(path);

    if (source_path.is_absolute()) {
        return source_path;
    }

    return project_root_ / source_path;
}

std::filesystem::path PathResolver::audio_file_path(
    const std::filesystem::path& bands_root,
    const std::string& band_id,
    const std::string& song_id,
    const std::string& file_name) const {
    return bands_root / band_id / song_id / file_name;
}

}  // namespace cherry
