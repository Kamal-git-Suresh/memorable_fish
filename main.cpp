#include "analyzer.h"
#include "audio_capture.h"
#include "uart.h"
#include <csignal>
#include <iostream>
#include <vector>
#include <atomic>
#include <algorithm>
#include <termios.h>
#include <unistd.h>

std::atomic keepRunning(true);

void signalHandler(int)
{
    keepRunning = false;
}

int main()
{
    bool init = true;
    const int sampleRate = 48000;
    const int fftSize = 2048;
    const int channels = 2;
    const size_t buffer_samples = 128;
    const char *device = "alsa_output.usb-Apple__Inc._USB-C_to_3.5mm_Headphone_Jack_Adapter_DWH3143075FL1MQAE-00.analog-stereo.monitor";//"alsa_output.pci-0000_04_00.6.analog-stereo.monitor";//
    // or alsa_output.pci-0000_04_00.6.analog-stereo.monitor
    std::vector<int16_t> audioBuffer;
    std::signal(SIGINT, signalHandler);

    AudioCapture audioCapture(device, sampleRate, channels, buffer_samples);
    Analyzer analyzer(sampleRate, fftSize);
    UART uart("/dev/ttyACM0", B115200);

    std::cout << "Capturing system audio... press ctrl+C to quit.\n";
    while (keepRunning)
    {
        if (audioCapture.getAudioBuffer(audioBuffer))
        {
            std::vector<float> floatBuffer(audioBuffer.size() / channels);
            for (size_t i = 0; i < floatBuffer.size(); i++)
            {
                int16_t left = audioBuffer[i * 2];
                int16_t right = audioBuffer[i * 2 + 1];
                floatBuffer[i] = ((left + right) / 2.0f) / 32768.0f;
            }
            analyzer.process(floatBuffer.data(), floatBuffer.size());

            auto levels = analyzer.getBandLevels();
            std::vector<int> bandMax(levels.size(), 0);
            for (size_t i = 0; i < levels.size(); i++)
            {
                levels[i] = (levels[i]<10)? 0 :(levels[i] - 10);
            
                std::cout << static_cast<int>(levels[i]) << " ";
            }
            std::cout << std::endl;

            if (init)
            {
                std::cout << "Initialized UART connection on /dev/ttyACM0 at 115200 baud." << std::endl;
                init = false;
                sleep(2);
            }
            uart.sendData(std::vector<uint8_t>(levels.begin(), levels.end()));
        }
        else
            std::cerr << "Failed to get audio buffer" << std::endl;
    }
    return 0;
}