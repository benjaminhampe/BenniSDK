/**
	testRtAudio.cpp
*/

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

#include <irrlicht.h>
#include <core.h>

#include <locale.h>

using namespace irr;
using namespace std;

void testRtAudio()
{

}

/*
	The main function.

	Demonstrates a realtime Visualization of captured AudioSignal from SoundCard

	Visualizes the captured Waveforms and PowerFrequencySpectrums

	Seems to be Mono for now

*/
s32 main( s32 argc, c8** argv )
{
	dbPRINT( "Hällö Würld\n" )
	//std::locale::global(std::locale(""));
	//std::locale::global(std::locale("DE_de"));

	setlocale( LC_ALL, "de_DE" );

	dbPRINT( "Hällö Würld\n" )

//	setlocale( LC_ALL, NULL );

	dbPRINT( "Hällö Würld\n" )

	testRtAudio();

//	IrrlichtDevice* device = createOpenGlDevice( -100, -200, 0, false, false);
//	video::IVideoDriver* driver = device->getVideoDriver();
//	gui::IGUIEnvironment* env = device->getGUIEnvironment();
//	scene::ISceneManager* smgr = device->getSceneManager();
//	core::dimension2du ScreenSize = driver->getScreenSize();
//
//	//gui::IGUISkin* skin = env->createSkin( gui::EGST_BURNING_SKIN );
//	//gui::IGUISkin* skin = env->createSkin( gui::EGST_WINDOWS_CLASSIC );
//	gui::IGUISkin* skin = env->createSkin( gui::EGST_WINDOWS_METALLIC );
//	if (skin)
//	{
//		env->setSkin(skin);
//	}
//
////	gui::IGUIWindow* win = createWindow( env, L"Realtime-Monitor for AudioSignals",
////		100,100, ScreenSize.Width-200,ScreenSize.Height-200);
////
////	gui::CGUIAudioDisplaySFML* rtmon = new gui::CGUIAudioDisplaySFML(
////		44100, 4*1024, env, win, -1, win->getClientRect());
////
//	video::ITexture* backdrop = driver->getTexture( "../../media/background/city.jpg" );
//
//	runDeviceWithFrameLimit( device, 30, backdrop );

	return 0;
}
