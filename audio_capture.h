#pragma once
#include <pulse/simple.h>
#include <pulse/error.h>
#include <portaudio.h>
#include <functional>

class AudioCapture
{

public:
    using Callback = std::function<void(const float *samples, unsigned long count)>;

    AudioCapture(const char* device, int sampleRate, int channels, size_t BUF_SAMPLES);
    ~AudioCapture();

    bool getAudioBuffer(std::vector<int16_t>& outBuffer);

private:
    pa_sample_spec ss;
    const char * device;
    int error;
    pa_simple *stream = nullptr;
    const size_t BUF_SAMPLES;
};

