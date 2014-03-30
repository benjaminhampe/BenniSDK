// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_C_GUI_AUDIO_REALTIME_MONITOR_SFML_H__
#define __IRR_EXT_C_GUI_AUDIO_REALTIME_MONITOR_SFML_H__

#include <CompileConfig.h>

#ifdef _IRR_COMPILE_WITH_SFML_AUDIO_

#include <core.h>
#include <libAudio/RingBuffer.h>
#include <libAudio/FourierTransformRtoC.h>
#include <libAudio/goertzel.h>
#include <libAudio/irrAudio.h>

namespace irr
{
namespace gui
{
	///@brief class
	class CGUIAudioDisplaySFML : public IGUIElement, public sf::SoundRecorder
	{
	public:
		///@brief enum
		enum E_DRAW_MODE
		{
			EDM_BARS = 0,
			EDM_LINEPATH,
			EDM_POLYGON,
			EDM_TRIANGLES,
			EDM_COUNT
		};

		///@brief constructor
		CGUIAudioDisplaySFML( u32 sampleRate, u32 fft_size, IGUIEnvironment* env,
			IGUIElement* parent, s32 id, core::rect<s32> rectangle );

		///@brief destructor
		virtual ~CGUIAudioDisplaySFML();

		///@brief Called if an event happened.
		virtual bool OnEvent(const SEvent& event);

		///@brief Draws the element and its children
		virtual void draw();

		///@brief Calculates 2d screen position from a 3d position.
		virtual core::position2di getScreenPos( const core::vector3df& pos );

		///@brief Do FourierTransform on RingBuffer-Data
		virtual void updateFFT();

		///@brief
		// virtual void updateGrid();

		virtual void collectCandidates( );

		///@brief Implement sf::SoundRecorder
		virtual bool onStart();

		///@brief Implement sf::SoundRecorder
		virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount);

		///@brief Implement sf::SoundRecorder
		virtual void onStop();

		///@brief Get sample-rate used for recording
		virtual u32 getSampleRate() const;

		///@brief Set sample-rate used for recording
		virtual void setSampleRate( u32 sampleRate );

		///@brief Get size of RingBuffer/FourierTransform
		virtual u32 getInputSize() const;

		///@brief set SampleRate used for recording
		virtual void setInputSize( u32 fft_size );

		///@brief Get RenderMode
		virtual E_DRAW_MODE getDrawMode( ) const;

		///@brief Set RenderMode
		virtual void setDrawMode( E_DRAW_MODE drawMode );

		///@brief Get LineColor
		virtual video::SColor getLineColor( ) const;

		///@brief Set LineColor
		virtual void setLineColor( const video::SColor& color );

		///@brief Get FillColor
		virtual video::SColor getFillColor( ) const;

		///@brief Set FillColor
		virtual void setFillColor( const video::SColor& color );

	protected:
	private:
		core::RingBuffer<s16> Data;	// holds input data from recording device
		core::FourierTransformRtoC Transform;	// FourierTransform
		core::array<f32> Frequencies; // holds result from FourierTransform
		u32 SampleRate; // SamplingRate of RecordingDevice
		core::rectf ZoomRect; // BoundingRect for FrequencyData

		core::matrix4 ProjectionMatrix; // hold ProjectionMatrix for Camera
		core::matrix4 ViewMatrix; // hold ViewMatrix for Camera

		core::array<core::vector2df> Candidates; // TonePitchEstimationCandidates

		bool Is3D;
		E_DRAW_MODE DrawMode;
		video::SColor LineColor;
		video::SColor FillColor;

		core::stringw Caption;
	};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_SFML_AUDIO_

#endif // __IRR_EXT_C_GUI_AUDIO_REALTIME_MONITOR_SFML_H__
