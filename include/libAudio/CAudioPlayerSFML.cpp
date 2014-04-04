// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CAudioPlayerSFML.h"

#ifdef _IRR_COMPILE_WITH_SFML_AUDIO_

namespace irr
{

CAudioPlayerSFML::CAudioPlayerSFML( )
: IAudioPlayer()
{
	dbPRINT( "CAudioPlayerSFML::CAudioPlayerSFML()\n" );

//	/// FourierTransform
//	Transform = new core::FourierTransformRtoC( 1024 );
//	if ( !Transform )
//	{
//		dbERROR( "Could not create Transform\n" );
//	}
}

//! dtor
CAudioPlayerSFML::~CAudioPlayerSFML()
{
	dbPRINT( "CAudioPlayerSFML::~CAudioPlayerSFML()\n" );

	stop();
	closeFile();
	clearPlayList();

//	if (Transform)
//	{
//		Transform->drop();
//		Transform = 0;
//	}
}

/// PlayerControl

bool CAudioPlayerSFML::loadFile( const core::stringc& filename )
{
	dbPRINT( "CAudioPlayerSFML::loadFile(%s)\n", filename.c_str() );

	# if ( SFML_VERSION_MAJOR < 2 )
	if ( !Buffer.LoadFromFile( filename.c_str()) )
	# else
	if ( !Buffer.loadFromFile( filename.c_str()) )
	# endif
	{
		dbERROR( "Cant open audio-file\n" );
		return false;
	}

	///@todo test, if filename (filename) already in PlayList

	FileName = filename;
	PlayList.push_back( FileName );
	CurrentTrackIndex = PlayList.size()-1;

	#if ( SFML_VERSION_MAJOR < 2 )
	Sound.SetBuffer( Buffer);
	# else
	Sound.setBuffer( Buffer);
	# endif
    return true;
}

bool CAudioPlayerSFML::saveFile( const core::stringc& filename )
{
	dbPRINT( "CAudioPlayerSFML::saveFile(%s)\n", filename.c_str() );
    return true;
}

bool CAudioPlayerSFML::closeFile()
{
	dbPRINT( "CAudioPlayerSFML::closeFile()\n" );
    return true;
}

u32 CAudioPlayerSFML::getChannelCount( ) const
{
	#if ( SFML_VERSION_MAJOR < 2 )
	return Buffer.GetChannelsCount();
	#else
	return Buffer.getChannelCount();
	#endif
}

size_t CAudioPlayerSFML::getSampleCount() const
{
	#if ( SFML_VERSION_MAJOR < 2 )
	return Buffer.GetSamplesCount();
	#else
	return Buffer.getSampleCount();
	#endif
}

u32 CAudioPlayerSFML::getSampleRate() const
{
	#if ( SFML_VERSION_MAJOR < 2 )
	return Buffer.GetSampleRate();
	#else
	return Buffer.getSampleRate();
	#endif
}

u32 CAudioPlayerSFML::getDuration() const
{
	return sfx::getDurationFromSoundBufferInMillis( &Buffer );
}

u32 CAudioPlayerSFML::getPosition()
{
	#if ( SFML_VERSION_MAJOR < 2 )
	return Sound.GetPlayingOffset();
	#else
	return Sound.getPlayingOffset().asMilliseconds();
	#endif
}

void CAudioPlayerSFML::setPosition( u32 time_index_in_ms )
{
	dbPRINT( "CAudioPlayerSFML::setPosition(%d)\n", time_index_in_ms );

	# if ( SFML_VERSION_MAJOR < 2 )
	Sound.Pause();
	Sound.SetPlayingOffset( (f32)time_index_in_ms / (f32)getDuration() );
	Sound.Play();
	# else
	Sound.pause();
	Sound.setPlayingOffset( sf::milliseconds( time_index_in_ms ) );
	Sound.play();
	# endif
}


f32 CAudioPlayerSFML::getVolume() const
{
    return Volume;
}

void CAudioPlayerSFML::setVolume( f32 value )
{
	dbPRINT( "CAudioPlayerSFML::setVolume(%f)\n", value );

    if (value < 0.0f)
        value = 0.0f;
    if (value > 1.0f)
        value = 1.0f;

    Volume = value;

	# if ( SFML_VERSION_MAJOR < 2 )
		Sound.SetVolume( 100.0f*Volume );
	# else
		Sound.setVolume( 100.0f*Volume );
	# endif
}

f32 CAudioPlayerSFML::getPitch() const
{
    return Pitch;
}

void CAudioPlayerSFML::setPitch( f32 value )
{
	dbPRINT( "CAudioPlayerSFML::setPitch(%f)\n", value );

    if (value < 0.001f)
        value = 0.001f;
    if (value > 1000.0f)
        value = 1000.0f;

    Pitch = value;

	#if ( SFML_VERSION_MAJOR < 2 )
	Sound.SetPitch( Pitch );
	#else
	Sound.setPitch( Pitch );
	#endif
}


f32 CAudioPlayerSFML::getPan() const
{
    return Pan;
}

void CAudioPlayerSFML::setPan( f32 value )
{
	dbPRINT( "CAudioPlayerSFML::setPan(%f)\n", value );

    Pan = core::clamp<f32>( value, -1.0f, 1.0f);

	#if ( SFML_VERSION_MAJOR < 2 )

	#else

	#endif
}


bool CAudioPlayerSFML::isPlaying() const
{
	return (Sound.getStatus() == sf::Sound::Playing);
}

bool CAudioPlayerSFML::isLooped() const
{
	return Sound.getLoop();
}

void CAudioPlayerSFML::mute()
{
	dbPRINT( "CAudioPlayerSFML::mute()\n" );

	#if ( SFML_VERSION_MAJOR < 2 )
	Sound.Stop();
	#else
	Sound.stop();
	#endif
}

void CAudioPlayerSFML::panic()
{
	dbPRINT( "CAudioPlayerSFML::panic()\n" );

	# if ( SFML_VERSION_MAJOR < 2 )
	Sound.Stop();
	# else
	Sound.stop();
	# endif
}

void CAudioPlayerSFML::stop()
{
	dbPRINT( "CAudioPlayerSFML::stop()\n" );

	# if ( SFML_VERSION_MAJOR < 2 )
	Sound.Stop();
	# else
	Sound.stop();
	# endif
}

void CAudioPlayerSFML::play( )
{
	dbPRINT( "CAudioPlayerSFML::play()\n" );

	#if ( SFML_VERSION_MAJOR < 2 )
	Sound.SetVolume( 100.0f*Volume );
	Sound.SetPitch( Pitch );
	Sound.SetLoop( IsLooped );
	Sound.Play();
	#else
	Sound.setVolume( 100.0f*Volume );
	Sound.setPitch( Pitch );
	Sound.setLoop( IsLooped );
	Sound.play();
	#endif
}

void CAudioPlayerSFML::pause()
{
	dbPRINT( "CAudioPlayerSFML::pause()\n" );

	#if ( SFML_VERSION_MAJOR < 2 )
		Sound.Pause();
	#else
		Sound.pause();
	#endif
}

void CAudioPlayerSFML::resume( )
{
	dbPRINT( "CAudioPlayerSFML::resume()\n" );

	#if ( SFML_VERSION_MAJOR < 2 )
	Sound.Play();
	#else
	Sound.play();
	#endif
}

void CAudioPlayerSFML::next( )
{
	dbPRINT( "CAudioPlayerSFML::next()\n" );
}

void CAudioPlayerSFML::prev( )
{
	dbPRINT( "CAudioPlayerSFML::prev()\n" );
}

void CAudioPlayerSFML::rewind( )
{
	dbPRINT( "CAudioPlayerSFML::rewind()\n" );
}

void CAudioPlayerSFML::forward( )
{
	dbPRINT( "CAudioPlayerSFML::forward()\n" );
}

/// PlayListControl

bool CAudioPlayerSFML::clearPlayList()
{
	stop();
	PlayList.clear();
	return true;
}

u32 CAudioPlayerSFML::getPlayListItemCount() const
{
    return PlayList.size();
}

bool CAudioPlayerSFML::loadPlayList( const core::stringc& filename )
{
	dbPRINT( "CAudioPlayerSFML::loadPlayList(%s)\n", filename.c_str() );
	return true;
}

bool CAudioPlayerSFML::savePlayList( const core::stringc& filename )
{
	dbPRINT( "CAudioPlayerSFML::savePlayList(%s)\n", filename.c_str() );
	return true;
}

/// AudioDeviceControl

core::stringc CAudioPlayerSFML::getAPIName( ) const
{
	core::stringc txt = "";

	txt += "SFML-";
	txt += SFML_VERSION_MAJOR;
	txt += ".";
	txt += SFML_VERSION_MINOR;
//	#if (SFML_VERSION_MAJOR < 2)
//		txt += "-1.6";
//	#else
//		txt += "-2.1";
//	#endif
    txt += " + FFTW3";
	return txt;
}

bool CAudioPlayerSFML::EnumerateDevices( )
{
	dbPRINT( "CAudioPlayerSFML::EnumerateDevices()\n" );

	# if ( SFML_VERSION_MAJOR < 2 )

	# else

	# endif
	return true;
}

bool CAudioPlayerSFML::setAudioDevice( u32 card, u32 sub_device )
{
	dbPRINT( "CAudioPlayerSFML::setAudioDevice()\n" );
	return true;
}

u32 CAudioPlayerSFML::getAudioDeviceCount( ) const
{
    return 0;
}

s32 CAudioPlayerSFML::getDefaultAudioDevice( ) const
{
    return 0;
}

core::stringc CAudioPlayerSFML::getAudioDeviceName( u32 card, u32 sub_device ) const
{
	core::stringc name = "unknown";
    return name;
}

/// EffectControl

bool CAudioPlayerSFML::setReverbEffect( bool bEnable, f32 value )
{
	return false;
}

bool CAudioPlayerSFML::setEchoEffect( bool bEnable, f32 value )
{
	return false;
}

bool CAudioPlayerSFML::setFlangerEffect( bool bEnable, f32 value )
{
	return false;
}

bool CAudioPlayerSFML::setCompressorEffect( bool bEnable, f32 value )
{
	return false;
}

bool CAudioPlayerSFML::setDistortionEffect( bool bEnable, f32 value )
{
	return false;
}

bool CAudioPlayerSFML::setChorusEffect( bool bEnable, f32 value )
{
	return false;
}

void CAudioPlayerSFML::getSamples( core::array<s16>& container, u32 numSamples, u32 timeStart, u32 channelIndex )
{
	if (numSamples==0) return;

	if (container.allocated_size() < numSamples)
		container.reallocate( numSamples );

	const u32 sample_rate = getSampleRate();
	const u32 sample_count = getSampleCount();
	const u32 channel_count = getChannelCount();
	const u32 duration = getDuration();

	if ( channelIndex >= channel_count ) return;
	if ( timeStart >= duration ) return;

	const s16* p_start = Buffer.getSamples();
	const s16* p_end = p_start + sample_count;

	u32 sample_start = core::round32( (f32)sample_count * (f32)timeStart / (f32)duration );
	sample_start -= sample_start % channel_count;
	sample_start += channelIndex;

	/// From here there is no turning back, meaning
	/// the container is set to size zero ( no reallocation involved )
	container.set_used( 0 );

	s16* p = const_cast<s16*>(p_start) + sample_start;

	// fill array with samples
	u32 i=0;
	for ( ; i<numSamples; i++)
	{
		p += channel_count;

		if ( p >= p_end ) break;

		container.push_back( s16(*p) );
	}

	if (i<numSamples)
	{
		for (u32 j=i; j<numSamples; j++)
		{
			container.push_back( 0 );
		}
	}
}

void CAudioPlayerSFML::getSamples( core::array<f32>& container, u32 numSamples, u32 timeStart, u32 channelIndex )
{
	if (numSamples==0) return;

	if (container.allocated_size() < numSamples)
		container.reallocate( numSamples );

	const u32 sample_rate = getSampleRate();
	const u32 sample_count = getSampleCount();
	const u32 channel_count = getChannelCount();
	const u32 duration = getDuration();

	if ( channelIndex >= channel_count ) return;
	if ( timeStart >= duration ) return;

	const s16* p_start = Buffer.getSamples();
	const s16* p_end = p_start + sample_count;

	u32 sample_start = core::round32( (f32)sample_count * (f32)timeStart / (f32)duration );
	sample_start -= sample_start % channel_count;
	sample_start += channelIndex;

	/// From here there is no turning back, meaning
	/// the container is set to size zero ( no reallocation involved )
	container.set_used( 0 );

	s16* p = const_cast<s16*>(p_start) + sample_start;

	// fill array with samples
	u32 i=0;
	for ( ; i<numSamples; i++)
	{
		p += channel_count;

		if ( p >= p_end ) break;

		container.push_back( f32(*p) );
	}

	if (i<numSamples)
	{
		for (u32 j=i; j<numSamples; j++)
		{
			container.push_back( 0.0f );
		}
	}
}

} // end namespace irr

#endif // _IRR_COMPILE_WITH_SFML_AUDIO_
