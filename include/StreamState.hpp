#pragma once

#include <array>
#include <atomic>
#include <memory>

#include "../include/Envelope.hpp"

constexpr size_t c_tableSize = 1ull << 12; // 4096 (must be power of 2)
constexpr size_t c_tableMask = c_tableSize - 1;

/**
 * \class StreamState
 *  Holds the state for audio streaming, including phase, frequency,
 * wavetable, and envelope.
 */
class StreamState
{
  /**
   *  Current phase of the oscillator (atomic for thread safety).
   */
  std::atomic<float> m_currentPhase = 0.0f;

  /**
   *  Current frequency in Hz (atomic for thread safety). Modified by
   * user thread, used by audio thread.
   */
  std::atomic<float> m_freq;

  /**
   *  Wavetable for oscillator synthesis.
   */
  std::array<float, c_tableSize> m_waveTable;

  /**
   *  Envelope generator for amplitude shaping.
   */
  Envelope m_envelope;

public:
  /**
   *  Construct a new StreamState object.
   * \param initFreq Initial frequency in Hz.
   * \param env Envelope object for amplitude shaping.
   */
  explicit StreamState(float initFreq = 0.0f, Envelope const &env = Envelope{});

  /**
   *  Set the oscillator phase.
   * \param phase New phase value.
   */
  void setPhase(float phase);

  /**
   *  Set the oscillator frequency.
   * \param frequency New frequency in Hz.
   */
  void setFrequency(float frequency);

  /**
   *  Get the current oscillator phase.
   * \return Current phase value.
   */
  [[nodiscard]] float getPhase() const;

  /**
   *  Get the current frequency in Hz.
   * \return Current frequency value.
   */
  [[nodiscard]] float getCurrentFrequency() const;

  /**
   *  Get the wavetable used for synthesis.
   * \return Reference to the wavetable array.
   */
  [[nodiscard]] std::array<float, c_tableSize> const &getWaveTable() const;

  /**
   *  Get the envelope generator.
   * \return Reference to the Envelope object.
   */
  [[nodiscard]] Envelope &getEnvelope();
};