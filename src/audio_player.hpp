#pragma once

#include <string>

namespace cherry {

// Loads one audio clip and drives it on whatever shared playback channel the
// concrete implementation is bound to. PlayerService depends only on this
// interface, never on a concrete backend (DIP) — swapping FakeAudioPlayer for
// PortAudioPlayer changes nothing above this seam.
class AudioPlayer {
public:
    virtual ~AudioPlayer() = default;

    virtual void load(const std::string& file) = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual bool is_playing() const = 0;

    // Fraction of the loaded clip already played, in [0.0, 1.0]. 0.0 when
    // nothing has played yet.
    virtual double progress() const = 0;
};

}  // namespace cherry
