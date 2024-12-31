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
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);
    
private:
    void initializeOscillators();
    std::vector<float> generateSineWaveTable();
    std::vector<float> generateSawWaveTable();
    std::vector<float> generateSquareWaveTable();
    void handleMidiEvent(const juce::MidiMessage& midiEvent);
    float midiNoteNumberToFrequency(int midiNoteNumber);
    void render(juce::AudioBuffer<float>& buffer, int startSample, int endSample);
    
    double sampleRate;
    std::vector<WavetableOscillator> oscillators;
};
