#pragma once

#include <filesystem>
#include <vector>

#include <nlohmann/json.hpp>

#include "audio_file_validator.hpp"
#include "models.hpp"
#include "path_resolver.hpp"

namespace cherry {

// Orchestrates SourceService's collaborators to expose the domain view of
// source.json: which bands exist, which songs/audios belong to a band, and
// whether every configured audio file is present on disk. Delegates file IO,
// path resolution and validation to their own single-responsibility classes.
class SourceService {
public:
    explicit SourceService(std::filesystem::path config_file = "source.json");

    std::vector<Band> bands() const;
    std::vector<Song> songs(const Band& band) const;
    void validate_audio_files() const;

    // Where source.json (and, by convention, bands/) live — the destination
    // ContentImporter writes an imported update to.
    const std::filesystem::path& project_root() const { return path_resolver_.project_root(); }

private:
    std::filesystem::path bands_root() const;

    nlohmann::json data_;
    PathResolver path_resolver_;
    AudioFileValidator validator_;
};

}  // namespace cherry
