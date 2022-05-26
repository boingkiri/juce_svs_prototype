/*
  ==============================================================================

    MIDIProcessor.cpp
    Created: 19 May 2022 3:56:08pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MIDIProcessor.h"
#include "MIDIEditor.h"

//==============================================================================
MIDIProcessor::MIDIProcessor()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    numTracks.store(0);
}

MIDIProcessor::~MIDIProcessor()
{
}

const juce::String MIDIProcessor::getName() const
{
    //return JucePlugin_Name;
    return "";
}

bool MIDIProcessor::acceptsMidi() const
{
    #if JucePlugin_WantsMidiInput
    return true;
    #else
    return false;
    #endif
}

bool MIDIProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MIDIProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MIDIProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MIDIProcessor::getNumPrograms()
{
    return 1;  // At least 1.
}

int MIDIProcessor::getCurrentProgram()
{
    return 0;
}

void MIDIProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MIDIProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MIDIProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index);
    juce::ignoreUnused(newName);
}

void MIDIProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate);
    juce::ignoreUnused(samplesPerBlock);
    nextStartTime = -1.0;
}

void MIDIProcessor::releaseResources() 
{
}


void MIDIProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear all the incoming audio here
    for (auto i = 0; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    const juce::ScopedTryLock myScopedLock(processLock);

    if (myScopedLock.isLocked() > 0) {
        juce::AudioPlayHead::CurrentPositionInfo thePositionInfo;
        getPlayHead()->getCurrentPosition(thePositionInfo);

        if (numTracks.load() > 0)
        {
            // The MIDI file is played only when the transport is active
            if (thePositionInfo.isPlaying) 
            {
                const juce::MidiMessageSequence* theSequence = theMIDIFile.getTrack(currentTrack.load());

                auto startTime = thePositionInfo.timeInSeconds;
                auto endTime = startTime + buffer.getNumSamples() / getSampleRate();
                auto sampleLength = 1.0 / getSampleRate();

                // If the transport bar position has been moved by the user or because of looping
                if (std::abs(startTime - nextStartTime) > sampleLength && nextStartTime > 0.0)
                    sendAllNotesOff(midiMessages);
                
                nextStartTime = endTime;

                // If the MIDI file doesn't contain any event anymore
                if (isPlayingMIDI && startTime >= theSequence->getEndTime())
                    sendAllNotesOff(midiMessages);
                else 
                {
                    // Called when the user changes the track during playback
                    if (trackHasChanged)
                    {
                        trackHasChanged = false;
                        sendAllNotesOff(midiMessages);
                    }

                    // Iterating through the MIDI file contents and trying to find an event that
                    // needs to be called in the current time frame
                    for (auto i = 0; i < theSequence->getNumEvents(); i++)
                    {
                        juce::MidiMessageSequence::MidiEventHolder* event = theSequence->getEventPointer(i);

                        if (event->message.getTimeStamp() >= startTime && event->message.getTimeStamp() < endTime)
                        {
                            auto samplePosition = juce::roundToInt((event->message.getTimeStamp() - startTime) * getSampleRate());
                            midiMessages.addEvent(event->message, samplePosition);

                            isPlayingMIDI = true;
                        }
                    }
                }

            }
            else
            {
                // If the transport isn't active anymore
                if (isPlayingMIDI)
                    sendAllNotesOff(midiMessages);
            }
        }
    }
    else
    {
        // If we had just opened a MIDI file with no content
        if (isPlayingMIDI)
            sendAllNotesOff(midiMessages);
    }
}

// ==========
bool MIDIProcessor::hasEditor() const
{
    return true; 
    // Change this to false if you choose to not supply an editor
    // But I have to implement editor, so its true!!!!!
}

juce::AudioProcessorEditor* MIDIProcessor::createEditor()
{
    return new MIDIEditor(*this);
}

void MIDIProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void MIDIProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data);
    juce::ignoreUnused(sizeInBytes);
}


//=========================
// This creates new instance of the plugins.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MIDIProcessor();
}

//==========================
void MIDIProcessor::loadMIDIFile(juce::File fileMIDI)
{
    const juce::ScopedLock myScopedLock(processLock);

    theMIDIFile.clear();

    juce::FileInputStream theStream(fileMIDI);
    theMIDIFile.readFrom(theStream);

    /*
        This function call means that the MIDI ile is going to e played with the
        original tempo and signature

        To play it at the host tempo, we might need to do it manually in processBlock
        and retrieve all the time the current tempo to track tempo changes.
    */
    theMIDIFile.convertTimestampTicksToSeconds();

    numTracks.store(theMIDIFile.getNumTracks());
    currentTrack.store(0);
    trackHasChanged = false;
}

int MIDIProcessor::getNumTracks()
{
    return numTracks.load();
}

void MIDIProcessor::setCurrentTrack(int value)
{
    jassert(value >= 0 && value < numTracks.load());

    if (numTracks.load() == 0)
        return;
    currentTrack.store(value);
    trackHasChanged = true;
}

int MIDIProcessor::getCurrentTrack()
{
    if (numTracks.load() == 0)
        return -1;
    else
        return currentTrack.load();
}

void MIDIProcessor::sendAllNotesOff(juce::MidiBuffer& midiMessages)
{
    for (auto i = 1; i <= 16; i++)
    {
        midiMessages.addEvent(juce::MidiMessage::allNotesOff(i), 0);
        midiMessages.addEvent(juce::MidiMessage::allSoundOff(i), 0);
        midiMessages.addEvent(juce::MidiMessage::allControllersOff(i), 0);
    }

    isPlayingMIDI = false;
}
//
//void MIDIProcessor::paint (juce::Graphics& g)
//{
//    /* This demo code just fills the component's background and
//       draws some placeholder text to get you started.
//
//       You should replace everything in this method with your own
//       drawing code..
//    */
//
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
//
//    g.setColour (juce::Colours::grey);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
//
//    g.setColour (juce::Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("MIDIProcessor", getLocalBounds(),
//                juce::Justification::centred, true);   // draw some placeholder text
//}
//
//void MIDIProcessor::resized()
//{
//    // This method is where you should set the bounds of any child
//    // components that your component contains..
//
//}
