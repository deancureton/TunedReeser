/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

//==============================================================================
TunedReeserAudioProcessor::TunedReeserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (detuneAmount = new juce::AudioParameterFloat ("detuneAmount",
                                                                "Detune Amount",
                                                                juce::NormalisableRange<float> (0.0f, 100.0f),
                                                                0.0f));
    addParameter (waveform = new juce::AudioParameterInt ("waveform", // parameterID
                                                          "Waveform", // parameter name
                                                          0,
                                                          2,
                                                          0));
    addParameter (gain = new juce::AudioParameterFloat ("gain",
                                                        "Gain",
                                                        juce::NormalisableRange<float> (0.0f, 1.0f),
                                                        0.5f));
}

TunedReeserAudioProcessor::~TunedReeserAudioProcessor()
{
}

//==============================================================================
const juce::String TunedReeserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TunedReeserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TunedReeserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TunedReeserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TunedReeserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TunedReeserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TunedReeserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TunedReeserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TunedReeserAudioProcessor::getProgramName (int index)
{
    return {};
}

void TunedReeserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TunedReeserAudioProcessor::prepareToPlay (double sampleRate, int)
{
    synth.prepareToPlay(sampleRate);
}

void TunedReeserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TunedReeserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TunedReeserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    buffer.clear();
    
    synth.processBlock(buffer, midiMessages, *detuneAmount, *waveform, *gain);
}

//==============================================================================
bool TunedReeserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TunedReeserAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void TunedReeserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("TunedReeser"));
    
    xml->setAttribute ("detuneAmount", (double) *detuneAmount);
    xml->setAttribute ("waveform", (int) *waveform);
    xml->setAttribute ("gain", (double) *gain);
    
    copyXmlToBinary (*xml, destData);
}

void TunedReeserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName ("TunedReeser")) {
            *detuneAmount = (float) xmlState->getDoubleAttribute ("detuneAmount", 0.0);
            *waveform = (int) xmlState->getIntAttribute("waveform", 0);
            *gain = (float) xmlState->getDoubleAttribute ("gain", 0.5);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TunedReeserAudioProcessor();
}
