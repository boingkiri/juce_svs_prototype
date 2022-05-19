#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, 
    public juce::ChangeListener,
    private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &transportSource) transportSourceChanged();
        if (source == &thumbnail) thumbnailChanged();
    }

    void transportSourceChanged()
    {
        if (transportSource.isPlaying()) changeState(Playing);
        else if ((state == Stopping) || (state == Playing)) changeState(Stopped);
        else if (state == Pausing) changeState(Paused);
    }

    void thumbnailChanged()
    {
        repaint();
    }

    void paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);

private:
    //==============================================================================
    // Your private member variables go here...
    void timerCallback() override
    {
        repaint();
    }
    enum TrasportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };

    void changeState(TrasportState newState)
    {
        if (state != newState) {
            state = newState;

            switch (state) {
            case Stopped:
                playButton.setButtonText("Play");
                stopButton.setButtonText("Stop");
                stopButton.setEnabled(false);
                //playButton.setEnabled(true);
                transportSource.setPosition(0.0);
                break;

            case Starting:
                //playButton.setEnabled(false);
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
                stopButton.setButtonText("Return to Zero");
                break;

            case Stopping:
                transportSource.stop();
                break;
            
            }
        }
    }

    void openButtonClicked()
    {
        chooser = std::make_unique<juce::FileChooser>(
            "Select a Wave file to play..",
            juce::File{},
            "*.wav"
            );
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();

                if (file != juce::File{})
                {
                    auto* reader = formatManager.createReaderFor(file);

                    if (reader != nullptr)
                    {
                        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                        playButton.setEnabled(true);
                        thumbnail.setSource(new juce::FileInputSource(file));
                        readerSource.reset(newSource.release());
                    }
                }
            });
    }

    void playButtonClicked()
    {
        if ((state == Stopped) || (state == Paused)) changeState(Starting);
        else if (state == Playing) changeState(Pausing);
    }

    void stopButtonClicked()
    {
        if (state == Paused) changeState(Stopped);
        else changeState(Stopping);
    }

    // ===================
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    std::unique_ptr<juce::FileChooser> chooser;

    // [members]
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TrasportState state;
    
    // Make window opening one or more audio sustainable. (It maintains thumbnail of audio when closing the window and reopening.)
    juce::AudioThumbnailCache thumbnailCache; 
    juce::AudioThumbnail thumbnail;

    // [members]
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
