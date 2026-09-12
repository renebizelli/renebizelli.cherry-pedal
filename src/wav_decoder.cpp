#include "wav_decoder.hpp"

#include <sndfile.h>

#include "errors.hpp"

namespace cherry {

DecodedAudio decode_wav_file(const std::string& path) {
    SF_INFO info{};
    SNDFILE* file = sf_open(path.c_str(), SFM_READ, &info);

    if (file == nullptr) {
        throw AudioNotFoundError("Could not open audio file: " + path);
    }

    DecodedAudio decoded;
    decoded.channels = info.channels;
    decoded.sample_rate = static_cast<double>(info.samplerate);
    decoded.interleaved_samples.resize(static_cast<std::size_t>(info.frames) * info.channels);

    const sf_count_t frames_read =
        sf_readf_float(file, decoded.interleaved_samples.data(), info.frames);

    sf_close(file);

    decoded.interleaved_samples.resize(static_cast<std::size_t>(frames_read) * info.channels);

    return decoded;
}

}  // namespace cherry
