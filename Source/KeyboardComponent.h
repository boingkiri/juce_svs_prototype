/*
  ==============================================================================

    KeyboardComponent.h
    Created: 27 May 2022 3:43:40pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"

class KeyboardComponent :
    public juce::MidiKeyboardComponent
{
public:
    KeyboardComponent(
        juce::MidiKeyboardState&,
        juce::KeyboardComponentBase::Orientation
    );
    ~KeyboardComponent() override;

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override;

    float keyWidth = constants::noteLineWidth;
    int scrollButtonWidth = 0;
    //juce::Range<float> getKeyPosition(int midiNoteNumber, float targetKeyWidth);
    //juce::Range<float> getRectangleForKey(int midiNoteNumber, float targetKeyWidth);
    juce::Range<float> KeyboardComponent::getKeyPosition(int midiNoteNumber, float targetKeyWidth) const;
    juce::Rectangle<float> KeyboardComponent::getRectangleForKey(int note) const;
    float getKeyWidthModifier(int midiNoteNumber) const;

};