#include "audio_capture.h"
#include <iostream>

AudioCapture::AudioCapture(const char* device, int sampleRate, int channels, size_t buffer_samples) 
    : device(device), BUF_SAMPLES(buffer_samples)
{
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = sampleRate;
    ss.channels = channels;
    stream = pa_simple_new(nullptr,               
                           "AudioVisualizer",     
                           PA_STREAM_RECORD,
                           device,               
                           "Record",             
                           &ss,                  
                           nullptr,              
                           nullptr,              
                           &error);     

    if (!stream) {
        std::cerr << "Failed to open audio device: " << pa_strerror(error) << std::endl;
    }
} 

AudioCapture::~AudioCapture() 
{
    if (stream)
        pa_simple_free(stream);
}

bool AudioCapture::getAudioBuffer(std::vector<int16_t>& outBuffer)
{
    if (!stream) return false;
    outBuffer.resize(BUF_SAMPLES * 2);
    if(pa_simple_read(stream, outBuffer.data(), outBuffer.size() * sizeof(int16_t), &error) < 0){
        std::cerr << "read failed" << pa_strerror(error) << std::endl;
        return false;
    }
    
    return true;
}
