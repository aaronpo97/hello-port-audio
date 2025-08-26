#include "../include/Envelope.hpp"
#include <algorithm>

void Envelope::setAttackMs(uint64_t ms)
{
    m_attackTimeMs.store(ms, std::memory_order_relaxed);
}

void Envelope::setDecayMs(uint64_t ms)
{
    m_decayTimeMs.store(ms, std::memory_order_relaxed);
}

void Envelope::setSustain(float const level)
{
    m_sustainLevel.store(std::clamp(level, 0.0f, 1.0f),
                         std::memory_order_relaxed);
}

void Envelope::setReleaseMs(uint64_t ms)
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

float Envelope::processSample()
{
    constexpr float sampleTime = 1.0f / constants::audio::sample_rate;

    EnvelopeStage const stage     = m_stage.load(std::memory_order_relaxed);
    float const         stageTime = m_stageTime.load(std::memory_order_relaxed);
    float currentLevel = m_currentLevel.load(std::memory_order_relaxed);

    switch (stage)
    {
        case EnvelopeStage::Attack:
        {
            float const attackTimeSec = static_cast<float>(m_attackTimeMs.load(
                                            std::memory_order_relaxed)) /
                                        1000.0f;
            currentLevel = stageTime / std::max(attackTimeSec, 0.001f);

            if (currentLevel >= 1.0f)
            {
                currentLevel = 1.0f;
                m_stage.store(EnvelopeStage::Decay, std::memory_order_relaxed);
                m_stageTime.store(0.0f, std::memory_order_relaxed);
            }
            else
            {
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
            float const decayAmount =
                (1.0f - sustainLevel) *
                (stageTime / std::max(decayTimeSec, 0.001f));

            currentLevel = 1.0f - decayAmount;

            if (currentLevel <= sustainLevel || stageTime >= decayTimeSec)
            {
                currentLevel = sustainLevel;
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
            currentLevel = m_sustainLevel.load(std::memory_order_relaxed);
            // Stay in sustain until noteOff() is called
            break;
        }

        case EnvelopeStage::Release:
        {
            float const releaseTimeSec =
                static_cast<float>(
                    m_releaseTimeMs.load(std::memory_order_relaxed)) /
                1000.0f;
            float const releaseStart =
                m_currentLevel.load(std::memory_order_relaxed);
            float const releaseAmount =
                releaseStart * (stageTime / std::max(releaseTimeSec, 0.001f));

            currentLevel = releaseStart - releaseAmount;

            if (currentLevel <= 0.0f || stageTime >= releaseTimeSec)
            {
                currentLevel = 0.0f;
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
            currentLevel = 0.0f;
            break;
    }

    m_currentLevel.store(currentLevel, std::memory_order_relaxed);
    return currentLevel;
}

float Envelope::getCurrentLevel() const
{
    return m_currentLevel.load(std::memory_order_relaxed);
}

EnvelopeStage Envelope::getCurrentStage() const
{
    return m_stage.load(std::memory_order_relaxed);
}

bool Envelope::isActive() const
{
    return m_stage.load(std::memory_order_relaxed) != EnvelopeStage::Idle;
}