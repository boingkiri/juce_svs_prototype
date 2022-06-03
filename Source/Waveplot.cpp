/*
  ==============================================================================

    Waveplot.cpp
    Created: 2 Jun 2022 5:31:09pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include "Waveplot.h"

Waveplot::Waveplot()
    : state(Stopped),
    thumbnailCache(5),
    thumbnail(512, formatManager, thumbnailCache)
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
    thumbnail.addChangeListener(this);

    setSize(150, 100);
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
        else if ((state == Stopping) || (state == Playing))
            changeState(Stopped);
        else if (state == Pausing)
            changeState(Paused);
    }
    if (source == &thumbnail)
        thumbnailChanged();
}

void Waveplot::thumbnailChanged()
{
    repaint();
}

void Waveplot::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case Stopped:
                playButton.setButtonText("Play");
                stopButton.setButtonText("Stop");
                stopButton.setEnabled(false);
                transportSource.setPosition(0.0);
                break;

            case Starting:
                transportSource.start();
                break;

            case Playing:
                playButton.setButtonText("Pause");
                stopButton.setButtonText("Stop");
                stopButton.setEnabled(true);
                break;

            case Pausing:
                transportSource.stop();
                break;

            case Paused:
                playButton.setButtonText("Resume");
                stopButton.setButtonText("Return to zero");
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
    if ((state == Stopped) || (state == Paused)) changeState(Starting);
    else if (state == Playing) changeState(Pausing);
}

void Waveplot::stopButtonClicked()
{
    if (state == Paused) changeState(Stopped);
    else changeState(Stopping);
}

void Waveplot::paint(juce::Graphics& g)
{
    juce::Rectangle<int> thumbnailBounds(
        10, 100, getWidth() - 20, getHeight() - 120);

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void Waveplot::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No File Loaded",
        thumbnailBounds, juce::Justification::centred, 1);
}

void Waveplot::paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::white);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::red);

    thumbnail.drawChannels(
        g,
        thumbnailBounds,
        0.0,
        thumbnail.getTotalLength(),
        1.0f
    );
}

void Waveplot::resized()
{
    auto r = getBounds();
    /*playButton.setBounds(r.removeFromLeft(100));
    stopButton.setBounds(r.removeFromLeft(100));*/
    playButton.setBounds(10, 40, getWidth() - 20, 20);
    stopButton.setBounds(10, 70, getWidth() - 20, 20);
    
}

void Waveplot::setWave(juce::File file)
{
    juce::AudioFormatReader* reader = 
        formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        playButton.setEnabled(true);
        readerSource.reset(newSource.release());

        thumbnail.setSource(new juce::FileInputSource(file));
    }
}