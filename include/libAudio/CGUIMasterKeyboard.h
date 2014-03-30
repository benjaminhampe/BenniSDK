//
// Filename:      CGUIMasterKeyboard.h
// Programmer:    Benjamin Hampe <benjaminhampe@gmx.de>
// Creation Date: Do Feb 15 12:29:45 PST 2012
// Last Modified: Fr Feb 16 15:14:04 PST 2012
// URL:
// Syntax:        GNU C/C++
//
// Description:   The example program shows how to open MIDI output,
//                send a MIDI message, and close MIDI output using the
//                PortMidi C library instead of accessing.
//
#ifndef __C_MIDI_MASTER_KEYBOARD_M_AUDIO_KEYSTATION_88_PRO_H__
#define __C_MIDI_MASTER_KEYBOARD_M_AUDIO_KEYSTATION_88_PRO_H__

#include "../CompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PORTMIDI_
//#include "CGUIMAudioSlider.h"
//#include "CGUIMAudioButton.h"
//#include "CGUIMAudioModWheel.h"
//#include "CGUIMAudioPitchWheel.h"
//#include "CGUIMAudioKnobSlider.h"
//#include "CGUIMAudioLCD.h"
//#include "CGUIMAudioLED.h"
//#include "CGUIMAudioPushButton.h"
//#include "CGUIMAudioPianoKey.h"

namespace irr
{

class CGUIMasterKeyboard : public gui::IGUIElement
{

private:
	IrrlichtDevice* 	Device;
	core::stringw 		Title;
	video::ITexture* 	Texture;
	video::SColor	 	FgColor;
	video::SColor	 	BgColor;

	gui::IGUICheckBox* 	PowerButton;
	gui::IGUIButton* 	PanicButton;


	struct KeyboardKey
	{
		core::stringw Name;
		int MidiNote;
		int BindKey;
		int Octave;
		gui::IGUIButton* Button;
		gui::IGUIStaticText* Label;
	};

	/// GUI Elements
	core::array<KeyboardKey> Keys;

	gui::IGUIComboBox* MidiInBox;
	gui::IGUIComboBox* MidiOutBox;

	core::array<gui::IGUIButton*> Buttons;
	core::array<gui::IGUIButton*> PushButtons;
	core::array<gui::IGUIScrollBar*> Fader;
	core::array<gui::IGUIScrollBar*> Knobs;
	core::array<gui::IGUIStaticText*> LCD;
	core::array<gui::IGUIButton*> Lamps;
	core::array<gui::IGUIStaticText*> Texts;

	gui::IGUIScrollBar* SustainPedal;
	gui::IGUIScrollBar* SoftPedal;
	gui::IGUIScrollBar* PitchWheel;
	gui::IGUIScrollBar*	ModWheel;

	s32 CurrentMidiChannel;
	s32 CurrentMidiProgram;
	s32 MasterVolume;
	s32 MasterPan;

#ifdef _IRR_COMPILE_WITH_PORTMIDI_
	PmDeviceID 											DefaultMidiInputDeviceID;
	PmDeviceID 											DefaultMidiOutputDeviceID;
	PortMidiStream* MidiOutputStream;
#endif // _IRR_COMPILE_WITH_PORTMIDI_

private:
	void createWindow();
	void createControls();
	void createKeys();
	void openMidi();
	void closeMidi();

public:
	CGUIMasterKeyboard(
		IrrlichtDevice* irrDevice,
		const core::recti& irrRect,
		const video::SColor& irrColor,
		bool new_thread);

	~CGUIMasterKeyboard();

	virtual bool OnEvent(const SEvent& event);

	void Panic();

	static core::stringc GetName(int midiNote, bool major);

};

} // end namespace irr

//Regler (MSB):
//
// 0  Bank Select MSB
// 1  Modulation
// 2  Breath Controller
// 3  nicht definiert
// 4  Foot Pedal
// 5  Portamento Time
// 6  DataEntry MSB
// 7  Volume
// 8  Balance
// 9  nicht definiert
// 10  Panpot
// 11  Expression
// 12-15 nicht definiert
// 16  General Purpose 1
// 17  General Purpose 2
// 18  General Purpose 3
// 19  General Purpose 4
// 20 - 31 nicht definiert	Regler (LSB):
//
// 32  Bank Select LSB
// 33  Modulation LSB
// 34  Breath Controller LSB
// 35  LSB
// 36  Foot Pedal LSB
// 37  Portamento Time LSB
// 38  DataEntry LSB
// 39  Volume LSB
// 40  Balance LSB
// 41  LSB
// 42  Panpot LSB
// 43  Expression LSB
// 44 - 47 nichtdefiniert LSB
// 48  General Purpose 1 LSB
// 49  General Purpose 2 LSB
// 50  General Purpose 3 LSB
// 51  General Purpose 4 LSB
// 52-63 nicht definiert LSB
//
//
//Schalter:
//
// 64  Sustain Pedal (Damper)
// 65  Portamento
// 66  Sustenuto
// 67  Soft Pedal
// 68  nicht definiert
// 69  Hold 2
// 70 nicht definiert
// 71 Harmonic Content (XG)
// 72  Release Time (XG)
// 73 Attack Time (XG)
// 74 Brightness (XG)
// 75 - 79 nicht definiert
// 80  General Purp 5
// 81  General Purp 6
// 82  General Purp 7
// 83  General Purp 8
// 84 - 90 nicht definiert
// 91  External Effects Depth
// 92  Tremolo Depth
// 93  Chorus Depth
// 94  Celeste Depth
// 95  Phaser Depth
// 96  Data Increment
// 97  Data Decrement
// 98  Non Registered Parameter LSB
// 99  Non Registered Parameter MSB
//100  Registered Parameter LSB
//101  Registered Parameter MSB
//102 - 120 nicht definiert
//
//Channel Mode Messages:
//
//121  Reset All Controllers
//122  Local Control Off
//123  All Notes Off
//124  Omni Mode Off
//125  Omni Mode On
//126  Mono Mode On = Poly Mode Off
//127  Poly Mode On = Mono Mode Off
#endif // _IRR_COMPILE_WITH_PORTMIDI_

#endif
