/**
 * @page AudioAnimator3d for Linux, Windows and MacOSX
 *
 * @brief this program creates animated 3d powerspectrum from audio-files
 *
 * @author Benjamin Hampe <benjaminhampe@gmx.de>
 * @copyright Copyright (C) 2002-2014 by Benjamin Hampe
 * @license This file is part of the "irrlicht-engine"
 * For conditions of distribution and use, see copyright notice in irrlicht.h

 * @desc idea found at http://www.youtube.com/watch?v=gbuZVcw3ZiM
 * no copyright infringement intended i just like working with audio-data
 *
 * to compile this program u need fftw3, SFML2.1 Audio, freetype, OpenGL, Irrlicht
 *
 * tested on Windows7 32-bit Professional
 * tested on Windows7 64-bit Home Premium
 *
 * test system 1: Core2Duo E6600 2x 2,4 GHz, 2GiB RAM, Ati HD5450 512MiByte VRAM
 * test system 2: Core i5 2x 2,53 GHz, 4GiB RAM, Geforce 310M 1024MiByte VRAM + Intel Graphics Hybrid
 * 	+ 	load audio-file using SFML 2.0 Audio Library
 * 	+ 	calculate its PowerSpectrum (0...160 dB) using fftw3 FastFourierTransform
 *  	create image from wavform and power-spectrum using CLinearColorGradient
 * 	+ 	create 2D and 3D animated meshes from power-spectrum
 * each loop:
 *	+ 	get N samples at timeNow - lastRenderTime -> can lead to fluctuations when paused
 *	- 	do some windowing, filtering stuff
 * 	+ 	calculate power-spectrum with N samples
 * 	+ 	use only first N * 16 frequencies to create meshes ( much faster )
 * 	+ 	shift matrix one row
 * 	+ 	set matrix data at first row( y = row-index = 0 ) from power-spectrum
 * 	+ 	create front mesh from power-spectrum
 * 	+ 	create matrix mesh
 * 	+ 	render power-spectrum as background-texture
 * 	+ 	render matrix-mesh
 * 	+ 	render front-mesh
 * 	+ 	render x,y,z meter ( currently pure OpenGL )
 * 	+ 	render GUI
 * 	+ 	set WindowCaption with new information
 * 	+ 	loop until ESCAPE-Key
 *
 * @todo
 * + Speed improvements! ( pls give advices )
 * - Use assembler to speed up sin * cos ( both can be computed with one instruction )
 * - Use Goertzel's algorithm to speed up FFT at specific frequencies ( tones )
 * - WindowFunctions ( Cos, Gauss, Hamming, Hann, Blackman )
 *   impulse-response functions -> each create different responses in freq-domain
 * - 2D Plot Window ( linear and logarithmic scales ) for current data
 * - 3D Plot Window ( insert CMatrixSceneNode in a IGUIElement )
 * - Equalizer window with user interactive curve-manipulation ( 1-8 point polynom )
 * - do chromatic-tone-pitch estimation
 * - build piano-tuner using microphone
 */

#include <AudioAnimator3d_App.h>

#include <../source/Irrlicht/os.cpp>
#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CMeshSceneNode.cpp>
#include <../source/Irrlicht/CShadowVolumeSceneNode.cpp>

using namespace irr;

s32 main( s32 argc, c8** argv)
{
	video::E_DRIVER_TYPE driverType = driverChoiceConsole(true);
	IrrlichtDevice* device = createDevice( driverType );
	if (!device)
	{
		printf("Could not create Irrlicht-Device\n");
		exit(-2);
	}

	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	gui::IGUISkin* skin = env->createSkin( gui::EGST_WINDOWS_CLASSIC );
	//gui::IGUISkin* skin = env->createSkin( gui::EGST_WINDOWS_METALLIC );
	//gui::IGUISkin* skin = env->createSkin( gui::EGST_BURNING_SKIN );
	env->setSkin( skin );

	dbPRINT( "chosen skin-type = %d\n", env->getSkin()->getType() )

	Application app( device );
	app.run();

	return 0;
}

/// Working code before creation of class Application
/** @code
s32 main( s32 argc, c8** argv)
{
	video::E_DRIVER_TYPE driverType = driverChoiceConsole(true);

	core::stringw WindowCaption( AppTitle );
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

	Application receiver( device );

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
	Wallpaper = driver->getTexture( DefaultIntroTexture );
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
	video::ITexture* skyDomeTex = driver->getTexture( DefaultSkydomeTexture );

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
	CAudioPlayerSFML player;

	gui::IGUIWindow* playerWindow = env->addWindow(
		core::recti( 50, 3*ScreenSize.Height/4, ScreenSize.Width-50, ScreenSize.Height-10),
		false, L"GUI AudioPlayer (SFML API)", env->getRootGUIElement(), -1);

	gui::CGUIAudioPlayer* playerPanel = new gui::CGUIAudioPlayer(
		&player, env, playerWindow, -1, playerWindow->getClientRect() );

	player.loadFile( DefaultAudioFilename );

	/// +++ WAV ColorGradient +++
	video::CLinearColorGradientTable WAV_Gradient;
	WAV_Gradient.addColor( video::SColor(255,0,0,0), 0.00f );
	WAV_Gradient.addColor( video::SColor(255,0,0,255), 0.25f );
	WAV_Gradient.addColor( video::SColor(255,0,255,0), 0.50f );
	WAV_Gradient.addColor( video::SColor(255,255,255,0), 0.75f );
	WAV_Gradient.addColor( video::SColor(255,255,0,0), 1.00f );
	WAV_Gradient.setTableSize( 128 );
	WAV_Gradient.updateTable();

//	/// +++ WAV 2D MatrixData +++
//	u32 mm_waveform_cols = 512;
//	u32 mm_waveform_rows = 10;
//	core::CMatrix<f32> mm_waveform( mm_waveform_rows, mm_waveform_cols );
//	mm_waveform.fill( 0 );
//
//	/// +++ WAV 3D MatrixSceneNode +++
//	scene::CMatrixSceneNode* node_wav = new scene::CMatrixSceneNode(
//		scene::EPT_TRIANGLES, MeshSize, &WAV_Gradient, &mm_waveform,
//		smgr, smgr->getRootSceneNode(), -1, core::vector3df(0,0,0) );
//
//	if (!node_wav)
//	{
//		printf("Could not create CMatrixSceneNode\n");
//	}
//	else
//	{
//		node_wav->setMaterialFlag( video::EMF_LIGHTING, false );
//		// node_wav->setDebugDataVisible( scene::EDS_BBOX );
//		node_wav->setPosition( core::vector3df( -MeshSize.X, 0,0 ) );
//	}
//
//	/// +++ WAV FrontMeshBuffer +++
//	scene::SMeshBuffer FrontWav;
//	FrontWav.Vertices.reallocate( 4*(mm_waveform_cols-1) );
//	FrontWav.Indices.reallocate( 6*(mm_waveform_cols-1) );
//	FrontWav.Vertices.set_used( 0 );
//	FrontWav.Indices.set_used( 0 );
//	FrontWav.Material.MaterialType = video::EMT_SOLID;
//	FrontWav.Material.Lighting = false;
//	FrontWav.Material.Wireframe = false;
//	FrontWav.Material.FogEnable = false;

	/// +++ FFT ColorGradient +++
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

	/// +++ FFT +++
	core::FourierTransformRtoC Transform(2*1024);

	/// +++ FFT IO-Buffer +++
	core::array<s16> FFT_Input;
	FFT_Input.reallocate( Transform.size() );
	FFT_Input.set_used( Transform.size() );

	core::array<f32> FFT_Output;
	FFT_Output.reallocate( Transform.size() / MY_OUTPUTBUFFER_SCALE );
	FFT_Output.set_used( Transform.size() / MY_OUTPUTBUFFER_SCALE );

	/// +++ FFT 2D MatrixData +++
	core::CMatrix<f32> FFT_Matrix( MY_OUTPUTBUFFER_ROWS, FFT_Output.size() );
	FFT_Matrix.fill( 0 );

	/// +++ FFT 3D MatrixSceneNode +++
	scene::CMatrixSceneNode* FFT_SceneNode = new scene::CMatrixSceneNode(
		scene::EPT_TRIANGLES, MeshSize, &FFT_Gradient, &FFT_Matrix,
		smgr, smgr->getRootSceneNode(), -1, core::vector3df(0,0,0) );

	if (!FFT_SceneNode)
	{
		printf("Could not create CMatrixSceneNode\n");
	}
	else
	{
		FFT_SceneNode->setMaterialFlag( video::EMF_LIGHTING, false );
		// FFT_SceneNode->setDebugDataVisible( scene::EDS_BBOX );
	}

	/// +++ FFT FrontMeshBuffer +++
	scene::SMeshBuffer FFT_FrontMesh;
	FFT_FrontMesh.Vertices.reallocate( 4*(FFT_Output.size()-1) );
	FFT_FrontMesh.Indices.reallocate( 6*(FFT_Output.size()-1) );
	FFT_FrontMesh.Vertices.set_used( 0 );
	FFT_FrontMesh.Indices.set_used( 0 );
	FFT_FrontMesh.Material.MaterialType = video::EMT_SOLID;
	FFT_FrontMesh.Material.Lighting = false;
	FFT_FrontMesh.Material.Wireframe = false;
	FFT_FrontMesh.Material.FogEnable = false;


//	gui::IGUIWindow* win = createWindow( env, L"Realtime-Monitor for AudioSignals",
//		100,100, ScreenSize.Width-200,ScreenSize.Height-200);
//
//	gui::CGUIAudioDisplaySFML* rtmon = new gui::CGUIAudioDisplaySFML(
//		44100, 4*1024, env, win, -1, win->getClientRect());
//

	/// +++ create UI controls +++

	core::recti r_screen = makeRect( 0,0,ScreenSize.Width, ScreenSize.Height );

	{
		s32 x = 100;
		s32 y = 100;
		s32 dx = 100;
		s32 dy = 22;

		gui::IGUIStaticText* label;
		video::SColor fgColor(255,255,255,255);
		video::SColor bgColor(200,0,0,0);

		label = createLabel( env, env->getRootGUIElement(), L"FFT Size", x,y, 1,-1,5);
		label->setOverrideColor( fgColor );
		label->setBackgroundColor( bgColor );
		y += dy;

		label = createLabel( env, env->getRootGUIElement(), L"FFT Output Size", x,y, 1,-1,5);
		label->setOverrideColor( fgColor );
		label->setBackgroundColor( bgColor );
		y += dy;

		y = 100;
		x = 105;
		gui::IGUISpinBox* input;

		input = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
		input->setDecimalPlaces( 0 );
		input->setStepSize( 1 );
		input->setRange( 2, 65536 );
		input->setValue( Transform.size() );
		y += dy;

		input = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
		input->setDecimalPlaces( 0 );
		input->setStepSize( 1 );
		input->setRange( 2, Transform.size() );
		input->setValue( FFT_Output.size() );
		y += dy;

//		gui::CGUIFloatInput* ui_fft_input_size = new gui::CGUIFloatInput(
//			L"FFT Input Size", Transform.size(), true, false, env, env->getRootGUIElement(), -1,
//			core::recti( x,y, x+dx-1, y+dy-1 ), true );
//		y += dy;
//
//		gui::CGUIFloatInput* ui_fft_output_size = new gui::CGUIFloatInput(
//			L"FFT Output Size", FFT_Output.size(), true, false, env, env->getRootGUIElement(), -1,
//			core::recti( x,y, x+dx-1, y+dy-1 ), true );
//		y += dy;

	}
	/// BEGIN MAIN LOOP ///

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
				/// Waveform
//				mm_waveform.shiftRow();
//
//				const f32 y_scale_wav = MeshSize.Y / 32768.0f;
//				const f32 y_offset_wav = 0.5f*MeshSize.Y;
//
//				const u32 i_max_wav = core::min_<u32>( FFT_Input.size(), mm_waveform.getCols() );
//
//				for (u32 i=0; i<i_max_wav; i++)
//				{
//					const f32 y = y_scale_wav*(f32)FFT_Input[i]+y_offset_wav;
//					mm_waveform.setElement( 0, i, y );
//				}
//
//				node_wav->createMesh();

				/// fill SampleBuffer for FFT
				sfx::fillSampleBuffer<s16>( FFT_Input, player.getSoundBuffer(), 0, PlayPosition, FFT_Input.size() );

				/// do FourierTransform
				Transform.setInputData<s16>( FFT_Input );
				Transform.fft();
				Transform.getPowerSpectrumAsDecibels<f32>( FFT_Output );

				/// shift Matrix
				FFT_Matrix.shiftRow();

				/// fill Matrix with new values from FourierTransform
				const u32 i_max = core::min_<u32>( FFT_Output.size(), FFT_Matrix.getCols() );
				for (u32 i=0; i<i_max; i++)
				{
					FFT_Matrix.setElement( 0, i, FFT_Output[i] );
				}

				/// create Mesh
				FFT_SceneNode->createMesh();

				/// glBegin()
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
//					sfx::createFilledPath(
//						&FrontWav,
//						core::dimension2df(MeshSize.X, MeshSize.Y),
//						FFT_Input,
//						&WAV_Gradient,
//						core::vector3df( -MeshSize.X,0,0),
//						mm_waveform_cols );
//
//					driver->setMaterial( FrontWav.getMaterial() );
//
//					video::drawMeshBufferEx( driver, &FrontWav, scene::EPT_TRIANGLES );

					/// render FFT_FrontMesh
					sfx::createFilledPath(
						&FFT_FrontMesh,
						core::dimension2df(MeshSize.X, MeshSize.Y),
						FFT_Output,
						&FFT_Gradient,
						core::vector3df(0,0,0) );

					driver->setMaterial( FFT_FrontMesh.getMaterial() );

					video::drawMeshBufferEx( driver, &FFT_FrontMesh, scene::EPT_TRIANGLES );

					/// draw CoordSystems
					video::drawXMeter( core::vector3df( 0.0f,0.0f,-1.0f), 0 , core::round32(MeshSize.X) );
					video::drawZMeter( core::vector3df( -1.0f,0.0f,0.0f), 0 , core::round32(MeshSize.Z) );
					video::drawYMeter( core::vector3df( 0.0f,0.0f,-1.0f), -1, core::round32(MeshSize.Y) );

					/// draw GUI
					env->drawAll();

				/// glEnd()
				driver->endScene();

				/// Update Window Title Text
				if ( timer && (timer->getRealTime() - timeLastWindowTitleUpate > timeWaitWindowTitleUpate ) )
				{
					core::stringw txt = AppTitle;
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

**/
