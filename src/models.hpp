#pragma once

#include <string>
#include <vector>

namespace cherry {

struct Band {
    std::string id;
    std::string name;
    bool logo;
};

struct Audio {
    std::string id;
    std::string name;
    std::string file;
};

struct Song {
    std::string id;
    std::string name;
    bool autoforward;
    std::vector<Audio> audios;
};

}  // namespace cherry
