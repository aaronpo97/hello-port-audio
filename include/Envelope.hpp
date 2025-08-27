#pragma once
#include "../include/constants.hpp"
#include <atomic>

/**
 * \enum EnvelopeStage
 * Represents the stages of an ADSR envelope.
 */
enum class EnvelopeStage
{
    Attack,  /** Attack stage */
    Decay,   /** Decay stage */
    Sustain, /** Sustain stage */
    Release, /** Release stage */
    Idle     /** Idle stage */
};

/**
 * \class Envelope
 * Implements an ADSR envelope generator with thread-safe parameters.
 */
class Envelope
{
    std::atomic<uint64_t> m_attackTimeMs;
    std::atomic<uint64_t> m_decayTimeMs;
    std::atomic<float>    m_sustainLevel;
    std::atomic<uint64_t> m_releaseTimeMs;

    std::atomic<EnvelopeStage> m_stage{EnvelopeStage::Idle};
    std::atomic<float>         m_stageTime{}; // time spent in current stage
    std::atomic<float>         m_amplitude{};

  public:
    /**
     * Construct a new Envelope object.
     * \param attackMs Attack time in milliseconds.
     * \param decayMs Decay time in milliseconds.
     * \param sustain Sustain level (0.0 to 1.0).
     * \param releaseMs Release time in milliseconds.
     */
    explicit Envelope(uint64_t attackMs  = 100,
                      uint64_t decayMs   = 200,
                      float    sustain   = 0.7f,
                      uint64_t releaseMs = 500);

    /**
     * Copy constructor.
     * \param other Envelope to copy from.
     */
    Envelope(Envelope const &other);

    // Set ADSR parameters (thread-safe)
    void setAttackMs(uint64_t ms);
    void setDecayMs(uint64_t ms);
    void setSustain(float level); // 0.0 to 1.0
    void setReleaseMs(uint64_t ms);

    // Trigger note on/off
    void noteOn();
    void noteOff();

    // Process one sample and return amplitude multiplier (0.0 to 1.0)
    float processEnvelope();

    // Get current state
    [[nodiscard]] float         getCurrentLevel() const;
    [[nodiscard]] EnvelopeStage getCurrentStage() const;
    [[nodiscard]] bool          isActive() const;
};