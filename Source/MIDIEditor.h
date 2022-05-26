/*
  ==============================================================================

    MIDIEditor.h
    Created: 19 May 2022 1:33:19pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MIDIProcessor.h"


//==============================================================================
/*
*/
class MIDIEditor :
    //public juce::Component,
    //public juce::AudioAppComponent,
    public juce::AudioProcessorEditor,
    private juce::Slider::Listener,
    public juce::Button::Listener,
    public juce::ComboBox::Listener
{
public:
    MIDIEditor(MIDIProcessor&);
    ~MIDIEditor();

    //==============================================================================

    //void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    //void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    //void releaseResources() override;

    //==============================================================================
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* combo) override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    float noteOnVel = 0.0f;


private:
    void sliderValueChanged(juce::Slider* slider) override;

    juce::Slider midiVolume;
    juce::MidiFile midifile;

    std::unique_ptr<juce::FileChooser> chooser;
    juce::AudioFormatManager formatManager;

    // =================
    void updateTrackComboBox();

    // =================
    juce::TextButton* buttonLoadMIDIFile;
    juce::ComboBox* comboTrack;
    // =================
    MIDIProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIEditor)
};
