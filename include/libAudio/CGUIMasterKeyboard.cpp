//
// Filename:      CGUIMasterKeyboard.cpp
// Programmer:    Benjamin Hampe <benjaminhampe@gmx.de>
// Creation Date: Do Feb 15 12:29:45 PST 2012
// Last Modified: Fr Feb 16 15:14:04 PST 2012
// URL:
// Syntax:        GNU C/C++0x
//
// Description:   The example program shows how to open MIDI output,
//                send a MIDI message, and close MIDI output using the
//                PortMidi C library instead of accessing.
//

#include "CGUIMasterKeyboard.h"

#ifdef _IRR_COMPILE_WITH_PORTMIDI_

namespace irr {

CGUIMasterKeyboard::CGUIMasterKeyboard(
	IrrlichtDevice* irrDevice,
	const core::recti& irrRect,
	const video::SColor& irrColor,
	bool new_thread)
: gui::IGUIElement( gui::EGUIET_ELEMENT, irrDevice->getGUIEnvironment(), irrDevice->getGUIEnvironment()->getRootGUIElement(), -1, irrRect )
, Device(irrDevice), FgColor(255,0,0,0), BgColor(irrColor), Texture(0)
{
	if (Device)
		Device->getLogger()->log(L"CGUIMasterKeyboard::ctr()", ELL_INFORMATION);

	createWindow();
	createControls();
	createKeys();
	openMidi();
}

CGUIMasterKeyboard::~CGUIMasterKeyboard()
{
	if (Device)
		Device->getLogger()->log(L"CGUIMasterKeyboard::dtr()", ELL_INFORMATION);
	closeMidi();
}

void CGUIMasterKeyboard::createWindow()
{
	if (!Device)
		return;

	Device->getLogger()->log(L"CGUIMasterKeyboard::createWindow()", ELL_INFORMATION);

	gui::IGUIEnvironment* env = Device->getGUIEnvironment();
	video::IVideoDriver* Driver = Device->getVideoDriver();

	Title = L"CGUIMasterKeyboard (2012) by benjaminhampe@gmx.de | M-Audio KeyStation Pro 88 style";

//	this = env->addWindow(WinRect, false, Title.c_str(), env->getRootGUIElement(), -1);
//	if (!this)
//	{
//		Device->getLogger()->log(L"no window.", ELL_ERROR);
//		return;
//	}

	Texture = Driver->getTexture("../media/keystation88pro.jpg");
}

void CGUIMasterKeyboard::createKeys()
{
	if (!Device)
		return;

	Device->getLogger()->log(L"CGUIMasterKeyboard::createKeys()", ELL_INFORMATION);

//	if (!this)
//	{
//		Device->getLogger()->log(L"no window.", ELL_ERROR);
//		return;
//	}

	gui::IGUIEnvironment* env = Device->getGUIEnvironment();
	if (!env)
	{
		Device->getLogger()->log(L"no gui pointer.", ELL_ERROR);
		return;
	}

	// create Black & White Texture for PianoKeys
	video::IImage* imgBlack = Device->getVideoDriver()->createImage(video::ECF_A8R8G8B8, core::dimension2du(4,4));
	video::IImage* imgWhite = Device->getVideoDriver()->createImage(video::ECF_A8R8G8B8, core::dimension2du(4,4));
	imgBlack->fill(0xFF000000);
	imgWhite->fill(0xFFFFFFFF);
	video::ITexture* texBlack = Device->getVideoDriver()->addTexture("blackTex", imgBlack, 0);
	video::ITexture* texWhite = Device->getVideoDriver()->addTexture("whiteTex", imgWhite, 0);
	imgBlack->drop();
	imgWhite->drop();


	// create Piano Keys
	const core::recti& ClientRect = AbsoluteRect;
	s32 kx = ClientRect.UpperLeftCorner.X;
	s32 ky = ClientRect.UpperLeftCorner.Y + ClientRect.getHeight()/2;
	u32 white_key_w = (u32)((f32)ClientRect.getWidth()/(128.0f-fmodf(128.0f,12.0f)*5.0f)); // width / white_key_count
	u32 white_key_h = ClientRect.getHeight()/3;
	u32 black_key_w = white_key_w;
	u32 black_key_h = white_key_h/2;

	u32 white_keys=0;

	s32 kx1, ky1, kx2, ky2;

//		env->getSkin()->setColor(gui::EGDC_3D_SHADOW, 0xFF000000);
//		env->getSkin()->setColor(gui::EGDC_3D_DARK_SHADOW, 0xFF000000);
//		env->getSkin()->setColor(gui::EGDC_BUTTON_TEXT, 0xFF0000FF);
//		env->getSkin()->setColor(gui::EGDC_3D_LIGHT, 0xFFFF0000);

	Keys.reallocate(128);
	Keys.set_used(0);
	for (u32 i = 0; i<128; i++)
	{
		KeyboardKey key;
		key.Name = GetName(i, true);
		key.MidiNote = i;
		int Note = i%12;
		key.Octave = (i - Note)/12;
		key.BindKey = 65536;

		bool black = true;

		// if C,D,E,F,G,A,H
		if (Note==0 || Note==2 || Note==4 || Note==5 || Note==7 || Note==9 || Note==11)
			black = false;

		if (black)
		{
			kx1=kx+white_key_w*white_keys-black_key_w/2;
			ky1=ky;
			kx2=kx+white_key_w*white_keys+black_key_w/2;
			ky2=ky+black_key_h;
			key.Button = env->addButton(
				core::recti(kx1, ky1, kx2, ky2), this, -1,
				L"",key.Name.c_str());

			key.Button->setScaleImage(true);
			key.Button->setImage(texBlack);
			key.Button->setDrawBorder(true);

			key.Label = env->addStaticText(key.Name.c_str(),
				core::recti(kx1, ky2-20, kx2+5, ky2), false, false, this, -1);

			key.Label->setOverrideColor(0xFFFFFF00);
		}
		else
		{
			kx1=kx+(white_key_w)*white_keys;
			ky1=ky;
			kx2=kx+white_key_w*(white_keys+1)-1;
			ky2=ky+white_key_h;

			key.Button = env->addButton(
				core::recti(kx1, ky1, kx2, ky2), this, -1,
				L"",key.Name.c_str());

			key.Button->setScaleImage(true);
			key.Button->setImage(texWhite);
			key.Button->setDrawBorder(true);

			key.Label = env->addStaticText(key.Name.c_str(),
				core::recti(kx1, ky2-20, kx2+5, ky2), false, false, this, -1);

			key.Label->setOverrideColor(0xFF000000);

			white_keys++;
		}

		Keys.push_back(key);
	}

}
void CGUIMasterKeyboard::createControls()
{
	if (!Device)
		return;

	Device->getLogger()->log(L"CGUIMasterKeyboard::createControls()", ELL_INFORMATION);

//	if (!this)
//	{
//		Device->getLogger()->log(L"no window.", ELL_ERROR);
//		return;
//	}

	gui::IGUIEnvironment* env = Device->getGUIEnvironment();
	if (!env)
	{
		Device->getLogger()->log(L"no gui pointer.", ELL_ERROR);
		return;
	}

	const core::recti& ClientRect = AbsoluteRect;
	s32 x1=ClientRect.UpperLeftCorner.X;
	s32 y1=ClientRect.UpperLeftCorner.Y;
	s32 x2=x1+ClientRect.getWidth()/4;
	s32 x3=x2+ClientRect.getWidth()/4;
	s32 x4=x3+ClientRect.getWidth()/4;
	s32 x5=x4+ClientRect.getWidth()/4;
	s32 h1=ClientRect.getHeight()/2-5;

	PowerButton = env->addCheckBox(false,
		core::recti(x1,y1,x2,y1+25), this, -1, L"Power OFF");

	PanicButton = env->addButton(
		core::recti(x1,y1+25,x2,y1+50), this, -1, L"Panic", L"Mute All Notes now!!!");

	MidiInBox = env->addComboBox(core::recti(x3,y1,x4,y1+40), this, -1);
	MidiOutBox = env->addComboBox(core::recti(x3,y1+40,x4,y1+80), this, -1);

	u32 x;
	u32 y;
	u32 s=static_cast<u32>(static_cast<s32>((f32)ClientRect.getWidth()/37.0f));

	// ModWheels
	x=0;
	y=0;
	ModWheel = new gui::CGUIMAudioPitchWheel(env, this, -1, core::recti(1,1,s,4*s) + core::position2di(x,y));
	PitchWheel = new gui::CGUIMAudioPitchWheel(env, this, -1, core::recti(1,1,s,4*s) + core::position2di(x+s,y));

	// Play/Record Buttons
	x=2*s;
	y=0;
	u32 d = 4*s/5;
	for (u32 c=0; c<5; c++)
	{
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,d,d) + core::position2di(x,y+c*d));
	}

	// Main Control Buttons
	x=3*s;
	y=s;
	for (u32 c=0; c<9; c++)
	{
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y));
	}

	for (u32 c=0; c<9; c++)
	{
		if ((c!=1) && (c!=6))
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+s));
	}

	// LCD Display
	x=12*s;
	y=0;
	new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,6*s,4*s) + core::position2di(x,y));

	// Number Buttons --> Program Change/Bank Select etc...
	x=18*s;
	y=0;
	for (u32 c=0; c<3; c++)
	{
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y));
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+s));
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+2*s));
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+3*s));
	}

	// 8* (3 KnobSlider + 1 Button)
	x=21*s;
	y=0;
	for (u32 c=0; c<8; c++)
	{
		new gui::CGUIMAudioKnobSlider(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+0*s));
		new gui::CGUIMAudioKnobSlider(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+1*s));
		new gui::CGUIMAudioKnobSlider(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+2*s));
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+3*s));
	}

	// 9* (1 Slider + 1 Button)
	x=29*s;
	y=0;
	for (u32 c=0; c<8; c++)
	{
		new gui::CGUIMAudioSlider(env, this, -1, core::recti(1,1,s,3*s) + core::position2di(x+c*s,y+0*s));
		new gui::CGUIMAudioButton(env, this, -1, core::recti(1,1,s,s) + core::position2di(x+c*s,y+3*s));
	}
}

void CGUIMasterKeyboard::openMidi()
{
	if (!Device)
		return;

	Device->getLogger()->log(L"CGUIMasterKeyboard::openMidi()");
//	int midiport = 128;        // select which MIDI output port to open
//	int velocity = 100;  // MIDI note velocity parameter value

	// MIDI Initialize
	core::stringc txt;
#ifdef _IRR_COMPILE_WITH_PORTMIDI_

	PmError error = Pm_Initialize();
	if (error)
	{
		txt = "Pm_Initialize(): error ";
		txt += error;
		txt += ", ";
		txt += Pm_GetErrorText(error);

	}
	else
	{
		txt = "Pm_Initialize(): done";
	}

	Device->getLogger()->log(txt.c_str(), ELL_ERROR);

	// MIDI Device Count
	int MidiDeviceCount = Pm_CountDevices();
	txt = "Pm_CountDevices():";	txt += MidiDeviceCount;
	Device->getLogger()->log(txt.c_str(), ELL_INFORMATION);

	// MIDI Device Infos
	int MidiInputDeviceCount = 0;
	int MidiOutputDeviceCount = 0;

	for (int i=0; i<MidiDeviceCount; ++i)
	{
		const PmDeviceInfo& info = *Pm_GetDeviceInfo(i);

		if (info.input>0)
		{
			txt="MidiInputDevice [";
			txt+=MidiInputDeviceCount;
			txt+="], Name=";
			txt+=info.name;
			txt+=", Interface=";
			txt+=info.interf;
			txt+=", Open=";
			txt+=info.opened;

			if (MidiInBox)
				MidiInBox->addItem(core::stringw(txt.c_str()).c_str(),i);

			MidiInputDeviceCount++;
		}
		else
		{
			txt="MidiOutputDevice [";
			txt+=MidiOutputDeviceCount;
			txt+="], Name=";
			txt+=info.name;
			txt+=", Interface=";
			txt+=info.interf;
			txt+=", Open=";
			txt+=info.opened;

			if (MidiOutBox)
				MidiOutBox->addItem(core::stringw(txt.c_str()).c_str(),i);

			MidiOutputDeviceCount++;
		}
	}

	if (MidiInBox)
	MidiInBox->setSelected(-1);
	if (MidiOutBox)
	MidiOutBox->setSelected(-1);

	// portmidi Default Connectors
	DefaultMidiOutputDeviceID = Pm_GetDefaultOutputDeviceID();
	DefaultMidiInputDeviceID = Pm_GetDefaultInputDeviceID();

	// portmidi Open OutputStream
//		error = Pm_OpenOutput(&MidiOutputStream, 2, NULL, 1, NULL, NULL, 0);
//		if (error)
//		{
//			txt = "Pm_OpenOutput(): error ";
//			txt += error;
//			txt += ", ";
//			txt += Pm_GetErrorText(error);
//			pDevice->getLogger()->log(txt.c_str(), ELL_ERROR);
//		}
//		else
//		{
//			pDevice->getLogger()->log("Pm_OpenOutput(): done", ELL_NONE);
//		}
//
//		error = Pm_WriteShort(MidiOutputStream, 0, Pm_Message(0xC0, 0x00, 0x00)); // Program Change auf Kanal 0, Instrument 5 (Electric Piano 1)
//		if (error)
//		{
//			txt = "Pm_WriteShort(): error ";
//			txt += error;
//			txt += ", ";
//			txt += Pm_GetErrorText(error);
//			pDevice->getLogger()->log(txt.c_str(), ELL_ERROR);
//		}
//		else
//		{
//			pDevice->getLogger()->log("Pm_WriteShort(): done", ELL_NONE);
//		}
#endif // _IRR_COMPILE_WITH_PORTMIDI_
}

void CGUIMasterKeyboard::closeMidi()
{
	core::stringc txt;

#ifdef _IRR_COMPILE_WITH_PORTMIDI_
	PmError error;

	// All Notes Off
//	error = Pm_WriteShort(MidiOutputStream, 0, Pm_Message(0x7B, 0x00, 0x00));
//	if (error)
//	{
//		txt = "Pm_WriteShort(MidiOutputStream, 0, Pm_Message(0x7B, 0x00, 0x00)): error ";
//		txt += error;
//		txt += ", ";
//		txt += Pm_GetErrorText(error);
//		Device->getLogger()->log(txt.c_str(), ELL_ERROR);
//	}
//	else
//	{
//		Device->getLogger()->log("Pm_WriteShort(): done", ELL_NONE);
//	}

	// Reset Midi Device

	error = Pm_WriteShort(MidiOutputStream, 0, Pm_Message(0xFF, 0x00, 0x00));
	if (error)
	{	if (Device)
		{	txt = "Pm_WriteShort(MidiOutputStream, 0, Pm_Message(0xFF, 0x00, 0x00)): error ";
			txt += error;
			txt += ", ";
			txt += Pm_GetErrorText(error);
			Device->getLogger()->log(txt.c_str(), ELL_ERROR);
		}
	}
	else
	{	if (Device)
			Device->getLogger()->log("Pm_WriteShort(): done", ELL_NONE);
	}


	// portmidi Close OutputStream
	error = Pm_Close(MidiOutputStream);
	if (error)
	{	if (Device)
		{
			txt = "Pm_Close(): error ";
			txt += error;
			txt += ", ";
			txt += Pm_GetErrorText(error);
			Device->getLogger()->log(txt.c_str(), ELL_ERROR);
		}
	}
	else
	{	if (Device)
			Device->getLogger()->log("Pm_Close(): done", ELL_NONE);
	}

	// portmidi Terminate
	error = Pm_Terminate();
	if (error)
	{	if (Device)
		{	txt = "Pm_Terminate(): error ";
			txt += error;
			txt += ", ";
			txt += Pm_GetErrorText(error);
			Device->getLogger()->log(txt.c_str(), ELL_ERROR);
		}
	}
	else
	{
		if (Device)
			Device->getLogger()->log("Pm_Terminate(): done", ELL_NONE);
	}
#endif // _IRR_COMPILE_WITH_PORTMIDI_
}

bool CGUIMasterKeyboard::OnEvent(const SEvent& event)
{
//	if (event.EventType == EET_GUI_EVENT)
//	{
//		if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED)
//		{
//			gui::IGUIElement* caller = event.GUIEvent.Caller;
//
//			if (caller == PanicButton)
//			{
//				Panic();
//			}
//
//			s32 found = -1;
//
//			for (u32 i=0; i<Keys.size(); ++i)
//			{
//				if (caller == (gui::IGUIElement*)Keys[i].Button)
//				{
//					found = i;
//					break;
//				}
//			}
//
//			if (found != -1)
//			{
#ifdef _IRR_COMPILE_WITH_PORTMIDI_
//				if (MidiOutputStream)
//				{
//					PmEvent event;
//					event.message = Pm_Message(0x90, Keys[found].MidiNote, 0x7F);
//					if (Device)
//						event.timestamp = 0; //Device->getTimer()->getRealTime();
//					else
//						event.timestamp = 0;
//					Pm_Write(MidiOutputStream, &event, 1);
//
#endif // _IRR_COMPILE_WITH_PORTMIDI_
//					// Log
//					core::stringw txt(L"Play MidiNote ");
//					txt+=Keys[found].MidiNote;
//					txt+=L", ";
//					txt+=Keys[found].Name;
//					Device->getLogger()->log(txt.c_str(), ELL_INFORMATION);
//				}
//				else
//				{
//					Device->getLogger()->log(L"No valid OutputStream", ELL_ERROR);
//				}
//			}
//			else
//			{
//				Device->getLogger()->log(L"Caller Pointer not found", ELL_ERROR);
//			}
//		}
//		return false;
//	}
//
//	if (event.EventType == EET_KEY_INPUT_EVENT)
//	{
//		return false;
//	}
//
//	if (event.EventType == EET_MOUSE_INPUT_EVENT)
//	{
//		return false;
//	}
//
//	if (event.EventType == EET_LOG_TEXT_EVENT)
//	{
////		if (LogBox)
////		{
////			u32 id = LogBox->addItem(core::stringw(event.LogEvent.Text).c_str());
////
////			video::SColor color;
////
////			switch (event.LogEvent.Level)
////			{
////				case ELL_INFORMATION: color = 0xFFFFFFFF; break;
////				case ELL_NONE: color = 0xFF00BF00; break;
////				case ELL_WARNING: color = 0xFFFFFF00; break;
////				case ELL_ERROR: color = 0xFFFF0000; break;
////#if (IRRLICHT_VERSION_MINOR>7)
////				case ELL_DEBUG: color = 0xFFFF00FF; break;
////#endif
////				default: break;
////			}
////			LogBox->setItemOverrideColor(id, color);
////			LogBox->setSelected(id);
////		}
//		return false;
//	}

	return false;
}

void CGUIMasterKeyboard::Panic()
{
#ifdef _IRR_COMPILE_WITH_PORTMIDI_
	PmError error = Pm_WriteShort(MidiOutputStream, 0, Pm_Message(0xB0, 123, 0x00));
#endif // _IRR_COMPILE_WITH_PORTMIDI_
}

core::stringc CGUIMasterKeyboard::GetName(int midiNote, bool major)
{
	core::stringc name;

	int note = midiNote%12; // RestDivision, Rest ist note innerhalb der Octave
	int octave = (midiNote-note)/12; // (Start-Rest)/12 ist exakte Oktave

	switch (note)
	{
		case 0: name+="C"; break;
		case 1: if (major) name+="C#"; else name+="Des"; break;
		case 2: name+="D"; break;
		case 3: if (major) name+="D#"; else name+="Es"; break;
		case 4: name+="E"; break;
		case 5: name+="F"; break;
		case 6: if (major) name+="F#"; else name+="Ges"; break;
		case 7: name+="G"; break;
		case 8: if (major) name+="G#"; else name+="Aes"; break;
		case 9: name+="A"; break;
		case 10: if (major) name+="A#"; else name+="B"; break;
		case 11: name+="H"; break;
		default: name+="?"; break;
	}

	switch (octave)
	{
		case 0: name+="-1"; break;
		case 1: name+="0"; break;
		case 2: name+="1"; break;
		case 3: name+="2"; break;
		case 4: name+="3"; break;
		case 5: name+="4"; break;
		case 6: name+="5"; break;
		case 7: name+="6"; break;
		case 8: name+="7"; break;
		case 9: name+="8"; break;
		case 10: name+="9"; break;
		case 11: name+="10"; break;
		default: name+="?"; break;
	}

	return name;
}

//	while (pDevice->run())
//	{
//
//		if (kbhit())
//		{
//			key = dbInkey();
//
//			switch(key)
//			{
//				case 27: pDevice->closeDevice(); break;
//				case 'c': notekey = 48; break;
//				case 'C': notekey = 60; break;
////				case 'c#': notekey = 49; break;
////				case 'C#': notekey = 61; break;
//				case 'd': notekey = 50; break;
//				case 'D': notekey = 62; break;
////				case 'd#': notekey = 51; break;
////				case 'D#': notekey = 63; break;
//				case 'e': notekey = 52; break;
//				case 'E': notekey = 64; break;
//				case 'f': notekey = 53; break;
//				case 'F': notekey = 65; break;
////				case 'f': notekey = 54; break;
////				case 'F': notekey = 66; break;
//				case 'g': notekey = 55; break;
//				case 'G': notekey = 67; break;
////				case 'g': notekey = 56; break;
////				case 'G': notekey = 68; break;
//				case 'a': notekey = 57; break; // A 440 Hz
//				case 'A': notekey = 69; break; // A 880 Hz
//				case 'b': notekey = 58; break; //
//				case 'B': notekey = 70; break; //
//				case 'h': notekey = 59; break; //
//				case 'H': notekey = 71; break; //
//				default: notekey=key; break;
//			}
//
//			std::cout<<"Keyboard: "<<key<<" "<<Midi_GetNoteName(notekey, true).c_str()<<std::endl;
//
//			PmEvent event;
//			event.message = Pm_Message(0x90, notekey, 0x7F);
//			event.timestamp = pTimer->getRealTime();
//			Pm_Write(midiOut, &event, 1);
//
////			event;
////			event.message = Pm_Message(0x80, notekey, 0x7F);
////			event.timestamp = pTimer->getRealTime()+3000;
////			Pm_Write(midiOut, &event, 1);
//
//			pDevice->sleep(100);
//		}
//	}
//
//	std::cout << "END MAIN LOOP"<<std::endl;
//
////	error = Pm_WriteShort(midiOut, 0, Pm_Message(0x7B, 0x00, 0x00)); // All Notes Off
////	if (error)
////		std::cout<<"Pm_WriteShort(): error "<<error<<", "<<Pm_GetErrorText(error)<<std::endl;
////	else
////		std::cout<<"Pm_WriteShort(): done"<<std::endl;
//
//	error = Pm_WriteShort(midiOut, 0, Pm_Message(0xFF, 0x00, 0x00)); // Reset Midi Device
//	if (error)
//		std::cout<<"Pm_WriteShort(): error "<<error<<", "<<Pm_GetErrorText(error)<<std::endl;
//	else
//		std::cout<<"Pm_WriteShort(): done"<<std::endl;
//
//	close_keyboard();
//
//	std::cout << "close Irrlicht-Device" << std::endl;
//
//	pDevice->drop();
//
////	PmEvent event;
////	event.message = PmMessage(0xff, 0, 0);	event.timestamp = 0; Pm_Write(midiOut, &event, 1); // reset
////	event.message = PmMessage(0x90, 0x60, 0x7F); event.timestamp = 1000; Pm_Write(midiOut, &event, 1); // Note ON
////	event.message = PmMessage(0x90, 0x63, 0x7F); event.timestamp = 2000; Pm_Write(midiOut, &event, 1); // Note ON
////	event.message = PmMessage(0x90, 0x67, 0x7F); event.timestamp = 3000; Pm_Write(midiOut, &event, 1); // Note ON
////	event.message = PmMessage(0x80, 0x60, 0x7F); event.timestamp = 3000; Pm_Write(midiOut, &event, 1); // Note OFF
////	event.message = PmMessage(0x80, 0x63, 0x7F); event.timestamp = 4000; Pm_Write(midiOut, &event, 1); // Note OFF
////	event.message = PmMessage(0x80, 0x67, 0x7F); event.timestamp = 5000; Pm_Write(midiOut, &event, 1); // Note OFF
//
//	// portmidi Note OFF
//	error = Pm_Close(midiOut);
//	if (error)
//		std::cout<<"Pm_Close(): error "<<error<<", "<<Pm_GetErrorText(error)<<std::endl;
//	else
//		std::cout<<"Pm_Close(): done"<<std::endl;
//
//	// portmidi Device Infos
//	for (int i=0; i<deviceCount; ++i)
//	{
//		const PmDeviceInfo& info = *Pm_GetDeviceInfo(i);
//		std::cout << "PmDeviceInfo ("<<i<<"): " << ((info.input>0)?"IN ":"OUT") << " name="<<info.name<<" interf="<<info.interf;
//		std::cout << " opened="<<info.opened<<" ver="<<info.structVersion<<std::endl;
//	}
//
//	// portmidi END
//	error = Pm_Terminate();
//	if (error)
//		std::cout<<"Pm_Terminate(): error "<<error<<", "<<Pm_GetErrorText(error)<<std::endl;
//	else
//		std::cout<<"Pm_Terminate(): done"<<std::endl;
//
//   return 0;
//}
//
//void init_keyboard()
//{
//    tcgetattr(0,&initial_settings);
//    new_settings = initial_settings;
//    new_settings.c_lflag &= ~ICANON;
//    new_settings.c_lflag &= ~ECHO;
//    new_settings.c_lflag &= ~ISIG;
//    new_settings.c_cc[VMIN] = 1;
//    new_settings.c_cc[VTIME] = 0;
//    tcsetattr(0, TCSANOW, &new_settings);
//}
//
//void close_keyboard()
//{
//    tcsetattr(0, TCSANOW, &initial_settings);
//}
//
//int kbhit()
//{
//    char ch;
//    int nread;
//
//    if(peek_character != -1)
//        return 1;
//    new_settings.c_cc[VMIN]=0;
//    tcsetattr(0, TCSANOW, &new_settings);
//    nread = read(0,&ch,1);
//    new_settings.c_cc[VMIN]=1;
//    tcsetattr(0, TCSANOW, &new_settings);
//
//    if(nread == 1) {
//        peek_character = ch;
//        return 1;
//    }
//    return 0;
//}
//
//int dbInkey()
//{
//    char ch;
//
//    if(peek_character != -1) {
//        ch = peek_character;
//        peek_character = -1;
//        return ch;
//    }
//    read(0,&ch,1);
//    return ch;
//}

} // end namespace irr

#endif
