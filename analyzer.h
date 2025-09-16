#pragma once
#include <vector>
#include <fftw3.h>
#include <sndfile.h>
#include <string>

struct Band
{
    float lowFreq;
    float highFreq;
    float level;
};

class Analyzer
{
public:
    Analyzer(int sampleRate, int fftSize);
    ~Analyzer();

    void process(const float* samples, size_t count);
    std::vector<float> getBandLevels();
private:
    int fftSize;
    int sampleRate;
    int N = 512;
    float *spectrum;
    std::vector<float> inputBuffer;
    std::vector<Band> bands = 
    {
        {20, 250, 0},
        {250, 1000, 0},
        {1000, 5000, 0},
        {5000, 20000, 0}
    };
    double *in;
    fftw_complex *out;
    SF_INFO sfinfo;
    fftw_plan plan;
    
    void computeFFT();
};