/*
  ==============================================================================

    KeyboardComponent.cpp
    Created: 27 May 2022 3:43:40pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include "KeyboardComponent.h"

KeyboardComponent::KeyboardComponent(
    juce::MidiKeyboardState& state,
    juce::KeyboardComponentBase::Orientation orientation):
    MidiKeyboardComponent(state, orientation)
{
}

KeyboardComponent::~KeyboardComponent()
{
}
