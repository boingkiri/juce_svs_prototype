#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() 
    //:state(Stopped),
    //thumbnailCache (5), // Number of thumbnails to store
    //thumbnail (512, formatManager, thumbnailCache) // Type of ChangeBroadcaster. Listener can be attached.
{

   // Set default font
    const juce::Typeface::Ptr myfont = juce::Typeface::createSystemTypefaceFor(
        BinaryData::NotoSansKRBold_otf,
        BinaryData::NotoSansKRBold_otfSize
    );
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(myfont);

    // Set PianoRollComponent
    //addAndMakeVisible(pianoRollComponent);
    pianoRollComponent = std::make_unique<PianoRollComponent>();
    //this->addChildComponent(*pianoRollComponent, -1);
    addAndMakeVisible(*pianoRollComponent);

    // Set Waveplot
    waveplotComponent = std::make_unique<Waveplot>();
    addAndMakeVisible(*waveplotComponent);
    
    // Set Configure and send button
    setButtonComponent(&configButton, "Config");
    setButtonComponent(&sendButton, "Send");
    sendButton.onClick = [this] {sendButtonClicked(); };
    //sendButton.setEnabled(false);

    // Set save component
    setComponentModule(&savePathLabel, &savePathEditor, &savePathButton, "Save..", "", "...");
    savePathEditor.setReadOnly(true);
    savePathButton.onClick = [this] { savePathButtonClicked(); };

    // Set midi component
    setComponentModule(&MIDIPathLabel, &MIDIPathEditor, &MIDIPathButton, "MIDI", "", "...");
    MIDIPathButton.onClick = [this] { MIDIPathButtonClicked(); };
    setLabelComponent(&MIDINoteCountLabel,"Count");

    // Set lyrics component
    setComponentModule(&lyricsLabel, &lyricsEditor, &lyricsButton, "Lyrics", "", "...");
    lyricsButton.onClick = [this] { LyricsButtonClicked(); };
    lyricsEditor.addListener(this);
    setLabelComponent(&lyricsCountLabel, "Count");

    // Make sure you set the size of the component after
    // you add any child components.
    setSize(900, 600);

    //formatManager.registerBasicFormats(); // It makes various audio file readable

    //transportSource.addChangeListener(this);
    //thumbnail.addChangeListener(this);

    setAudioChannels(0, 2);

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
    this->removeAllChildren();
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

    waveplotComponent->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    // 
    //bufferToFill.clearActiveBufferRegion();
    waveplotComponent->getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    waveplotComponent->releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
    //juce::Rectangle<int> thumbnailBounds(10, 100, getWidth() - 20, getHeight() - 120);

    //if (thumbnail.getNumChannels() == 0) MainComponent::paintIfNoFileLoaded(g, thumbnailBounds);
    //else MainComponent::paintIfFileLoaded(g, thumbnailBounds);

}
//
//void MainComponent::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
//{
//    g.setColour(juce::Colours::darkgrey);
//    g.fillRect(thumbnailBounds);
//    g.setColour(juce::Colours::white);
//    g.drawFittedText("No file Loaded", thumbnailBounds, juce::Justification::centred, 1);
//}
//
//void MainComponent::paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds) 
//{
//    g.setColour(juce::Colours::white);
//    g.fillRect(thumbnailBounds);
//
//    g.setColour(juce::Colours::red);
//
//    auto audioLength = (float)thumbnail.getTotalLength();
//    thumbnail.drawChannels(
//        g,
//        thumbnailBounds,
//        0.0,
//        audioLength,
//        1.0f
//    );
//
//    g.setColour(juce::Colours::green);
//
//    auto audioPosition = (float)transportSource.getCurrentPosition();
//    auto drawPosition = (audioPosition / audioLength) * (float)thumbnailBounds.getWidth() + (float)thumbnailBounds.getX();
//
//    g.drawLine(drawPosition, (float)thumbnailBounds.getY(), drawPosition, (float)thumbnailBounds.getBottom(), 2.0f);
//
//}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    auto r = getBounds();

    configButton.setBounds(10, 10, 50, 20);
    sendButton.setBounds(getWidth() - 70, 10, 50, 20);

    setComponentPosition(&savePathLabel, &savePathEditor, &savePathButton, 1);
    setComponentPosition(&MIDIPathLabel, &MIDIPathEditor, &MIDIPathButton, 2);
    
    MIDIPathEditor.setBounds(75, 70, getWidth() - 170, 20);
    MIDINoteCountLabel.setBounds(getWidth() - 90, 70, 50, 20);

    setComponentPosition(&lyricsLabel, &lyricsEditor, &lyricsButton, 3);

    lyricsEditor.setBounds(75, 100, getWidth() - 170, 20);
    lyricsCountLabel.setBounds(getWidth() - 90, 100, 50, 20);

    r.removeFromTop(80);
    
    auto pianoRollBound = r.removeFromTop(400);

    pianoRollComponent->setBounds(pianoRollBound);

    /// <summary>
    /// ////
    /// </summary>
    
    waveplotComponent->setBounds(r);

    //pianoRollComponent->setBounds(getBounds().removeFromBottom(400));
    //pianoRollComponent->setBounds(getLocalBounds());
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
                const juce::String midiPath = file.getFullPathName();
                MIDIPathEditor.setText(midiPath);

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

void MainComponent::LyricsButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>(
        "Select a Lyrics file to copy..",
        juce::File{},
        "*.txt"
        );
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                juce::String lyricsPath = file.getFullPathName().toUTF8();
                lyricsEditor.setText(lyricsPath);

                juce::FileInputStream inputStream(file);
                while (!inputStream.isExhausted()) 
                {
                    auto line = inputStream.readNextLine();
                    lyricsEditor.setText(line);
                }
            }
        });
}

void MainComponent::sendButtonClicked() 
{
    juce::Value lyricsString = lyricsEditor.getTextValue();
    std::string rawLyricsString = lyricsString.toString().toUTF8();
    
    juce::File midifile = juce::File(MIDIPathEditor.getTextValue().toString().toStdString());

    const juce::URL modelURL = juce::URL("http://127.0.0.1:5000/synthesize")
        .withParameter("lyrics", rawLyricsString)
        .withFileToUpload("midi", midifile, "audio/midi");
    
    auto r = new juce::WebInputStream(modelURL, true);
    bool success = r->connect(nullptr);

    juce::MemoryBlock wavMB(1024*1024*20);
    r->read(wavMB.getData(), r->getTotalLength());

    juce::File wavfile(
        "D:/JUCE/Programming/Simple_Prototype/NewProject/result_wav.wav"
    );

    if (wavfile.exists()) {
        wavfile.deleteFile();
    }

    auto result = wavfile.create();

    if (result.failed())
    {
        DBG("Creating wav file failed");
    }
    
    juce::FileOutputStream stream(wavfile);
    stream.write(wavMB.getData(), r->getTotalLength());
    stream.flush();

    waveplotComponent->setWave(wavfile);

}

void MainComponent::textEditorTextChanged(juce::TextEditor& texteditor)
{
    juce::String editorValue = texteditor.getTextValue().getValue();
    
    juce::StringRef whitespace = " ";
    int lyricsLength = editorValue.removeCharacters(whitespace).length();

    lyricsCountLabel.setText(juce::String(lyricsLength), juce::dontSendNotification);
}
