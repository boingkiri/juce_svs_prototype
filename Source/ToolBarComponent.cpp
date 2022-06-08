/*
  ==============================================================================

    ToolBarComponent.cpp
    Created: 27 May 2022 3:49:40pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include "ToolBarComponent.h"


ToolBarComponent::ToolBarComponent()
{
    midiSave = std::make_unique<juce::TextButton>();

    midiSave->setButtonText("Save MIDI");
    addAndMakeVisible(*midiSave);

    setSize(300, 200);
}


ToolBarComponent::~ToolBarComponent()
{
    this->removeAllChildren();
}

void ToolBarComponent::resized()
{
    auto bounds = getBounds();
    bounds.removeFromTop(10).removeFromBottom(10).removeFromLeft(100).removeFromRight(100);
    bounds.setY(bounds.getY() - 75);

    midiSave->setBounds(bounds);
}