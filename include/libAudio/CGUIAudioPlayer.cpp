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

	MasterVolume = env->addSpinBox( L"0.10", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterVolume)
	{
		MasterVolume->setDecimalPlaces(2);
		MasterVolume->setRange(0,1);
		MasterVolume->setStepSize(0.01f);
		MasterVolume->setValue( Player->getVolume() );
	}
	x+=100; // y+=dy;

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
	x+=100; // y+=dy;

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
	x+=100; // y+=dy;



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

//
//	printf( "CGUIAudioPlayer::create fft colorgradients()\n" );
//
//	/// ColorGradient for waveforms
//	WAV_Gradient = new video::CLinearColorGradientTable();
//	if (WAV_Gradient)
//	{
////		WAV_Gradient->addColor( video::SColor(255,255,255,255), 0.0f );
////		WAV_Gradient->addColor( video::SColor(255,255,255,255), 1.0f );
//		WAV_Gradient->addColor( video::SColor(255,0,0,0), 0.00f );
//		WAV_Gradient->addColor( video::SColor(255,0,0,255), 0.25f );
//		WAV_Gradient->addColor( video::SColor(255,0,255,0), 0.50f );
//		WAV_Gradient->addColor( video::SColor(255,255,255,0), 0.75f );
//		WAV_Gradient->addColor( video::SColor(255,255,0,0), 1.00f );
//
//		((video::CLinearColorGradientTable*)WAV_Gradient)->setTableSize( 128 );
//		((video::CLinearColorGradientTable*)WAV_Gradient)->updateTable();
//	}
//
//	// #ifdef DEBUG
//	// printf( "CGUIAudioPlayer::create fft colorgradients()\n" );
//	// #endif // DEBUG
//
//	/// ColorGradient for waveforms
//	FFT_Gradient = new video::CLinearColorGradientTable();
//	if (FFT_Gradient)
//	{
//	/// black
////		FFT_Gradient->addColor( video::SColor(255,0,0,0), 0.00f );
////		FFT_Gradient->addColor( video::SColor(255,25,25,25), 0.25f );
////		FFT_Gradient->addColor( video::SColor(255,75,75,75), 0.50f );
////		FFT_Gradient->addColor( video::SColor(255,95,95,95), 0.60f );
////		FFT_Gradient->addColor( video::SColor(255,115,115,115), 0.70f );
////		FFT_Gradient->addColor( video::SColor(255,155,155,155), 0.80f );
////		FFT_Gradient->addColor( video::SColor(255,255,255,255), 0.90f );
////		FFT_Gradient->addColor( video::SColor(255,255,255,0), 0.95f );
////		FFT_Gradient->addColor( video::SColor(255,255,0,0), 1.00f );
//		FFT_Gradient->addColor( video::SColor(255,0,0,0), 0.00f );
//		FFT_Gradient->addColor( video::SColor(255,25,25,25), 0.10f );
//		FFT_Gradient->addColor( video::SColor(255,50,50,50), 0.20f );
//		FFT_Gradient->addColor( video::SColor(255,75,75,75), 0.30f );
//		FFT_Gradient->addColor( video::SColor(255,100,100,100), 0.40f );
//		FFT_Gradient->addColor( video::SColor(255,125,125,125), 0.45f );
//		//FFT_Gradient->addColor( video::SColor(255,0,0,100), 0.5f );
//		FFT_Gradient->addColor( video::SColor(255,0,0,255), 0.50f );
//		FFT_Gradient->addColor( video::SColor(255,0,200,0), .60f );
//		FFT_Gradient->addColor( video::SColor(255,255,255,0), .80f );
//		FFT_Gradient->addColor( video::SColor(255,255,0,0), 1.0f );
//		//FFT_Gradient->addColor( video::SColor(255,255,255,255), 1.0f );
//
//	/// blue
//
//	//	FFT_Gradient->addColor( video::SColor(255,0,0,255), 0.00f );
//	//	FFT_Gradient->addColor( video::SColor(255,25,25,50), 0.25f );
//	//	FFT_Gradient->addColor( video::SColor(255,50,50,75), 0.50f );
//	//	FFT_Gradient->addColor( video::SColor(255,75,75,95), 0.60f );
//	//	FFT_Gradient->addColor( video::SColor(255,95,95,155), 0.70f );
//	//	FFT_Gradient->addColor( video::SColor(255,105,105,255), 0.80f );
//	//	FFT_Gradient->addColor( video::SColor(255,255,255,255), 0.90f );
//	//	FFT_Gradient->addColor( video::SColor(255,255,255,0), 0.95f );
//	//	FFT_Gradient->addColor( video::SColor(255,255,0,0), 1.00f );
//
//	/// blue1
////		FFT_Gradient->addColor( video::SColor(255,0,0,0), 0.00f );
////		FFT_Gradient->addColor( video::SColor(255,0,0,255), 0.25f );
////		FFT_Gradient->addColor( video::SColor(255,0,255,0), 0.50f );
////		FFT_Gradient->addColor( video::SColor(255,255,255,0), 0.75f );
////		FFT_Gradient->addColor( video::SColor(255,255,0,0), 1.00f );
//
//		((video::CLinearColorGradientTable*)FFT_Gradient)->setTableSize( 1024 );
//		((video::CLinearColorGradientTable*)FFT_Gradient)->updateTable();
//	}
//
//	// #ifdef DEBUG
//	// printf( "load fonts.\n" );
//	// #endif // DEBUG
//

//
////! Writes attributes of the element.
//void CGUIAudioPlayer::serializeAttributes( io::IAttributes* out, io::SAttributeReadWriteOptions* options ) const
//{
//	IGUIElement::serializeAttributes(out,options);
//
//	//	out->addBool	("PushButton",		IsPushButton );
//	//	if (IsPushButton)
//	//		out->addBool("Pressed",		Pressed);
//	//
//	//	out->addTexture ("Image",		Image);
//	//	out->addRect	("ImageRect",		ImageRect);
//	//	out->addTexture	("PressedImage",	PressedImage);
//	//	out->addRect	("PressedImageRect",	PressedImageRect);
//	//	out->addBool	("UseAlphaChannel",	isAlphaChannelUsed());
//	//	out->addBool	("Border",		isDrawingBorder());
//	//	out->addBool	("ScaleImage",		isScalingImage());
//	//  out->addString  ("OverrideFont",	OverrideFont);
//}
//
////! Reads attributes of the element
//void CGUIAudioPlayer::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
//{
//	IGUIElement::deserializeAttributes(in,options);
//
//	//	IsPushButton	= in->getAttributeAsBool("PushButton");
//	//	Pressed		= IsPushButton ? in->getAttributeAsBool("Pressed") : false;
//	//
//	//	core::rect<s32> rec = in->getAttributeAsRect("ImageRect");
//	//	if (rec.isValid())
//	//		setImage( in->getAttributeAsTexture("Image"), rec);
//	//	else
//	//		setImage( in->getAttributeAsTexture("Image") );
//	//
//	//	rec = in->getAttributeAsRect("PressedImageRect");
//	//	if (rec.isValid())
//	//		setPressedImage( in->getAttributeAsTexture("PressedImage"), rec);
//	//	else
//	//		setPressedImage( in->getAttributeAsTexture("PressedImage") );
//	//
//	//	setDrawBorder(in->getAttributeAsBool("Border"));
//	//	setUseAlphaChannel(in->getAttributeAsBool("UseAlphaChannel"));
//	//	setScaleImage(in->getAttributeAsBool("ScaleImage"));
//	//  setOverrideFont(in->getAttributeAsString("OverrideFont"));
//
//	updateAbsolutePosition();
//}
//
/////@brief create image from soundbuffer-waveform ( 16bit per sample * channel_count * sample_rate = bits per sec = 8*bytes per sec )
//video::IImage* CGUIAudioPlayer::createImageFromSoundBufferWaveform( const core::dimension2du& img_size, const video::SColor& clearColor ) const
//{
//	#ifdef DEBUG
//	printf( "CGUIAudioPlayer::createImageFromSoundBufferWaveform( %d, %d )\n", img_size.Width, img_size.Height );
//	#endif // DEBUG
//
//	video::CImage* img = new video::CImage( video::ECF_A8R8G8B8, img_size );
//	if (!img)
//	{
//		return 0;
//	}
//
//	#ifdef _DEBUG
//	const u32 img_bpp = video::IImage::getBitsPerPixelFromFormat( img->getColorFormat() );
//	printf("Created image(%d,%d,%d);\n", img_size.Width, img_size.Height, img_bpp );
//	#endif // _DEBUG
//
//	//! fill image pixels to fully transparent ( alpha == 0 )
//	img->fill( clearColor );
//
//	//! make place for a change in drawing positions
//	const u32 d = core::round32( (f32)img_size.Height / (f32)SoundBuffer.getChannelCount() );
//	const core::dimension2du small_size( img_size.Width, d );
//	const s32 x = 0;
//	s32 y = 0;
//
//	/// loop channels
//	for ( u32 channel_index = 0; channel_index < SoundBuffer.getChannelCount(); channel_index++)
//	{
//		sfx::drawAudioWaveformFast( img, core::recti( core::position2di(x,y), small_size ), WAV_Gradient, &SoundBuffer, channel_index, false );
//		y += d;
//	}
//
//	return img;
//}
//
///// create image from FFT-PowerSpectrum
//video::IImage* CGUIAudioPlayer::createImageFromSoundBufferPowerSpectrum( const core::dimension2du& img_size ) const
//{
//	#ifdef DEBUG
//	printf( "CGUIAudioPlayer::createImageFromSoundBufferPowerSpectrum( %d, %d )\n", img_size.Width, img_size.Height );
//	#endif // DEBUG
//
//	video::CImage* img = new video::CImage( video::ECF_A8R8G8B8, img_size );
//	if (!img)
//	{
//		return 0;
//	}
//
//	#ifdef _DEBUG
//	const u32 img_bpp = video::IImage::getBitsPerPixelFromFormat( img->getColorFormat() );
//	printf("Created image(%d,%d,%d);\n", img_size.Width, img_size.Height, img_bpp );
//	#endif // _DEBUG
//
//	//! fill image pixels to fully transparent ( alpha == 0 )
//	img->fill( video::SColor(200,0,0,0) );
//
//	//! make place for a change in drawing positions
//
//	// const u32 d = core::round32( (f32)img_size.Height / (f32)my_channel_count );
//	// const core::dimension2du small_size( img_size.Width, d );
//	s32 x = 0;
//	s32 y = 0;
//
//	/// loop channels
//	// for ( u32 channel_index = 0; channel_index < SoundBuffer.getChannelCount(); channel_index++)
//	// {
//
//		/// draw spectrum
//		sfx::drawAudioSpectrum( img, core::recti(core::position2di(x,y),img_size), FFT_Gradient, &SoundBuffer, 0);
//	//	y += d;
//	// }
//
//	return img;
//}

} // end namespace gui

} // end namespace irr
