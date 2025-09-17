#include "analyzer.h"
#include <iostream>
#include <math.h>

Analyzer::Analyzer(int sampleRate, int fftSize) 
    : sampleRate(sampleRate), fftSize(fftSize)
{
    N = fftSize;
    spectrum = new float[N/2];
    in = (double*)fftw_malloc(sizeof(double) * N);

    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (N/2 + 1));    
    plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
    
    inputBuffer.reserve(N);
}

Analyzer::~Analyzer() 
{
    delete[] spectrum;
    fftw_destroy_plan(plan);
    fftw_free(in); 
    fftw_free(out);
}

void Analyzer::process(const float* samples, size_t count) 
{
    inputBuffer.insert(inputBuffer.end(), samples, samples + count);
    //std::cout<<"Input buffer size: "<<inputBuffer.size()<<std::endl;
    //std::cout<<"Input Buffer contents:" << inputBuffer[0]<<" "<<inputBuffer[1]<<" "<<inputBuffer[2]<<" "<<inputBuffer[3]<<" "<<inputBuffer[4]<<std::endl;
    while (inputBuffer.size() >= N) 
    {
        computeFFT();
        inputBuffer.erase(inputBuffer.begin(), inputBuffer.begin() + N);
        //inputBuffer.clear();
    }
}

void Analyzer::computeFFT() 
{
    std::copy(inputBuffer.begin(), inputBuffer.begin() + N, reinterpret_cast<float*>(in));
    for (int i = 0; i < N; i++) 
    {
        in[i] = inputBuffer[i];
    }
    //std::cout<<"Input to FFT: "<<in[0]<<" "<<in[1]<<" "<<in[2]<<" "<<in[3]<<" "<<in[4]<<std::endl;
    fftw_execute(plan);
    for (int k = 0; k < N/2; k++) 
    {
        //std::cout <<"out: "<< out[k][0]<<std::endl;
        //std::cout <<"out: "<< out[k][0] <<" " <<out[k][1] << std::endl;
        float mag = sqrt(out[k][0]*out[k][0] + out[k][1]*out[k][1]);    
        spectrum[k] = mag;
    }
}

std::vector<float> Analyzer::getBandLevels()
{
    std::vector<float> magnitudes;
    magnitudes.reserve(bands.size());

    float freqPerBin = static_cast<float>(sampleRate) / static_cast<float>(N);
    
    for (auto& band: bands)
    {
        band.level = 0; 
        int lowBin = static_cast<int>(floor(band.lowFreq / freqPerBin));
        int highBin = static_cast<int>(ceil(band.highFreq / freqPerBin));
        lowBin = std::max(lowBin, 0);
        highBin = std::min(N/2, highBin);
        for (int k = lowBin; k <= highBin && k < N/2; k++)
        {
            band.level += spectrum[k];
        }
        magnitudes.push_back(band.level / (highBin - lowBin + 1));
    }
    return magnitudes;
}