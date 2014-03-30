/**
	testSFML_recording.cpp
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
#include <libAudio/goertzel.h>
#include <libAudio/irrAudio.h>
#include <libAudio/CGUIAudioDisplaySFML.h>

#include <../source/Irrlicht/os.cpp>
#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CMeshSceneNode.cpp>
#include <../source/Irrlicht/CShadowVolumeSceneNode.cpp>

using namespace irr;
using namespace std;

/*
	The main function.

	Demonstrates a realtime Visualization of captured AudioSignal from SoundCard

	Visualizes the captured Waveforms and PowerFrequencySpectrums

	Seems to be Mono for now

*/
s32 main( s32 argc, c8** argv )
{
	PRINT_INFO();

	IrrlichtDevice* device = createIrrlichtDevice( -100, -200, 0, false, false);
	video::IVideoDriver* driver = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	scene::ISceneManager* smgr = device->getSceneManager();
	core::dimension2du ScreenSize = driver->getScreenSize();

	//gui::IGUISkin* skin = env->createSkin( gui::EGST_BURNING_SKIN );
	//gui::IGUISkin* skin = env->createSkin( gui::EGST_WINDOWS_CLASSIC );
	gui::IGUISkin* skin = env->createSkin( gui::EGST_WINDOWS_METALLIC );
	if (skin)
	{
		env->setSkin(skin);
	}

	gui::IGUIWindow* win = createWindow( env, L"Realtime-Monitor for AudioSignals",
		100,100, ScreenSize.Width-200,ScreenSize.Height-200);

	gui::CGUIAudioDisplaySFML* rtmon = new gui::CGUIAudioDisplaySFML(
		44100, 4*1024, env, win, -1, win->getClientRect());

	video::ITexture* backdrop = driver->getTexture( "../../media/background/city.jpg" );

	runDeviceWithFrameLimit( device, 30, backdrop );

	return 0;
}
