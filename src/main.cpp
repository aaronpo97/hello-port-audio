#include "../include/MidiNotes.hpp"
#include "../include/constants.hpp"
#include "../include/PortAudioStream.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <portaudio.h>

constexpr long num_seconds{5};
struct WaveTableData
{
    float phase = 0.0;
    float freq = midi_to_frequency(MidiNotes::DX3);
};

int main()
{
    WaveTableData wave_table_data;

    // Note: The audio callback runs in a special context (often a real-time
    // thread or interrupt). Avoid calling functions that may block, allocate
    // memory, or take unpredictable time, as this can cause audio glitches or
    // dropouts. Keep the callback fast and deterministic.
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
            // @note calling std::sinf here is bad practice
            // @todo use a precomputed table of values or different algorithm
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