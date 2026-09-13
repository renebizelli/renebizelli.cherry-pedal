#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "errors.hpp"
#include "source_service.hpp"

using namespace cherry;

namespace {

void print_summary(const SourceService& source) {
    for (const auto& band : source.bands()) {
        std::cout << "Band: " << band.name << " (" << band.id << ")\n";

        for (const auto& song : source.songs(band)) {
            std::cout << "  Song: " << song.name << " ["
                       << (song.autoforward ? "AUTO" : "MANUAL") << "]\n";

            for (const auto& audio : song.audios) {
                std::cout << "    Audio: " << audio.name << " -> " << audio.file << "\n";
            }
        }
    }
}

void fail(const std::string& message) {
    std::cerr << "FAIL: " << message << "\n";
    std::exit(1);
}

void check_real_project_config() {
    SourceService source("source.json");
    print_summary(source);
    source.validate_audio_files();
    std::cout << "OK: all audio files configured in source.json exist\n";
}

void check_missing_audio_file_is_detected() {
    const std::filesystem::path config_dir = std::filesystem::temp_directory_path() / "cherry_broken_config";
    std::filesystem::create_directories(config_dir);
    const std::filesystem::path config_file = config_dir / "source.json";

    std::ofstream out(config_file);
    out << R"({
        "path": "bands",
        "bands": [{"id": "ghost", "name": "Ghost", "logo": false}],
        "ghost": [{
            "id": "song-1",
            "name": "Song 1",
            "autoforward": false,
            "audios": [{"file": "missing.wav", "name": "audio"}]
        }]
    })";
    out.close();

    SourceService source(config_file);

    try {
        source.validate_audio_files();
        fail("expected MissingAudioFilesError but none was thrown");
    } catch (const MissingAudioFilesError&) {
        std::cout << "OK: missing audio file detected as expected\n";
    }
}

}  // namespace

int main() {
    check_real_project_config();
    check_missing_audio_file_is_detected();

    std::cout << "All SourceService checks passed\n";
    return 0;
}
