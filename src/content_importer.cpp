#include "content_importer.hpp"

#include "source_service.hpp"

namespace cherry {

void ContentImporter::import_from(
    const std::filesystem::path& candidate, const std::filesystem::path& project_root) {
    const std::filesystem::path candidate_config = candidate / "source.json";

    try {
        SourceService candidate_source(candidate_config);
        candidate_source.validate_audio_files();
    } catch (const std::exception& error) {
        throw ContentImportError(std::string("Pacote invalido: ") + error.what());
    }

    const std::filesystem::path candidate_bands = candidate / "bands";
    const std::filesystem::path destination_bands = project_root / "bands";
    const std::filesystem::path destination_config = project_root / "source.json";

    // Staged into sibling ".importing" paths first, on the same filesystem as
    // the real destination, and only swapped in via rename() (metadata-only,
    // effectively atomic) once the full copy from the drive has succeeded —
    // so a drive that fails mid-copy (unplugged, read error, corrupted
    // filesystem) leaves the working destination untouched, per this class's
    // own documented contract, rather than having already deleted it before
    // the failure was known.
    const std::filesystem::path staging_bands = project_root / "bands.importing";
    const std::filesystem::path staging_config = project_root / "source.json.importing";

    try {
        std::filesystem::remove_all(staging_bands);
        std::filesystem::copy(
            candidate_bands, staging_bands, std::filesystem::copy_options::recursive);
        std::filesystem::copy_file(
            candidate_config,
            staging_config,
            std::filesystem::copy_options::overwrite_existing);
    } catch (const std::exception& error) {
        std::filesystem::remove_all(staging_bands);
        std::filesystem::remove(staging_config);
        throw ContentImportError(std::string("Falha ao copiar do pendrive: ") + error.what());
    }

    std::filesystem::remove_all(destination_bands);
    std::filesystem::rename(staging_bands, destination_bands);
    std::filesystem::rename(staging_config, destination_config);
}

}  // namespace cherry
