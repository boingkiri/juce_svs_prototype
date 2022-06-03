/*
  ==============================================================================

    Waveplot.h
    Created: 2 Jun 2022 5:31:09pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Waveplot:
    public juce::Component,
    private juce::ChangeListener
{
public:
    Waveplot();
    ~Waveplot() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void prepareToPlay(int, double);
    void releaseResources();
    void playButtonClicked();
    void stopButtonClicked();
    void setWave(juce::File);


private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    juce::TextButton playButton;
    juce::TextButton stopButton;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;

    void changeListenerCallback(juce::ChangeBroadcaster*) override;
    void changeState(TransportState);
};