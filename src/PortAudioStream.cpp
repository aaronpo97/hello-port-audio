#include "../include/PortAudioStream.hpp"
#include "../include/constants.hpp"
#include <stdexcept>

PortAudioStream::PortAudioStream(PaStreamParameters const &input_parameters,
                                 PaStreamParameters const &output_parameters,
                                 PaStreamCallback         *callback,
                                 void                     *user_data)
{
    PaError err = Pa_OpenStream(
        &m_paStream, nullptr, &output_parameters, constants::audio::sample_rate,
        constants::audio::frames_per_buffer, paClipOff, callback, user_data);

    if (err != paNoError)
    {
        throw std::runtime_error(Pa_GetErrorText(err));
    }
}
void PortAudioStream::setFinishedCallback(PaStreamFinishedCallback *cb) const
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
