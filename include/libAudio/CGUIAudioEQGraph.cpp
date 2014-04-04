// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIAudioEQGraph.h"

namespace irr
{
namespace gui
{
//! constructor
CGUIAudioEQGraph::CGUIAudioEQGraph(
	IAudioPlayer* player,
	IGUIEnvironment* environment,
	IGUIElement* parent,
	s32 id,
	core::rect<s32> rectangle )
: IGUIElement(EGUIET_ELEMENT, environment, parent, id, rectangle)
, Player(player), SelectedChannel(0), DrawMode(EDM_BARS), LineColor(255,255,128,0)
, FillColor(255,255,255,0), ShapeCount(32)
{
	setName("CGUIAudioEQGraph");
	setTabStop(true);
	setTabOrder(-1);

	FrequencyList.clear();

	for (u32 oktave=0; oktave<12; oktave++)
	{
		for (u32 ton=0; ton<12; ton++)
		{
			FrequencyList.push_back( sfx::getFrequencyFromNote( ton, oktave ) );
		}
	}

	dbPRINT( "FrequencyList.size() = %d\n", FrequencyList.size() );

	for (u32 i=0; i<FrequencyList.size(); i++)
	{
		dbPRINT( "FrequencyList[%d] = %f Hz\n", i, FrequencyList[i] );
	}

	DetectionList.clear();
	DetectionList.reallocate( FrequencyList.size() );
	DetectionList.set_used( FrequencyList.size() );

	const u32 fft_size = 8*1024;
	Samples.reallocate( fft_size );
	Samples.set_used( fft_size );
}

//! destructor
CGUIAudioEQGraph::~CGUIAudioEQGraph()
{
	// empty
}

//! updates the elements to draw
void CGUIAudioEQGraph::update()
{
	Player->getSamples( Samples, Samples.size(), Player->getPosition(), SelectedChannel );

	/// Calculate Array Min/Max Range

	f32 f_min = 0.0f;
	f32 f_max = 0.0f;

	for (u32 i=0; i<DetectionList.size(); i++)
	{
		DetectionList[i] = core::goertzel_mag2( Samples, FrequencyList[i], Player->getSampleRate() );
		if (f_max < DetectionList[i])
			f_max = DetectionList[i];
	}

	/// Normalize Array to Range [0..1]

	const f32 f_max_inv = core::reciprocal( f_max );

	for (u32 i=0; i<DetectionList.size(); i++)
	{
		DetectionList[i] = f_max_inv * DetectionList[i];
	}
}

//! draws the element and its children
void CGUIAudioEQGraph::draw()
{
	if (!IsVisible)
		return;

	if (!Player->isPlaying())
		return;

	update();

	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = skin->getFont( EGDF_DEFAULT );
	video::IVideoDriver* driver = Environment->getVideoDriver();

	const s32 w = AbsoluteClippingRect.getWidth();
	const s32 h = AbsoluteClippingRect.getHeight();

	core::position2di pos = AbsoluteRect.UpperLeftCorner;
	core::stringw txt = L"";

	if (DrawMode == EDM_BARS)
	{
		txt = L"EDM_BARS";
		drawText( font, txt, pos, LineColor, -1, -1, 0);
		pos.Y += getTextHeight( font, txt );
	}
	else if ( DrawMode == EDM_CIRCLE )
	{
		txt = L"EDM_CIRCLE";
		drawText( font, txt, pos, LineColor, -1, -1, 0);
		pos.Y += getTextHeight( font, txt );
	}

	/// draw TrackName
	txt = Player->getTrackName();
	drawText( font, txt, pos, LineColor, -1, -1, 0);
	pos.Y += getTextHeight( font, txt );

	/// draw TrackInfo
	txt = Player->getTrackInfo();
	drawText( font, txt, pos, LineColor, -1, -1, 0);
	pos.Y += getTextHeight( font, txt );

//    driver->draw2DLine(
//        r0.UpperLeftCorner,
//        r0.LowerRightCorner,
//        video::SColor(255,255,255,255) );
//
//    driver->draw2DLine(
//        core::position2di(r0.LowerRightCorner.X,r0.UpperLeftCorner.Y),
//        core::position2di(r0.UpperLeftCorner.X,r0.LowerRightCorner.Y),
//        video::SColor(255,255,255,255) );

	const s32 bar_width = core::round32( (f32)w / (f32)DetectionList.size() );

	s32 x = AbsoluteRect.UpperLeftCorner.X;
	s32 y = AbsoluteRect.LowerRightCorner.Y;

	for ( u32 i=0; i<DetectionList.size(); i++ )
	{
		s32 bar_height = core::round32( DetectionList[i] * h );

		const core::recti pos( core::position2di(x,y-bar_height), core::dimension2du( bar_width-1, bar_height ) );

		driver->draw2DRectangle( FillColor, pos, 0);

		driver->draw2DRectangleOutline( pos, LineColor );

		x += bar_width;
	}

	IGUIElement::draw();
}





//! called if an event happened.
bool CGUIAudioEQGraph::OnEvent(const SEvent& event)
{
	if (!isEnabled())
		return IGUIElement::OnEvent(event);

	switch(event.EventType)
	{
		case EET_KEY_INPUT_EVENT:
			break;

		case EET_GUI_EVENT:
			break;

		case EET_MOUSE_INPUT_EVENT:
			break;

		default:
			break;
	}

	//return Parent ? Parent->OnEvent(event) : false;
	//return Parent ? Parent->OnEvent(event) : IGUIElement::OnEvent(event);
	return IGUIElement::OnEvent(event);
}


} // end namespace gui
} // end namespace irr



