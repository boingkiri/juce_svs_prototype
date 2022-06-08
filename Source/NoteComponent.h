/*
  ==============================================================================

    noteComponent.h
    Created: 8 Jun 2022 9:11:59am
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "GridComponent.h"

//==============================================================================
/*
*/
class NoteComponent  : 
    public juce::Component,
    public juce::MouseListener
{
public:
    NoteComponent(GridComponent* gridComponent, int note, int noteOnTime, int noteOffTime);
    ~NoteComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int convertStamptoPosition(int stamp);
    int convertPositiontoStamp(int position);

    void updateBounds();

private:
    juce::Component startComponent;
    juce::Component endComponent;

    GridComponent* parentGrid;

    int noteNum;
    int noteOnStamp;
    int noteOffStamp;

    void setStampBy(juce::Rectangle<int>);
    void mouseDrag(const juce::MouseEvent& event) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteComponent)
};
