#include "../include/MidiNotes.hpp"
#include "../include/PortAudioStream.hpp"
#include "../include/constants.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <portaudio.h>

constexpr size_t table_size = 2048;

std::array<float, table_size>
make_sine_table(float const frequency = standard_A4_hz)
{
    std::array<float, table_size> table{};
    for (std::size_t i = 0; i < table_size; ++i)
    {
        float const phase = static_cast<float>(i) / table_size; // 0..1
        table[i]          = std::sinf(phase * tau);
    }
    return table;
}

constexpr long num_seconds{5};

struct WaveTableData
{
    float                         phase;
    float                         freq;
    std::array<float, table_size> wave_table;
};

int main()
{
    WaveTableData wave_table_data{.phase = 0.0,
                                  .freq  = midi_to_frequency(MidiNotes::A3)};

    wave_table_data.wave_table = make_sine_table(wave_table_data.freq);

    PaStreamCallback *cb = +[](void const         *_inputBuffer,              //
                               void               *output_buffer,             //
                               unsigned long const frames_per_buffer,         //
                               PaStreamCallbackTimeInfo const *_time_info,    //
                               PaStreamCallbackFlags           _status_flags, //
                               void                           *test_data) -> int
    {
        auto *out  = static_cast<float *>(output_buffer);
        auto *data = static_cast<WaveTableData *>(test_data);

        // Calculate how much to advance phase each sample (normalized 0–1)
        float const phaseInc = data->freq / sample_rate;

        for (unsigned long i = 0; i < frames_per_buffer; ++i)
        {
            float const sample =
                data->wave_table[static_cast<size_t>(data->phase * table_size) %
                                 table_size];
            *out++ = sample; // Left channel
            *out++ = sample; // Right channel

            // Advance phase, wrap around at 1.0
            data->phase += phaseInc;
            if (data->phase >= 1.0f)
                data->phase -= 1.0f;
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
        PortAudioStream const audio_stream(&output_parameters, cb,
                                           &wave_table_data);

        audio_stream.setFinishedCallback(
            +[](void *) -> void { std::cout << "Stream Completed.\n"; });

        audio_stream.start();

        std::cout << "Playing for " << num_seconds << " seconds." << std::endl;
        Pa_Sleep(num_seconds * 1000);

        audio_stream.stop();
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