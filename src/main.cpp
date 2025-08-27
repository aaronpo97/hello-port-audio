#include "../include/MidiNote.hpp"
#include "../include/PortAudioStream.hpp"
#include "../include/StreamState.hpp"
#include "../include/constants.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <portaudio.h>

int main()
{
    StreamState stream_state(midi_to_frequency(MidiNote::A4),
                             Envelope{100, 200, 0.7f, 500});

    /**
     * \note This callback runs on the real-time audio thread and therefore must
     * not perform any blocking operations. It should complete within a
     * deterministic amount of time.
     *
     * \warning Any state shared between the application thread and the callback
     * thread must be communicated safely using std::atomic (or another
     * lock-free mechanism) to prevent data races.
     */
    PaStreamCallback *stream_cb =
        +[](void const                     *inputBuffer,     //
            void                           *outputBuffer,    //
            unsigned long const             framesPerBuffer, //
            PaStreamCallbackTimeInfo const *timeInfo,        //
            PaStreamCallbackFlags           statusFlags,     //
            void                           *userData) -> int
    {
        auto *out  = static_cast<float *>(outputBuffer);
        auto *data = static_cast<StreamState *>(userData);

        float const freq     = data->getCurrentFrequency();
        float const phaseInc = freq / constants::audio::sample_rate;

        for (unsigned long i = 0; i < framesPerBuffer; ++i)
        {
            size_t const idx =
                c_tableMask &
                static_cast<size_t>(data->getPhase() * c_tableSize);

            float const oscillator = data->getWaveTable()[idx];

            // Apply envelope to the oscillator output
            float const envelope = data->getEnvelope().processEnvelope();
            float const sample   = oscillator * envelope;

            *out++ = sample; // L
            *out++ = sample; // R

            float ph = data->getPhase() + phaseInc;
            if (ph >= 1.0f)
                ph -= 1.0f;
            data->setPhase(ph);
        }

        return paContinue;
    };

    PaStreamFinishedCallback *finished_cb =
        +[](void *userData) { std::cout << "Stream completed." << std::endl; };

    PaStreamParameters output_parameters{.device           = paNoDevice,
                                         .channelCount     = 2,
                                         .sampleFormat     = paFloat32,
                                         .suggestedLatency = 0.0,
                                         .hostApiSpecificStreamInfo = nullptr};

    try
    {
        if (PaError const err = Pa_Initialize(); err != paNoError)
            throw std::runtime_error(Pa_GetErrorText(err));

        output_parameters.device = Pa_GetDefaultOutputDevice();
        if (output_parameters.device == paNoDevice)
            throw std::runtime_error("No default output device.");

        // Create and run stream
        PortAudioStream audio_stream({}, output_parameters, stream_cb,
                                     &stream_state);

        audio_stream.setFinishedCallback(finished_cb);
        audio_stream.start();

        // Play diminished seventh arpeggio ascending and descending with
        // envelope
        {
            constexpr int64_t note_duration = 100; // ms per note
            constexpr int64_t note_gap      = 80;  // ms between notes

            using U = std::underlying_type_t<MidiNote>;

            Envelope &env = stream_state.getEnvelope();

            constexpr auto upper = MidiNote::A2;
            constexpr auto lower = MidiNote::A7;

            auto play_note = [&](MidiNote const n) -> void
            {
                stream_state.setFrequency(midi_to_frequency(n));
                env.noteOn(); // Trigger envelope
                Pa_Sleep(note_duration);
                env.noteOff();      // Release envelope
                Pa_Sleep(note_gap); // Wait for release to complete
            };
            // Ascending
            for (auto note = upper; note < lower;
                 note      = static_cast<MidiNote>(static_cast<U>(note) + 3))
            {
                play_note(note);
            }

            // Descending
            for (auto note = lower; note >= upper;
                 note      = static_cast<MidiNote>(static_cast<U>(note) - 3))
            {
                play_note(note);
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