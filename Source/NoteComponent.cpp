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
NoteComponent::NoteComponent(GridComponent* gridComponent, int note, int noteOnTime, int noteOffTime)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    parentGrid = gridComponent;
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

int NoteComponent::convertStamptoPosition(int stamp)
{
    int gridWidth = parentGrid->getWidth();
    int gridX = parentGrid->getX();
    int lastStamp = parentGrid->getLastTimestamp();
    return (int)((float)stamp / (float)lastStamp * (float) gridWidth) + gridX;
}

int NoteComponent::convertPositiontoStamp(int position)
{
    int gridWidth = parentGrid->getWidth();
    int gridX = parentGrid->getX();
    int lastStamp = parentGrid->getLastTimestamp();
    return (int)((position - gridX) / gridWidth) * lastStamp;
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

void NoteComponent::mouseDrag(const juce::MouseEvent& event)
{
    auto rec = juce::Rectangle<int>();

    // Modify length of note by start position
    if (event.eventComponent == &startComponent) 
    {
        auto originalEndPos = this->getX() + this->getWidth();
        auto modifiedStartPos = event.mouseDownPosition.getX();
        rec = juce::Rectangle<int>(modifiedStartPos, this->getY(), originalEndPos, this->getHeight());
    }
    // Modify length of note by end position
    else if (event.eventComponent == &endComponent)
    {
        auto originalStartPos = this->getX();
        auto modifiedEndPos = event.mouseDownPosition.getX();
        rec = juce::Rectangle<int>(originalStartPos, this->getY(), modifiedEndPos, this->getHeight());
    }
    // Modify note itself
    else if (event.eventComponent == this)
    {
        auto modifiedNote = event.mouseDownPosition.getY();
        juce::Range<float> range = parentGrid->getRangeBy(noteNum);

        if (!range.contains(modifiedNote))
        {
            noteNum = parentGrid->getNoteBy(modifiedNote);
            juce::Range<float> range = parentGrid->getRangeBy(noteNum);
            rec = juce::Rectangle<int>
                (
                    this->getX(),
                    range.getStart(),
                    this->getWidth(),
                    range.getLength()
                );
        }
        else {
            rec = getBounds();
        }
    }
    jassert(rec != juce::Rectangle<int>());
    
    setStampBy(rec);
    this->setBounds(rec);
}

void NoteComponent::setStampBy(juce::Rectangle<int> rec)
{
    auto startPos = rec.getX();
    auto endPos = rec.getX() + rec.getWidth();

    noteOnStamp = convertPositiontoStamp(startPos);
    noteOffStamp = convertPositiontoStamp(endPos);
}

void NoteComponent::updateBounds()
{
    juce::Range<float> heightRange = parentGrid->getRangeBy(noteNum);
    this->setBounds(this->getX(), heightRange.getStart(), this->getWidth(), heightRange.getLength());

    repaint();
}