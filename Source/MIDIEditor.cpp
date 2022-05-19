/*
  ==============================================================================

    MIDIEditor.cpp
    Created: 19 May 2022 1:33:19pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MIDIEditor.h"

//==============================================================================
MIDIEditor::MIDIEditor()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    // Setting size of GUI
    setSize(200, 200);

    // The parameters of our slider object
    midiVolume.setSliderStyle(juce::Slider::LinearBarVertical);
    midiVolume.setRange(0.0, 127.0, 1.0);
    midiVolume.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    midiVolume.setPopupDisplayEnabled(true, false, this);
    midiVolume.setTextValueSuffix(" Volume");
    midiVolume.setValue(1.0);

    // this function adds the slider to the editor
    addAndMakeVisible(&midiVolume);

    // Add listener to the slider
    midiVolume.addListener(this);
}

MIDIEditor::~MIDIEditor()
{
}

void MIDIEditor::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("MIDIEditor", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
    
    g.setColour(juce::Colours::black);
    g.setFont(15.0f);
    g.drawFittedText("MIDI Volume", 0, 0, getWidth(), 30, juce::Justification::centred, 1);

}

void MIDIEditor::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    midiVolume.setBounds(40, 30, 20, getHeight() - 60);
}

void MIDIEditor::sliderValueChanged(juce::Slider* slider)
{
    // In this case, I will put this value on this instance naively.
    this->noteOnVel = midiVolume.getValue();
}