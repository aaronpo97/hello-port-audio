#pragma once
#include <cstdint>
#include <numbers>

constexpr uint32_t      standard_A4_hz = 440;
constexpr float         sample_rate{44100};
constexpr unsigned long frames_per_buffer{64};
constexpr float         tau{2.0f * std::numbers::pi_v<float>};
