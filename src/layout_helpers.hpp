#pragma once

#include <cstddef>
#include <string>

namespace cherry {

// Pure font-sizing heuristics ported from PainelScreen (song/audio label
// sizing based on text length / audio count). Kept free of any SDL
// dependency so they can be unit-tested directly.
int song_font_size(const std::string& text);
int audio_font_size(std::size_t audio_count);
int audio_row_height(std::size_t audio_count);

}  // namespace cherry
