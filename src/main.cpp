#include <array>
#include <cstdlib>
#include <iostream>
#include <numbers>
#include <portaudio.h>

constexpr long          num_seconds{5};
constexpr float         sample_rate{44100};
constexpr unsigned long frames_per_buffer{64};
constexpr float         tau{2.0f * std::numbers::pi_v<float>};

struct WaveTableData
{
    float phase = 0.0;
    float freq  = 220.0;
};

class PortAudioStream
{
    PaStream *m_paStream = nullptr;

  public:
    PortAudioStream(PaStreamParameters const *output_parameters,
                    PaStreamCallback         *callback,
                    void                     *user_data);

    void setFinishedCallback(PaStreamFinishedCallback *cb,
                             void                     *user_data) const;
    void start() const;
    void stop() const;

    ~PortAudioStream();
};

PortAudioStream::PortAudioStream(PaStreamParameters const *output_parameters,
                                 PaStreamCallback         *callback,
                                 void                     *user_data)
{
    PaError err =
        Pa_OpenStream(&m_paStream, nullptr, output_parameters, sample_rate,
                      frames_per_buffer, paClipOff, callback, user_data);

    if (err != paNoError)
    {
        throw std::runtime_error(Pa_GetErrorText(err));
    }
}
void PortAudioStream::setFinishedCallback(PaStreamFinishedCallback *cb,
                                          void * /*user_data*/) const
{
    PaError err = Pa_SetStreamFinishedCallback(m_paStream, cb);
    if (err != paNoError)
    {
        throw std::runtime_error(Pa_GetErrorText(err));
    }
}

void PortAudioStream::start() const
{
    PaError err = Pa_StartStream(m_paStream);
    if (err != paNoError)
    {
        throw std::runtime_error(Pa_GetErrorText(err));
    }
}

void PortAudioStream::stop() const
{
    if (m_paStream)
    {
        Pa_StopStream(m_paStream);
    }
}

PortAudioStream::~PortAudioStream()
{
    if (m_paStream)
    {
        Pa_CloseStream(m_paStream);
    }
}

int main()
{
    WaveTableData wave_table_data;

    PaStreamCallback *cb =
        +[](void const * /*_inputBuffer*/, void *output_buffer,
            unsigned long const frames_per_buffer,
            PaStreamCallbackTimeInfo const * /*_time_info*/,
            PaStreamCallbackFlags /*_status_flags*/, void *test_data) -> int
    {
        auto *out  = static_cast<float *>(output_buffer);
        auto *data = static_cast<WaveTableData *>(test_data);

        // Calculate how much to advance phase each sample (normalized 0–1)
        float const phaseInc = data->freq / sample_rate;

        for (unsigned long i = 0; i < frames_per_buffer; ++i)
        {
            auto const sample = std::sinf(tau * data->phase);

            // Write same sample to both channels (stereo)
            *out++ = sample;
            *out++ = sample;

            // Advance phase
            data->phase += phaseInc;
            if (data->phase >= 1.0)
                data->phase -= 1.0;
        }

        return paContinue;
    };

    PaStreamParameters output_parameters{.channelCount              = 2,
                                         .sampleFormat              = paFloat32,
                                         .suggestedLatency          = 0.0,
                                         .hostApiSpecificStreamInfo = nullptr};
    try
    {
        if (PaError const err = Pa_Initialize(); err != paNoError)
        {
            throw std::runtime_error(Pa_GetErrorText(err));
        }

        output_parameters.device = Pa_GetDefaultOutputDevice();
        if (output_parameters.device == paNoDevice)
        {
            throw std::runtime_error("No default output device.");
        }

        // Create a PortAudio stream for audio output
        PortAudioStream const s(&output_parameters, cb, &wave_table_data);

        s.setFinishedCallback(
            +[](void *) -> void { std::cout << "Stream Completed.\n"; },
            &wave_table_data);

        s.start();

        std::cout << "Playing for " << num_seconds << " seconds." << std::endl;
        Pa_Sleep(num_seconds * 1000);

        s.stop();
        Pa_Terminate();
    }
    catch (std::exception const &e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Test finished." << std::endl;
    return EXIT_SUCCESS;
}