#include "CombFilter.h"

CombFilter::CombFilter(int size, float RT60, int sampleRate)
    : buffer(size, 0.0f), bufferIndex(0), delaySize(size) {
    float delayInSeconds = static_cast<float>(size) / sampleRate;
    feedback = calculateFeedbackGain(delayInSeconds, RT60);
}

float CombFilter::calculateFeedbackGain(float delayInSeconds, float RT60) {
    return std::pow(10.0f, (-3.0f * delayInSeconds / RT60));
}

float CombFilter::process(float input) {
    // Get the delayed output
    float output = buffer[bufferIndex];

    // Add the delayed output to the input and store it in the buffer
    buffer[bufferIndex] = input + (output * feedback);                      

    // Increment buffer index and wrap around
    bufferIndex = (bufferIndex + 1) % delaySize;

    return output;
}