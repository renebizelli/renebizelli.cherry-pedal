#include "source_service.hpp"

#include "json_file_reader.hpp"

namespace cherry {

namespace {

std::filesystem::path project_root_of(const std::filesystem::path& config_file) {
    return std::filesystem::absolute(config_file).parent_path();
}

}  // namespace

SourceService::SourceService(std::filesystem::path config_file)
    : data_(JsonFileReader().read(config_file)),
      path_resolver_(project_root_of(config_file)) {}

std::vector<Band> SourceService::bands() const {
    std::vector<Band> result;

    for (const auto& band_json : data_.at("bands")) {
        result.push_back(Band{
            band_json.at("id").get<std::string>(),
            band_json.at("name").get<std::string>(),
            band_json.at("logo").get<bool>(),
        });
    }

    return result;
}

std::filesystem::path SourceService::bands_root() const {
    return path_resolver_.resolve(data_.value("path", std::string("bands")));
}

std::vector<Song> SourceService::songs(const Band& band) const {
    std::vector<Song> result;
    const auto bands_root_path = bands_root();

    for (const auto& song_json : data_.at(band.id)) {
        const auto song_id = song_json.at("id").get<std::string>();

        std::vector<Audio> audios;
        for (const auto& audio_json : song_json.at("audios")) {
            const auto file_name = audio_json.at("file").get<std::string>();
            const auto file_path =
                path_resolver_.audio_file_path(bands_root_path, band.id, song_id, file_name);

            audios.push_back(Audio{
                file_name,
                audio_json.at("name").get<std::string>(),
                file_path.string(),
            });
        }

        result.push_back(Song{
            song_id,
            song_json.at("name").get<std::string>(),
            song_json.at("autoforward").get<bool>(),
            std::move(audios),
        });
    }

    return result;
}

void SourceService::validate_audio_files() const {
    std::vector<std::filesystem::path> audio_paths;

    for (const auto& band : bands()) {
        for (const auto& song : songs(band)) {
            for (const auto& audio : song.audios) {
                audio_paths.emplace_back(audio.file);
            }
        }
    }

    validator_.validate(audio_paths);
}

}  // namespace cherry
