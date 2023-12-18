#pragma once


#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

class CombFilter 
{
private:
    std::vector<float> buffer;
    int bufferIndex;
    float feedback;
    int delaySize;

    float calculateFeedbackGain(float delayInSeconds, float RT60);

public:
    CombFilter(int size, float RT60, int sampleRate);
    float process(float input);

};

