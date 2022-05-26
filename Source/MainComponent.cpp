#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() 
    :state(Stopped),
    thumbnailCache (5), // Number of thumbnails to store
    thumbnail (512, formatManager, thumbnailCache) // Type of ChangeBroadcaster. Listener can be attached.
{
    
    setComponentModule(&savePathLabel, &savePathEditor, &savePathButton, "Save..", "", "...");
    savePathEditor.setReadOnly(true);
    savePathButton.onClick = [this] { savePathButtonClicked(); };

    setComponentModule(&MIDIPathLabel, &MIDIPathEditor, &MIDIPathButton, "MIDI", "", "...");
    MIDIPathButton.onClick = [this] { MIDIPathButtonClicked(); };
    setLabelComponent(&MIDINoteCountLabel,"Count");

    setComponentModule(&LyricsLabel, &LyricsEditor, &LyricsButton, "Lyrics", "", "...");
    setLabelComponent(&LyricsCountLabel, "Count");

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

void MainComponent::setComponentModule(
    juce::Label* label, juce::TextEditor* editor, juce::TextButton* button,
    juce::String label_str, juce::String editor_str, juce::String button_str
)
{
    setLabelComponent(label, label_str);
    setEditorComponent(editor, editor_str);
    setButtonComponent(button, button_str);
}

void MainComponent::setLabelComponent(juce::Label* label, juce::String label_str)
{
    addAndMakeVisible(label);
    label->setFont(juce::Font(16.0f, juce::Font::bold));
    label->setText(label_str, juce::dontSendNotification);
    label->setJustificationType(juce::Justification::centred);
}

void MainComponent::setEditorComponent(juce::TextEditor* editor, juce::String editor_str)
{
    addAndMakeVisible(editor);
    editor->setFont(juce::Font(16.0f, juce::Font::bold));
    editor->setText(editor_str, juce::dontSendNotification);
    editor->setJustification(juce::Justification::centred);
}

void MainComponent::setButtonComponent(juce::TextButton* button, juce::String button_str)
{
    addAndMakeVisible(button);
    button->setButtonText(button_str);
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
    setComponentPosition(&savePathLabel, &savePathEditor, &savePathButton, 0);
    setComponentPosition(&MIDIPathLabel, &MIDIPathEditor, &MIDIPathButton, 1);

    MIDIPathEditor.setBounds(75, 40, getWidth() - 170, 20);
    MIDINoteCountLabel.setBounds(getWidth() - 90, 40, 50, 20);

    setComponentPosition(&LyricsLabel, &LyricsEditor, &LyricsButton, 2);

    LyricsEditor.setBounds(75, 70, getWidth() - 170, 20);
    LyricsCountLabel.setBounds(getWidth() - 90, 70, 50, 20);
    
}

void MainComponent::setComponentPosition(juce::Label* label, juce::TextEditor* editor, juce::TextButton* button, int index)
{
    label->setBounds(10, 10 + index * 30, 50, 20);
    editor->setBounds(75, 10 + index * 30, getWidth() - 120, 20);
    button->setBounds(getWidth() - 30, 10 + index * 30, 20, 20);
}



//=========
void MainComponent::savePathButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>(
        "Select a MIDI file to play..",
        juce::File{},
        "*.mid"
        );
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                /*auto* reader = formatManager.createReaderFor(file);

                if (reader != nullptr)
                {
                    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                    transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                    MIDIPathButton.setEnabled(true);
                    thumbnail.setSource(new juce::FileInputSource(file));
                    readerSource.reset(newSource.release());
                }*/
                const juce::String savePath = file.getFullPathName();
                savePathEditor.setText(savePath);
            }
        });
}

void MainComponent::MIDIPathButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>(
        "Select a MIDI file to play..",
        juce::File{},
        "*.mid"
        );
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                const juce::String savePath = file.getFullPathName();
                MIDIPathEditor.setText(savePath);

                juce::FileInputStream MIDIStream(file);
                bool read_flag = MIDIfile.readFrom(MIDIStream);
                if (!read_flag)
                {
                    MIDIPathEditor.setText("Reading midi file failed");
                }
                else {
                    jassert(MIDIfile.getNumTracks() == 1);

                    const juce::MidiMessageSequence* midiseq = MIDIfile.getTrack(0);
                    int numOfNote = midiseq->getNumEvents();

                    //Count NoteOn event
                    //TODO: Need to study MIDI format
                    int count = 0;
                    for (int i = 0; i < numOfNote; i++) 
                    {
                        juce::MidiMessageSequence::MidiEventHolder* walker = midiseq->getEventPointer(i);
                        juce::MidiMessage msg = walker->message;
                        if (msg.isNoteOn())
                            count += 1;
                    }

                    MIDINoteCountLabel.setText(juce::String(count), juce::dontSendNotification);
                }
            }
        });
}