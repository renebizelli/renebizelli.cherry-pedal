#include <iostream>
#include <string>

#include "layout_helpers.hpp"
#include "test_support.hpp"

using namespace cherry;
using cherry::testing::fail;

namespace {

void test_song_font_size_thresholds() {
    if (song_font_size("short") != 52) {
        fail("expected 52 for short text");
    }
    if (song_font_size(std::string(25, 'x')) != 42) {
        fail("expected 42 for medium-length text");
    }
    if (song_font_size(std::string(30, 'x')) != 34) {
        fail("expected 34 for long text");
    }

    std::cout << "OK: song_font_size thresholds match the original heuristic\n";
}

void test_audio_font_size_and_row_height_thresholds() {
    if (audio_font_size(2) != 36 || audio_row_height(2) != 76) {
        fail("expected the default sizing for fewer than 4 audios");
    }
    if (audio_font_size(4) != 30 || audio_row_height(4) != 62) {
        fail("expected medium sizing at 4 audios");
    }
    if (audio_font_size(6) != 24 || audio_row_height(6) != 48) {
        fail("expected compact sizing at 6+ audios");
    }

    std::cout << "OK: audio_font_size/audio_row_height thresholds match the original heuristic\n";
}

}  // namespace

int main() {
    test_song_font_size_thresholds();
    test_audio_font_size_and_row_height_thresholds();

    std::cout << "All layout helper checks passed\n";
    return 0;
}
