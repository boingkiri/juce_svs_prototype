/*
  ==============================================================================

    GridComponent.h
    Created: 27 May 2022 3:43:49pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"

class GridComponent :
    public juce::Component
{

public:
    GridComponent();
    ~GridComponent() override;
    void updateNoteLineRanges(float);
    void paint(juce::Graphics&);

private:
    // The index of this OwnedArray will refer to a **midi note number**
    // In order to get the y values of the note line of note number 60,
    // we can call noteLineRanges[60]
    juce::OwnedArray<juce::Range<float>> noteLineRanges;
};