#include "analyzer.h"
#include "audio_capture.h"
#include <csignal>
#include <iostream>
#include <vector>
#include <atomic>
#include <algorithm>

std::atomic keepRunning(true);

void signalHandler(int)
{
    keepRunning = false;
}

int main()
{
    const int sampleRate = 48000;
    const int fftSize = 1024;
    const int channels = 2;
    const size_t buffer_samples = 128;
    const char *device = "alsa_output.usb-Apple__Inc._USB-C_to_3.5mm_Headphone_Jack_Adapter_DWH3143075FL1MQAE-00.analog-stereo.monitor";
    std::vector<int16_t> audioBuffer;
    std::signal(SIGINT, signalHandler);

    AudioCapture audioCapture(device, sampleRate, channels, buffer_samples);
    Analyzer analyzer(sampleRate, fftSize);

    std::cout << "Capturing system audio... press ctrl+C to quit.\n";
    while (keepRunning)
    {
        if (audioCapture.getAudioBuffer(audioBuffer))
        {
            std::vector<float> floatBuffer(audioBuffer.begin(), audioBuffer.end());

            analyzer.process(floatBuffer.data(), floatBuffer.size());

            auto levels = analyzer.getBandLevels();
            std::vector<int> bandMax(levels.size(), 0);

            int minLevel = *std::min_element(levels.begin(), levels.end());
            int maxLevel = *std::max_element(levels.begin(), levels.end());
            int range = maxLevel - minLevel;
            //std::cout << "Levels: " << minLevel << " to " << maxLevel << " | ";
            if (range == 0)  range = 1; 
            for (size_t i = 0; i < levels.size(); i++)
            {
                bandMax[i] = std::max(bandMax[i], static_cast<int>(levels[i]));
                float norm = (float)(levels[i] / (bandMax[i] +5));
                int eqLevel = static_cast<int>( norm * 100 );
                std::cout << eqLevel << " ";
            }
            std::cout << std::endl;
        }
        else
        {
            std::cerr << "Failed to get audio buffer" << std::endl;
        }
    }
    return 0;
}