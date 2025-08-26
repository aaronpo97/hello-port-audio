#pragma once
#include <portaudio.h>
/**
 * \class PortAudioStream
 *  RAII wrapper for a PortAudio stream, managing its lifecycle and
 * callbacks.
 */
class PortAudioStream
{
    PaStream *m_paStream = nullptr;

    void cleanupStream();

  public:
  
    /**
     * Construct a new PortAudioStream object.
     * \param input_parameters Input stream parameters.
     * \param output_parameters Output stream parameters.
     * \param callback Pointer to the PortAudio callback function.
     * \param user_data Pointer to user data passed to the callback.
     */
    PortAudioStream(PaStreamParameters const &input_parameters,
                    PaStreamParameters const &output_parameters,
                    PaStreamCallback         *callback,
                    void                     *user_data);

    // Disable copying instances of the PortAudioStream
    PortAudioStream(PortAudioStream const &)            = delete;
    PortAudioStream &operator=(PortAudioStream const &) = delete;

    // Disable moving instances of PortAudioStream
    PortAudioStream(PortAudioStream &&other)            = delete;
    PortAudioStream &operator=(PortAudioStream &&other) = delete;

    /**
     * Set a callback to be called when the stream finishes.
     * \param cb Pointer to the finished callback function.
     */
    void setFinishedCallback(PaStreamFinishedCallback *cb);

    /**
     * Start the audio stream.
     */
    void start();

    /**
     * Stop the audio stream.
     */
    void stop();

    /**
     * Destructor. Cleans up the PortAudio stream.
     */
    ~PortAudioStream();
};
