//
//  WavetableSynth.cpp
//  TunedReeser
//
//  Created by Dean Cureton on 12/30/24.
//  Copyright © 2024 Dean Cureton. All rights reserved.
//

#include "TunedReeser.h"

std::vector<float> TunedReeser::generateWaveTable(int waveform)
{
    constexpr auto WAVETABLE_LENGTH = 512;
    
    std::vector<float> waveTable(WAVETABLE_LENGTH);
    
    const auto TWO_PI = juce::MathConstants<float>::twoPi;
    
    for (auto i = 0; i < WAVETABLE_LENGTH; ++i)
    {
        switch(waveform) {
            case 1:
                waveTable[i] = 1 - 2 * static_cast<float>(i) / static_cast<float>(WAVETABLE_LENGTH - 1);
                break;
            case 2:
                waveTable[i] = (i >= WAVETABLE_LENGTH / 2) ? 1 : -1;
                break;
            case 0:
            default:
                waveTable[i] = std::sinf(TWO_PI * static_cast<float>(i) / static_cast<float>(WAVETABLE_LENGTH));
                break;
        }
    }
    
    return waveTable;
}

void TunedReeser::initializeOscillators(int waveform)
{
    constexpr auto OSCILLATORS_COUNT = 128;
    
    const auto waveTable = generateWaveTable(waveform);
    
    oscillators.clear();
    for (auto i = 0; i < OSCILLATORS_COUNT; ++i)
    {
        oscillators.emplace_back(waveTable, sampleRate);
    }
}

void TunedReeser::updateOscillators(int waveform)
{
    const auto waveTable = generateWaveTable(waveform);
    
    for (auto& oscillator: oscillators)
    {
        oscillator.updateWavetable(waveTable);
    }
}

void TunedReeser::prepareToPlay(double sampleRate)
{
    this->sampleRate = sampleRate;
    
    initializeOscillators(0);
}
void TunedReeser::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, float detuneAmount, float gain, int waveform)
{
    if (waveform != previousWaveform)
    {
        updateOscillators(waveform);
    }
    previousWaveform = waveform;
    
    if (detuneAmount != previousDetuneAmount)
    {
        // update detune
    }
    previousDetuneAmount = detuneAmount;
    
    auto currentSample = 0;
    
    for (const auto midiMessage : midiMessages)
    {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        
        render(buffer, currentSample, midiEventSample, gain);
        handleMidiEvent(midiEvent);
        
        currentSample = midiEventSample;
    }
    
    render(buffer, currentSample, buffer.getNumSamples(), gain);
}

void TunedReeser::render(juce::AudioBuffer<float> &buffer, int startSample, int endSample, float gainMultiplier)
{
    auto* firstChannel = buffer.getWritePointer(0);
    
    for (auto& oscillator: oscillators)
    {
        if (oscillator.isPlaying())
        {
            for (auto sample = startSample; sample < endSample; ++sample)
            {
                firstChannel[sample] += oscillator.getSample() * gainMultiplier;
            }
        }
    }
    
    for (auto channel = 1; channel < buffer.getNumChannels(); ++channel)
    {
        std::copy(firstChannel + startSample, firstChannel + endSample, buffer.getWritePointer(channel) + startSample);
    }
}

void TunedReeser::handleMidiEvent(const juce::MidiMessage &midiEvent)
{
    if (midiEvent.isNoteOn())
    {
        const auto oscillatorId = midiEvent.getNoteNumber();
        const auto frequency = midiNoteNumberToFrequency(oscillatorId);
        oscillators[oscillatorId].setFrequency(frequency);
    }
    else if (midiEvent.isNoteOff())
    {
        const auto oscillatorId = midiEvent.getNoteNumber();
        oscillators[oscillatorId].stop();
    }
    else if (midiEvent.isAllNotesOff())
    {
        for (auto& oscillator : oscillators)
        {
            oscillator.stop();
        }
    }
}

float TunedReeser::midiNoteNumberToFrequency(int midiNoteNumber)
{
    constexpr auto A4_FREQUENCY = 440.f;
    constexpr auto A4_NOTE_NUMBER = 69.f;
    constexpr auto SEMITONES_IN_AN_OCTAVE = 12.f;
    return A4_FREQUENCY * std::powf(2.f, (midiNoteNumber - A4_NOTE_NUMBER) / SEMITONES_IN_AN_OCTAVE);
}
