#pragma once
#include <portaudio.h>

class PortAudioStream
{
    PaStream *m_paStream = nullptr;

public:
    PortAudioStream(PaStreamParameters const *output_parameters,
                    PaStreamCallback         *callback,
                    void                     *user_data);

    void setFinishedCallback(PaStreamFinishedCallback *cb) const;
    void start() const;
    void stop() const;

    ~PortAudioStream();
};
