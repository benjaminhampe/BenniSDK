//// Copyright (C) 2002-2014 Benjamin Hampe
//// This file is part of the "irrlicht-engine"
//// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_C_GUI_AUDIO_PLAYER_H__
#define __IRR_EXT_C_GUI_AUDIO_PLAYER_H__

#include <CompileConfig.h>

#ifdef _IRR_COMPILE_WITH_FLTK_
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#endif // _IRR_COMPILE_WITH_FLTK_

#include <libAudio/IAudioPlayer.h>

namespace irr
{
namespace gui
{

class CGUIAudioPlayer : public IGUIElement
{

protected:

private:
	gui::IGUIFont* Font;	// big font for drawing filename
	IAudioPlayer* Player;
	IGUIStaticText* TrackName;
	IGUIStaticText* TrackInfo;
	IGUIButton* LoadButton;		// Load File
	IGUIButton* PlayButton;		// press play
	IGUIButton* PauseButton;		// press pause
	IGUIButton* StopButton;		// press stop
	IGUIButton* RewindButton;		// press rewind
	IGUIButton* ForwardButton;		// press forward
	IGUIButton* PrevButton;		// press previous track
	IGUIButton* NextButton;		// press next track
	// IGUIButton* RecordButton;
	IGUISpinBox* MasterVolume;	// MasterVolume ( Loudness ) 0.0f ... 1.0f
	IGUISpinBox* MasterPan;		// MasterPan ( Centered Sound at 0.0f, -1.0f ... 1.0f
	IGUISpinBox* MasterPitch;		// MasterPitch ( PlaySpeed ) -100.0f ... 100.0f
	IGUICheckBox* ChkMute;			// Mute ( MasterVolume = 0, panic button )
	IGUICheckBox* ChkLoopMode;		// LoopMode	( playlist )
	IGUICheckBox* ChkShuffleMode;	// ShuffleMode ( playlist )
	core::recti PlayPositionRect;
	video::ITexture* PlayPositionTexture;
public:

	//! value constructor
	CGUIAudioPlayer( IAudioPlayer* player, IGUIEnvironment* env,
		IGUIElement* parent, s32 id, core::rect<s32> rectangle );

	//! destructor
	virtual ~CGUIAudioPlayer();

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event);

	//! draws the element and its children
	virtual void draw();

	virtual IAudioPlayer* getAudioPlayer()
	{
		return Player;
	}

	core::stringw getTrackInfo()
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

	void setPlayer( IAudioPlayer* player )
	{
		Player = player;

		if (!player)
		{
			if (TrackName)
				TrackName->setText( L"Pointer to IAudioPlayer is zero\n");
		}
		else
		{
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
		}
	}
	virtual bool loadFile( const core::stringc& filename = "" )
	{
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

		if (!Player)
		{
			dbERROR("Invalid pointer to Player\n")
			return false;
		}

		if (!Player->loadFile( myFilename ))
		{
		#ifdef _IRR_COMPILE_WITH_FLTK_
			core::stringc txt = "Error, could not open file ";
			txt += myFilename;
			txt += ".\nMaybe the file is broken or the extension is not supported.";
			fl_alert( txt.c_str() );
		#endif
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

		return true;
	}

	virtual bool closeFile()
	{
		if (!Player)
			return false;

		Player->stop();
	}

//	//! Writes attributes of the element.
//	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;
//
//	//! Reads attributes of the element
//	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);
//
//	///@brief create image from soundbuffer-waveform ( 16bit per sample * channel_count * sample_rate = bits per sec = 8*bytes per sec )
//	virtual video::IImage* createImageFromSoundBufferWaveform( const core::dimension2du& img_size, const video::SColor& clearColor = video::SColor(64,0,0,0) ) const;
//
//	/// create image from FFT-PowerSpectrum
//	virtual video::IImage* createImageFromSoundBufferPowerSpectrum( const core::dimension2du& img_size ) const;

};

} // end namespace gui
} // end namespace irr

#endif // __IRR_EXT_C_GUI_AUDIO_PLAYER_H__
