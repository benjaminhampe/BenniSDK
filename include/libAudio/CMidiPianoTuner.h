// Copyright (C) 2008-2012 Benjamin Hampe
// This file uses parts of the "Irrlicht Engine" and PortMidi Libraries.
// For conditions of distribution and use, see copyright notice in common.h

#ifndef __C_MIDI_PIANO_TUNER_H_INCLUDED__
#define __C_MIDI_PIANO_TUNER_H_INCLUDED__

#include <irrlicht.h>

	class CMidiPianoTuner : public irr::gui::IGUIElement
	{
	public:

		//! constructor
		CMidiPianoTuner(irr::gui::IGUIEnvironment* environment, irr::gui::IGUIElement* parent, irr::s32 id, irr::core::rect<irr::s32> rectangle);

		//! called if an event happened.
		virtual bool OnEvent(const irr::SEvent& event);

		//! draws the element and its children
		virtual void draw();

		//! Updates the absolute position.
		virtual void updateAbsolutePosition();

	private:
		irr::gui::IGUIButton* ToneUpButton;
		irr::gui::IGUIButton* ToneDownButton;

		irr::gui::IGUIStaticText* Text;
	};

#endif
