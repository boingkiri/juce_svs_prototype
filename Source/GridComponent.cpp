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
    this->setTitle("GridComponent");
    for (int i = 0; i <= 127; ++i)
        noteLineRanges.add(new juce::Range<float>(0, constants::noteLineWidth));

}

GridComponent::~GridComponent()
{
    this->removeAllChildren();

}

void GridComponent::updateNoteLineRanges(float firstKeyStartPosition)
{
    for (int i = 0; i <= 127; ++i)
    {   
        noteLineRanges[i]->setStart(getHeight() - firstKeyStartPosition - (i + 1) * constants::noteLineWidth);
        noteLineRanges[i]->setLength(constants::noteLineWidth);
    }
    
    for (int i = 0; i <= noteVector.size(); i++)
    {
        updateBounds(i);
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

void GridComponent::setMidiFile(juce::MidiFile external_midifile)
{
    midiFile = external_midifile;
    showMidiMessageSequence();
}

void GridComponent::setNoteComponentArray(juce::MidiMessage noteOnmsg, juce::MidiMessage noteOffmsg)
{
    //jassert(noteOnmsg.getNoteNumber() == noteOffmsg.getNoteNumber());


    //int note = noteOnmsg.getNoteNumber();
    //double startTime= noteOnmsg.getTimeStamp();
    //double length = noteOffmsg.getTimeStamp() - noteOnmsg.getTimeStamp();
    //double totalLength = midiFile.getLastTimestamp();
    //
    //float width = constants::noteLineWidth;
    //int rectStartX = (int)(startTime / totalLength) * getWidth();
    //int rectStartY = (int)noteLineRanges[note]->getStart();
    //int rectWidth = (int)(startTime / totalLength) * getWidth();
    //int rectHeight = (int)constants::noteLineWidth;

    //std::unique_ptr<Component> noteComponent =
    //    std::make_unique<juce::Component>(noteVector.size());

    //noteVector.push_back(noteComponent);

    //juce::Rectangle<int> noteBounds =
    //    juce::Rectangle<int>(rectStartX, rectStartY, rectWidth, rectHeight);
    ////noteComponent-> Set colour
    //noteComponent->setBounds(noteBounds);

}

void GridComponent::showMidiMessageSequence()
{
    if (midiFile.getNumTracks() == 0)
        DBG("Midi file is not set yet");

    const juce::MidiMessageSequence* midiseq = midiFile.getTrack(0);
    int numOfNote = midiseq->getNumEvents();

    //Count NoteOn event
    //TODO: Need to study MIDI format
    int count = 0;
    for (int i = 0; i < numOfNote; i++)
    {
        juce::MidiMessageSequence::MidiEventHolder* walker = midiseq->getEventPointer(i);
        juce::MidiMessage msg = walker->message;
        DBG(getMidiMessageDescription(msg));
        DBG(msg.getTimeStamp());
        if (msg.isMetaEvent()) {
            DBG("METALENGTH");
            DBG(msg.getMetaEventLength());
        }

        //TODO: iterate MIDImessage and get noteon, noteoff
        // If noteon is occured, get into the stack
        // If noteoff is occurred, check the stack and LIFO
        // Draw note component with noteon and noteoff msg
        // Set mouse cursor to finger on both of end
        
    }
    test();
}

void GridComponent::test()
{
    //TEST
    int noteOnStamp = 100;
    int noteOffstamp = 300;


    std::unique_ptr<NoteComponent> noteComponent =
        std::make_unique<NoteComponent>(40, noteOnStamp, noteOffstamp);

    noteComponent->setTitle("TEST");

    noteVector.push_back(noteComponent);

    addAndMakeVisible(*noteComponent);

}


juce::String GridComponent::getMidiMessageDescription(const juce::MidiMessage& m)
{
    if (m.isNoteOn())           return "Note on " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isNoteOff())          return "Note off " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isProgramChange())    return "Program change " + juce::String(m.getProgramChangeNumber());
    if (m.isPitchWheel())       return "Pitch wheel " + juce::String(m.getPitchWheelValue());
    if (m.isAftertouch())       return "After touch " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + juce::String(m.getAfterTouchValue());
    if (m.isChannelPressure())  return "Channel pressure " + juce::String(m.getChannelPressureValue());
    if (m.isAllNotesOff())      return "All notes off";
    if (m.isAllSoundOff())      return "All sound off";
    if (m.isMetaEvent())        return "Meta event";

    if (m.isController())
    {
        juce::String name(juce::MidiMessage::getControllerName(m.getControllerNumber()));

        if (name.isEmpty())
            name = "[" + juce::String(m.getControllerNumber()) + "]";

        return "Controller " + name + ": " + juce::String(m.getControllerValue());
    }

    return juce::String::toHexString(m.getRawData(), m.getRawDataSize());
}

juce::Range<float> GridComponent::getRangeBy(int note)
{
    return *noteLineRanges[note];
}

int GridComponent::getLastTimestamp()
{
    if (midiFile.getNumTracks() == 0)
        return 0;
    return midiFile.getLastTimestamp();
}

int GridComponent::getNoteBy (int ScreenPosition)
{
    for (auto i = 0; i < noteLineRanges.size(); i++)
    {
        if ((*noteLineRanges[i]).contains(ScreenPosition)) {
            return i;
        }
    }
    jassertfalse;
}


int GridComponent::convertStamptoPosition(int stamp)
{
    int gridWidth = getWidth();
    int gridX = getX();
    int lastStamp = getLastTimestamp();
    return (int)((float)stamp / (float)lastStamp * (float)gridWidth) + gridX;
}

int GridComponent::convertPositiontoStamp(int position)
{
    int gridWidth = getWidth();
    int gridX = getX();
    int lastStamp = getLastTimestamp();
    return (int)((position - gridX) / gridWidth) * lastStamp;
}


void GridComponent::updateBounds(int i)
{
    juce::Range<float> heightRange = *noteLineRanges[i];
    this->setBounds(this->getX(), heightRange.getStart(), this->getWidth(), heightRange.getLength());

    repaint();
}


void GridComponent::mouseDrag(const juce::MouseEvent& event)
{
    DBG(event.eventComponent->getTitle());

    //auto rec = juce::Rectangle<int>();

    //// Modify length of note by start position
    //if (event.eventComponent == &startComponent)
    //{
    //    auto originalEndPos = this->getX() + this->getWidth();
    //    auto modifiedStartPos = event.mouseDownPosition.getX();
    //    rec = juce::Rectangle<int>(modifiedStartPos, this->getY(), originalEndPos, this->getHeight());
    //}
    //// Modify length of note by end position
    //else if (event.eventComponent == &endComponent)
    //{
    //    auto originalStartPos = this->getX();
    //    auto modifiedEndPos = event.mouseDownPosition.getX();
    //    rec = juce::Rectangle<int>(originalStartPos, this->getY(), modifiedEndPos, this->getHeight());
    //}
    //// Modify note itself
    //else if (event.eventComponent == this)
    //{
    //    auto modifiedNote = event.mouseDownPosition.getY();
    //    juce::Range<float> range = parentGrid->getRangeBy(noteNum);

    //    if (!range.contains(modifiedNote))
    //    {
    //        noteNum = parentGrid->getNoteBy(modifiedNote);
    //        juce::Range<float> range = parentGrid->getRangeBy(noteNum);
    //        rec = juce::Rectangle<int>
    //            (
    //                this->getX(),
    //                range.getStart(),
    //                this->getWidth(),
    //                range.getLength()
    //                );
    //    }
    //    else {
    //        rec = getBounds();
    //    }
    //}
    //jassert(rec != juce::Rectangle<int>());

    //setStampBy(rec);
    //this->setBounds(rec);
}
