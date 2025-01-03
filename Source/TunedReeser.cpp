//
//  WavetableSynth.cpp
//  TunedReeser
//
//  Created by Dean Cureton on 12/30/24.
//  Copyright © 2024 Dean Cureton. All rights reserved.
//

#include "TunedReeser.h"

std::vector<float> TunedReeser::generateWaveTable(int waveType)
{
    constexpr auto WAVETABLE_LENGTH = 512;
    
    std::vector<float> waveTable(WAVETABLE_LENGTH);
    
    const auto TWO_PI = juce::MathConstants<float>::twoPi;
    
    for (auto i = 0; i < WAVETABLE_LENGTH; ++i)
    {
        switch(waveType) {
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

void TunedReeser::initializeOscillators()
{
    const auto waveTable = generateWaveTable(waveform);
    
    oscillators.clear();
    oscillators.emplace_back(waveTable, sampleRate);
    oscillators.emplace_back(waveTable, sampleRate);
}

void TunedReeser::updateOscillators()
{
    const auto waveTable = generateWaveTable(waveform);
    
    oscillators[0].updateWavetable(waveTable);
    oscillators[1].updateWavetable(waveTable);
}

void TunedReeser::updateFrequencies()
{
    const auto noteWithBend = static_cast<float>(midiNote) + pitchBend;
    const auto frequency = midiNoteNumberToFrequency(noteWithBend);
    const auto offsetFrequency = detuneAmountToFrequency(noteWithBend, detuneAmount);
    oscillators[0].setFrequency(frequency + offsetFrequency);
    oscillators[1].setFrequency(frequency - offsetFrequency);
}

void TunedReeser::prepareToPlay(double sampleRate)
{
    this->sampleRate = sampleRate;
    initializeOscillators();
}

void TunedReeser::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, float detuneAmount, int waveform, float gain)
{
    this->detuneAmount = detuneAmount;
    this->waveform = waveform;
    this->gain = gain;
    
    if (waveform != previousWaveform)
    {
        updateOscillators();
    }
    previousWaveform = waveform;
    
    if (detuneAmount != previousDetuneAmount && oscillators[0].isPlaying())
    {
        updateFrequencies();
    }
    previousDetuneAmount = detuneAmount;
    
    auto currentSample = 0;

    for (const auto midiMessage : midiMessages)
    {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        
        render(buffer, currentSample, midiEventSample);
        handleMidiEvent(midiEvent);
        
        currentSample = midiEventSample;
    }
    
    render(buffer, currentSample, buffer.getNumSamples());
}

void TunedReeser::render(juce::AudioBuffer<float> &buffer, int startSample, int endSample)
{
    auto* firstChannel = buffer.getWritePointer(0);
    
    if (oscillators[0].isPlaying())
    {
        for (auto sample = startSample; sample < endSample; ++sample)
        {
            // both oscillators are either both playing or both not playing by design
            firstChannel[sample] += oscillators[0].getSample() * gain / 2;
            firstChannel[sample] += oscillators[1].getSample() * gain / 2;
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
        midiNote = midiEvent.getNoteNumber();
        updateFrequencies();
    }
    else if ((midiEvent.isNoteOff() || midiEvent.isAllNotesOff()) && midiEvent.getNoteNumber() == midiNote)
    {
        oscillators[0].stop();
        oscillators[1].stop();
    }
    else if (midiEvent.isPitchWheel())
    {
        constexpr auto PITCHBEND_RANGE = 4; // +- 2 semitones
        constexpr auto PITCHWHEEL_DEFAULT = 8192;
        constexpr auto PITCHWHEEL_RANGE = 8192.f; // real range, 16384, over 2 to account for positive/negative
        pitchBend = PITCHBEND_RANGE * static_cast<float>(midiEvent.getPitchWheelValue() - PITCHWHEEL_DEFAULT) / PITCHWHEEL_RANGE;
        if (oscillators[0].isPlaying()) {
            updateFrequencies();
        }
    }
}

float TunedReeser::midiNoteNumberToFrequency(float midiNoteNumber)
{
    constexpr auto A4_FREQUENCY = 440.f;
    constexpr auto A4_NOTE_NUMBER = 69.f;
    constexpr auto SEMITONES_IN_AN_OCTAVE = 12.f;
    return A4_FREQUENCY * std::powf(2.f, (midiNoteNumber - A4_NOTE_NUMBER) / SEMITONES_IN_AN_OCTAVE);
}

float TunedReeser::detuneAmountToFrequency(float midiNoteNumber, float detuneAmount)
{
    return midiNoteNumberToFrequency(midiNoteNumber + detuneAmount / 100.f) - midiNoteNumberToFrequency(midiNoteNumber); // frequency difference between detuning up detuneAmount cents and not detuning at all
}
