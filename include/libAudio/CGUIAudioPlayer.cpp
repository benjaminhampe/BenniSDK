//// Copyright (C) 2002-2014 Benjamin Hampe
//// This file is part of the "irrlicht-engine"
//// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIAudioPlayer.h"

#include <libAudio/irrAudio.h>

namespace irr
{
namespace gui
{

//! constructor
CGUIAudioPlayer::CGUIAudioPlayer( IAudioPlayer* player,
	IGUIEnvironment* env, IGUIElement* parent, s32 id, core::rect<s32> rectangle )
: IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle)
, Font(0), Player(0), PreviewTexName(_IRR_TEXT("CGUIAudioPlayer_PreviewTexture.png"))
, IsMouseClicked(false), IsMouseOver(false), MousePos(-10000,-10000)
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

	// track-name
	dx = 15*24;
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
	gui::IGUIStaticText* label;
	label = env->addStaticText( L"Volume", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	label->setTextAlignment( EGUIA_CENTER, EGUIA_CENTER );
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	x+=dx;

	MasterVolume = env->addSpinBox( L"0.10", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterVolume)
	{
		MasterVolume->setDecimalPlaces(2);
		MasterVolume->setRange(0,1);
		MasterVolume->setStepSize(0.1f);
		MasterVolume->setValue( 0.0f );
	}
	x+=dx;

	/// Speed / Pitch
	label = env->addStaticText( L"Speed", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	label->setTextAlignment( EGUIA_CENTER, EGUIA_CENTER );
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	x+=dx;

	MasterPitch = env->addSpinBox( L"1.00", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterPitch)
	{
		MasterPitch->setDecimalPlaces(3);
		MasterPitch->setRange(0,10);
		MasterPitch->setStepSize(0.05f);
		MasterPitch->setValue( 1.0f );
	}
	x+=dx;

	/// Pan
	label = env->addStaticText( L"Pan", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	label->setTextAlignment( EGUIA_CENTER, EGUIA_CENTER );
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	x+=dx;

	MasterPan = env->addSpinBox( L"0.00", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterPan)
	{
		MasterPan->setDecimalPlaces(2);
		MasterPan->setRange(-1.f,1.f);
		MasterPan->setStepSize(.05f);
		MasterPan->setValue( 1.0f );
	}
	x+=dx; // y+=dy;

	/// track-info
	y += dy;
	x = Border;
	dx = 100;
	dy = AbsoluteClippingRect.getHeight()-y;
	TrackInfo = env->addStaticText( L"TrackInfo", core::recti(x,y,x+dx-1,y+dy-1), false, false, this, -1, true );
	TrackInfo->setTextAlignment( EGUIA_CENTER, EGUIA_CENTER );
	TrackInfo->setOverrideColor( 0xffffffff );
	TrackInfo->setBackgroundColor( video::SColor(200,0,0,0) );
	x += dx;

	/// PreviewTexture
	dx = AbsoluteClippingRect.getWidth()-x;
	dy = AbsoluteClippingRect.getHeight()-y;
	PreviewRect = core::recti(x,y, x+dx-1, y+dy-1);

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
	setPlayer( player );
	//createPreviewTexture();
//	if (Player)
//	{
//		loadFile( Player->getFileName() );
//	}
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

	///@todo Should be integrated as event inside event-handler
	if (Player && IsMouseOver && IsMouseClicked)
	{
		s32 x1 = AbsoluteRect.UpperLeftCorner.X+PreviewRect.UpperLeftCorner.X;
		s32 x2 = x1 + PreviewRect.getWidth();
		f32 duration = 0.001f*(f32)Player->getDuration();
		f32 dt = ((f32)duration*(f32)(MousePos.X-x1)) / (f32)(x2-x1);
		OnSetPlayPosition( dt );
	}
	/// update here player when playing
	/// position
	/// scrollbar position
	if (TrackInfo)
		TrackInfo->setText( getTrackInfo().c_str() );

	video::IVideoDriver* driver = Environment->getVideoDriver();

	const core::recti r = PreviewRect + AbsoluteRect.UpperLeftCorner;

	// driver->draw2DRectangleOutline( r, video::SColor(255,255,0,0) );
	video::ITexture* tex = driver->getTexture( PreviewTexName );

	if (tex)
	{
		driver->draw2DImage(
			tex,
			AbsoluteRect.UpperLeftCorner + PreviewRect.UpperLeftCorner,
			core::recti( core::position2di(0,0), tex->getOriginalSize() ),
			&AbsoluteClippingRect, video::SColor(255,255,255,255), true);
	}
	else
	{
		driver->draw2DRectangle( video::SColor(255,0,0,0), r, &AbsoluteClippingRect);

		if (Font)
			Font->draw( L"No PreviewTexture available, try reloading a file.",
				PreviewRect, 0xffffffff, true,true);
	}

	if (Player && Player->getStatus() > EAPS_STOPPED )
	{
		const s32 x = core::round32( (f32)r.getWidth() * (f32)Player->getPosition() / (f32)Player->getDuration() );

		driver->draw2DLine(
			core::position2di( r.UpperLeftCorner.X+x,r.UpperLeftCorner.Y),
			core::position2di( r.UpperLeftCorner.X+x,r.LowerRightCorner.Y),
			video::SColor(255,255,255,0) );
	}

	if (IsMouseOver)
	{
		driver->draw2DRectangleOutline( PreviewRect + AbsoluteRect.UpperLeftCorner, video::SColor(255,255,0,0) );

		if (IsMouseClicked)
		{
			driver->draw2DRectangleOutline( core::recti(-10,-10,10,10)+MousePos, video::SColor(255,0,0,255) );
		}
	}

	IGUIElement::draw();
}

/// event-handler
bool CGUIAudioPlayer::OnSetPlayPosition( f32 seconds )
{
	if (!Player)
		return false;

	Player->setPosition( (u32)core::round32(1000.0f * seconds) );
}

//! called if an event happened.
bool CGUIAudioPlayer::OnEvent(const SEvent& event)
{
	if (!isEnabled())
		return IGUIElement::OnEvent(event);

	/// handle MouseEvents
	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		const SEvent::SMouseInput& e = event.MouseInput;

		if (e.Event == EMIE_MOUSE_MOVED)
		{
			MousePos.X = e.X;
			MousePos.Y = e.Y;

			bool bMouseOver = isMouseOver( MousePos, PreviewRect + AbsoluteRect.UpperLeftCorner );

			if (IsMouseOver != bMouseOver)
			{
				IsMouseOver = bMouseOver;

				if (!IsMouseClicked)
				{
					return true;
				}
			}
		}

		if (e.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			IsMouseClicked = true;
			if (IsMouseOver)
				return true;
		}

		if (e.Event == EMIE_LMOUSE_LEFT_UP)
		{
			IsMouseClicked = false;
			if (IsMouseOver)
				return true;
		}

		return IGUIElement::OnEvent(event);
	}
	else if (event.EventType == EET_KEY_INPUT_EVENT)
	{

	}
	else if (event.EventType == EET_GUI_EVENT)
	{
		IGUIElement* caller = event.GUIEvent.Caller;

		if ( !this->isMyChild(caller) )
			return IGUIElement::OnEvent(event);

		if ( event.GUIEvent.EventType == EGET_SPINBOX_CHANGED )
		{
			const f32& value = ((IGUISpinBox*)caller)->getValue();

			if (caller == MasterVolume)
			{
				if (Player)
					Player->setVolume( value );
				return true;
			}

			if (caller == MasterPan)
			{
				if (Player)
					Player->setPan( value );
				return true;
			}

			if (caller == MasterPitch)
			{
				if (Player)
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

			if (caller == PlayButton)
			{
				if (Player)
					Player->play();
				return true;
			}

			else if (caller == PauseButton)
			{
				if (Player)
					Player->pause();
				return true;
			}

			else if (caller == StopButton)
			{
				if (Player)
					Player->stop();
				return true;
			}

			else if (caller == RewindButton)
			{
//					if (Player)
//						Player->rewind();
				return true;
			}

			else if (caller == ForwardButton)
			{
//					if (Player)
//						Player->forward();
				return true;
			}

			else if (caller == PrevButton)
			{
//					if (Player)
//						Player->prev();
				return true;
			}

			else if (caller == NextButton)
			{
//					if (Player)
//						Player->next();
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
	}

	return IGUIElement::OnEvent(event);
}

IAudioPlayer* CGUIAudioPlayer::getAudioPlayer()
{
	return Player;
}

core::stringw CGUIAudioPlayer::getTrackInfo()
{
	core::stringw txt = L"";
	if (!Player)
	{
		txt += L"No player set.";
		return txt;
	}

	switch (Player->getStatus())
	{
		case EAPS_PLAYING: txt += L"(playing)"; break;
		case EAPS_PAUSED: txt += L"(paused)"; break;
		case EAPS_STOPPED: txt += L"(stopped)"; break;
		default:
			txt += L"(invalid/error)"; break;
	}
	txt += L"\n";
	txt += Player->getSampleRate();	txt += L" Hz\n";
	txt += Player->getChannelCount(); txt += L" Channel";
	if (Player->getChannelCount()>1)
		txt += L"(s)";
	txt += L"\n";
	txt += Player->getPosition(); txt += L" ms\n";
	txt += Player->getDuration(); txt += L" ms";
	return txt;
}

void CGUIAudioPlayer::setPlayer( IAudioPlayer* player )
{
	Player = player;

	if (!Player)
	{
		if (TrackName)
			TrackName->setText( L"Pointer to IAudioPlayer is zero\n");
		return;
	}

	if (TrackName)
	{
		TrackName->setText( core::stringw(
			Player->getFileName() ).c_str() );
	}
	if (TrackInfo)
	{
		TrackInfo->setText( getTrackInfo().c_str() );
	}
	if (MasterVolume)
	{
		MasterVolume->setValue( Player->getVolume() );
	}
	if (MasterPitch)
	{
		MasterPitch->setValue( Player->getPitch() );
	}
	if (MasterPan)
	{
		MasterPan->setValue( Player->getPan() );
	}

	//createPreviewTexture();
}

bool CGUIAudioPlayer::loadFile( const core::stringc& filename )
{
	if (!Player)
	{
		dbERROR("CGUIAudioPlayer::loadFile() - Invalid pointer to IAudioPlayer\n")
		return false;
	}

	core::stringc myFilename = filename;

	if ( myFilename.size() == 0 )
	{
	#ifdef _IRR_COMPILE_WITH_FLTK_
		Fl_Native_File_Chooser dlg;
		dlg.title("Load an audio-file");
		dlg.type(Fl_Native_File_Chooser::BROWSE_FILE);
		dlg.filter("Audio-Files\t*.{ogg,wav,mp3,flac}");
		dlg.directory("../../media/music");
		switch ( dlg.show() )
		{
			case -1: break; // ERROR
			case 1:	break; // CANCEL
			default: // FILE CHOSEN
				myFilename = dlg.filename();
				break;
		}
	#else
		return false;
	#endif
	}

	if (!Player->loadFile( myFilename ))
	{
		dbERROR("CGUIAudioPlayer::loadFile() - Could not load file\n")
		return false;
	}

	/// all success
	if (TrackName)
	{
		TrackName->setText( core::stringw(
			Player->getFileName() ).c_str() );
	}
	if (TrackInfo)
	{
		TrackInfo->setText( getTrackInfo().c_str() );
	}
	if (MasterVolume)
	{
		Player->setVolume( MasterVolume->getValue() );
	}
	if (MasterPitch)
	{
		Player->setPitch( MasterPitch->getValue() );
	}
	if (MasterPan)
	{
		Player->setPan( MasterPan->getValue() );
	}
	createPreviewTexture();

	return true;
}

bool CGUIAudioPlayer::closeFile()
{
	if (!Player)
		return false;

	Player->stop();
	return true;
}

bool CGUIAudioPlayer::createPreviewTexture()
{
	if (!Player)
	{
		dbERROR( "CGUIAudioPlayer::createPreviewTexture() - Invalid pointer to IAudioPlayer\n")
		return false;
	}

	if (!Player->isLoaded())
	{
		dbERROR( "CGUIAudioPlayer::createPreviewTexture() - IAudioPlayer has no file loaded\n")
		return false;
	}

	video::IVideoDriver* driver = Environment->getVideoDriver();
	if (!driver)
	{
		dbERROR( "CGUIAudioPlayer::createPreviewTexture() - Invalid pointer to IVideoDriver\n")
		return false;
	}

	core::dimension2du img_size( (u32)PreviewRect.getWidth(), (u32)PreviewRect.getHeight() );
	img_size.getOptimalSize(
			!driver->queryFeature( video::EVDF_TEXTURE_NPOT ),
			!driver->queryFeature( video::EVDF_TEXTURE_NSQUARE ), true);

	if ((img_size.Width == 0) || (img_size.Height == 0))
	{
		dbERROR( "CGUIAudioPlayer::createPreviewTexture() - Invalid image size\n")
		return false;
	}

//	const u32 sample_rate = Player->getSampleRate();
//	const u32 sample_count = Player->getSampleCount();
//	const u32 channel_count = Player->getChannelCount();
//
//	if ( sample_count == 0 )
//		return false;
//
//	if ( channel_count == 0 )
//		return false;
//
//	if ( channel_index >= (s32)channel_count)
//		return false;

	const video::SColor bgColor(1,255,255,255);
	const video::SColor fgColor(255,255,255,255);
	const f32 duration = 1000.0f*Player->getDuration(); // in seconds
	const u32 channelCount = Player->getChannelCount();
	const f32 channelHeight = (f32)img_size.Height / (f32)channelCount;

	/// Create image
	video::IImage* img = driver->createImage( video::ECF_A8R8G8B8, img_size );
	if (!img)
	{
		dbERROR( "CGUIAudioPlayer::createPreviewTexture() - Could not create image (%d x %d)\n", img_size.Width, img_size.Height)
		return false;
	}

	img->fill( bgColor );

	core::array<s16> samples;

	/// loop channels
	for ( u32 ch = 0; ch < channelCount; ch++)
	{
		/// write channel-data to array
		Player->getSamples( samples, 100 * img_size.Width, ch, 0.0f, duration );

		/// minimum and maximum values for proper scaling ( more to see for the user )
		s16 y_min = 32767;
		s16 y_max = -32768;
		for ( u32 i = 0; i < samples.size(); i++ )
		{
			const s16 y = samples[i];
			if (y_min > y)
				y_min = y;
			if (y_max < y)
				y_max = y;
		}

		/// set scaling factor to inverse of diff in y
		const f32 y_scale = 0.9f*core::reciprocal( (f32)(y_max - y_min) );
		const f32 x_step = (f32)img_size.Width / (f32)(samples.size()-1);

		/// draw image
		for ( u32 i = 0; i < samples.size()-1; i++ )
		{
			s16 s0 = samples[i];
			s16 s1 = samples[i+1];
			f32 y0 = channelHeight * (core::clamp<f32>( 1.0f - (0.5f + y_scale * s0), 0.0f, 1.0f ) + ch);
			f32 y1 = channelHeight * (core::clamp<f32>( 1.0f - (0.5f + y_scale * s1), 0.0f, 1.0f ) + ch);
			//f32 y0 = y + core::clamp<f32>( 0.5f + y_scale * s0, 0.0f, 1.0f ) * channelHeight;
			//f32 y1 = y + core::clamp<f32>( 0.5f + y_scale * s1, 0.0f, 1.0f ) * channelHeight;
			f32 x0 = x_step * i;
			f32 x1 = x_step * (i+1);
			sfx::drawLine( img, core::rectf( x0,y0, x1,y1 ), fgColor );
		}
	}

	driver->writeImageToFile( img, PreviewTexName );

	/// Delete Texture
	video::ITexture* tex = driver->getTexture( PreviewTexName );
	if (tex)
	{
		driver->removeTexture( tex );
		tex = 0;
	}

	/// Create Texture ( setting texture-flags is a workaround for not funtioning
	/// preview-texture creation on startup of the program )
	bool bMipMaps = driver->getTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS );
	driver->setTextureCreationFlag( video::ETCF_ALWAYS_32_BIT, true );
	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, false );
	driver->setTextureCreationFlag( video::ETCF_ALLOW_NON_POWER_2, driver->queryFeature( video::EVDF_TEXTURE_NSQUARE ) );
	tex = driver->addTexture( PreviewTexName, img );
	if (!tex)
	{
		img->drop();
		dbERROR( "CGUIAudioPlayer::createPreviewTexture() - Could not create PreviewTexture\n")
		return false;
	}
	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, bMipMaps );

	/// Delete Image
	img->drop();
	return true;
}

} // end namespace gui

} // end namespace irr
