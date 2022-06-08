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
#include "NoteComponent.h"

class GridComponent :
    public juce::Component
{

public:
    GridComponent();
    ~GridComponent() override;
    void updateNoteLineRanges(float);
    void paint(juce::Graphics&);
    void setMidiFile(juce::MidiFile);
    void showMidiMessageSequence();

    void setNoteComponentArray(juce::MidiMessage noteOnmsg, juce::MidiMessage noteOffmsg);
    juce::String getMidiMessageDescription(const juce::MidiMessage& m);

    juce::Range<float> getRangeBy(int note);
    int getLastTimestamp();
    int getNoteBy(int ScreenPosition);

private:
    // The index of this OwnedArray will refer to a **midi note number**
    // In order to get the y values of the note line of note number 60,
    // we can call noteLineRanges[60]
    juce::OwnedArray<juce::Range<float>> noteLineRanges;
    juce::MidiFile midiFile = juce::MidiFile{};
    std::vector<std::unique_ptr<NoteComponent>> noteVector;

    void test();

};