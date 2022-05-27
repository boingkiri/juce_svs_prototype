/*
  ==============================================================================

    KeyboardComponent.h
    Created: 27 May 2022 3:43:40pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class KeyboardComponent :
    public juce::MidiKeyboardComponent
    //public juce::Component
{
public:
    KeyboardComponent(
        juce::MidiKeyboardState&,
        juce::KeyboardComponentBase::Orientation
    );
    ~KeyboardComponent() override;
    //juce::Range<float> getKeyPosition(int midiNoteNumber, float targetKeyWidth);
    //juce::Range<float> getRectangleForKey(int midiNoteNumber, float targetKeyWidth);

//private:

};