//// Copyright (C) 2002-2014 Benjamin Hampe
//// This file is part of the "irrlicht-engine"
//// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_C_GUI_AUDIO_PLAYER_H__
#define __IRR_EXT_C_GUI_AUDIO_PLAYER_H__

#include <CompileConfig.h>

#include <libAudio/IAudioPlayer.h>

namespace irr
{
namespace gui
{

class CGUIAudioPlayer : public IGUIElement
{
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
	core::recti PreviewRect;
	io::path PreviewTexName;
	//video::ITexture* PreviewTexture;

	bool IsMouseOver;
	bool IsMouseClicked;
	core::position2di MousePos;

protected:

	bool createPreviewTexture();
	bool OnSetPlayPosition( f32 seconds );

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

	virtual IAudioPlayer* getAudioPlayer();

	core::stringw getTrackInfo();

	void setPlayer( IAudioPlayer* player );

	bool loadFile( const core::stringc& filename = "" );

	bool closeFile();

	void play();
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
