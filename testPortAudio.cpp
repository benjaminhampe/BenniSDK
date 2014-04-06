/**
	testSFML_recording.cpp
*/

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

#include <irrlicht.h>
#include <core.h>
#include <portaudio.h>
#include <locale.h>

using namespace irr;
using namespace std;

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_NONE, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_NONE, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void printErrorAndTerminate( PaError e )
{
    Pa_Terminate();
    dbERROR( "An error occured while using the portaudio stream\n" );
    dbERROR( "Error number: %d\n", e );
    dbERROR( "Error message: %s\n", Pa_GetErrorText( e ) );
}

const c8* PaDeviceType[] = { "Input", "Output", "Duplex", "" };

const c8* Pa_GetDeviceType( PaDeviceIndex index )
{
	if (index >= Pa_GetDeviceCount())
		return PaDeviceType[3];

	const PaDeviceInfo* info = Pa_GetDeviceInfo( index );
	if (info->maxInputChannels > 0)
		if (info->maxOutputChannels > 0)
			return PaDeviceType[2];
		else
			return PaDeviceType[0];
	else
		if (info->maxOutputChannels > 0)
			return PaDeviceType[1];
		else
			return PaDeviceType[3];
}

void PaPrintHostApis()
{
	const PaHostApiIndex defaultApi = Pa_GetDefaultHostApi();
	const PaHostApiIndex apiCount = Pa_GetHostApiCount();

	dbPRINT( "DefaultHostApi = %d\n", defaultApi )
	dbPRINT( "HostApiCount = %d\n", apiCount )

	for ( PaHostApiIndex apiIndex = 0; apiIndex < apiCount; apiIndex++ )
	{
		const PaHostApiInfo* apiInfo = Pa_GetHostApiInfo( apiIndex );
		if ( apiInfo )
		{
			dbPRINT( "HostApi[%d] = %s, id=%d", apiIndex, apiInfo->name, apiInfo->type )

			for ( s32 i = 0; i < apiInfo->deviceCount; i++ )
			{
				PaDeviceIndex devIndex = Pa_HostApiDeviceIndexToDeviceIndex( apiIndex, i );

				dbPRINT( " [%d]", i)
			}

			dbPRINT( "\n" );
		}
	}
}

void PaPrintDeviceInfos()
{
	const PaDeviceIndex devCount = Pa_GetDeviceCount();
	const PaDeviceIndex defaultInput = Pa_GetDefaultInputDevice();
	const PaDeviceIndex defaultOutput = Pa_GetDefaultOutputDevice();

	dbPRINT( "DeviceCount = %d\n", devCount )
	dbPRINT( "DefaultInputDevice = %d\n", defaultInput )
	dbPRINT( "DefaultOutputDevice = %d\n", defaultOutput )

	for ( PaDeviceIndex i = 0; i < devCount; i++ )
	{
		const PaDeviceInfo* devInfo = Pa_GetDeviceInfo( i );
		if (devInfo)
		{
			//std::wstring devName( utf8_decode( std::string(devInfo->name) );
			std::string devName = devInfo->name;

			dbPRINT( "[%d] name=%s, type=%s, sr=%.0lf\n" \
				"in_ch = %d, out_ch = %d\n" \
				"low_in_lat = %.1lfms, low_out_lat=%.1lfms\n" \
				"high_in_lat = %.1lfms, high_out_lat=%.1lfms\n",
				i, devName.c_str(), Pa_GetDeviceType(i), devInfo->defaultSampleRate,
				devInfo->maxInputChannels, devInfo->maxOutputChannels,
				1000.0*devInfo->defaultLowInputLatency, 1000.0*devInfo->defaultLowOutputLatency,
				1000.0*devInfo->defaultHighInputLatency, 1000.0*devInfo->defaultHighOutputLatency )
		}
	}

}

void testPortAudio()
{
	PaError e = Pa_Initialize();
	if( e != paNoError )
		printErrorAndTerminate(e);


	PaPrintHostApis();
	PaPrintDeviceInfos();

    Pa_Terminate();
    dbPRINT("Test finished.\n");
    return;
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

	testPortAudio();

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
