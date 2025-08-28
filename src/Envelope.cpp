#include "../include/Envelope.hpp"
#include <algorithm>

Envelope::Envelope(uint64_t const attackMs,
                   uint64_t const decayMs,
                   float const    sustain,
                   uint64_t const releaseMs)
    : m_attackTimeMs(attackMs), m_decayTimeMs(decayMs), m_sustainLevel(sustain),
      m_releaseTimeMs(releaseMs)
{
}
Envelope::Envelope(Envelope const &other)
    : m_attackTimeMs(other.m_attackTimeMs.load(std::memory_order_relaxed)),
      m_decayTimeMs(other.m_decayTimeMs.load(std::memory_order_relaxed)),
      m_sustainLevel(other.m_sustainLevel.load(std::memory_order_relaxed)),
      m_releaseTimeMs(other.m_releaseTimeMs.load(std::memory_order_relaxed)),
      m_stage(other.m_stage.load(std::memory_order_relaxed)),
      m_stageTime(other.m_stageTime.load(std::memory_order_relaxed)),
      m_amplitude(other.m_amplitude.load(std::memory_order_relaxed))
{
}
void Envelope::setAttackMs(uint64_t const ms)
{
    m_attackTimeMs.store(ms, std::memory_order_relaxed);
}

void Envelope::setDecayMs(uint64_t const ms)
{
    m_decayTimeMs.store(ms, std::memory_order_relaxed);
}

void Envelope::setSustain(float const level)
{
    m_sustainLevel.store(std::clamp(level, 0.0f, 1.0f),
                         std::memory_order_relaxed);
}

void Envelope::setReleaseMs(uint64_t const ms)
{
    m_releaseTimeMs.store(ms, std::memory_order_relaxed);
}

void Envelope::noteOn()
{
    m_stage.store(EnvelopeStage::Attack, std::memory_order_relaxed);
    m_stageTime.store(0.0f, std::memory_order_relaxed);
}

void Envelope::noteOff()
{
    if (m_stage.load(std::memory_order_relaxed) != EnvelopeStage::Idle)
    {
        m_stage.store(EnvelopeStage::Release, std::memory_order_relaxed);
        m_stageTime.store(0.0f, std::memory_order_relaxed);
    }
}

float Envelope::processEnvelope()
{
    constexpr float sampleTime = 1.0f / constants::audio::sample_rate;

    EnvelopeStage const stage     = m_stage.load(std::memory_order_relaxed);
    float const         stageTime = m_stageTime.load(std::memory_order_relaxed);
    float currentAmplitude        = m_amplitude.load(std::memory_order_relaxed);

    switch (stage)
    {
        case EnvelopeStage::Attack:
        {
            float const attackTimeSec = static_cast<float>(m_attackTimeMs.load(
                                            std::memory_order_relaxed)) /
                                        1000.0f;

            // Linear ramp: 0% to 100% over attack time
            currentAmplitude = stageTime / std::max(attackTimeSec, 0.001f);

            // Example: if stageTime = 0.025s and ATTACK_TIME = 0.05s
            // currentLevel = 0.025 / 0.05 = 0.5 (50% amplitude)
            if (currentAmplitude >= 1.0f)
            {
                // Attack is complete, move to decay
                // currentLevel = 1.0f; -- reset down to 1.0
                m_stage.store(EnvelopeStage::Decay, std::memory_order_relaxed);
                m_stageTime.store(0.0f, std::memory_order_relaxed);
            }
            else
            {
                // Continue attack, increment time
                m_stageTime.store(stageTime + sampleTime,
                                  std::memory_order_relaxed);
            }
            break;
        }

        case EnvelopeStage::Decay:
        {
            float const decayTimeSec = static_cast<float>(m_decayTimeMs.load(
                                           std::memory_order_relaxed)) /
                                       1000.0f;
            float const sustainLevel =
                m_sustainLevel.load(std::memory_order_relaxed);

            // Calculate how much to decay from peak to get to sustain level
            float const decayAmount =
                (1.0f - sustainLevel) *
                (stageTime / std::max(decayTimeSec, 0.001f));

            currentAmplitude = 1.0f - decayAmount;

            if (currentAmplitude <= sustainLevel || stageTime >= decayTimeSec)
            {
                // Delay complete, move to sustain
                currentAmplitude = sustainLevel;
                m_stage.store(EnvelopeStage::Sustain,
                              std::memory_order_relaxed);
                m_stageTime.store(0.0f, std::memory_order_relaxed);
            }
            else
            {
                m_stageTime.store(stageTime + sampleTime,
                                  std::memory_order_relaxed);
            }
            break;
        }

        case EnvelopeStage::Sustain:
        {
            currentAmplitude = m_sustainLevel.load(std::memory_order_relaxed);
            // Stay in sustain until noteOff() is called
            break;
        }

        case EnvelopeStage::Release:
        {
            float const releaseTimeSec =
                static_cast<float>(
                    m_releaseTimeMs.load(std::memory_order_relaxed)) /
                1000.0f;

            // How much amplitude should be reduced
            float const releaseAmount =
                currentAmplitude *
                (stageTime / std::max(releaseTimeSec, 0.001f));

            currentAmplitude = currentAmplitude - releaseAmount;

            // If amplitude reaches 0 or release time is up, go to idle
            if (currentAmplitude <= 0.0f || stageTime >= releaseTimeSec)
            {
                currentAmplitude = 0.0f;
                m_stage.store(EnvelopeStage::Idle, std::memory_order_relaxed);
                m_stageTime.store(0.0f, std::memory_order_relaxed);
            }
            else
            {
                m_stageTime.store(stageTime + sampleTime,
                                  std::memory_order_relaxed);
            }
            break;
        }

        case EnvelopeStage::Idle:
        default:
            currentAmplitude = 0.0f;
            break;
    }

    m_amplitude.store(currentAmplitude, std::memory_order_relaxed);
    return currentAmplitude;
}

float Envelope::getCurrentLevel() const
{
    return m_amplitude.load(std::memory_order_relaxed);
}

EnvelopeStage Envelope::getCurrentStage() const
{
    return m_stage.load(std::memory_order_relaxed);
}

bool Envelope::isActive() const
{
    return m_stage.load(std::memory_order_relaxed) != EnvelopeStage::Idle;
}