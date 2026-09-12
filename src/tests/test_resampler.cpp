#include <cmath>
#include <iostream>
#include <vector>

#include "resampler.hpp"
#include "test_support.hpp"

using namespace cherry;
using cherry::testing::fail;

namespace {

void test_matching_rates_returns_input_unchanged() {
    const std::vector<float> input{0.1f, 0.2f, 0.3f, 0.4f};
    const auto output = resample_linear(input, 2, 44100.0, 44100.0);

    if (output != input) {
        fail("expected resample_linear to return the input unchanged when rates match");
    }

    std::cout << "OK: matching rates return the input unchanged\n";
}

void test_upsampling_roughly_doubles_frame_count() {
    // 100 mono frames at 22050Hz resampled to 44100Hz should yield ~200 frames.
    std::vector<float> input(100, 0.5f);
    const auto output = resample_linear(input, 1, 22050.0, 44100.0);

    if (output.size() < 190 || output.size() > 210) {
        fail("expected upsampling 22050Hz to 44100Hz to roughly double the frame count, got " +
             std::to_string(output.size()));
    }

    std::cout << "OK: upsampling roughly doubles the frame count\n";
}

void test_downsampling_roughly_halves_frame_count() {
    std::vector<float> input(200, 0.5f);
    const auto output = resample_linear(input, 1, 44100.0, 22050.0);

    if (output.size() < 90 || output.size() > 110) {
        fail("expected downsampling 44100Hz to 22050Hz to roughly halve the frame count, got " +
             std::to_string(output.size()));
    }

    std::cout << "OK: downsampling roughly halves the frame count\n";
}

void test_constant_signal_stays_constant() {
    // Resampling a constant-value signal should not introduce ripple: every
    // interpolated sample should still equal the constant.
    const std::vector<float> input(50, 0.75f);
    const auto output = resample_linear(input, 1, 22050.0, 44100.0);

    for (float sample : output) {
        if (std::fabs(sample - 0.75f) > 1e-6f) {
            fail("expected a constant signal to stay constant after resampling");
        }
    }

    std::cout << "OK: a constant signal stays constant after resampling\n";
}

void test_preserves_interleaved_stereo_channels() {
    // Left channel constant at 1.0, right channel constant at -1.0 — after
    // resampling, channel separation must still hold for every frame.
    std::vector<float> input;
    for (int i = 0; i < 20; ++i) {
        input.push_back(1.0f);
        input.push_back(-1.0f);
    }

    const auto output = resample_linear(input, 2, 22050.0, 44100.0);

    for (std::size_t frame = 0; frame < output.size() / 2; ++frame) {
        if (output[frame * 2] < 0.99f || output[frame * 2 + 1] > -0.99f) {
            fail("expected stereo channel separation to be preserved after resampling");
        }
    }

    std::cout << "OK: stereo channel separation is preserved after resampling\n";
}

}  // namespace

int main() {
    test_matching_rates_returns_input_unchanged();
    test_upsampling_roughly_doubles_frame_count();
    test_downsampling_roughly_halves_frame_count();
    test_constant_signal_stays_constant();
    test_preserves_interleaved_stereo_channels();

    std::cout << "All resampler checks passed\n";
    return 0;
}
