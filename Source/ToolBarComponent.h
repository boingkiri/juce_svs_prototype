/*
  ==============================================================================

    ToolBarComponent.h
    Created: 27 May 2022 3:49:40pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ToolBarComponent :
    public juce::Component
{
public:
    ToolBarComponent();
    ~ToolBarComponent() override;

    void resized() override;
private:
    std::unique_ptr<juce::TextButton> midiSave;
};