#pragma once

#include <string>
#include <vector>

namespace cherry {

struct DecodedAudio {
    std::vector<float> interleaved_samples;
    int channels = 0;
    double sample_rate = 0;
};

// Decodes a WAV file into interleaved float32 PCM via libsndfile. Throws
// AudioNotFoundError if the file cannot be opened.
DecodedAudio decode_wav_file(const std::string& path);

}  // namespace cherry
