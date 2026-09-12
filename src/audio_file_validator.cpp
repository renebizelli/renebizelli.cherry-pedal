#include "audio_file_validator.hpp"

#include "errors.hpp"

namespace cherry {

void AudioFileValidator::validate(const std::vector<std::filesystem::path>& audio_files) const {
    std::vector<std::filesystem::path> missing_files;

    for (const auto& file : audio_files) {
        if (!std::filesystem::is_regular_file(file)) {
            missing_files.push_back(file);
        }
    }

    if (!missing_files.empty()) {
        throw MissingAudioFilesError(std::move(missing_files));
    }
}

}  // namespace cherry
