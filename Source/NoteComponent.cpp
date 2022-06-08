/*
  ==============================================================================

    noteComponent.cpp
    Created: 8 Jun 2022 9:12:00am
    Author:  vdjfelrl11

  ==============================================================================
*/

#include <JuceHeader.h>
#include "noteComponent.h"

//==============================================================================
NoteComponent::NoteComponent(int note, int noteOnTime, int noteOffTime)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    noteNum = note;
    noteOnStamp = noteOnTime;
    noteOffStamp = noteOffTime;

    startComponent.setMouseCursor(juce::MouseCursor::LeftEdgeResizeCursor);
    startComponent.addMouseListener(this, true);
    addAndMakeVisible(startComponent);

    endComponent.setMouseCursor(juce::MouseCursor::RightEdgeResizeCursor);
    endComponent.addMouseListener(this, true);
    addAndMakeVisible(endComponent);

    setSize(100, 50);
}

NoteComponent::~NoteComponent()
{
    this->removeAllChildren();
}


void NoteComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::red);

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("noteComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void NoteComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    // startComponent and EndComponent should have 10 width at least.
    auto r = getBounds();
    auto width = getWidth();

    if (width <= 20)
    {
        startComponent.setBounds(r.removeFromLeft(width / 2));
        endComponent.setBounds(r);
        return;
    }

    auto startComponentBounds = r.removeFromLeft(10);
    auto endComponentBounds = r.removeFromRight(10);

    startComponent.setBounds(startComponentBounds);
    endComponent.setBounds(endComponentBounds);
}
