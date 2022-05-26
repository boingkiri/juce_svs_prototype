#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() 
    :state(Stopped),
    thumbnailCache (5), // Number of thumbnails to store
    thumbnail (512, formatManager, thumbnailCache) // Type of ChangeBroadcaster. Listener can be attached.
{

    addAndMakeVisible(&savePathLabel);
    savePathLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    savePathLabel.setText("Save path: ", juce::dontSendNotification);
    savePathLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(&savePathEditor);
    savePathEditor.setFont(juce::Font(16.0f, juce::Font::bold));
    savePathEditor.setText("Click me", juce::dontSendNotification);
    savePathEditor.setJustification(juce::Justification::centred);

    addAndMakeVisible(&savePathButton);
    savePathButton.setButtonText("...");
    savePathButton.onClick = [this] { savePathButtonClicked(); };


    addAndMakeVisible(&MIDIPathButton);
    MIDIPathButton.setButtonText("...");
    //MIDIPathButton.onClick = [this] { playButtonClicked(); };
    MIDIPathButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    MIDIPathButton.setEnabled(false);

    addAndMakeVisible(&LyricsButton);
    LyricsButton.setButtonText("Stop");
    //LyricsButton.onClick = [this] {stopButtonClicked(); };
    LyricsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    LyricsButton.setEnabled(false);

    setSize(300, 200);

    formatManager.registerBasicFormats(); // It makes various audio file readable

    transportSource.addChangeListener(this);
    thumbnail.addChangeListener(this);

    setAudioChannels(0, 2);
    // Make sure you set the size of the component after
    // you add any child components.
    //setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    startTimer(40); // The Timer will be called every 40ms
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    //bufferToFill.clearActiveBufferRegion();
    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    transportSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
    juce::Rectangle<int> thumbnailBounds(10, 100, getWidth() - 20, getHeight() - 120);

    if (thumbnail.getNumChannels() == 0) MainComponent::paintIfNoFileLoaded(g, thumbnailBounds);
    else MainComponent::paintIfFileLoaded(g, thumbnailBounds);
}

void MainComponent::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No file Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void MainComponent::paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds) 
{
    g.setColour(juce::Colours::white);
    g.fillRect(thumbnailBounds);

    g.setColour(juce::Colours::red);

    auto audioLength = (float)thumbnail.getTotalLength();
    thumbnail.drawChannels(
        g,
        thumbnailBounds,
        0.0,
        audioLength,
        1.0f
    );

    g.setColour(juce::Colours::green);

    auto audioPosition = (float)transportSource.getCurrentPosition();
    auto drawPosition = (audioPosition / audioLength) * (float)thumbnailBounds.getWidth() + (float)thumbnailBounds.getX();

    g.drawLine(drawPosition, (float)thumbnailBounds.getY(), drawPosition, (float)thumbnailBounds.getBottom(), 2.0f);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    savePathLabel.setBounds(10, 10, 50, 20);
    savePathEditor.setBounds(75, 10, getWidth() - 150, 20);
    savePathButton.setBounds(getWidth() - 30, 10, 20, 20);
    //MIDIPathButton.setBounds(10, 70, getWidth() - 20, 20);
    //LyricsButton.setBounds(10, 100, getWidth() - 20, 20);
}
