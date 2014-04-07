//// Copyright (C) 2002-2014 Benjamin Hampe
//// This file is part of the "irrlicht-engine"
//// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIAudioPlayer.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIAudioPlayer::CGUIAudioPlayer( IAudioPlayer* player,
	IGUIEnvironment* env, IGUIElement* parent, s32 id, core::rect<s32> rectangle )
: IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle)
, Font(0), Player(player)
{
	setName("CGUIAudioPlayer");
	setTabStop(true);
	setTabOrder(-1);

	const s32 w = AbsoluteRect.getWidth();
	const s32 h = AbsoluteRect.getHeight();
    const s32 Border = 2;

	video::SColor fgColor(255,255,255,255);
	video::SColor bgColor(200,0,0,0);

	video::IVideoDriver* driver = env->getVideoDriver();
	Font = env->getBuiltInFont();
	gui::IGUIStaticText* label;

	/// Buttons
	s32 dx,dy,x,y;

	/// Buttons
	dy = 24;
	x = Border;
	y = Border;

	// load
	dx = 48;
	LoadButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Load");
	x += dx;

	// close
	dx = 24;
	CloseButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"X");
	x += dx;

	// track-name
	dx = 10*24;
	TrackName = env->addStaticText( L"TrackName", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	TrackName->setTextAlignment( EGUIA_UPPERLEFT, EGUIA_CENTER );
	TrackName->setOverrideColor( fgColor );
	TrackName->setBackgroundColor( bgColor );
	x += dx;

	/// Buttons
	dx = 24;
	PlayButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"|>"); x+=dx;
	PauseButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"||"); x+=dx;
    StopButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"[]"); x+=dx;
	RewindButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"<<"); x+=dx;
	ForwardButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L">>"); x+=dx;
	PrevButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"<"); x+=dx;
	NextButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L">"); x+=dx;

	/// Volume
	dx = 2*24;
	label = env->addStaticText( L"Volume", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	label->setTextAlignment( EGUIA_CENTER, EGUIA_CENTER );
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	x+=dx;

	dx = 3*24;
	MasterVolume = env->addSpinBox( L"0.10", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterVolume)
	{
		MasterVolume->setDecimalPlaces(2);
		MasterVolume->setRange(0,1);
		MasterVolume->setStepSize(0.1f);
		MasterVolume->setValue( Player->getVolume() );
	}
	x+=dx;

	/// Speed / Pitch
	dx = 2*24;
	label = env->addStaticText( L"Speed", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	label->setTextAlignment( EGUIA_CENTER, EGUIA_CENTER );
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	x+=dx;

	dx = 3*24;
	MasterPitch = env->addSpinBox( L"1.00", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterPitch)
	{
		MasterPitch->setDecimalPlaces(3);
		MasterPitch->setRange(0,1);
		MasterPitch->setStepSize(0.05f);
		MasterPitch->setValue( Player->getPitch() );
	}
	x+=dx;

	/// Pan
	dx = 2*24;
	label = env->addStaticText( L"Pan", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	label->setTextAlignment( EGUIA_CENTER, EGUIA_CENTER );
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	x+=dx;

	dx = 3*24;
	MasterPan = env->addSpinBox( L"0.00", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterPan)
	{
		MasterPan->setDecimalPlaces(2);
		MasterPan->setRange(-1.f,1.f);
		MasterPan->setStepSize(.05f);
		MasterPan->setValue( Player->getPan() );
	}
	x+=dx; // y+=dy;


	/// track-info
	y += dy;
	x = Border;
	dx = 100;
	dy = 3*24;
	TrackInfo = env->addStaticText( L"TrackInfo", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	TrackInfo->setTextAlignment( EGUIA_UPPERLEFT, EGUIA_CENTER );
	TrackInfo->setOverrideColor( 0xffffffff );
	TrackInfo->setBackgroundColor( video::SColor(200,0,0,0) );
	x += dx;


	/// Mute
	//	ChkMute = Environment->addCheckBox( false, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Mute"); y+=dy;
	/// ShuffleMode
	//	ChkShuffleMode = Environment->addCheckBox( false, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Shuffle"); y+=dy;
	/// LoopMode
	//	ChkLoopMode = Environment->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Loop"); y+=dy;
	/// AudioDevices
	//    txt = L"ChannelList"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//	Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;
}

//! destructor
CGUIAudioPlayer::~CGUIAudioPlayer()
{

}

//! draws the element and its children
void CGUIAudioPlayer::draw()
{
	if (!IsVisible)
		return;

	IGUIElement::draw();
}

//! called if an event happened.
bool CGUIAudioPlayer::OnEvent(const SEvent& event)
{
	if (!isEnabled())
		return IGUIElement::OnEvent(event);

	switch(event.EventType)
	{
		case EET_KEY_INPUT_EVENT:
		{
		} break;

		case EET_GUI_EVENT:
		{
			IGUIElement* caller = event.GUIEvent.Caller;

			if ( !this->isMyChild(caller) )
				break;

			if ( !Player )
				break;

			if ( event.GUIEvent.EventType == EGET_SPINBOX_CHANGED )
			{
				const f32& value = ((IGUISpinBox*)caller)->getValue();

				if (caller == MasterVolume)
				{
					Player->setVolume( value );
					return true;
				}

				if (caller == MasterPan)
				{
					Player->setPan( value );
					return true;
				}

				if (caller == MasterPitch)
				{
					Player->setPitch( value );
					return true;
				}
			}

			if ( event.GUIEvent.EventType == EGET_BUTTON_CLICKED )
			{
				if (caller == LoadButton)
				{
					loadFile();
					return true;
				}

				if (caller == CloseButton)
				{
					closeFile();
					return true;
				}

				if (caller == PlayButton)
				{
					Player->play();
					return true;
				}

				else if (caller == PauseButton)
				{
					Player->pause();
					return true;
				}

				else if (caller == StopButton)
				{
					Player->stop();
					return true;
				}

				else if (caller == RewindButton)
				{
					Player->rewind();
					return true;
				}

				else if (caller == ForwardButton)
				{
					Player->forward();
					return true;
				}

				else if (caller == PrevButton)
				{
					Player->prev();
					return true;
				}

				else if (caller == NextButton)
				{
					Player->next();
					return true;
				}

//				else if (caller == ButtonResume)
//				{
//                  Player->resume();
//					return true;
//				}
//				else if (caller == ButtonRecord)
//				{
//                  Player->record();
//					return true;
//				}
			}

			if ( event.GUIEvent.EventType == EGET_CHECKBOX_CHANGED )
			{
//				if (caller == ChkMute )
//				{
//					Player->mute();
//					return true;
//				}
//				if (caller == ChkLoopMode )
//				{
//					Player->setLooped( !Player->isLooped() );
//					return true;
//				}
//				if (caller == ChkShuffleMode )
//				{
//					Player->setShuffled( !Player->isShuffled() );
//					return true;
//				}
			}
		} break;

		case EET_MOUSE_INPUT_EVENT:
		{
//			if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
//			{
//				if (Environment->hasFocus(this) && !AbsoluteClippingRect.isPointInside(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y)))
//				{
//					Environment->removeFocus(this);
//					return false;
//				}
//				Environment->setFocus(this);
//				return true;
//			}
//			else
//			if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
//			{
//				if ( !AbsoluteClippingRect.isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y ) ) )
//				{
//					return true;
//				}
//
//				return true;
//			}
		} break;
	default:
		break;
	}

	return IGUIElement::OnEvent(event);
}

} // end namespace gui

} // end namespace irr
