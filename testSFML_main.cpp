/**
	testMath.cpp
*/

#include <iostream>
#include <fstream>
#include <cmath>

#include <irrlicht.h>
#include <CompileConfig.h>
#include <libImage/CMath.h>
#include <libImage/IColorGradient.h>
#include <libImage/CLinearColorGradient.h>
#include <libAudio/FourierTransformRtoC.h>
#include <libAudio/irrAudio.h>

#include <libAudio/IAudioPlayer.h>
#include <libAudio/CAudioPlayerSFML.h>
#include <libAudio/CGUIAudioPlayer.h>
#include <libAudio/CGUIAudioEQGraph.h>

#ifdef _DEBUG
#include <../source/Irrlicht/os.cpp>
#endif // _DEBUG

#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/CColorConverter.cpp>

using namespace irr;
using namespace std;

/*
The main function. Creates all objects and does the XML handling.
*/
s32 main( s32 argc, c8** argv )
{
	PRINT_INFO();

	core::stringc snd_filename = "../../media/music/razor.ogg";

	if (argc>1)
		snd_filename = argv[1];

	sf::SoundBuffer snd_buffer;
	if (!snd_buffer.loadFromFile( snd_filename.c_str() ) )
	{
		dbERROR("Could not open audio-file %s\n", snd_filename.c_str())
		exit(202020);
	}

	dbPRINT( "sizeof(size_t) = %d\n", sizeof(size_t))

	u32 snd_duration = sfx::getDurationFromSoundBufferInMillis( &snd_buffer );
	u32 snd_channels = snd_buffer.getChannelCount();
	u32 snd_sample_rate = snd_buffer.getSampleRate();
	u64 snd_sample_count = snd_buffer.getSampleCount();

	dbPRINT( "snd_filename = %s\n", snd_filename.c_str() )
	dbPRINT( "snd_duration = %d ms\n", snd_duration )
	dbPRINT( "snd_channels = %d%s\n", snd_channels, (snd_channels==2)?" (Stereo)":((snd_channels==1)?" (Mono)":"") )
	dbPRINT( "snd_samplerate = %d Hz\n", snd_sample_rate )
	dbPRINT( "snd_sample_count = %ld\n", snd_sample_count )

	u32 snd_position = 0; // ms

//	sf::Clock timer;
//	u32 timeStart = timer.restart().asMilliseconds();
//	u32 timeNow = timer.getElapsedTime().asMilliseconds();

	u32 snd_fft_size = 1024;
    core::FourierTransformRtoC transform( 1024 );

	u32 snd_fft_in_size = snd_fft_size;
    core::array<s16> snd_fft_input;
    snd_fft_input.reallocate( snd_fft_in_size );
    snd_fft_input.set_used( snd_fft_in_size );

	u32 snd_fft_out_size = 1024;
    core::array<f32> snd_fft_output;
    snd_fft_output.reallocate( snd_fft_out_size );
    snd_fft_output.set_used( snd_fft_out_size );

	sfx::fillSampleBuffer( snd_fft_input, &snd_buffer, 0, 0, snd_fft_in_size );

    transform.setInputData( snd_fft_input );
    transform.fft();
    transform.getPowerSpectrumAsDecibels( snd_fft_output );

	IrrlichtDevice* device = 0;
	video::IVideoDriver* driver = 0;
	gui::IGUIEnvironment* env = 0;
	core::dimension2du ScreenSize(0,0);

	/// draw waveform to image
	video::IColorGradient* gradient = new video::CLinearColorGradient(2);
	gradient->addColor( video::SColor(255,255,0,0),0.0f);
	gradient->addColor( video::SColor(255,0,200,0),0.5f);
	gradient->addColor( video::SColor(255,0,0,255),1.0f);
//	device = createSilentNullDevice();
	device = createIrrlichtDevice( -100, -200, 0, false, false);
	driver = device->getVideoDriver();
	video::IImage* img = driver->createImage( video::ECF_A8R8G8B8, core::dimension2du(1024,768) );
	sfx::drawAudioWaveformFast( img, core::recti(0,0,1023,768/2-1), gradient, &snd_buffer, 0, false);
	sfx::drawAudioWaveformFast( img, core::recti(0,768/2-1,1023,768-1), gradient, &snd_buffer, 1, false);
	driver->writeImageToFile( img, "testSFML_waveform.png");
	img->drop();
	gradient->drop();
//	device->closeDevice();
//	device->drop();
//	device = 0;
//	driver = 0;
	img = 0;
	gradient = 0;

	/// create OpenGL Device ( or similar )
//	device = createIrrlichtDevice();
	driver = device->getVideoDriver();
	env = device->getGUIEnvironment();
	ScreenSize = driver->getScreenSize();

	IAudioPlayer* audioPlayer = new CAudioPlayerSFML();

	dbPRINT( "IAudioPlayer::getAPIName() = %s\n", audioPlayer->getAPIName().c_str());

	audioPlayer->loadFile( "../../media/music/razor.ogg" );

	gui::CGUIAudioEQGraph* leftEQ = new gui::CGUIAudioEQGraph(
		audioPlayer, env, env->getRootGUIElement(), -1,
		core::recti( 0,ScreenSize.Height/2,ScreenSize.Width/2-1, ScreenSize.Height-1));
	leftEQ->setChannel(0);

	gui::CGUIAudioEQGraph* rightEQ = new gui::CGUIAudioEQGraph(
		audioPlayer, env, env->getRootGUIElement(), -1,
		core::recti( ScreenSize.Width/2,ScreenSize.Height/2,ScreenSize.Width-1, ScreenSize.Height-1));
	rightEQ->setChannel(1);
	rightEQ->setLineColor( video::SColor(255,255,0,0));
	rightEQ->setFillColor( video::SColor(255,255,128,0));

	audioPlayer->play();
	runDevice( device );

	//app destroys device in destructor
	return 0;
}
