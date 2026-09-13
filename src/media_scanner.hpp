#pragma once

#include <filesystem>
#include <vector>

namespace cherry {

// Looks for removable-media content-update packages: any directory named
// exactly "cherry-pedal-data" containing a source.json directly inside it,
// found by searching under a root directory (real usage: "/media", where
// Raspberry Pi OS auto-mounts USB drives under /media/<user>/<label>/).
// Kept free of any hardware/mount-point-detection specifics so it can be
// tested against a plain temporary directory tree instead of a real USB
// drive.
class MediaScanner {
public:
    explicit MediaScanner(std::filesystem::path search_root = "/media");

    std::vector<std::filesystem::path> find_candidates() const;

private:
    std::filesystem::path search_root_;
};

}  // namespace cherry
