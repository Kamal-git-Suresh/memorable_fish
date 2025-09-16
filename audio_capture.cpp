#include "audio_capture.h"
#include <iostream>

AudioCapture::AudioCapture(int deviceIndex, int sampleRate, int framesPerBuffer) 
    : sampleRate(sampleRate), framesPerBuffer(framesPerBuffer)
{
        Pa_Initialize();

        PaStreamParameters inputParams;
        inputParams.device = deviceIndex;
        inputParams.channelCount = 1;
        inputParams.sampleFormat = paFloat32;
        inputParams.suggestedLatency = 
            Pa_GetDeviceInfo(deviceIndex)->defaultLowInputLatency;
        inputParams.hostApiSpecificStreamInfo = nullptr;

        PaError err = Pa_OpenStream(&stream, &inputParams, nullptr, sampleRate, 
                        framesPerBuffer, paClipOff, 
                        &AudioCapture::paCallback, this);
        if (err != paNoError)
        {
            std::cerr << "Failed to open audio stream: " << Pa_GetErrorText(err) << "\n";
            stream = nullptr;
        }
        

}

AudioCapture::~AudioCapture() 
{
    stop();
    Pa_Terminate();
}

bool AudioCapture::start(Callback callback) 
{
    if (stream && Pa_IsStreamStopped(stream) == 1) 
    {
        userCallback = callback;
        Pa_StartStream(stream);
        return true;
    }
    return false;
}

void AudioCapture::stop() 
{
    if (stream && Pa_IsStreamActive(stream) == 1) 
    {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        userCallback = nullptr;
        stream = nullptr;
    }
}

int AudioCapture::paCallback(const void* input, void*,
                             unsigned long frameCount,
                             const PaStreamCallbackTimeInfo*,
                             PaStreamCallbackFlags,
                             void* userData) {
    auto* self = static_cast<AudioCapture*>(userData);
    if (self->userCallback && input) {
        self->userCallback(static_cast<const float*>(input), frameCount * 2);
    }
    return paContinue;
}
