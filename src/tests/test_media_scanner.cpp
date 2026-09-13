#include <fstream>
#include <iostream>

#include "media_scanner.hpp"
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

void test_empty_root_finds_nothing() {
    const auto root = make_temp_root("media_scanner_empty");
    MediaScanner scanner(root);

    if (!scanner.find_candidates().empty()) {
        fail("expected no candidates in an empty root");
    }

    std::cout << "OK: an empty root finds no candidates\n";
}

void test_finds_a_valid_package_nested_under_the_root() {
    const auto root = make_temp_root("media_scanner_valid");
    const auto package_dir = root / "pi" / "USB-DRIVE" / "cherry-pedal-data";
    write_file(package_dir / "source.json", "{}");

    MediaScanner scanner(root);
    const auto candidates = scanner.find_candidates();

    if (candidates.size() != 1 || candidates[0] != package_dir) {
        fail("expected to find the single valid package directory");
    }

    std::cout << "OK: finds a valid package nested under the search root\n";
}

void test_ignores_a_package_directory_without_source_json() {
    const auto root = make_temp_root("media_scanner_incomplete");
    const auto package_dir = root / "pi" / "USB-DRIVE" / "cherry-pedal-data";
    std::filesystem::create_directories(package_dir);
    // No source.json written inside — should not count as a candidate.

    MediaScanner scanner(root);

    if (!scanner.find_candidates().empty()) {
        fail("expected a cherry-pedal-data directory without source.json to be ignored");
    }

    std::cout << "OK: ignores a cherry-pedal-data directory missing source.json\n";
}

void test_ignores_unrelated_directories() {
    const auto root = make_temp_root("media_scanner_unrelated");
    write_file(root / "pi" / "USB-DRIVE" / "some-other-folder" / "source.json", "{}");

    MediaScanner scanner(root);

    if (!scanner.find_candidates().empty()) {
        fail("expected directories not named cherry-pedal-data to be ignored");
    }

    std::cout << "OK: ignores unrelated directory names\n";
}

}  // namespace

int main() {
    test_empty_root_finds_nothing();
    test_finds_a_valid_package_nested_under_the_root();
    test_ignores_a_package_directory_without_source_json();
    test_ignores_unrelated_directories();

    std::cout << "All MediaScanner checks passed\n";
    return 0;
}
