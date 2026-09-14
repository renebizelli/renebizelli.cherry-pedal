#pragma once

#include <string>

#include <SDL.h>

namespace cherry::tools {

// Shared by the CHERRY_UI_ONLY test tools (screenshot_demo, desktop_demo):
// resolves the DejaVu Bold font they render text with. On Linux, the same
// system path the real app (main.cpp) uses. On Windows there is no
// equivalent system path, so it's expected bundled next to the executable
// itself, resolved via SDL_GetBasePath() rather than cwd so the tools still
// find it when launched from another working directory.
inline std::string demo_font_path() {
#ifdef _WIN32
    char* base_path_raw = SDL_GetBasePath();
    std::string path =
        std::string(base_path_raw != nullptr ? base_path_raw : "") + "DejaVuSans-Bold.ttf";
    SDL_free(base_path_raw);
    return path;
#else
    return "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";
#endif
}

}  // namespace cherry::tools
