#pragma once
#include <cstdint>
#include <numbers>

/**
 * \namespace constants::audio
 * Audio-related constants such as sample rate and buffer size.
 */
namespace constants::audio
{
  constexpr float    standard_A4_hz    = 440.0f;
  constexpr float    sample_rate       = 44100;
  constexpr uint64_t frames_per_buffer = 64;
} // namespace constants::audio


namespace constants::audio::midi
{
  constexpr int8_t min = 0;
  constexpr int8_t max = 127;
}
/**
 * \namespace constants::math
 *  Mathematical constants used in audio processing.
 */
namespace constants::math
{
  constexpr float tau = 2.0f * std::numbers::pi_v<float>;
} // namespace constants::math
