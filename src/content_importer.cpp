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

    // A removable drive can fail mid-copy (unplugged, read error, corrupted
    // filesystem): std::filesystem::copy/copy_file throw filesystem_error in
    // that case, which the caller (SyncScreen) only knows how to handle as a
    // ContentImportError, not as an arbitrary uncaught exception that would
    // otherwise crash the whole app.
    try {
        std::filesystem::remove_all(destination_bands);
        std::filesystem::copy(
            candidate_bands,
            destination_bands,
            std::filesystem::copy_options::recursive |
                std::filesystem::copy_options::overwrite_existing);

        std::filesystem::copy_file(
            candidate_config,
            project_root / "source.json",
            std::filesystem::copy_options::overwrite_existing);
    } catch (const std::exception& error) {
        throw ContentImportError(std::string("Falha ao copiar do pendrive: ") + error.what());
    }
}

}  // namespace cherry
