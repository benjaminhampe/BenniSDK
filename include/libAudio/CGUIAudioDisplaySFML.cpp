// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIAudioDisplaySFML.h"

#include <libAudio/drawData1D.h>
#include <libAudio/drawGrid.h>

namespace irr
{
namespace gui
{

//! constructor
CGUIAudioDisplaySFML::CGUIAudioDisplaySFML(
	u32 sampleRate, u32 fft_size, IGUIEnvironment* env,
	IGUIElement* parent, s32 id, core::rect<s32> rectangle )

: IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle)
, Data(fft_size)
, Is3D(false)
, DrawMode(EDM_BARS), LineColor(255,255,128,0)
, FillColor(255,255,255,0)
, Transform(fft_size, "CGUIAudioDisplaySFML.wisdom")
, SampleRate(sampleRate)
, ZoomRect(0 /* Hz */, 0 /* min dB */, 1 /* Hz */, 130 /* max dB */)
, Caption( Parent ? Parent->getText() : L"" )
{
	setName("CGUIAudioDisplaySFML");
	setTabStop(true);
	setTabOrder(-1);

	const u32 FrequencyCount = 1024;
	Frequencies.reallocate( FrequencyCount );
	Frequencies.set_used( FrequencyCount );

	dbPRINT( "isAvailable() = %s\n", isAvailable()?"true":"false");
	if (isAvailable())
	{
		dbPRINT("Start recording with %d Hz and FFTSize %d\n", SampleRate, Data.size())
		start( SampleRate );
	}
	else
	{
		dbERROR("No recording device available or active!\n")
	}

	if ( Parent && (Parent != Environment->getRootGUIElement()) )
	{
		Environment->setFocus( Parent );
	}
	else
	{
		Environment->setFocus( this );
	}
}

//! destructor
CGUIAudioDisplaySFML::~CGUIAudioDisplaySFML()
{
	stop();
}

bool CGUIAudioDisplaySFML::onStart() // optional
{
	// Initialize whatever has to be done before the capture starts

	// Return true to start playing
	return true;
}

void CGUIAudioDisplaySFML::onStop() // optional
{
	// Clean up whatever has to be done after the capture ends
}

bool CGUIAudioDisplaySFML::onProcessSamples(const sf::Int16* samples, std::size_t sampleCount)
{
	// Do something with the new chunk of samples (store them, send them, ...)
	for ( u32 i=0; i<(u32)sampleCount; i++)
		Data.push_back( s16(samples[i]) );

	// dbPRINT("Processed %ld samples\n", sampleCount);

	// Return true to continue playing
	return true;
}

void CGUIAudioDisplaySFML::updateFFT()
{
	Transform.setInputData( Data.getArray() );
	Transform.fft();
	Transform.getPowerSpectrumThreshold32( Frequencies, ZoomRect.UpperLeftCorner.Y, ZoomRect.LowerRightCorner.Y, 0.0f );

	// collectCandidates();
}

void CGUIAudioDisplaySFML::collectCandidates( )
{
	const u32 max_candidates = 10;

	Candidates.reallocate( max_candidates );
	Candidates.set_used( 0 );

	for ( u32 i=0; i<Frequencies.size(); i++)
	{
		core::vector2df candidate(i, Frequencies[i] );

		if ( Candidates.size() < max_candidates )
		{
			Candidates.push_back( candidate );
		}
		else
		{
			bool found_lower = false;
			u32 found_lower_index = 0;

			for ( u32 k=0; k<Candidates.size(); k++)
			{
				const core::vector2df& other = Candidates[k];

				if ( other.Y < candidate.Y )
				{
					found_lower = true;
					found_lower_index = k;
					break;
				}
			}

			if (found_lower)
			{
				Candidates.insert( candidate, found_lower_index );
			}
		}
	}
}

void CGUIAudioDisplaySFML::draw()
{
	if (!IsVisible)
		return;

	updateFFT();

	const s32 w = AbsoluteClippingRect.getWidth();
	const s32 h = AbsoluteClippingRect.getHeight();

	core::position2di pos = AbsoluteClippingRect.UpperLeftCorner;
	core::stringw txt;
	core::stringw info = Caption;
	info += L" | ";

	/// Calculate average decibels
	f32 dB_Average = 0.0f;

	if (Frequencies.size()>0)
	{
		for ( u32 i=0; i<Frequencies.size(); i++)
		{
			dB_Average += Frequencies[i];
		}

		dB_Average /= (f32)Frequencies.size();
	}

	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = skin->getFont( EGDF_DEFAULT );
	video::IVideoDriver* driver = Environment->getVideoDriver();

	/// Render in 3D Mode
	Is3D = true;

	// save old states
	const core::recti viewPort = driver->getViewPort();
	const core::matrix4 worldMat = driver->getTransform( video::ETS_WORLD );
	const core::matrix4 viewMat = driver->getTransform( video::ETS_VIEW );
	const core::matrix4 projMat = driver->getTransform( video::ETS_PROJECTION );

	// set new states
	ProjectionMatrix = buildProjectionMatrixOrthoLH(
		0, 1, ZoomRect.UpperLeftCorner.Y, ZoomRect.LowerRightCorner.Y, -1, 1 );

	driver->setViewPort( AbsoluteClippingRect );
	driver->setTransform( video::ETS_WORLD, core::IdentityMatrix );
	driver->setTransform( video::ETS_VIEW, core::IdentityMatrix );
	driver->setTransform( video::ETS_PROJECTION, ProjectionMatrix );

	/// addto info-text - ViewPort
	info += L"Viewport(";
	info += driver->getViewPort().UpperLeftCorner.X; info += L",";
	info += driver->getViewPort().UpperLeftCorner.Y; info += L",";
	info += driver->getViewPort().LowerRightCorner.X; info += L",";
	info += driver->getViewPort().LowerRightCorner.Y; info += L") ";

	// render whatever
	video::SMaterial material;
	material.Wireframe = false;
	material.Lighting = false;
	material.FogEnable = false;
	material.MaterialType = video::EMT_SOLID;

	//driver->setMaterial( material );

	/// drawGrid
	drawGrid( driver, ZoomRect, EAS_LINEAR, EAS_LINEAR, 0xffffffff );

	/// drawData1D - Frequencies f in [Hz], A in [dB].
	{
		video::SColor color = LineColor;
		color.setAlpha( 200 );

		drawData1D( driver, Frequencies, color, scene::EPT_TRIANGLES,
			ZoomRect, EAS_LOG10, EAS_LINEAR);
	}

	/// drawData1D - Waveform
	{
		const f32 y_half = 0.5f*ZoomRect.getHeight();
		const f32 y_scale = y_half / 32768.0f;
		const video::SColor WaveColor(255,10,10,200);

		const f32 x_step = core::reciprocal( (f32)(Data.size()-1) );
		for ( u32 i=0; i<Data.size()-1; i++)
		{
			const f32 x1 = x_step*(f32)i;
			const f32 x2 = x_step*(f32)(i+1);
			const f32& y1 = 10.0f*Data[i]*y_scale + y_half;
			const f32& y2 = 10.0f*Data[i+1]*y_scale + y_half;
			driver->draw3DLine(
				core::vector3df(x1,y1,0),
				core::vector3df(x2,y2,0),
				WaveColor );
		}

//		core::array<f32> Points;
//		Points.reallocate( Data.size() );
//		Points.set_used( 0 );
//
//		for ( u32 i=0; i<Data.size(); i++)
//		{
//			Points.push_back( 4.0f*Data[i]*y_scale + y_half );
//		}
//
//		drawData1D( driver, Points, WaveColor, scene::EPT_LINE_STRIP,
//			ZoomRect, EAS_LINEAR, EAS_LINEAR);
	}

	/// draw3DLine DecibelAverage
	driver->draw3DLine(
		core::vector3df(0,dB_Average,0),
		core::vector3df(1,dB_Average,0),
		video::SColor(255,255,0,0) );

	/// restore 2D mode
	Is3D = false;
	driver->setViewPort( viewPort );
	driver->setTransform( video::ETS_WORLD, worldMat );
	driver->setTransform( video::ETS_VIEW, viewMat );
	driver->setTransform( video::ETS_PROJECTION, projMat );

	/// drawRect - Candidates
	{
		const f32 radius = 2.5f;
		const u32& max_candidates = Candidates.size();
		if ( max_candidates > 1)
		{
			for ( u32 i=0; i<max_candidates-1; i++)
			{
				const core::vector2df& p = Candidates[i];
				const core::position2di p2 = AbsoluteRect.UpperLeftCorner + getScreenPos( core::vector3df(p.X,p.Y,0) );
				driver->draw2DRectangleOutline(
					core::recti(p2.X-radius,p2.Y-radius,p2.X+radius,p2.Y+radius),
					0xffffffff );
			}
		}
	}

	/// drawRect - Test getScreenPos()
	{
		core::position2di p = getScreenPos( core::vector3df(0.5f,0.5f*ZoomRect.getHeight(),0) );
		driver->draw2DRectangleOutline(
			core::recti(p.X-3,p.Y-3,p.X+3,p.Y+3),
			video::SColor(255,255,255,0) );
	}

	/// drawText - DrawMode
	txt = L"DrawMode = ";
	switch (DrawMode)
	{
		case EDM_BARS: { txt += L"EDM_BARS"; } break;
		case EDM_LINEPATH: { txt += L"EDM_LINEPATH"; } break;
		case EDM_POLYGON: {	txt += L"EDM_POLYGON"; } break;
		case EDM_TRIANGLES: { txt += L"EDM_TRIANGLES"; } break;
		default: { txt += L"EDM_UNKOWN"; } break;
	}
	drawText( font, txt, pos, LineColor, -1, -1, 0);
	pos.Y += getTextHeight( font, txt );

	/// drawText - Decibel Average
	txt = L"Decibel-Average = ";
	txt += dB_Average;
	drawText( font, txt, pos, video::SColor(255,255,0,0), -1, -1, 0);
	pos.Y += getTextHeight( font, txt );

	/// drawText - Candidates
	txt = L"Candidates = ";
	txt += Candidates.size();
	pos.X = AbsoluteClippingRect.LowerRightCorner.X;
	pos.Y = AbsoluteClippingRect.UpperLeftCorner.Y;
	drawText( font, txt, pos, video::SColor(255,255,0,0), 1, -1, 0);
	pos.Y += getTextHeight( font, txt );

	for ( u32 i=0; i<Candidates.size(); i++)
	{
		txt = L"Candidate[";
		txt += i;
		txt += L"] = (";
		txt += Candidates[i].X;
		txt += L", ";
		txt += Candidates[i].Y;
		txt += L")";

		drawText( font, txt, pos, video::SColor(255,255,0,0), 1, -1, 0);
		pos.Y += getTextHeight( font, txt );
	}

	if (Parent)
		Parent->setText( info.c_str() );

	IGUIElement::draw();
}

///@brief Calculates 2d screen position from a 3d position.
core::position2di CGUIAudioDisplaySFML::getScreenPos( const core::vector3df& pos )
{
	video::IVideoDriver* driver = Environment->getVideoDriver();
	if (!driver)
		return core::position2di(-10000,-10000);

	core::recti viewPort = AbsoluteClippingRect;
//	if (Is3D)
//		viewPort = driver->getViewPort();

	core::dimension2du dim( (u32)viewPort.getWidth()/2, (u32)viewPort.getHeight()/2);

	f32 vec[4] = { pos.X, pos.Y, pos.Z, 1.0f };

	core::matrix4 mat = ProjectionMatrix;
	//trans *= ViewMatrix;

	mat.multiplyWith1x4Matrix(vec);

	if (vec[3] < 0)
		return core::position2di(-10000,-10000);

	const f32 zDiv = (vec[3] == 0.0f) ? 1.0f : core::reciprocal(vec[3]);

	return core::position2di(
		AbsoluteClippingRect.UpperLeftCorner.X + dim.Width + core::round32(dim.Width * (vec[0] * zDiv)),
		AbsoluteClippingRect.UpperLeftCorner.Y + dim.Height - core::round32(dim.Height * (vec[1] * zDiv)));
}

//! called if an event happened.
bool CGUIAudioDisplaySFML::OnEvent(const SEvent& event)
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

///@brief Get sample-rate used for recording
u32 CGUIAudioDisplaySFML::getSampleRate() const
{
	return SampleRate;
}

///@brief Set sample-rate used for recording
void CGUIAudioDisplaySFML::setSampleRate( u32 sampleRate )
{
	SampleRate = core::clamp<u32>( sampleRate, 1024, 96000 );
	stop();
	start(SampleRate);
}

///@brief Get size of RingBuffer/FourierTransform
u32 CGUIAudioDisplaySFML::getInputSize() const
{
	return Data.size();
}

///@brief set SampleRate used for recording
void CGUIAudioDisplaySFML::setInputSize( u32 fft_size )
{
	Data.resize( fft_size );
	Transform.resize( fft_size );
}

///@brief Get RenderMode
CGUIAudioDisplaySFML::E_DRAW_MODE CGUIAudioDisplaySFML::getDrawMode( ) const
{
	return DrawMode;
}

///@brief Set RenderMode
void CGUIAudioDisplaySFML::setDrawMode( CGUIAudioDisplaySFML::E_DRAW_MODE drawMode )
{
	DrawMode = drawMode;
}

///@brief Get LineColor
video::SColor CGUIAudioDisplaySFML::getLineColor( ) const
{
	return LineColor;
}

///@brief Set LineColor
void CGUIAudioDisplaySFML::setLineColor( const video::SColor& color )
{
	LineColor = color;
}

///@brief Get FillColor
video::SColor CGUIAudioDisplaySFML::getFillColor( ) const
{
	return FillColor;
}

///@brief Set FillColor
void CGUIAudioDisplaySFML::setFillColor( const video::SColor& color )
{
	FillColor = color;
}

} // end namespace gui
} // end namespace irr



