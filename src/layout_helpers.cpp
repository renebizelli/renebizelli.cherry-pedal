#include "layout_helpers.hpp"

namespace cherry {

int song_font_size(const std::string& text) {
    if (text.size() > 28) return 34;
    if (text.size() > 20) return 42;
    return 52;
}

int audio_font_size(std::size_t audio_count) {
    if (audio_count >= 6) return 24;
    if (audio_count >= 4) return 30;
    return 36;
}

int audio_row_height(std::size_t audio_count) {
    if (audio_count >= 6) return 48;
    if (audio_count >= 4) return 62;
    return 76;
}

}  // namespace cherry
