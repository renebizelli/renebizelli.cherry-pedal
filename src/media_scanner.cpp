#include "media_scanner.hpp"

namespace cherry {

namespace {
constexpr int kMaxSearchDepth = 5;
constexpr const char* kPackageDirName = "cherry-pedal-data";
}  // namespace

MediaScanner::MediaScanner(std::filesystem::path search_root)
    : search_root_(std::move(search_root)) {}

std::vector<std::filesystem::path> MediaScanner::find_candidates() const {
    std::vector<std::filesystem::path> candidates;

    if (!std::filesystem::is_directory(search_root_)) {
        return candidates;
    }

    std::error_code error;
    auto it = std::filesystem::recursive_directory_iterator(
        search_root_, std::filesystem::directory_options::skip_permission_denied, error);
    const auto end = std::filesystem::recursive_directory_iterator();

    for (; it != end; it.increment(error)) {
        if (error) {
            break;
        }

        if (it.depth() > kMaxSearchDepth) {
            it.disable_recursion_pending();
            continue;
        }

        if (it->is_directory(error) && it->path().filename() == kPackageDirName &&
            std::filesystem::is_regular_file(it->path() / "source.json")) {
            candidates.push_back(it->path());
        }
    }

    return candidates;
}

}  // namespace cherry
