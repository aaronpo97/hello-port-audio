#pragma once
#include <cstdint>
#include <numbers>

/**
 * \namespace constants::audio
 *  Audio-related constants such as sample rate and buffer size.
 */
namespace constants::audio
{
    constexpr float standard_A4_hz = 440.0f; /** Standard A4 pitch in Hz. */
    constexpr float sample_rate{44100};      /** Default sample rate. */
    constexpr unsigned long frames_per_buffer{
        64}; /** Frames per audio buffer. */
}

/**
 * \namespace constants::math
 *  Mathematical constants used in audio processing.
 */
namespace constants::math
{
    constexpr float tau{
        2.0f * std::numbers::pi_v<float>}; /** Tau constant (2 * pi). */
}
