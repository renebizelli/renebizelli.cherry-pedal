#include "resampler.hpp"

#include <algorithm>
#include <cstddef>

namespace cherry {

std::vector<float> resample_linear(
    const std::vector<float>& input, int channels, double from_rate, double to_rate) {
    if (from_rate == to_rate || input.empty()) {
        return input;
    }

    const std::size_t input_frames = input.size() / static_cast<std::size_t>(channels);
    const double ratio = to_rate / from_rate;
    const std::size_t output_frames = static_cast<std::size_t>(static_cast<double>(input_frames) * ratio);

    std::vector<float> output(output_frames * static_cast<std::size_t>(channels));

    for (std::size_t out_frame = 0; out_frame < output_frames; ++out_frame) {
        const double source_position = static_cast<double>(out_frame) / ratio;
        const std::size_t source_index = static_cast<std::size_t>(source_position);
        const double fraction = source_position - static_cast<double>(source_index);
        const std::size_t next_index = std::min(source_index + 1, input_frames - 1);

        for (int channel = 0; channel < channels; ++channel) {
            const float previous_sample = input[source_index * channels + channel];
            const float next_sample = input[next_index * channels + channel];
            output[out_frame * channels + channel] =
                static_cast<float>(previous_sample + (next_sample - previous_sample) * fraction);
        }
    }

    return output;
}

}  // namespace cherry
