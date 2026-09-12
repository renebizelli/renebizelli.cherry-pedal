#pragma once

#include <filesystem>
#include <vector>

namespace cherry {

// Single responsibility: check a list of audio file paths exist on disk,
// raising MissingAudioFilesError with all offenders when they don't.
class AudioFileValidator {
public:
    void validate(const std::vector<std::filesystem::path>& audio_files) const;
};

}  // namespace cherry
