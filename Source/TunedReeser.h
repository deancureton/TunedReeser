//
//  WavetableSynth.h
//  TunedReeser
//
//  Created by Dean Cureton on 12/30/24.
//  Copyright © 2024 Dean Cureton. All rights reserved.
//

#pragma once
#include "JuceHeader.h"
#include "WavetableOscillator.h"

class TunedReeser
{
public:
    void prepareToPlay(double sampleRate);
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&, float detuneAmount, float gain, int waveform);
    
private:
    void initializeOscillators(int waveform);
    void updateOscillators(int waveform);
    std::vector<float> generateWaveTable(int waveform);
    void handleMidiEvent(const juce::MidiMessage& midiEvent);
    float midiNoteNumberToFrequency(int midiNoteNumber);
    void render(juce::AudioBuffer<float>& buffer, int startSample, int endSample, float gainMultiplier);
    
    double sampleRate;
    std::vector<WavetableOscillator> oscillators;
    int previousWaveform = 0;
    float previousDetuneAmount = 0;
};
