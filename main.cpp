#include "analyzer.h"
#include "audio_capture.h"
#include <csignal>
#include <iostream>
#include <vector>
#include <atomic>

std::atomic keepRunning(true);

void signalHandler(int) {
    keepRunning = false;
}

int main()
{
    const int sampleRate = 48000;
    const int fftSize = 1024;
    const int channels = 2;
    const size_t buffer_samples = 2048;
    const char * device = "alsa_output.usb-Apple__Inc._USB-C_to_3.5mm_Headphone_Jack_Adapter_DWH3143075FL1MQAE-00.analog-stereo.monitor";
    std::vector<int16_t> audioBuffer;
    std::signal(SIGINT, signalHandler);
    
    AudioCapture audioCapture(device, sampleRate, channels, buffer_samples);
    Analyzer analyzer(sampleRate, fftSize);

    std::cout << "Capturing system audio... press ctrl+C to quit.\n";
    while(keepRunning) 
    {
        if (audioCapture.getAudioBuffer(audioBuffer)) 
        {
            //std::cout<<"AudioBuffer: "<<audioBuffer[0]<<" "<<audioBuffer[1]<<" "<<audioBuffer[2]<<" "<<audioBuffer[3]<<" "<<audioBuffer[4]<<std::endl;
            std::vector<float> floatBuffer(audioBuffer.begin(), audioBuffer.end());
            analyzer.process(floatBuffer.data(), floatBuffer.size());
            auto levels = analyzer.getBandLevels();
            for (auto level : levels) std::cout << level << " ";
            std::cout << std::endl;
        } 
        else 
        {
            std::cerr << "Failed to get audio buffer" << std::endl;
        }
    }
    return 0;
}