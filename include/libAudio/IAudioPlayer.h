// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_I_AUDIO_PLAYER_H__
#define __IRR_EXT_I_AUDIO_PLAYER_H__

#include <libAudio/FourierTransformRtoC.h>

namespace irr
{

class IAudioPlayer : public IReferenceCounted
{
private:
protected:
	core::stringc FileName; // current played file
	u32 CurrentTrackIndex;
	core::array<core::stringc> PlayList;

	bool IsLoaded;
	bool IsPlaying;
	bool IsShuffled;
	bool IsLooped;
	// s32 Loops; // -1 = inf loop, 0 = no looping, >0 number of loops

	f32 Volume;
	f32 Pitch;
	f32 Pan;

//	core::FourierTransformRtoC* Transform;
//	core::array<s16> FFT_InputBuffer;	// stores waveform
//	core::array<f32> FFT_OutputBuffer;	// stores spectrum

public:
	IAudioPlayer()
	: FileName(""), CurrentTrackIndex(0)
	, IsLoaded(false), IsPlaying(false), IsShuffled(false), IsLooped(false)
	, Volume(0.5f), Pitch(1.0f), Pan(0.0f) //, Transform(0)

	{
	}

//	virtual core::FourierTransformRtoC* getFourierTransform() = 0;

/// Direct Access to SoundBuffer
//	virtual void* getSoundBuffer() = 0;

//	virtual const s16* getSamples() = 0;

/// AudioDeviceControl

	virtual core::stringc getAPIName( ) const = 0;

	virtual u32 getAudioDeviceCount( ) const = 0;

	virtual io::path getAudioDeviceName( u32 card = 0, u32 sub_device = 0) const = 0;

	virtual s32 getDefaultAudioDevice( ) const = 0;

	virtual bool setAudioDevice( u32 card = 0, u32 sub_device = 0 ) = 0;



	virtual bool isLooped() const
	{
		return IsLooped;
	}

	virtual bool setLooped( bool looped )
	{
		IsLooped = looped;
	}

	virtual bool isShuffled() const
	{
		return IsShuffled;
	}

	virtual void setShuffled( bool shuffled )
	{
		IsShuffled = shuffled;
	}

/// TrackControl

	virtual bool loadFile( const core::stringc& filename ) = 0;

	virtual bool saveFile( const core::stringc& filename ) = 0;

	virtual bool closeFile( ) = 0;

	virtual void play() = 0;

	virtual void pause() = 0;

	virtual void resume() = 0;

	virtual void stop() = 0;

	virtual void rewind() = 0;

	virtual void forward() = 0;

	virtual void next() = 0;

	virtual void prev() = 0;

	virtual void mute() = 0;

	virtual void panic() = 0;


/// TrackInfo

	virtual bool isPlaying() const = 0;

	virtual u32 getPosition() = 0;

	virtual void setPosition( u32 time_index_in_ms ) = 0;

	virtual u32 getDuration() const = 0;

	virtual u32 getChannelCount( ) const = 0;

	virtual size_t getSampleCount() const = 0;

	virtual u32 getSampleRate() const = 0;

	virtual f32 getPitch() const = 0;

	virtual void setPitch( f32 value = 1.0f ) = 0;

	virtual f32 getVolume() const = 0;

	virtual void setVolume( f32 value = 0.5f ) = 0;

	virtual f32 getPan() const = 0;

	virtual void setPan( f32 value = 0.0f ) = 0;


	virtual void getSamples( core::array<s16>& container, u32 numSamples, u32 timeStart, u32 channelIndex ) = 0;

	virtual void getSamples( core::array<f32>& container, u32 numSamples, u32 timeStart, u32 channelIndex ) = 0;


/// PlayListControl

	virtual bool clearPlayList()
	{
		PlayList.clear();
		FileName = "";
		CurrentTrackIndex = PlayList.size();
	}

	virtual u32 getPlayListCount() const
	{
		return PlayList.size();
	}

	virtual bool loadPlayList( const core::stringc& uri )
	{
		return false;
	}

	virtual bool savePlayList( const core::stringc& uri )
	{
		return false;
	}

	virtual core::stringc getTrackName() const
	{
		return FileName;
	}

	virtual core::stringc getTrackInfo()
	{
		core::stringc txt = "";
		txt += getSampleRate();
		txt += " Hz, ";
		txt += getChannelCount();
		txt += " Channel";
		if (getChannelCount()>1)
			txt += "(s)";
		txt += ", Time ";
		txt += getPosition();
		txt += " ms / ";
		txt += getDuration();
		txt += " ms";
		return txt;
	}
};

} // end namespace irr

#endif // __IRR_EXT_I_AUDIO_PLAYER_H__
