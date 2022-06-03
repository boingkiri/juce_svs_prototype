/*
  ==============================================================================

    Waveplot.cpp
    Created: 2 Jun 2022 5:31:09pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include "Waveplot.h"

Waveplot::Waveplot()
    : state (Stopped)
{
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);

    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);
}

Waveplot::~Waveplot()
{
    // This shuts down the audio device and clears the audio source.
    this->removeAllChildren();
    //shutdownAudio();
}

void Waveplot::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState(Playing);
        else
            changeState(Stopped);
    }
}

void Waveplot::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case Stopped:
                stopButton.setEnabled(false);
                playButton.setEnabled(true);
                transportSource.setPosition(0.0);
                break;

            case Starting:
                playButton.setEnabled(false);
                transportSource.start();
                break;

            case Playing:
                stopButton.setEnabled(true);
                break;

            case Stopping:
                transportSource.stop();
                break;
        }
    }
}

void Waveplot::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);
}


void Waveplot::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Waveplot::releaseResources()
{
    transportSource.releaseResources();
}

void Waveplot::playButtonClicked()
{
    changeState(Starting);
    //if ((state == Stopped) || (state == Paused)) changeState(Starting);
    //else if (state == Playing) changeState(Pausing);
}

void Waveplot::stopButtonClicked()
{
    changeState(Stopping);
    //if (state == Paused) changeState(Stopped);
    //else changeState(Stopping);
}

void Waveplot::paint(juce::Graphics& g)
{
}

void Waveplot::resized()
{
    auto r = getBounds();
    playButton.setBounds(r.removeFromLeft(100));
    stopButton.setBounds(r.removeFromLeft(100));
    
}