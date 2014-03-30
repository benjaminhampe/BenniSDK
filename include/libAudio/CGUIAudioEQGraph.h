// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_C_GUI_AUDIO_EQUALIZER_H__
#define __IRR_EXT_C_GUI_AUDIO_EQUALIZER_H__

/// Irrlicht-Engine to render this class to
#include <irrlicht.h>
#include <core.h>
#include <libAudio/irrAudio.h>
#include <libAudio/goertzel.h>
#include <libAudio/IAudioPlayer.h>

namespace irr
{
namespace gui
{
	class CGUIAudioEQGraph : public IGUIElement
	{
	public:
		//! constructor
		CGUIAudioEQGraph( IAudioPlayer* player, IGUIEnvironment* environment,
			IGUIElement* parent, s32 id, core::rect<s32> rectangle );

		//! destructor
		virtual ~CGUIAudioEQGraph();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event);

		//! draws the element and its children
		virtual void draw();

		//! updates the elements to draw
		virtual void update();

	public:
		//! set
		virtual void setChannel( u32 channel )
		{
			u32 channelCount = Player->getChannelCount();
			if (channel < channelCount)
				SelectedChannel = channel;
			else
				dbERROR( "Selected channel %d does not exist\n", channel );
		}

		//! get
		virtual u32 getChannel( ) const
		{
			return SelectedChannel;
		}

		enum E_DRAW_MODE
		{
			EDM_BARS = 0,
			EDM_CIRCLE,
			EDM_COUNT
		};

		//! set
		virtual void setDrawMode( E_DRAW_MODE drawMode )
		{
			DrawMode = drawMode;
		}

		//! get
		virtual E_DRAW_MODE getDrawMode( ) const
		{
			return DrawMode;
		}

		//! set
		virtual void setLineColor( const video::SColor& color )
		{
			LineColor = color;
		}

		//! get
		virtual video::SColor getLineColor( ) const
		{
			return LineColor;
		}

		//! set
		virtual void setFillColor( const video::SColor& color )
		{
			FillColor = color;
		}

		//! get
		virtual video::SColor getFillColor( ) const
		{
			return FillColor;
		}

	protected:
	private:
		IAudioPlayer* Player;
		u32 SelectedChannel;
		E_DRAW_MODE DrawMode;
		video::SColor LineColor;
		video::SColor FillColor;
		u32 ShapeCount;

		core::array<f32> Samples;
		core::array<f32> FrequencyList;
		core::array<f32> DetectionList;
	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_AUDIO_PLAYER_H_INCLUDED__
