/*
  ==============================================================================

    PianoRollComponent.h
    Created: 27 May 2022 3:53:28pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ToolBarComponent.h"
#include "KeyboardComponent.h"
#include "GridComponent.h"

class PianoRollComponent : 
    public juce::Component
{
public:
    PianoRollComponent();
    ~PianoRollComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override;

private:
    std::unique_ptr<KeyboardComponent> keyboardComponent;
    std::unique_ptr<GridComponent> gridComponent;
    std::unique_ptr<ToolBarComponent> toolBarComponent;

    std::unique_ptr<juce::MidiKeyboardState> keyboardState;

    juce::TextButton button;
};