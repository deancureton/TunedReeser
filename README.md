# TunedReeser
source code + VST3 plugin accompanying a post on my website about reese basses and the AM-GM inequality (https://deancureton.com/post/reeses).

the VST3 plugin features a frequency-oriented detuning system to create in-tune reese basses (as opposed to pitch-oriented out-of-tune basses; explained in the post above).

the plugin has three parameters:
- the detune amount (0-200), measured in cents upwards to detune the upper voice. this number is converted to a frequency offset and applied downwards to the bottom voice as well. higher values = more wobble.
- the waveform (0-2). 0 = sine, 1 = saw, 2 = square.
- gain (0-1). measured in amplitude, not decibels for simplicity. the value is divided by half and applied equally to both oscillators, meaning a value of 1 will peak at 0db.

the sound produced is a two-oscillator reese bass of the chosen waveform.

(note: aliasing is not accounted for, since notes are meant to be played at low frequencies so that any artifacts should be virtually inaudible)

references:
- https://www.youtube.com/@WolfSoundAudio
- https://thewolfsound.com/sound-synthesis/wavetable-synth-plugin-in-juce/
- https://github.com/JanWilczek/wavetable-synth
    - huge thanks to WolfSound for his wonderful wavetable synth JUCE tutorial; I used it as starter code for the project.
- https://juce.com/
- https://github.com/juce-framework/JUCE
