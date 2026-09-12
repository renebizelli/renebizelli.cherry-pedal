#pragma once

#include <vector>

namespace cherry {

// Linear-interpolation resampling of interleaved multi-channel audio from
// one sample rate to another. Not audiophile-grade, but sufficient for a
// live-triggered sampler: WAV files in this project are expected to already
// be 44.1kHz (see PRD), so this only needs to gracefully handle the
// occasional file that isn't, rather than distort audible pitch/speed the
// way playing it unresampled at the wrong rate would.
//
// Returns `input` unchanged (same vector) when the rates already match, to
// avoid a pointless copy on the common path.
std::vector<float> resample_linear(
    const std::vector<float>& input, int channels, double from_rate, double to_rate);

}  // namespace cherry
