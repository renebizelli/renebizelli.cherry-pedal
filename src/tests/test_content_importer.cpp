#include <fstream>
#include <iostream>

#include "content_importer.hpp"
#include "test_support.hpp"

using namespace cherry;
using cherry::testing::fail;

namespace {

std::filesystem::path make_temp_root(const std::string& name) {
    const std::filesystem::path root = std::filesystem::temp_directory_path() / name;
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);
    return root;
}

void write_file(const std::filesystem::path& path, const std::string& content) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path);
    out << content;
}

constexpr const char* kValidSourceJson = R"({
    "path": "bands",
    "bands": [{"id": "ghost", "name": "Ghost", "logo": false}],
    "ghost": [{
        "id": "song-1",
        "name": "Song 1",
        "autoforward": false,
        "audios": [{"file": "audio.wav", "name": "audio"}]
    }]
})";

constexpr const char* kBrokenSourceJson = R"({
    "path": "bands",
    "bands": [{"id": "ghost", "name": "Ghost", "logo": false}],
    "ghost": [{
        "id": "song-1",
        "name": "Song 1",
        "autoforward": false,
        "audios": [{"file": "missing.wav", "name": "audio"}]
    }]
})";

void test_valid_candidate_replaces_destination_content() {
    const auto root = make_temp_root("content_importer_valid");
    const auto candidate = root / "usb" / "cherry-pedal-data";
    write_file(candidate / "source.json", kValidSourceJson);
    write_file(candidate / "bands" / "ghost" / "song-1" / "audio.wav", "new-audio-bytes");

    const auto project_root = root / "app";
    write_file(project_root / "source.json", "old content, should be replaced");
    write_file(project_root / "bands" / "leftover" / "stale.wav", "stale");

    ContentImporter importer;
    importer.import_from(candidate, project_root);

    if (!std::filesystem::is_regular_file(project_root / "bands" / "ghost" / "song-1" / "audio.wav")) {
        fail("expected the imported audio file to exist at the destination");
    }
    if (std::filesystem::exists(project_root / "bands" / "leftover")) {
        fail("expected the old bands/ content to be fully replaced, not merged");
    }

    std::ifstream copied_config(project_root / "source.json");
    const std::string copied_content(
        (std::istreambuf_iterator<char>(copied_config)), std::istreambuf_iterator<char>());
    if (copied_content != kValidSourceJson) {
        fail("expected source.json at the destination to match the imported one");
    }

    std::cout << "OK: a valid candidate replaces the destination's source.json and bands/\n";
}

void test_invalid_candidate_leaves_destination_untouched() {
    const auto root = make_temp_root("content_importer_invalid");
    const auto candidate = root / "usb" / "cherry-pedal-data";
    write_file(candidate / "source.json", kBrokenSourceJson);
    // Deliberately not creating bands/ghost/song-1/missing.wav.

    const auto project_root = root / "app";
    write_file(project_root / "source.json", "original content");
    write_file(project_root / "bands" / "ghost" / "song-1" / "audio.wav", "original-audio");

    ContentImporter importer;
    bool threw = false;
    try {
        importer.import_from(candidate, project_root);
    } catch (const ContentImportError&) {
        threw = true;
    }

    if (!threw) {
        fail("expected an invalid candidate (missing WAV) to be rejected");
    }

    std::ifstream original_config(project_root / "source.json");
    const std::string original_content(
        (std::istreambuf_iterator<char>(original_config)), std::istreambuf_iterator<char>());
    if (original_content != "original content") {
        fail("expected the destination's source.json to be untouched after a rejected import");
    }
    if (!std::filesystem::is_regular_file(project_root / "bands" / "ghost" / "song-1" / "audio.wav")) {
        fail("expected the destination's bands/ to be untouched after a rejected import");
    }

    std::cout << "OK: an invalid candidate is rejected and the destination stays untouched\n";
}

}  // namespace

int main() {
    test_valid_candidate_replaces_destination_content();
    test_invalid_candidate_leaves_destination_untouched();

    std::cout << "All ContentImporter checks passed\n";
    return 0;
}
