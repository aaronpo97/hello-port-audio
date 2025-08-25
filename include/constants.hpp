#pragma once
#include <cstdint>
#include <numbers>

constexpr float         standard_A4_hz = 440.0f;
constexpr float         sample_rate{44100};
constexpr unsigned long frames_per_buffer{64};
constexpr float         tau{2.0f * std::numbers::pi_v<float>};
