#include "analyzer.h"
#include "audio_capture.h"
#include <iostream>

int main()
{
    const int sampleRate = 48000;
    const int fftSize = 1024;
    
    Analyzer analyzer(sampleRate, fftSize);
    AudioCapture audioCapture(18 , sampleRate, 512);
    int numDevices = Pa_GetDeviceCount();
    audioCapture.start([&](const float* samples, size_t count){
        std::cout<<"Received " << count << " samples\n";
        //std::cout<<"Sampels: "<<samples[0]<<" "<<samples[1]<<" "<<samples[2]<<" "<<samples[3]<<" "<<samples[4]<<std::endl;
        analyzer.process(samples, count);
        auto levels = analyzer.getBandLevels();
        for (auto level : levels) std::cout << level << " ";
            std::cout << std::endl;
        });
    std::cout << "Capturing system audio... press Enter to quit.\n";
    std::cin.get();

    audioCapture.stop();
    return 0;
}