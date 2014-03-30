// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_C_AUDIO_PLAYER_SFML_H__
#define __IRR_EXT_C_AUDIO_PLAYER_SFML_H__

#include <CompileConfig.h>

#ifdef _IRR_COMPILE_WITH_SFML_AUDIO_

#include "IAudioPlayer.h"
#include <libAudio/FourierTransformRtoC.h>
#include <libAudio/irrAudio.h>

namespace irr
{

class CAudioPlayerSFML : public IAudioPlayer
{
public:
	// Default constructor
	CAudioPlayerSFML();

	// Default destructor
	virtual ~CAudioPlayerSFML();

	/// Player Control
	virtual bool loadFile( const core::stringc& uri );

	virtual bool saveFile( const core::stringc& uri );

	virtual bool closeFile( );

	virtual bool isPlaying() const;

	virtual bool isLooped() const;

	virtual void mute();

	virtual void panic();

	virtual void play();

	virtual void pause();

	virtual void resume();

	virtual void stop();

	virtual void next();

	virtual void prev();

	virtual void rewind();

	virtual void forward();

	virtual f32 getVolume() const;

	virtual void setVolume( f32 value = 0.5f );

	virtual f32 getPitch() const;

	virtual void setPitch( f32 value = 1.0f );

	virtual f32 getPan() const;

	virtual void setPan( f32 value = 0.0f );

	virtual u32 getPosition();

	virtual void setPosition( u32 time_index_in_ms );

	virtual u32 getDuration() const;

	virtual u32 getChannelCount( ) const;

	virtual size_t getSampleCount() const;

	virtual u32 getSampleRate() const;

	/// Effect Control
	virtual bool setReverbEffect( bool bEnable = false, f32 value = 0.0f );

	virtual bool setEchoEffect( bool bEnable = false, f32 value = 0.0f );

	virtual bool setFlangerEffect( bool bEnable = false, f32 value = 0.0f );

	virtual bool setCompressorEffect( bool bEnable = false, f32 value = 0.0f );

	virtual bool setDistortionEffect( bool bEnable = false, f32 value = 0.0f );

	virtual bool setChorusEffect( bool bEnable = false, f32 value = 0.0f );

	/// PlayList Control
	virtual bool clearPlayList();

	virtual u32 getPlayListItemCount() const;

	virtual bool loadPlayList( const core::stringc& uri );

	virtual bool savePlayList( const core::stringc& uri );

	/// AudioDevice Control
	virtual core::stringc getAPIName( ) const;

	virtual bool EnumerateDevices( );

	virtual u32 getAudioDeviceCount( ) const;

	virtual s32 getDefaultAudioDevice( ) const;

	virtual io::path getAudioDeviceName( u32 card = 0, u32 sub_device = 0) const;

	virtual bool setAudioDevice( u32 card = 0, u32 sub_device = 0 );

	// virtual core::array<f32> createWavData( u32 uChannel, f32 time_start, f32 time_duration = 1.0f) const;

	// virtual core::array<f32> createFFTData( u32 uChannel, f32 time_start, f32 time_delta = 0.1f, u32 fft_size = 1024, u32 max_rows = 1) const;

	virtual void getSamples( core::array<s16>& container, u32 numSamples, u32 timeStart, u32 channelIndex );

	virtual void getSamples( core::array<f32>& container, u32 numSamples, u32 timeStart, u32 channelIndex );

private:
	sf::Sound Sound;
	sf::SoundBuffer Buffer;

public:
//	virtual core::FourierTransformRtoC* getFourierTransform()
//	{
//		return Transform;
//	}

	// Direct Access to SoundBuffer

	virtual const s16* getSamples()
	{
		return Buffer.getSamples();
	}

	virtual sf::SoundBuffer* getSoundBuffer()
	{
		return &Buffer;
	}

	virtual const sf::SoundBuffer* getSoundBuffer() const
	{
		return &Buffer;
	}

	virtual sf::Sound& getSound()
	{
		return Sound;
	}

	virtual const sf::Sound& getSound() const
	{
		return Sound;
	}


};

}

#endif // _IRR_COMPILE_WITH_SFML_AUDIO_

#endif // __IRR_EXT_C_AUDIO_PLAYER_SFML_H__
