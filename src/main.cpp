#include "../include/MidiNotes.hpp"
#include "../include/PortAudioStream.hpp"
#include "../include/StreamState.hpp"
#include "../include/constants.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <portaudio.h>

constexpr long num_seconds{5};

int main()
{
    // Start on A4
    StreamState stream_state{midi_to_frequency(MidiNotes::A4)};

    /**
     * \note This callback runs on the real-time audio thread and therefore must
     * not perform any blocking operations. It should complete within a
     * deterministic amount of time.
     *
     * \warning Any state shared between the application thread and the callback
     * thread must be communicated safely using std::atomic (or another
     * lock-free mechanism) to prevent data races.
     */
    PaStreamCallback *cb =
        +[](void const * /*_inputBuffer*/, void *output_buffer,
            unsigned long const frames_per_buffer,
            PaStreamCallbackTimeInfo const * /*_time_info*/,
            PaStreamCallbackFlags /*_status_flags*/, void *test_data) -> int
    {
        auto *out  = static_cast<float *>(output_buffer);
        auto *data = static_cast<StreamState *>(test_data);

        // Per-buffer control fetch
        float const freq     = data->getCurrentFrequency();
        float const phaseInc = freq / constants::audio::sample_rate;

        for (unsigned long i = 0; i < frames_per_buffer; ++i)
        {
            size_t const idx =
                c_tableMask &
                static_cast<size_t>(data->getPhase() * c_tableSize);

            float const sample = data->getWaveTable()[idx];
            *out++             = sample; // L
            *out++             = sample; // R

            float ph = data->getPhase() + phaseInc;
            if (ph >= 1.0f)
                ph -= 1.0f;
            data->setPhase(ph);
        }

        return paContinue;
    };

    PaStreamParameters output_parameters{.device = paNoDevice, // will set below
                                         .channelCount              = 2,
                                         .sampleFormat              = paFloat32,
                                         .suggestedLatency          = 0.0,
                                         .hostApiSpecificStreamInfo = nullptr};

    try
    {

        if (PaError const err = Pa_Initialize(); err != paNoError)
            throw std::runtime_error(Pa_GetErrorText(err));

        output_parameters.device = Pa_GetDefaultOutputDevice();
        if (output_parameters.device == paNoDevice)
            throw std::runtime_error("No default output device.");

        // Create and run stream
        PortAudioStream const audio_stream({}, output_parameters, cb,
                                           &stream_state);

        audio_stream.setFinishedCallback(
            +[](void *) { std::cout << "Stream Completed.\n"; });
        audio_stream.start();

        // Play whole tone scale A2..A6 ascending and descending

        {
            constexpr int64_t sleep_time = 100;
            using U = std::underlying_type_t<MidiNotes>;
            for (auto n = MidiNotes::A2; n < MidiNotes::A6;
                 n      = static_cast<MidiNotes>(static_cast<U>(n) + 2))
            {
                stream_state.setFrequency(midi_to_frequency(n));
                Pa_Sleep(sleep_time);
            }

            for (auto n = MidiNotes::A6; n >= MidiNotes::A2;
                 n      = static_cast<MidiNotes>(static_cast<U>(n) - 2))
            {
                stream_state.setFrequency(midi_to_frequency(n));
                Pa_Sleep(sleep_time);
            }
        }
        
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
