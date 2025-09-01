#include <array>
#include <concepts>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <type_traits>

#include <portaudio.h>

#include "../include/MidiPitch.hpp"
#include "../include/PortAudioStream.hpp"
#include "../include/StreamState.hpp"
#include "../include/constants.hpp"

template <typename PlayNoteFn>
  requires std::invocable<PlayNoteFn &, MidiPitch>
void play_dim7_arp(MidiPitch const low,
                   MidiPitch const high,
                   PlayNoteFn    &&play_note)
{

  using U          = std::underlying_type_t<MidiPitch>;
  U const lo       = static_cast<U>(low);
  U const hi       = static_cast<U>(high);
  U constexpr step = 3;

  // Ascending (inclusive)
  for (U n = lo; n <= hi; n += step)
    play_note(static_cast<MidiPitch>(n));

  for (U n = hi;;)
  {
    play_note(static_cast<MidiPitch>(n));
    if (n <= lo)
      break;
    n -= step;
  }
}

int main()
{

  StreamState stream_state(midi_pitch_to_frequency(MidiPitch::A4),
                           Envelope{10, 10, 0.4f, 100});

  /**
   * \warning This callback runs on a seperate, real time thread and must not
   * contain any blocking operations. Due to this, it should complete within a
   * deterministic amount of time. Furthermore, any state shared between the
   * application thread and the callback thread must be communicated safely
   * using std::atomic to prevent data races.
   */
  PaStreamCallback *stream_cb = +[](void const         *inputBuffer,          //
                                    void               *outputBuffer,         //
                                    unsigned long const framesPerBuffer,      //
                                    PaStreamCallbackTimeInfo const *timeInfo, //
                                    PaStreamCallbackFlags statusFlags,        //
                                    void                 *userData) -> int
  {
    auto *out  = static_cast<float *>(outputBuffer);
    auto *data = static_cast<StreamState *>(userData);

    float const freq     = data->getCurrentFrequency();
    float const phaseInc = freq / constants::audio::sample_rate;

    for (unsigned long i = 0; i < framesPerBuffer; ++i)
    {
      size_t const idx =
          c_tableMask & static_cast<size_t>(data->getPhase() * c_tableSize);

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

  PaStreamParameters output_parameters{.device                    = paNoDevice,
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
    PortAudioStream audio_stream({}, output_parameters, stream_cb,
                                 &stream_state);

    audio_stream.setFinishedCallback(finished_cb);
    audio_stream.start();

    // Play diminished seventh arpeggio ascending and descending with
    // envelope

    constexpr int64_t note_duration = 60; // ms per note
    constexpr int64_t note_gap      = 30; // ms between notes

    auto const play_note = [&stream_state](MidiPitch const n) -> void
    {
      Envelope &env = stream_state.getEnvelope();
      stream_state.setFrequency(midi_pitch_to_frequency(n));
      env.noteOn(); // Trigger envelope
      Pa_Sleep(note_duration);
      env.noteOff();      // Release envelope
      Pa_Sleep(note_gap); // Wait for release to complete
    };

    constexpr size_t repetitions = 4;
    for (size_t i = 0; i < repetitions; ++i)
    {
      play_dim7_arp(MidiPitch::A3, MidiPitch::A6, play_note);
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