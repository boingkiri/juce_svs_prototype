/*
  ==============================================================================

    KeyboardComponent.cpp
    Created: 27 May 2022 3:43:40pm
    Author:  vdjfelrl11

  ==============================================================================
*/

#include "KeyboardComponent.h"

KeyboardComponent::KeyboardComponent(
    juce::MidiKeyboardState& state,
    juce::KeyboardComponentBase::Orientation orientation):
    MidiKeyboardComponent(state, orientation)
{
    
}

KeyboardComponent::~KeyboardComponent()
{
}


void KeyboardComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    juce::Component* parentComponent = getParentComponent();
    if (parentComponent == nullptr)
        return;

    parentComponent->mouseWheelMove(event, wheel);
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

	auto pos = getKeyPos(note);
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