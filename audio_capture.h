#pragma once

#include <portaudio.h>
#include <functional>

class AudioCapture
{

public:
    using Callback = std::function<void(const float *samples, unsigned long count)>;

    AudioCapture(int deviceIndex, int sampleRate, int framesPerBuffer);
    ~AudioCapture();

    bool start(Callback callback);
    void stop();

private:
    PaStream *stream = nullptr;
    Callback userCallback;
    int sampleRate;
    int framesPerBuffer;
    static int paCallback(const void* input, void* output,
                          unsigned long frameCount,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData);
};

