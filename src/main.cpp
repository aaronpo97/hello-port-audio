#include <portaudio.h>
#include <cmath>
#include <iostream>
#include <numbers>

constexpr unsigned long num_seconds       = 5;
constexpr unsigned long sample_rate       = 44100;
constexpr unsigned long frames_per_buffer = 64;
constexpr double        tau               = 2.0 * std::numbers::pi;

struct PaTestData
{
    double phase = 0.0;
};

// PortAudio callback function to generate a stereo sine wave
static int pa_test_callback(void const                     *,
                            void                           *output_buffer,
                            unsigned long                   frames_per_buffer,
                            PaStreamCallbackTimeInfo const *,
                            PaStreamCallbackFlags           ,
                            void                           *user_data)
{
    // Cast output buffer to float pointer (for stereo float samples)
    auto            *out            = static_cast<float *>(output_buffer);
    // Cast user data to PaTestData
    auto            *data           = static_cast<PaTestData *>(user_data);
    constexpr double freq           = 100.0; 

    // Phase step per sample
    constexpr double phase_increment = tau * freq / sample_rate; 

    // Generate samples for each frame in the buffer
    for (uint64_t i = 0; i < frames_per_buffer; ++i)
    {
        float sample = static_cast<float>(std::sin(data->phase)); // Compute sine value at current phase
        
        // Write to left channel
        *out++       = sample;
        
        // Write to right channel
        *out++       = sample; 

        data->phase += phase_increment; // Advance phase

        if (data->phase >= tau)
        {
            data->phase = std::fmod(data->phase, tau); // Wrap phase to [theta, tau) 
        }
    }
    return paContinue; // Continue playback
}

static void stream_finished(void *user_data)
{
    PaTestData *data = static_cast<PaTestData *>(user_data);
    std::cout << "Stream Completed." << std::endl;
}

class PortAudioStream
{
    PaStream *pa_stream = nullptr;

  public:
    PortAudioStream(PaStreamParameters const *output_parameters,
           PaStreamCallback         *callback,
           void                     *user_data)
    {
        PaError err =
            Pa_OpenStream(&pa_stream, nullptr, output_parameters, sample_rate,
                          frames_per_buffer, paClipOff, callback, user_data);
        if (err != paNoError)
        {
            throw std::runtime_error(Pa_GetErrorText(err));
        }
    }

    void set_finished_callback(PaStreamFinishedCallback *cb, void *user_data)
    {
        PaError err = Pa_SetStreamFinishedCallback(pa_stream, cb);
        if (err != paNoError)
        {
            throw std::runtime_error(Pa_GetErrorText(err));
        }
    }

    void start()
    {
        PaError err = Pa_StartStream(pa_stream);
        if (err != paNoError)
        {
            throw std::runtime_error(Pa_GetErrorText(err));
        }
    }

    void stop()
    {
        if (pa_stream)
        {
            Pa_StopStream(pa_stream);
        }
    }

    ~PortAudioStream()
    {
        if (pa_stream)
        {
            Pa_CloseStream(pa_stream);
        }
    }
};

int main(void)
{
    PaError      err;
    PaTestData data;

    std::cout << "PortAudio Test: output sine wave. SR = " << sample_rate
              << ", BufSize = " << frames_per_buffer << std::endl;

    PaStreamParameters output_parameters{.device       = 0, // Will be set below
                                         .channelCount = 2, // Stereo output
                                         .sampleFormat = paFloat32,
                                         .suggestedLatency          = 0.0,
                                         .hostApiSpecificStreamInfo = nullptr};

    try
    {
        err = Pa_Initialize();
        if (err != paNoError)
        {
            throw std::runtime_error(Pa_GetErrorText(err));
        }

        output_parameters.device = Pa_GetDefaultOutputDevice();
        if (output_parameters.device == paNoDevice)
        {
            throw std::runtime_error("No default output device.");
        }

        PortAudioStream s(&output_parameters, pa_test_callback, &data);
        s.set_finished_callback(&stream_finished, &data);
        s.start();

        std::cout << "Play for " << num_seconds << " seconds." << std::endl;
        Pa_Sleep(num_seconds * 1000);

        s.stop();
        Pa_Terminate();
        std::cout << "Test finished." << std::endl;
        return 0;
    }
    catch (std::exception const &ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        Pa_Terminate();
        return 1;
    }
}