/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include "KeyboardComponent.h"


//==============================================================================
KeyboardComponent::KeyboardComponent(juce::MidiKeyboardState& stateToUse, Orientation orientationToUse)
    : KeyboardComponentBase(orientationToUse), state(stateToUse)
{
    state.addListener(this);

    // initialise with a default set of qwerty key-mappings..
    int note = 0;

    for (char c : "awsedftgyhujkolp;")
        setKeyPressForNote({ c, 0, 0 }, note++);

    mouseOverNotes.insertMultiple(0, -1, 32);
    mouseDownNotes.insertMultiple(0, -1, 32);

    colourChanged();
    setWantsKeyboardFocus(true);

    startTimerHz(20);
}

KeyboardComponent::~KeyboardComponent()
{
    state.removeListener(this);
}

//==============================================================================
void KeyboardComponent::setVelocity(float v, bool useMousePosition)
{
    velocity = v;
    useMousePositionForVelocity = useMousePosition;
}

//==============================================================================
void KeyboardComponent::setMidiChannel(int midiChannelNumber)
{
    jassert(midiChannelNumber > 0 && midiChannelNumber <= 16);

    if (midiChannel != midiChannelNumber)
    {
        resetAnyKeysInUse();
        midiChannel = juce::jlimit(1, 16, midiChannelNumber);
    }
}

void KeyboardComponent::setMidiChannelsToDisplay(int midiChannelMask)
{
    midiInChannelMask = midiChannelMask;
    noPendingUpdates.store(false);
}

//==============================================================================
void KeyboardComponent::clearKeyMappings()
{
    resetAnyKeysInUse();
    keyPressNotes.clear();
    keyPresses.clear();
}

void KeyboardComponent::setKeyPressForNote(const juce::KeyPress& key, int midiNoteOffsetFromC)
{
    removeKeyPressForNote(midiNoteOffsetFromC);

    keyPressNotes.add(midiNoteOffsetFromC);
    keyPresses.add(key);
}

void KeyboardComponent::removeKeyPressForNote(int midiNoteOffsetFromC)
{
    for (int i = keyPressNotes.size(); --i >= 0;)
    {
        if (keyPressNotes.getUnchecked(i) == midiNoteOffsetFromC)
        {
            keyPressNotes.remove(i);
            keyPresses.remove(i);
        }
    }
}

void KeyboardComponent::setKeyPressBaseOctave(int newOctaveNumber)
{
    jassert(newOctaveNumber >= 0 && newOctaveNumber <= 10);

    keyMappingOctave = newOctaveNumber;
}

//==============================================================================
void KeyboardComponent::resetAnyKeysInUse()
{
    if (!keysPressed.isZero())
    {
        for (int i = 128; --i >= 0;)
            if (keysPressed[i])
                state.noteOff(midiChannel, i, 0.0f);

        keysPressed.clear();
    }

    for (int i = mouseDownNotes.size(); --i >= 0;)
    {
        auto noteDown = mouseDownNotes.getUnchecked(i);

        if (noteDown >= 0)
        {
            state.noteOff(midiChannel, noteDown, 0.0f);
            mouseDownNotes.set(i, -1);
        }

        mouseOverNotes.set(i, -1);
    }
}

void KeyboardComponent::updateNoteUnderMouse(const juce::MouseEvent& e, bool isDown)
{
    updateNoteUnderMouse(e.getEventRelativeTo(this).position, isDown, e.source.getIndex());
}

void KeyboardComponent::updateNoteUnderMouse(juce::Point<float> pos, bool isDown, int fingerNum)
{
    const auto noteInfo = getNoteAndVelocityAtPosition(pos);
    const auto newNote = noteInfo.note;
    const auto oldNote = mouseOverNotes.getUnchecked(fingerNum);
    const auto oldNoteDown = mouseDownNotes.getUnchecked(fingerNum);
    const auto eventVelocity = useMousePositionForVelocity ? noteInfo.velocity * velocity : velocity;

    if (oldNote != newNote)
    {
        repaintNote(oldNote);
        repaintNote(newNote);
        mouseOverNotes.set(fingerNum, newNote);
    }

    if (isDown)
    {
        if (newNote != oldNoteDown)
        {
            if (oldNoteDown >= 0)
            {
                mouseDownNotes.set(fingerNum, -1);

                if (!mouseDownNotes.contains(oldNoteDown))
                    state.noteOff(midiChannel, oldNoteDown, eventVelocity);
            }

            if (newNote >= 0 && !mouseDownNotes.contains(newNote))
            {
                state.noteOn(midiChannel, newNote, eventVelocity);
                mouseDownNotes.set(fingerNum, newNote);
            }
        }
    }
    else if (oldNoteDown >= 0)
    {
        mouseDownNotes.set(fingerNum, -1);

        if (!mouseDownNotes.contains(oldNoteDown))
            state.noteOff(midiChannel, oldNoteDown, eventVelocity);
    }
}

void KeyboardComponent::repaintNote(int noteNum)
{
    if (getRangeStart() <= noteNum && noteNum <= getRangeEnd())
        repaint(getRectangleForKey(noteNum).getSmallestIntegerContainer());
}


void KeyboardComponent::mouseMove(const juce::MouseEvent& e)
{
    updateNoteUnderMouse(e, false);
}

void KeyboardComponent::mouseDrag(const juce::MouseEvent& e)
{
    auto newNote = getNoteAndVelocityAtPosition(e.position).note;

    if (newNote >= 0 && mouseDraggedToKey(newNote, e))
        updateNoteUnderMouse(e, true);
}

void KeyboardComponent::mouseDown(const juce::MouseEvent& e)
{
    auto newNote = getNoteAndVelocityAtPosition(e.position).note;

    if (newNote >= 0 && mouseDownOnKey(newNote, e))
        updateNoteUnderMouse(e, true);
}

void KeyboardComponent::mouseUp(const juce::MouseEvent& e)
{
    updateNoteUnderMouse(e, false);

    auto note = getNoteAndVelocityAtPosition(e.position).note;

    if (note >= 0)
        mouseUpOnKey(note, e);
}

void KeyboardComponent::mouseEnter(const juce::MouseEvent& e)
{
    updateNoteUnderMouse(e, false);
}

void KeyboardComponent::mouseExit(const juce::MouseEvent& e)
{
    updateNoteUnderMouse(e, false);
}

void KeyboardComponent::timerCallback()
{
    if (noPendingUpdates.exchange(true))
        return;

    for (auto i = getRangeStart(); i <= getRangeEnd(); ++i)
    {
        const auto isOn = state.isNoteOnForChannels(midiInChannelMask, i);

        if (keysCurrentlyDrawnDown[i] != isOn)
        {
            keysCurrentlyDrawnDown.setBit(i, isOn);
            repaintNote(i);
        }
    }
}

bool KeyboardComponent::keyStateChanged(bool /*isKeyDown*/)
{
    bool keyPressUsed = false;

    for (int i = keyPresses.size(); --i >= 0;)
    {
        auto note = 12 * keyMappingOctave + keyPressNotes.getUnchecked(i);

        if (keyPresses.getReference(i).isCurrentlyDown())
        {
            if (!keysPressed[note])
            {
                keysPressed.setBit(note);
                state.noteOn(midiChannel, note, velocity);
                keyPressUsed = true;
            }
        }
        else
        {
            if (keysPressed[note])
            {
                keysPressed.clearBit(note);
                state.noteOff(midiChannel, note, 0.0f);
                keyPressUsed = true;
            }
        }
    }

    return keyPressUsed;
}

bool KeyboardComponent::keyPressed(const juce::KeyPress& key)
{
    return keyPresses.contains(key);
}

void KeyboardComponent::focusLost(FocusChangeType)
{
    resetAnyKeysInUse();
}

//==============================================================================
void KeyboardComponent::drawKeyboardBackground(juce::Graphics& g, juce::Rectangle<float> area)
{
    g.fillAll(findColour(whiteNoteColourId));

    auto width = area.getWidth();
    auto height = area.getHeight();
    auto currentOrientation = getOrientation();
    juce::Point<float> shadowGradientStart, shadowGradientEnd;

    if (currentOrientation == verticalKeyboardFacingLeft)
    {
        shadowGradientStart.x = width - 1.0f;
        shadowGradientEnd.x = width - 5.0f;
    }
    else if (currentOrientation == verticalKeyboardFacingRight)
    {
        shadowGradientEnd.x = 5.0f;
    }
    else
    {
        shadowGradientEnd.y = 5.0f;
    }

    auto keyboardWidth = getRectangleForKey(getRangeEnd()).getRight();
    auto shadowColour = findColour(shadowColourId);

    if (!shadowColour.isTransparent())
    {
        g.setGradientFill({ shadowColour, shadowGradientStart,
                                shadowColour.withAlpha(0.0f), shadowGradientEnd,
                                false });

        switch (currentOrientation)
        {
        case horizontalKeyboard:            g.fillRect(0.0f, 0.0f, keyboardWidth, 5.0f); break;
        case verticalKeyboardFacingLeft:    g.fillRect(width - 5.0f, 0.0f, 5.0f, keyboardWidth); break;
        case verticalKeyboardFacingRight:   g.fillRect(0.0f, 0.0f, 5.0f, keyboardWidth); break;
        default: break;
        }
    }

    auto lineColour = findColour(keySeparatorLineColourId);

    if (!lineColour.isTransparent())
    {
        g.setColour(lineColour);

        switch (currentOrientation)
        {
        case horizontalKeyboard:            g.fillRect(0.0f, height - 1.0f, keyboardWidth, 1.0f); break;
        case verticalKeyboardFacingLeft:    g.fillRect(0.0f, 0.0f, 1.0f, keyboardWidth); break;
        case verticalKeyboardFacingRight:   g.fillRect(width - 1.0f, 0.0f, 1.0f, keyboardWidth); break;
        default: break;
        }
    }
}

void KeyboardComponent::drawWhiteNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
    bool isDown, bool isOver, juce::Colour lineColour, juce::Colour textColour)
{
    auto c = juce::Colours::transparentWhite;

    if (isDown)  c = findColour(keyDownOverlayColourId);
    if (isOver)  c = c.overlaidWith(findColour(mouseOverKeyOverlayColourId));

    g.setColour(c);
    g.fillRect(area);

    const auto currentOrientation = getOrientation();

    auto text = getWhiteNoteText(midiNoteNumber);

    if (text.isNotEmpty())
    {
        auto fontHeight = juce::jmin(12.0f, getKeyWidth() * 0.9f);

        g.setColour(textColour);
        g.setFont(juce::Font(fontHeight).withHorizontalScale(0.8f));

        switch (currentOrientation)
        {
        case horizontalKeyboard:            g.drawText(text, area.withTrimmedLeft(1.0f).withTrimmedBottom(2.0f), juce::Justification::centredBottom, false); break;
        case verticalKeyboardFacingLeft:    g.drawText(text, area.reduced(2.0f), juce::Justification::centredLeft, false); break;
        case verticalKeyboardFacingRight:   g.drawText(text, area.reduced(2.0f), juce::Justification::centredRight, false); break;
        default: break;
        }
    }

    if (!lineColour.isTransparent())
    {
        g.setColour(lineColour);

        switch (currentOrientation)
        {
        case horizontalKeyboard:            g.fillRect(area.withWidth(1.0f)); break;
        case verticalKeyboardFacingLeft:    g.fillRect(area.withHeight(1.0f)); break;
        case verticalKeyboardFacingRight:   g.fillRect(area.removeFromBottom(1.0f)); break;
        default: break;
        }

        if (midiNoteNumber == getRangeEnd())
        {
            switch (currentOrientation)
            {
            case horizontalKeyboard:            g.fillRect(area.expanded(1.0f, 0).removeFromRight(1.0f)); break;
            case verticalKeyboardFacingLeft:    g.fillRect(area.expanded(0, 1.0f).removeFromBottom(1.0f)); break;
            case verticalKeyboardFacingRight:   g.fillRect(area.expanded(0, 1.0f).removeFromTop(1.0f)); break;
            default: break;
            }
        }
    }
}

void KeyboardComponent::drawBlackNote(int /*midiNoteNumber*/, juce::Graphics& g, juce::Rectangle<float> area,
    bool isDown, bool isOver, juce::Colour noteFillColour)
{
    auto c = noteFillColour;

    if (isDown)  c = c.overlaidWith(findColour(keyDownOverlayColourId));
    if (isOver)  c = c.overlaidWith(findColour(mouseOverKeyOverlayColourId));

    g.setColour(c);
    g.fillRect(area);

    if (isDown)
    {
        g.setColour(noteFillColour);
        g.drawRect(area);
    }
    else
    {
        g.setColour(c.brighter());
        auto sideIndent = 1.0f / 8.0f;
        auto topIndent = 7.0f / 8.0f;
        auto w = area.getWidth();
        auto h = area.getHeight();

        switch (getOrientation())
        {
        case horizontalKeyboard:            g.fillRect(area.reduced(w * sideIndent, 0).removeFromTop(h * topIndent)); break;
        case verticalKeyboardFacingLeft:    g.fillRect(area.reduced(0, h * sideIndent).removeFromRight(w * topIndent)); break;
        case verticalKeyboardFacingRight:   g.fillRect(area.reduced(0, h * sideIndent).removeFromLeft(w * topIndent)); break;
        default: break;
        }
    }
}

juce::String KeyboardComponent::getWhiteNoteText(int midiNoteNumber)
{
    if (midiNoteNumber % 12 == 0)
        return juce::MidiMessage::getMidiNoteName(midiNoteNumber, true, true, getOctaveForMiddleC());

    return {};
}

void KeyboardComponent::colourChanged()
{
    setOpaque(findColour(whiteNoteColourId).isOpaque());
    repaint();
}

//==============================================================================
void KeyboardComponent::drawWhiteKey(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area)
{
    drawWhiteNote(midiNoteNumber, g, area, state.isNoteOnForChannels(midiInChannelMask, midiNoteNumber),
        mouseOverNotes.contains(midiNoteNumber), findColour(keySeparatorLineColourId), findColour(textLabelColourId));
}

void KeyboardComponent::drawBlackKey(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area)
{
    drawBlackNote(midiNoteNumber, g, area, state.isNoteOnForChannels(midiInChannelMask, midiNoteNumber),
        mouseOverNotes.contains(midiNoteNumber), findColour(blackNoteColourId));
}

//==============================================================================
void KeyboardComponent::handleNoteOn(juce::MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/)
{
    noPendingUpdates.store(false);
}

void KeyboardComponent::handleNoteOff(juce::MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/)
{
    noPendingUpdates.store(false);
}

juce::Range<float> KeyboardComponent::getKeyPosition(int midiNoteNumber, float targetKeyWidth) const
{
    jassert(midiNoteNumber >= 0 && midiNoteNumber < 128);

    static const float notePos[] = { 0.0f, 1,
                                        1.5f, 3,
                                        3.5f,
                                        5.0f, 6,
                                        6.5f, 8,
                                        8.5f, 10,
                                        10.5f };

    auto octave = midiNoteNumber / 12;
    auto note = midiNoteNumber % 12;

    auto start = octave * 12.0f * targetKeyWidth + notePos[note] * targetKeyWidth;
    auto width = juce::MidiMessage::isMidiNoteBlack(note) ? getBlackNoteWidthProportion() * targetKeyWidth : targetKeyWidth;

    return { start, start + width };
}

juce::Rectangle<float> KeyboardComponent::getRectangleForKey(int note) const
{
    jassert(note >= getRangeStart() && note <= getRangeEnd());

    auto pos = getKeyPosRevised(note);
    auto x = pos.getStart();
    auto w = pos.getLength();

    if (juce::MidiMessage::isMidiNoteBlack(note))
    {
        auto blackNoteLength = getBlackNoteLength();

        switch (getOrientation())
        {
        case horizontalKeyboard:            return { x, 0, w, blackNoteLength };
        case verticalKeyboardFacingLeft:    return { getWidth() - blackNoteLength, x, blackNoteLength, w };
        case verticalKeyboardFacingRight:   return { 0, getHeight() - x - w, blackNoteLength, w };
        default:                            jassertfalse; break;
        }
    }
    else
    {
        w *= getKeyWidthModifier(note);
        switch (getOrientation())
        {
        case horizontalKeyboard:            return { x, 0, w, (float)getHeight() };
        case verticalKeyboardFacingLeft:    return { 0, x, (float)getWidth(), w };
        case verticalKeyboardFacingRight:   return { 0, getHeight() - x - w, (float)getWidth(), w };
        default:                            jassertfalse; break;
        }
    }

    return {};
}

float KeyboardComponent::getKeyWidthModifier(int midiNoteNumber) const
{
    auto name = juce::MidiMessage::getMidiNoteName(midiNoteNumber, true, false, 3);

    if (name == "C" || name == "E" || name == "F" || name == "B")
        return 1.5f;
    if (name == "D" || name == "G" || name == "A")
        return 2;

    return 1;
}
 
juce::Range<float> KeyboardComponent::getKeyPosRevised(int midiNoteNumber) const
{
    return getKeyPosition(midiNoteNumber, keyWidth)
        - getLowestVisibleKey()
        - getKeyPosition(getRangeStart(), keyWidth).getStart();
}

void KeyboardComponent::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&)
{
    return;
}