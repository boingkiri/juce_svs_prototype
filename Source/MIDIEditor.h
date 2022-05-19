/*
  ==============================================================================

    MIDIEditor.h
    Created: 19 May 2022 1:33:19pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class MIDIEditor :
    public juce::Component,
    private juce::Slider::Listener
{
public:
    MIDIEditor();
    ~MIDIEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    float noteOnVel;

private:
    void sliderValueChanged(juce::Slider* slider) override;

    juce::Slider midiVolume;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIEditor)
};
