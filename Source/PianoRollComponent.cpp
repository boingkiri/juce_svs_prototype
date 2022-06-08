/*
  ==============================================================================

    PianoRollComponent.cpp
    Created: 27 May 2022 3:53:28pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include "PianoRollComponent.h"

PianoRollComponent::PianoRollComponent()
{

    gridComponent = std::make_unique<GridComponent>();
    toolBarComponent = std::make_unique<ToolBarComponent>();
    
    addAndMakeVisible(*gridComponent);
    addAndMakeVisible(*toolBarComponent);

    keyboardState = std::make_unique<juce::MidiKeyboardState>();
    keyboardComponent = std::make_unique<KeyboardComponent>(
        *keyboardState,
        juce::MidiKeyboardComponent::Orientation::verticalKeyboardFacingRight
    );
    keyboardComponent->setBlackNoteWidthProportion(1);
    addAndMakeVisible(*keyboardComponent);

    //addAndMakeVisible(button);
    //button.setButtonText(juce::String("AAAAA"));
    setSize(150, 100);
}

PianoRollComponent::~PianoRollComponent()
{
    this->removeAllChildren();
}

void PianoRollComponent::paint(juce::Graphics& g) 
{
}

void PianoRollComponent::resized() 
{
    auto r = getBounds();
    toolBarComponent->setBounds(r.removeFromTop(60));
    keyboardComponent->setBounds(r.removeFromLeft(100));
    gridComponent->setBounds(r);
    gridComponent->updateNoteLineRanges(keyboardComponent->getKeyStartPosition(0));

}

void PianoRollComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{   

    if (toolBarComponent->isMouseOver()) {
        return;
    }
    auto amount = (keyboardComponent->getOrientation() == KeyboardComponent::horizontalKeyboard && wheel.deltaX != 0)
        ? wheel.deltaX : (keyboardComponent->getOrientation() == KeyboardComponent::verticalKeyboardFacingLeft ? wheel.deltaY
            : -wheel.deltaY);


    keyboardComponent->setLowestVisibleKey(keyboardComponent->getLowestVisibleKey() - amount * keyboardComponent->getKeyWidth());
    gridComponent->updateNoteLineRanges(keyboardComponent->getKeyStartPosition(0));
}

void PianoRollComponent::setMIDIFile(juce::MidiFile& midiFile)
{
    gridComponent->setMidiFile(midiFile);
}