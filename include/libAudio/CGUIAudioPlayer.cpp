//// Copyright (C) 2002-2013 Benjamin Hampe
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

	video::IVideoDriver* driver = env->getVideoDriver();
	Font = env->getBuiltInFont();

	/// Buttons
	s32 dx = 24;
	s32 dy = dx;
	s32 x = Border;
	s32 y = Border;

	PlayButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"|>"); x+=dx;
	PauseButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"||"); x+=dx;
    StopButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"[]"); x+=dx;
	RewindButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"<<"); x+=dx;
	ForwardButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L">>"); x+=dx;
	PrevButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"<"); x+=dx;
	NextButton = env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L">"); x+=dx;

	/// Volume
	//	txt = L"Volume"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//	Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x+ex,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;

	dx = 3*24;
	MasterVolume = env->addSpinBox( L"0.10", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterVolume)
	{
		MasterVolume->setDecimalPlaces(2);
		MasterVolume->setRange(0,1);
		MasterVolume->setStepSize(0.01f);
		MasterVolume->setValue( Player->getVolume() );
	}
	x+=dx;

	/// Speed / Pitch
	//    txt = L"Speed"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//    Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x+ex,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;

	MasterPitch = env->addSpinBox( L"1.00", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterPitch)
	{
		MasterPitch->setDecimalPlaces(3);
		MasterPitch->setRange(0.001f,1000.0f);
		MasterPitch->setStepSize(0.1f);
		MasterPitch->setValue( Player->getPitch() );
	}
	x+=dx; // y+=dy;

	/// Pan
	// txt = L"Panning"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//	Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x+ex,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;

	MasterPan = env->addSpinBox( L"0.00", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterPan)
	{
		MasterPan->setDecimalPlaces(2);
		MasterPan->setRange(-1.f,1.f);
		MasterPan->setStepSize(.05f);
		MasterPan->setValue( Player->getPan() );
	}
	x+=dx; // y+=dy;



	/// Mute
	//	ChkMute = Environment->addCheckBox( false, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Mute"); y+=dy;
	/// ShuffleMode
	//	ChkShuffleMode = Environment->addCheckBox( false, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Shuffle"); y+=dy;
	/// LoopMode
	//	ChkLoopMode = Environment->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Loop"); y+=dy;

	/// TrackInfo
	//	TrackName = Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x,y+ey), txt_size), false, false, this, -1);
	//  ChannelList = Environment->addComboBox( core::recti(x,y,x+dx-1,y+dy-1), this, -1 ); y+=dy;

	// STATIC LABEL - Audio Information Text
	//	txt = getSoundInfoText();
	//
	//	txt_size = arial->getDimension( txt.c_str() );
	//	txt_size = font->getDimension( txt.c_str() );
	//
	//	x = w - txt_size.Width - Border;
	//	y = h - txt_size.Height - Border;
	//
	//	TrackInfo = Environment->addStaticText( txt.c_str(), core::recti( core::position2di(x,y), txt_size), true, false, this, -1, false );
	//
	//	if (TrackInfo)
	//	{
	//		TrackInfo->setOverrideFont( font );
	//		TrackInfo->setOverrideColor( video::SColor(255,255,255,255) );
	//	}

	/// AudioDevices
	//    txt = L"ChannelList"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//	Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;

	//COLUMN 2
	//	y = Border;
	//	x = 200;
	//	dx = 200;
	//

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
