#include "../include/StreamState.hpp"
#include "../include/Envelope.hpp"
#include "../include/constants.hpp"
#include <cmath>

StreamState::StreamState(float const initFreq, Envelope const &env)
    : m_freq(initFreq),
      m_waveTable(
          []
          {
              std::array<float, c_tableSize> t{};
              for (std::size_t i = 0; i < c_tableSize; ++i)
              {
                  float const phase = static_cast<float>(i) / c_tableSize;
                  t[i]              = std::sinf(phase * constants::math::tau);
              }
              return t;
          }()),
      m_envelope(env)
{
}

Envelope &StreamState::getEnvelope() { return m_envelope; }

float StreamState::getPhase() const
{
    return m_currentPhase.load(std::memory_order_relaxed);
}

void StreamState::setPhase(float const phase)
{
    m_currentPhase.store(phase, std::memory_order_relaxed);
}

float StreamState::getCurrentFrequency() const
{
    return m_freq.load(std::memory_order_relaxed);
}

void StreamState::setFrequency(float const frequency)
{
    m_freq.store(frequency, std::memory_order_relaxed);
}

std::array<float, c_tableSize> const &StreamState::getWaveTable() const
{
    return m_waveTable;
}