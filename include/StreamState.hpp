#pragma once

#include <array>
#include <atomic>

constexpr size_t c_tableSize = 1ull << 12; // 4096 (must be power of 2)
constexpr size_t c_tableMask = c_tableSize - 1;
class StreamState
{
    std::atomic<float> m_currentPhase = 0.0f;
    std::atomic<float> m_freq; // modified by user thread, used by audio thread
    std::array<float, c_tableSize> m_waveTable;

  public:
    explicit StreamState(float initFreq = 0.0f);

    void setPhase(float phase);
    void setFrequency(float frequency);

    [[nodiscard]] float getPhase() const;
    [[nodiscard]] float getCurrentFrequency() const;
    [[nodiscard]] std::array<float, c_tableSize> const &getWaveTable() const;
};
