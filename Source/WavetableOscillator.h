//
//  WavetableOscillator.h
//  TunedReeser
//
//  Created by Dean Cureton on 12/30/24.
//  Copyright © 2024 Dean Cureton. All rights reserved.
//

#pragma once
#include <vector>

class WavetableOscillator
{
public:
    WavetableOscillator(std::vector<float> waveTable, double sampleRate);
    
    void setFrequency(float frequency);
    float getSample();
    
    void updateWavetable(std::vector<float> newWaveTable);
    
    void stop();
    bool isPlaying();
    
private:
    float interpolateLinearly();
    
    std::vector<float> waveTable;
    double sampleRate;
    float index = 0.f;
    float indexIncrement = 0.f;
};
