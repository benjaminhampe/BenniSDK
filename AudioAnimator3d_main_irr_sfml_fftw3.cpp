/// Copyright (C) 2002-2013 Benjamin Hampe
/// This file is part of the "irrlicht-engine"
/// For conditions of distribution and use, see copyright notice in irrlicht.h

/**
 * @brief this program creates animated 3d powerspectrum from audio-files
 *
 * idea found at http://www.youtube.com/watch?v=gbuZVcw3ZiM
 * no copyright infringement intended i just like working with audio-data
 *
 * to compile this program u need fftw3, SFML2.0 Audio, freetype, OpenGL, Irrlicht
 *
 * tested so far only on Windows7 32-bit professional
 * on Core2Duo E6600 2x 2,4 GHz, 2GiB RAM, Ati HD5450 512MiByte VRAM
 * i got 30 FPS so far at FFT-Size 8192 and 8192/16 = 512 Tiles per MatrixRow ( == 2 Triangles )
 */

///@desc			+ load audio-file using SFML 2.0 Audio Library
/// 				+ calculate its PowerSpectrum (0...160 dB) using fftw3 FastFourierTransform
///					  create image from wavform and power-spectrum using CLinearColorGradient
/// 				+ create 2D and 3D animated meshes from power-spectrum
/// each loop:
///					+ get N samples at timeNow - lastRenderTime -> can lead to fluctuations when paused
///					- do some windowing, filtering stuff
///					+ calculate power-spectrum with N samples
///					+ use only first N/16 frequencies to create meshes ( much faster )
///					+ shift matrix one row
///					+ set matrix data at first row( y = row-index = 0 ) from power-spectrum
///					+ create front mesh from power-spectrum
///					+ create matrix mesh
///					+ render power-spectrum as background-texture
///					+ render matrix-mesh
///					+ render front-mesh
///					+ render x,y,z meter ( currently pure OpenGL )
///					+ render GUI
///					+ set WindowCaption with new information
///					+ loop until ESCAPE-Key
///
///	@todo
///					+ Speed improvements! ( pls give advices )
///					- Use assembler to speed up sin/cos ( both can be computed with one instruction )
///					- Use Goertzel's algorithm to speed up FFT at specific frequencies ( tones )
///					- WindowFunctions ( Cos, Gauss, Hamming, Hann, Blackman )
///					  impulse-response functions -> each create different responses in freq-domain
///					- 2D Plot Window ( linear and logarithmic scales ) for current data
///					- 3D Plot Window ( insert CMatrixSceneNode in a IGUIElement )
///					- Equalizer window with user interactive curve-manipulation ( 1-8 point polynom )
///					- do chromatic-tone-pitch estimation
///					- build piano-tuner using microphone

///   +++ Includes +++

// Irrlicht Engine
#include <irrlicht.h>
#include <driverChoice.h>

// LibC
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cwchar>
#include <ctime>
#include <cmath>
#include <getopt.h>

// STL
//#include <iostream>
//#include <iomanip>
//#include <fstream>
//#include <sstream>

// Terminal
//#if defined (_MSC_VER)
//	#include <conio.h>
//#elif !defined(_IRR_WINDOWS_) && defined(__GNUC__)
//	#include <termio.h>
//#endif

// OpenGL API
#include <GL/gl.h>
#include <GL/glu.h>

// SFML Audio
#include <SFML/Audio.hpp>

// Fast Fourier Transforms
#ifdef _IRR_LINUX_PLATFORM_
#include <fftw3.h>
#else
#include <fftw3.h>
#endif

// Irrlicht adds
#include <../source/Irrlicht/CColorConverter.h>
#include <../source/Irrlicht/CImage.h>
#include <../source/Irrlicht/os.h>

// extensions
#include <core.h>
#include <libImage/drawLoadingScreen.h>
#include <libImage/UString.h>
#include <libImage/IGUITTFont.h>
#include <libImage/CGUITTFont.h>
#include <libImage/CMatrix.h>
#include <libImage/sinCosTable.h>
#include <libImage/ColorConstants.h>
#include <libImage/IColorGradient.h>
#include <libImage/CLinearColorGradient.h>
#include <libImage/CLinearColorGradientTable.h>
#include <libImage/renderByPrimitiveType.h>
#include <libImage/Camera.h>
#include <libImage/Geometry.h>
#include <libImage/CMatrixSceneNode.h>
#include <libAudio/FourierTransformRtoC.h>
#include <libAudio/irrAudio.h>
#include <libAudio/IAudioPlayer.h>
#include <libAudio/CAudioPlayerSFML.h>
#include <libAudio/CGUIAudioPlayer.h>

/// Constants

#define MY_TITLE "AudioPlayer with 3d animated FFT-PowerSpectrum (c) 2013 by BenjaminHampe@gmx.de"
#define MY_DEFAULT_AUDIO_FILE "../../media/music/razor.ogg"
#define MY_BIG_FONT_FILE "../../media/fonts/courier22.png"
#define MY_DEFAULT_SKYDOME_TEXTURE "../../media/skydome/iras.jpg"
#define MY_LOAD_SCREEN_IMAGE "../../media/background/loadscreen.jpg"
#define MY_OUTPUTBUFFER_SCALE 20
#define MY_OUTPUTBUFFER_ROWS 64

const irr::core::vector3df MeshSize(1024,2*128,1024);

/// Definitions
//#include <../source/Irrlicht/os.cpp>
//#include <../source/Irrlicht/CImage.cpp>
//#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CMeshSceneNode.cpp>
#include <../source/Irrlicht/CShadowVolumeSceneNode.cpp>


namespace irr
{
	class BasicEventReceiver : public IEventReceiver
	{
	private:
		IrrlichtDevice* Device;
		gui::IGUIButton* WireframeButton;
	public:
		BasicEventReceiver( IrrlichtDevice* device )
		: Device(device), WireframeButton(0)
		{
			if (Device)
				Device->setEventReceiver(this);

			gui::IGUIEnvironment* env = Device->getGUIEnvironment();
			if (env)
			{
				WireframeButton = env->addButton(core::recti(5,5,5+48,5+48), env->getRootGUIElement(), -1, L"Wireframe", L"toggle wireframe");
			}
		}

		virtual ~BasicEventReceiver( )
		{

		}

		virtual bool OnEvent (const SEvent &event)
		{
			if (event.EventType == EET_GUI_EVENT)
			{
				if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED)
				{
					gui::IGUIElement* caller = event.GUIEvent.Caller;
					if (caller)
					{
						if (caller == WireframeButton)
						{
							const core::list<scene::ISceneNode*>& children = Device->getSceneManager()->getRootSceneNode()->getChildren();
							core::list<scene::ISceneNode*>::ConstIterator it = children.begin();
							while ( it != children.end() )
							{
								scene::ISceneNode* node = *it;
								if (node)
								{
									u32 matCount = node->getMaterialCount();
									for (u32 i=0; i<matCount; i++)
									{
										video::SMaterial& matRef = node->getMaterial(i);
										matRef.Wireframe = !matRef.Wireframe;
									}
								}
								it++;
							}
							return true;
						}
					}
				}
			}

			if(event.EventType == EET_KEY_INPUT_EVENT)
			{
				/// exit program if escape key hit
				if(event.KeyInput.Key == KEY_ESCAPE)
				{
					exit(0);//We exit
				}

				/// if print-screen key pressed ( create screenshot image with unique name ( timer based ) and save to hdd )
				if ( (event.KeyInput.Key == KEY_PRINT || event.KeyInput.Key == KEY_SNAPSHOT) && !event.KeyInput.PressedDown)
				{
					if (Device)
					{
						ITimer* timer = Device->getTimer();
						video::IVideoDriver* driver = Device->getVideoDriver();
						if (driver && timer)
						{
							video::IImage* screenshot = driver->createScreenShot();
							if (screenshot)
							{
								/// create filename
								io::path name = "./screenshot_render_audio_spectrum_";
								name += currentDateTimeString( timer );
								name += ".png";

								/// save screenshot
								driver->writeImageToFile( screenshot, name);

								/// delete
								screenshot->drop();
							}
						}
					}
				}

				/// space key - enable/disable camera eventreceiver
				if (event.KeyInput.Key == KEY_SPACE && !event.KeyInput.PressedDown)
				{
					if (Device)
					{
						scene::ISceneManager* smgr = Device->getSceneManager();
						scene::ICameraSceneNode* camera = smgr->getActiveCamera();
						if (camera)
						{
							camera->setInputReceiverEnabled( !camera->isInputReceiverEnabled() );
						}
						return true;
					}
				}
			}
			return false;
		}

	};


} // end namespace irr

///   +++ Implementation +++

#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/os.cpp>

using namespace irr;

s32 main( s32 argc, c8** argv)
{
	video::E_DRIVER_TYPE driverType = driverChoiceConsole(true);

	core::stringw WindowCaption( MY_TITLE );
	core::dimension2du ScreenSize(0,0);
	core::recti ScreenRect(0,0,0,0);
    ILogger* logger = 0;
	ITimer* timer = 0;
	u32 timeNow = 0;
	video::IVideoDriver* driver = 0;
	gui::IGUIEnvironment* env = 0;
	gui::IGUIFont* BigFont = 0;
    gui::IGUISkin* MySkin = 0;
	video::ITexture* Wallpaper = 0;


	IrrlichtDevice* device = createDevice( driverType );
	if (!device)
	{
		printf("Could not create Irrlicht-Device\n");
		exit(-2);
	}

	device->setWindowCaption( WindowCaption.c_str() );
    device->setResizable( true );

	BasicEventReceiver receiver( device );

    logger = device->getLogger();
    logger->log( WindowCaption.c_str() );

	timer = device->getTimer();
	timeNow = timer->getRealTime();
	srand( timeNow );

	driver = device->getVideoDriver();
	ScreenSize = driver->getScreenSize();

	env = device->getGUIEnvironment();
	ScreenRect = env->getRootGUIElement()->getAbsolutePosition();

	BigFont = env->getFont( MY_BIG_FONT_FILE );
    MySkin = env->createSkin( gui::EGST_BURNING_SKIN );
	//  if (MySkin && BigFont)
	//	{
	//		MySkin->setFont( BigFont );
	//	}

	driver->setTextureCreationFlag( video::ETCF_ALLOW_NON_POWER_2, true );
	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, false );
	Wallpaper = driver->getTexture( MY_LOAD_SCREEN_IMAGE );
	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, true );

	/// +++ Wallpaper +++
	driver->beginScene( true, true, video::SColor(255,0,0,0) );
		drawLoadingScreen( driver, Wallpaper, L"loading...", BigFont, 0xffffffff);
	driver->endScene();
	driver->removeTexture( Wallpaper );

	/// +++ Scene +++
	scene::ISceneManager* smgr = device->getSceneManager();

	/// +++ camera scenenode +++
	scene::ICameraSceneNode* camera = smgr->getActiveCamera();
	if (!camera)
	{
        camera = Camera::createFPS( smgr, 0.1f, 100.f );
	}

	/// +++ camera scenenode +++
	if (camera)
	{
        camera->setPosition( core::vector3df(0.5f*MeshSize.X,2.0f*MeshSize.Y,-0.25f*MeshSize.X) );
        // camera->setRotation( core::vector3df(0,528,0) );
        camera->setTarget( core::vector3df(0.5f*MeshSize.X,0.0f,0.25f*MeshSize.Z) );
        camera->setNearValue( 0.1f );
        camera->setFarValue( 10000.f );
        camera->setFOV( core::PI/2.1f );
	}

	/// +++ skydome texture +++
	video::ITexture* skyDomeTex = driver->getTexture( MY_DEFAULT_SKYDOME_TEXTURE );

	/// +++ skydome scenenode +++
	scene::ISceneNode* skyDomeNode = smgr->addSkyDomeSceneNode(
		skyDomeTex, 32,16, 0.99f, 2.0f, 1000.0f, smgr->getRootSceneNode(), -1);
	if (skyDomeNode)
	{
		skyDomeNode->setRotation( core::vector3df(0,180,0) );
	}

	/// +++ create light scenenode +++
	//	smgr->addLightSceneNode( smgr->getRootSceneNode(), core::vector3df(100,100,100), video::SColorf(1,1,1,1), 400, -1);
	//	smgr->addLightSceneNode( smgr->getRootSceneNode(), core::vector3df(0,0,0), video::SColorf(1,1,0,1), 200, -1);
	//	smgr->addLightSceneNode( smgr->getRootSceneNode(), core::vector3df(100,0,0), video::SColorf(0,0,1,1), 200, -1);

	/// +++ AudioPlayer +++

	s32 WindowPaddingX = 50;
	s32 WindowPaddingY = 10;

	CAudioPlayerSFML player;

	gui::IGUIWindow* playerWindow = env->addWindow(
		core::recti( WindowPaddingX,
			3*ScreenSize.Height/4,
			ScreenSize.Width-WindowPaddingX,
			ScreenSize.Height-WindowPaddingY), false, L"GUI AudioPlayer (SFML API)", env->getRootGUIElement(), -1);

	gui::CGUIAudioPlayer* playerPanel = new gui::CGUIAudioPlayer(
		&player, env, playerWindow, -1, playerWindow->getClientRect() );

	player.loadFile( MY_DEFAULT_AUDIO_FILE );



	/// +++ ColorGradient for Waveforms +++
	video::CLinearColorGradientTable WAV_Gradient;
	WAV_Gradient.addColor( video::SColor(255,0,0,0), 0.00f );
	WAV_Gradient.addColor( video::SColor(255,0,0,255), 0.25f );
	WAV_Gradient.addColor( video::SColor(255,0,255,0), 0.50f );
	WAV_Gradient.addColor( video::SColor(255,255,255,0), 0.75f );
	WAV_Gradient.addColor( video::SColor(255,255,0,0), 1.00f );
	WAV_Gradient.setTableSize( 128 );
	WAV_Gradient.updateTable();

	/// +++ ColorGradient for FourierTransform +++
	video::CLinearColorGradientTable FFT_Gradient;
	FFT_Gradient.addColor( video::SColor(255,0,0,0), 0.00f );
	FFT_Gradient.addColor( video::SColor(255,25,25,25), 0.10f );
	FFT_Gradient.addColor( video::SColor(255,50,50,50), 0.20f );
	FFT_Gradient.addColor( video::SColor(255,75,75,75), 0.30f );
	FFT_Gradient.addColor( video::SColor(255,100,100,100), 0.40f );
	FFT_Gradient.addColor( video::SColor(255,125,125,125), 0.45f );
	FFT_Gradient.addColor( video::SColor(255,0,0,255), 0.50f );
	FFT_Gradient.addColor( video::SColor(255,0,200,0), .60f );
	FFT_Gradient.addColor( video::SColor(255,255,255,0), .80f );
	FFT_Gradient.addColor( video::SColor(255,255,0,0), 1.0f );
	FFT_Gradient.setTableSize( 1024 );
	FFT_Gradient.updateTable();

	/// +++ 2D Matrix Container +++

	// +++ 2D Matrix Container +++
	dbPRINT("create Matrix for Waveform\n")
	u32 mm_waveform_cols = 512;
	u32 mm_waveform_rows = 10;
	core::CMatrix<f32> mm_waveform( mm_waveform_rows, mm_waveform_cols );
	mm_waveform.fill( 0 );

	// +++ 3D Matrix SceneNode +++
	scene::CMatrixSceneNode* node_wav = new scene::CMatrixSceneNode(
		scene::EPT_TRIANGLES, MeshSize, &WAV_Gradient, &mm_waveform,
		smgr, smgr->getRootSceneNode(), -1, core::vector3df(0,0,0) );

	if (!node_wav)
	{
		printf("Could not create CMatrixSceneNode\n");
	}
	else
	{
		node_wav->setMaterialFlag( video::EMF_LIGHTING, false );
		// node_wav->setDebugDataVisible( scene::EDS_BBOX );
		node_wav->setPosition( core::vector3df( -MeshSize.X, 0,0 ) );
	}

	/// +++ FrontWav +++
	scene::SMeshBuffer FrontWav;
	FrontWav.Vertices.reallocate( 4*(mm_waveform_cols-1) );
	FrontWav.Indices.reallocate( 6*(mm_waveform_cols-1) );
	FrontWav.Vertices.set_used( 0 );
	FrontWav.Indices.set_used( 0 );
	FrontWav.Material.MaterialType = video::EMT_SOLID;
	FrontWav.Material.Lighting = false;
	FrontWav.Material.Wireframe = false;
	FrontWav.Material.FogEnable = false;


	/// +++ 2D Matrix Container +++

	// +++ FourierTransform +++
	core::FourierTransformRtoC Transform(2*1024);
	const u32 FFT_Size = Transform.size();

	// +++ FourierTransform IO-Buffer +++
	core::array<s16> InputDataBuffer;
	InputDataBuffer.reallocate( FFT_Size );
	InputDataBuffer.set_used( FFT_Size );

	// +++ FourierTransform IO-Buffer +++
	core::array<f32> OutputDataBuffer;
	OutputDataBuffer.reallocate( FFT_Size / MY_OUTPUTBUFFER_SCALE );
	OutputDataBuffer.set_used( FFT_Size / MY_OUTPUTBUFFER_SCALE );

	// +++ 2D Matrix Container +++
	dbPRINT("create Matrix for Frequencies\n")
	core::CMatrix<f32> mm_frequencies( MY_OUTPUTBUFFER_ROWS, OutputDataBuffer.size() );
	mm_frequencies.fill( 0 );

	/// +++ 3D Matrix SceneNode +++
	scene::CMatrixSceneNode* node_fft = new scene::CMatrixSceneNode(
		scene::EPT_TRIANGLES, MeshSize, &FFT_Gradient, &mm_frequencies,
		smgr, smgr->getRootSceneNode(), -1, core::vector3df(0,0,0) );

	if (!node_fft)
	{
		printf("Could not create CMatrixSceneNode\n");
	}
	else
	{
		node_fft->setMaterialFlag( video::EMF_LIGHTING, false );
		// node_fft->setDebugDataVisible( scene::EDS_BBOX );
	}

	/// +++ FrontFFT +++
	scene::SMeshBuffer FrontFFT;
	FrontFFT.Vertices.reallocate( 4*(OutputDataBuffer.size()-1) );
	FrontFFT.Indices.reallocate( 6*(OutputDataBuffer.size()-1) );
	FrontFFT.Vertices.set_used( 0 );
	FrontFFT.Indices.set_used( 0 );
	FrontFFT.Material.MaterialType = video::EMT_SOLID;
	FrontFFT.Material.Lighting = false;
	FrontFFT.Material.Wireframe = false;
	FrontFFT.Material.FogEnable = false;




//	gui::IGUIWindow* win = createWindow( env, L"Realtime-Monitor for AudioSignals",
//		100,100, ScreenSize.Width-200,ScreenSize.Height-200);
//
//	gui::CGUIAudioDisplaySFML* rtmon = new gui::CGUIAudioDisplaySFML(
//		44100, 4*1024, env, win, -1, win->getClientRect());
//
	/// MAIN ///

	timeNow = timer->getRealTime();

	u32 renderTime = timeNow;
	u32 timeLastWindowTitleUpate = 0;
	u32 timeWaitWindowTitleUpate = 250;

	player.play();

    while (device->run())
    {

		/// Update Timer
		renderTime = timer->getRealTime() - timeNow;
		timeNow = timer->getRealTime();

		/// Resize-Event
      	if (ScreenSize != driver->getScreenSize() )
		{
			ScreenSize = driver->getScreenSize();

			ScreenRect = env->getRootGUIElement()->getAbsolutePosition();

			// anonymous block - update camera aspect
			{
				scene::ICameraSceneNode* camera = smgr->getActiveCamera();
				if (camera)
				{
					f32 aspect = (f32)ScreenSize.Width / ScreenSize.Height;
					camera->setAspectRatio( aspect );
				}
			}
		}

		/// if window is active ( can be minimized but still active )
        if (device->isWindowActive())
        {
			u32 ChannelCount = player.getChannelCount();

			/// get current PlayPosition
			u32 PlayPosition = player.getPosition();

			// decrease time to be sync with audio
			if (PlayPosition > renderTime * ChannelCount )
				PlayPosition -= renderTime * ChannelCount;


			/// if window is active ( can be minimized but still active )
			if (device->isWindowFocused())
			{
				sfx::fillSampleBuffer<s16>( InputDataBuffer, player.getSoundBuffer(), 0, PlayPosition, InputDataBuffer.size() );

				/// Waveform
				mm_waveform.shiftRow();

				const f32 y_scale_wav = MeshSize.Y / 32768.0f;
				const f32 y_offset_wav = 0.5f*MeshSize.Y;

				const u32 i_max_wav = core::min_<u32>( InputDataBuffer.size(), mm_waveform.getCols() );

				for (u32 i=0; i<i_max_wav; i++)
				{
					const f32 y = y_scale_wav*(f32)InputDataBuffer[i]+y_offset_wav;
					mm_waveform.setElement( 0, i, y );
				}

				node_wav->createMesh();

				/// FourierTransform
				Transform.setInputData<s16>( InputDataBuffer );
				Transform.fft();
				Transform.getPowerSpectrumAsDecibels<f32>( OutputDataBuffer );

				/// Fill Matrix with new values from FourierTransform

				mm_frequencies.shiftRow();

				const u32 i_max = core::min_<u32>( OutputDataBuffer.size(), mm_frequencies.getCols() );

				for (u32 i=0; i<i_max; i++)
				{
					mm_frequencies.setElement( 0, i, OutputDataBuffer[i] );
				}

				node_fft->createMesh();

				/// start rendering

				driver->beginScene( true, true, video::SColor(255,0,0,0) );

//					/// draw Wallpaper
//					audioPlayer->drawWallpaper( tex_spectrum ); // or use skyDome now

					/// draw SceneManager
					smgr->drawAll();

					/// draw few animated FFT Paths
					driver->setTransform( video::ETS_WORLD, core::IdentityMatrix );
					driver->setTransform( video::ETS_VIEW, camera->getViewMatrix() );
					driver->setTransform( video::ETS_PROJECTION, camera->getProjectionMatrix() );

					/// render FrontWav
					sfx::createFilledPath(
						&FrontWav,
						core::dimension2df(MeshSize.X, MeshSize.Y),
						InputDataBuffer,
						&WAV_Gradient,
						core::vector3df( -MeshSize.X,0,0),
						mm_waveform_cols );

					driver->setMaterial( FrontWav.getMaterial() );

					video::drawMeshBufferEx( driver, &FrontWav, scene::EPT_TRIANGLES );

					/// render FrontFFT
					sfx::createFilledPath(
						&FrontFFT,
						core::dimension2df(MeshSize.X, MeshSize.Y),
						OutputDataBuffer,
						&FFT_Gradient,
						core::vector3df(0,0,0) );

					driver->setMaterial( FrontFFT.getMaterial() );

					video::drawMeshBufferEx( driver, &FrontFFT, scene::EPT_TRIANGLES );

					/// draw CoordSystems
					//video::drawCoordSystem();
					video::drawXMeter( core::vector3df( 0.0f,0.0f,-1.0f), 0 , core::round32(MeshSize.X) );
					video::drawZMeter( core::vector3df( -1.0f,0.0f,0.0f), 0 , core::round32(MeshSize.Z) );
					//video::drawXMeter( core::vector3df( 0.0f,MeshSize.Y,-1.0f), -1 , core::round32(MeshSize.X) );
					//video::drawXMeter( core::vector3df( 0.0f,MeshSize.Y,0.5f*MeshSize.Z), 0 , core::round32(MeshSize.X) );
					//video::drawXMeter( core::vector3df( 0.0f,MeshSize.Y,MeshSize.Z), 0 , core::round32(MeshSize.X) );
					//
					//const f32 factor_x = 0.1f*MeshSize.X;
					//
					//for ( u32 x = 0; x<=10; x++)
					//{
						video::drawYMeter( core::vector3df( 0.0f,0.0f,-1.0f), -1, core::round32(MeshSize.Y) );
					//	video::drawYMeter( core::vector3df( factor_x*x,0.0f,0.5f*MeshSize.Z), 0, core::round32(MeshSize.Y) );
					//	video::drawYMeter( core::vector3df( factor_x*x,0.0f,1.0f*MeshSize.Z), 0, core::round32(MeshSize.Y) );
					//	video::drawZMeter( core::vector3df( factor_x*x,MeshSize.Y,-1.0f), -1, core::round32(MeshSize.Z) );
					//}

					/// draw GUI

					env->drawAll();

				/// end drawing
				driver->endScene();

				/// Update Window Title Text
				if ( timer && (timer->getRealTime() - timeLastWindowTitleUpate > timeWaitWindowTitleUpate ) )
				{
					core::stringw txt = MY_TITLE;
					txt += L" | fps( "; txt += driver->getFPS(); txt += L" ), ";
					txt += L"poly( "; txt += driver->getPrimitiveCountDrawn(); txt += L" / "; txt += driver->getMaximalPrimitiveCount(); txt += L" ), ";

					scene::ICameraSceneNode* cam = smgr->getActiveCamera();
					if (cam)
					{
						const core::vector3df& pos = cam->getAbsolutePosition();
						txt += L"cam( pos(";
						txt += core::round32(pos.X); txt += L",";
						txt += core::round32(pos.Y); txt += L",";
						txt += core::round32(pos.Z); txt += L"), ";

						const core::vector3df& eye = cam->getTarget();
						txt += L"eye(";
						txt += core::round32(eye.X); txt += L",";
						txt += core::round32(eye.Y); txt += L",";
						txt += core::round32(eye.Z); txt += L"), ";
						txt += L"near(";
						txt += cam->getNearValue();
						txt += L"), far(";
						txt += core::round32(cam->getFarValue() ); txt += L")";
						txt += L" )";
					}

					device->setWindowCaption( txt.c_str() );

					timeLastWindowTitleUpate = timer->getRealTime();
				}
			}
			else
			{

			}
        }
        else
        {
            device->yield();
        }
    }

	device->drop();
	return 0;
}
