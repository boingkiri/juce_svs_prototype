/*
  ==============================================================================

    GridComponent.cpp
    Created: 27 May 2022 3:43:49pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include "GridComponent.h"
GridComponent::GridComponent()
{
    for (int i = 0; i <= 127; ++i)
        noteLineRanges.add(new juce::Range<float>(0, constants::noteLineWidth));

}

GridComponent::~GridComponent()
{

}

void GridComponent::updateNoteLineRanges(float firstKeyStartPosition)
{
    

    for (int i = 0; i <= 127; ++i)
    {   
        noteLineRanges[i]->setStart(getHeight() - firstKeyStartPosition - (i + 1) * constants::noteLineWidth);
        noteLineRanges[i]->setLength(constants::noteLineWidth);
    }
    repaint();
}

void GridComponent::paint(juce::Graphics& g) 
{
    // Draw the background
    g.fillAll(juce::Colours::lightgrey);

    // Draw note lines
    g.setColour(juce::Colours::grey);
    for (int i = 0; i < noteLineRanges.size(); ++i)
    {
        auto rect = juce::Rectangle<float>(
            0, 
            noteLineRanges[i]->getStart(), 
            static_cast<float>(getWidth()), 
            noteLineRanges[i]->getLength());
        if (rect.getBottom() < 0 || rect.getY() >= getHeight()) // Skip if the line is out of bounds
            continue;
        g.drawRect(rect, 0.5f);
        if (!juce::MidiMessage::isMidiNoteBlack(i))
            continue;
        g.fillRect(rect);
    }
}