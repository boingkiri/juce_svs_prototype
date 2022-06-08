/*
  ==============================================================================

    noteComponent.h
    Created: 8 Jun 2022 9:11:59am
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>

//==============================================================================
/*
*/
class NoteComponent  : 
    public juce::Component,
    public juce::MouseListener
{
public:
    NoteComponent(int note, int noteOnTime, int noteOffTime);
    ~NoteComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;


private:
    juce::Component startComponent;
    juce::Component endComponent;

    int noteNum;
    int noteOnStamp;
    int noteOffStamp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteComponent)
};
