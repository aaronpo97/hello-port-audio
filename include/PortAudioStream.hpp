#pragma once
#include <portaudio.h>

class PortAudioStream
{
    PaStream *m_paStream = nullptr;

    void cleanupStream();

  public:
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

    void setFinishedCallback(PaStreamFinishedCallback *cb);
    void start();
    void stop();

    ~PortAudioStream();
};
