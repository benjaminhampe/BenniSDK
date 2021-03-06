// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_C_AUDIO_PLAYER_SFML_H__
#define __IRR_EXT_C_AUDIO_PLAYER_SFML_H__

#include <CompileConfig.h>

#ifdef _IRR_COMPILE_WITH_SFML_AUDIO_

#include "IAudioPlayer.h"

namespace irr
{

class CAudioPlayerSFML : public IAudioPlayer
{
private:
	sf::Sound* Sound;
	sf::SoundBuffer* Buffer;

public:
	/// @brief default constructor
	CAudioPlayerSFML()
		: IAudioPlayer(), Sound(0), Buffer(0)
	{
		dbPRINT( "CAudioPlayerSFML::CAudioPlayerSFML()\n" );
		setup();
	}

	/// @brief destructor
	virtual ~CAudioPlayerSFML()
	{
		dbPRINT( "CAudioPlayerSFML::~CAudioPlayerSFML()\n" );
		clear();
	}

	bool setup()
	{
		Sound = new sf::Sound();
	}

	bool clear()
	{
		if (Sound)
		{
			delete Sound;
			Sound = 0;
		}

		if (Buffer)
		{
			delete Buffer;
			Buffer = 0;
		}

		IsLoaded = false;
		IsPlaying = false;
		IsLooped = false;
		IsShuffled = false;
	}

	/// AudioDevice Control
	virtual core::stringc getAPIName( ) const
	{
		core::stringc txt = "SFML-";
		txt += SFML_VERSION_MAJOR;
		txt += ".";
		txt += SFML_VERSION_MINOR;
		return txt;
	}

	/// @brief Writable access to SoundBuffer
	virtual sf::SoundBuffer* getSoundBuffer()
	{
		return Buffer;
	}

	/// @brief Read-only access to SoundBuffer
	virtual const sf::SoundBuffer* getSoundBuffer() const
	{
		return Buffer;
	}

	/// @brief Access to SoundBuffer's internal sample-buffer
	virtual const s16* getSamples()
	{
		if (Buffer)
		# if ( SFML_VERSION_MAJOR < 2 )
			return Buffer->GetSamples();
		# else
			return Buffer->getSamples();
		# endif
		else
			return 0;
	}

	/// @brief
	virtual u32 getSampleRate() const
	{
		if (!Buffer)
			return 0;
	#if ( SFML_VERSION_MAJOR < 2 )
		return Buffer->GetSampleRate();
	#else
		return Buffer->getSampleRate();
	#endif
	}

	/// @brief
	virtual size_t getSampleCount() const
	{
		if (!Buffer)
			return 0;
	#if ( SFML_VERSION_MAJOR < 2 )
		return Buffer->GetSamplesCount();
	#else
		return Buffer->getSampleCount();
	#endif
	}

	/// @brief
	virtual u32 getChannelCount( ) const
	{
		if (!Buffer)
			return 0;
	#if ( SFML_VERSION_MAJOR < 2 )
		return Buffer->GetChannelsCount();
	#else
		return Buffer->getChannelCount();
	#endif
	}

	/// @brief This is a workaround for the obviously not working SFML pendant.
	virtual u32 getDuration() const
	{
		if (!Buffer)
			return 0;

	#if ( SFML_VERSION_MAJOR < 2 )
		const f32 cc = (f32)Buffer->GetChannelsCount();
		const f32 sr = (f32)Buffer->GetSampleRate();
		const f32 sc = (f32)Buffer->GetSamplesCount();
	#else
		const f32 cc = (f32)Buffer->getChannelCount();
		const f32 sr = (f32)Buffer->getSampleRate();
		const f32 sc = (f32)Buffer->getSampleCount();
	#endif
		return (u32)core::floor32( ( 1000.f * sc ) / (cc * sr) );
	}

	/// @brief This is a workaround for the obviously not working SFML pendant.
	virtual f32 getDurationAsSeconds() const
	{
		if (!Buffer)
			return 0;

	#if ( SFML_VERSION_MAJOR < 2 )
		const f32 cc = (f32)Buffer->GetChannelsCount();
		const f32 sr = (f32)Buffer->GetSampleRate();
		const f32 sc = (f32)Buffer->GetSamplesCount();
	#else
		const f32 cc = (f32)Buffer->getChannelCount();
		const f32 sr = (f32)Buffer->getSampleRate();
		const f32 sc = (f32)Buffer->getSampleCount();
	#endif
		return sc / (cc * sr);
	}

	virtual u32 getPosition()
	{
		if (!Sound)
			return 0;
	#if ( SFML_VERSION_MAJOR < 2 )
		return Sound->GetPlayingOffset();
	#else
		return Sound->getPlayingOffset().asMilliseconds();
	#endif
	}

	virtual f32 getPositionAsSeconds()
	{
		if (!Sound)
			return 0;
	#if ( SFML_VERSION_MAJOR < 2 )
		return Sound->GetPlayingOffset();
	#else
		return Sound->getPlayingOffset().asSeconds();
	#endif
	}

	virtual void setPosition( u32 time_index_in_ms )
	{
		dbPRINT( "CAudioPlayerSFML::setPosition(%d)\n", time_index_in_ms );

		if (!Sound)
			return;
	#if ( SFML_VERSION_MAJOR < 2 )
		Sound->SetPlayingOffset( (f32)time_index_in_ms / (f32)getDuration() );
	#else
		Sound->setPlayingOffset( sf::milliseconds( time_index_in_ms ) );
	#endif
	}

	virtual void setPositionAsSeconds( f32 seconds )
	{
		dbPRINT( "CAudioPlayerSFML::setPositionAsSeconds(%f)\n", seconds );

		if (!Sound)
			return;
	#if ( SFML_VERSION_MAJOR < 2 )
		Sound->SetPlayingOffset( seconds / (f32)getDurationAsSeconds() );
	#else
		Sound->setPlayingOffset( sf::seconds( seconds ) );
	#endif
	}

	/// @brief load a sound from given filename
	virtual bool loadFile( const core::stringc& filename )
	{
		dbPRINT( "CAudioPlayerSFML::loadFile(%s)\n", filename.c_str() );

		///@todo add some check for same filename here

		sf::SoundBuffer* bgBuffer = new sf::SoundBuffer();
		if (!bgBuffer)
			return false;

	#if ( SFML_VERSION_MAJOR < 2 )
		if ( !bgBuffer->LoadFromFile( filename.c_str()) )
	#else
		if ( !bgBuffer->loadFromFile( filename.c_str()) )
	#endif
		{
			delete bgBuffer;
			return false;
		}

		if (Buffer)
		{
			stop();
		}

		if (Sound)
		{
		#if ( SFML_VERSION_MAJOR < 2 )
			Sound->SetBuffer( *bgBuffer );
		#else
			Sound->setBuffer( *bgBuffer );
		#endif
		}

		if (Buffer)
		{
			delete Buffer;
		}

		Buffer = bgBuffer;
		FileName = filename;
		IsLoaded = true;

		return (Sound->getBuffer() == Buffer);
	}

	/// @brief save a sound to given filename
	virtual bool saveFile( const core::stringc& filename )
	{
		dbPRINT( "CAudioPlayerSFML::saveFile(%s)\n", filename.c_str() );
		return true;
	}

	virtual E_AUDIO_PLAY_STATUS getStatus() const
	{
		E_AUDIO_PLAY_STATUS status = EAPS_ERROR;

		if (!Sound)
			return status;

		switch (Sound->getStatus())
		{
			case sf::Sound::Playing: status = EAPS_PLAYING; break;
			case sf::Sound::Paused: status = EAPS_PAUSED; break;
			case sf::Sound::Stopped: status = EAPS_STOPPED; break;
			default:
				break;
		}
		return status;
	}

	/// @brief
	virtual bool isPlaying() const
	{
		if (!Sound)
			return false;

		return (Sound->getStatus() == sf::Sound::Playing);
	}

	/// @brief
	virtual bool isLooped() const
	{
		if (!Sound)
			return Sound->getLoop();
	}

	/// @brief
	virtual void stop()
	{
		dbPRINT( "CAudioPlayerSFML::stop()\n" );

		if (!Sound)
			return;

	#if ( SFML_VERSION_MAJOR < 2 )
		Sound->Stop();
	#else
		Sound->stop();
	#endif
	}

	/// @brief
	virtual void mute()
	{
		dbPRINT( "CAudioPlayerSFML::mute()\n" );

		if (!Sound)
			return;

		setVolume( 0 );
	}

	/// @brief
	virtual void panic()
	{
		dbPRINT( "CAudioPlayerSFML::panic()\n" );
		stop();
	}

	/// @brief
	virtual void play()
	{
		dbPRINT( "CAudioPlayerSFML::play()\n" );
		if (!Sound)
			return;

	#if ( SFML_VERSION_MAJOR < 2 )
//		Sound->SetVolume( 100.0f*Volume );
//		Sound->SetPitch( Pitch );
//		Sound->SetLoop( IsLooped );
		Sound->Play();
	#else
//		Sound->setVolume( 100.0f*Volume );
//		Sound->setPitch( Pitch );
//		Sound->setLoop( IsLooped );
		Sound->play();
	#endif
	}

	/// @brief
	virtual void pause()
	{
		dbPRINT( "CAudioPlayerSFML::pause()\n" );

		if (!Sound)
			return;
	#if ( SFML_VERSION_MAJOR < 2 )
		Sound->Pause();
	#else
		Sound->pause();
	#endif
	}

	/// @brief
	virtual void resume()
	{
		dbPRINT( "CAudioPlayerSFML::resume()\n" );

		if (!Sound)
			return;
	#if ( SFML_VERSION_MAJOR < 2 )
		Sound->Play();
	#else
		Sound->play();
	#endif
	}

	/// @brief
	virtual void seek( s32 timeInMillis )
	{
		if (timeInMillis)
		{
			// substract time from Position
		}
		else
		{
			// add time to Position
		}
	}

	virtual void setVolume( f32 value = 0.5f )
	{
		dbPRINT( "CAudioPlayerSFML::setVolume(%f)\n", value );

		Volume = core::clamp<f32>( 100.0f*value, 0.0f, 100.0f );

		if (!Sound)
			return;
	#if ( SFML_VERSION_MAJOR < 2 )
		Sound->SetVolume( Volume );
	#else
		Sound->setVolume( Volume );
	#endif
	}

	virtual void setPitch( f32 value = 1.0f )
	{
		dbPRINT( "CAudioPlayerSFML::setPitch(%f)\n", value );

		Pitch = core::clamp<f32>( value, -1000.0f, 1000.0f );

		if (!Sound)
			return;
	#if ( SFML_VERSION_MAJOR < 2 )
		Sound->SetPitch( Pitch );
	#else
		Sound->setPitch( Pitch );
	#endif
	}

	virtual void setPan( f32 value = 0.0f )
	{
		dbPRINT( "CAudioPlayerSFML::setPan(%f)\n", value );

		Pan = core::clamp<f32>( value, -1.0f, 1.0f);

		if (!Sound)
			return;
		#if ( SFML_VERSION_MAJOR < 2 )

		#else

		#endif
	}

public:

	void getSamples( core::array<s16>& container, u32 nSamples, u32 timeStart, u32 channelIndex )
	{
		if (nSamples==0) return;

		const s16* p_start = getSamples();
		if (!p_start)
		{
			dbERROR( "No valid pointer to sf::SoundBuffer\n" )
			return;
		}

		if (container.allocated_size() < nSamples)
			container.reallocate( nSamples );

		const u32 sample_rate = getSampleRate();
		const u32 sample_count = getSampleCount();
		const u32 channel_count = getChannelCount();
		const u32 duration = getDuration();

		if ( channelIndex >= channel_count ) return;
		if ( timeStart >= duration ) return;

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
		for ( ; i<nSamples; i++)
		{
			p += channel_count;

			if ( p >= p_end ) break;

			container.push_back( *p );
		}

		if (i<nSamples)
		{
			const s16 zero(0);
			for (u32 j=i; j<nSamples; j++)
			{
				container.push_back( zero );
			}
		}
	}

	void getSamples( core::array<f32>& container, u32 nSamples, u32 timeStart, u32 channelIndex )
	{
		if (nSamples==0) return;

		const s16* p_start = getSamples();
		if (!p_start)
		{
			dbERROR( "No valid pointer to sf::SoundBuffer\n" )
			return;
		}

		if (container.allocated_size() < nSamples)
			container.reallocate( nSamples );

		const u32 sample_rate = getSampleRate();
		const u32 sample_count = getSampleCount();
		const u32 channel_count = getChannelCount();
		const u32 duration = getDuration();

		if ( channelIndex >= channel_count ) return;
		if ( timeStart >= duration ) return;

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
		for ( ; i<nSamples; i++)
		{
			p += channel_count;

			if ( p >= p_end ) break;

			container.push_back( f32(*p) );
		}

		if (i<nSamples)
		{
			const f32 zero(0);
			for (u32 j=i; j<nSamples; j++)
			{
				container.push_back( zero );
			}
		}
	}

	bool getSamples( core::array<s16>& container, u32 nSamples, u32 channel_index, f32 time_start, f32 time_end )
	{
		if (!Buffer)
			return false;

		if ( nSamples == 0 )
			return false;

		const f32 duration = getDurationAsSeconds();
		const u32 sr = getSampleRate();
		const u32 sc = getSampleCount();
		const u32 cc = getChannelCount();

		if ( duration <= time_start )
			return false;

		if ( time_end > duration )
			time_end = duration;

		if ( time_end - time_start <= core::ROUNDING_ERROR_f32 )
			return false;

		if ( sc == 0 )
			return false;

		if ( cc == 0 )
			return false;

		if ( channel_index >= cc )
			return false;

		container.reallocate( nSamples );
		container.set_used( 0 );

		const f32 time_delta = (time_end - time_start) / (f32)(nSamples);
		const s16* buffer_start = getSamples();

		for ( u32 i=0; i<nSamples; i++)
		{
			u32 sample_delta = core::floor32( (f32)sc* ((time_start + time_delta*i) / duration) );

			sample_delta -= sample_delta % cc;

			sample_delta += channel_index;

			if ( sample_delta >= sc )
				break;

			s16* p = const_cast<s16*>(buffer_start);

			p += sample_delta;

			container.push_back( s16(*p) );
		}
	}

//	virtual bool EnumerateDevices( );
//
//	virtual u32 getAudioDeviceCount( ) const;
//
//	virtual s32 getDefaultAudioDevice( ) const;
//
//	virtual io::path getAudioDeviceName( u32 card = 0, u32 sub_device = 0) const;
//
//	virtual bool setAudioDevice( u32 card = 0, u32 sub_device = 0 );

};

}

#endif // _IRR_COMPILE_WITH_SFML_AUDIO_

#endif // __IRR_EXT_C_AUDIO_PLAYER_SFML_H__
