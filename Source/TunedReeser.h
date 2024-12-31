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
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&, float detuneAmount, int waveform, float gain);
    
private:
    void initializeOscillators();
    void updateOscillators();
    void updateFrequencies();
    std::vector<float> generateWaveTable(int waveType);
    void handleMidiEvent(const juce::MidiMessage& midiEvent);
    
    float midiNoteNumberToFrequency(float midiNoteNumber);
    float detuneAmountToFrequency(float midiNoteNumber, float detuneAmount);
    
    void render(juce::AudioBuffer<float>& buffer, int startSample, int endSample);
    
    double sampleRate;
    
    std::vector<WavetableOscillator> oscillators;
    
    int waveform = 0;
    float detuneAmount = 0.f;
    float gain = 0.7f;
    int midiNote = 69;
    float pitchBend = 0;
    
    int previousWaveform = 0;
    float previousDetuneAmount = 0;
};
