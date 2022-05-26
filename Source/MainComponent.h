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

    void setComponentModule(
        juce::Label* label, juce::TextEditor* editor, juce::TextButton* button,
        juce::String label_str, juce::String editor_str, juce::String button_str
    );

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        //if (source == &transportSource) transportSourceChanged();
        if (source == &thumbnail) thumbnailChanged();
    }

    /*void transportSourceChanged()
    {
        if (transportSource.isPlaying()) changeState(Playing);
        else if ((state == Stopping) || (state == Playing)) changeState(Stopped);
        else if (state == Pausing) changeState(Paused);
    }*/

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

    void MainComponent::setComponentPosition(juce::Label* label, juce::TextEditor* editor, juce::TextButton* button, int index);

    //void changeState(TrasportState newState)
    //{
    //    if (state != newState) {
    //        state = newState;

    //        switch (state) {
    //        case Stopped:
    //            MIDIPathButton.setButtonText("Play");
    //            stopButton.setButtonText("Stop");
    //            stopButton.setEnabled(false);
    //            //playButton.setEnabled(true);
    //            transportSource.setPosition(0.0);
    //            break;

    //        case Starting:
    //            //playButton.setEnabled(false);
    //            transportSource.start();
    //            break;

    //        case Playing:
    //            MIDIPathButton.setButtonText("Pause");
    //            stopButton.setButtonText("Stop");
    //            stopButton.setEnabled(true);
    //            break;

    //        case Pausing:
    //            transportSource.stop();
    //            break;
    //        
    //        case Paused:
    //            MIDIPathButton.setButtonText("Resume");
    //            stopButton.setButtonText("Return to Zero");
    //            break;

    //        case Stopping:
    //            transportSource.stop();
    //            break;
    //        
    //        }
    //    }
    //}

    void savePathButtonClicked();
    void MIDIPathButtonClicked();

    /*void playButtonClicked()
    {
        if ((state == Stopped) || (state == Paused)) changeState(Starting);
        else if (state == Playing) changeState(Pausing);
    }

    void stopButtonClicked()
    {
        if (state == Paused) changeState(Stopped);
        else changeState(Stopping);
    }*/

    // ===================
    //juce::TextButton openButton;
    juce::TextButton savePathButton;
    juce::TextEditor savePathEditor;
    juce::Label savePathLabel;
    //juce::TextButton playButton;
    juce::TextButton MIDIPathButton;
    juce::TextEditor MIDIPathEditor;
    juce::Label MIDINoteCountLabel;
    juce::Label MIDIPathLabel;
    //juce::TextButton stopButton;
    juce::TextButton LyricsButton;
    juce::TextEditor LyricsEditor;
    juce::Label LyricsLabel;

    std::unique_ptr<juce::FileChooser> chooser;

    juce::MidiFile MIDIfile;

    // [members]
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TrasportState state;
    
    // Make window opening one or more audio sustainable. 
    // (It maintains thumbnail of audio when closing the window and reopening.)
    juce::AudioThumbnailCache thumbnailCache; 
    juce::AudioThumbnail thumbnail;

    // [members]
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
