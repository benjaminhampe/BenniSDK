// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "AudioAnimator3d_App.h"

namespace irr
{

Application::Application( IrrlichtDevice* Device )
: Device(Device)
, AppTitle(L"Animated FFT-PowerSpectrum (c) 2014 by BenjaminHampe@gmx.de")
, DefaultAudioFilename("../../media/music/razor.ogg")
//, DefaultAudioFilename("../../media/music/benni_amelie_half.ogg")
//, DefaultAudioFilename("../../media/music/benni_coldplay_science_test.ogg")
//, DefaultAudioFilename("../../media/music/benni_klassik_test_001.ogg")
//, DefaultAudioFilename("../../media/music/benni_requiem_test.ogg")
//, DefaultAudioFilename("../../media/music/benni_coldplay_clocks_test.ogg")
//, DefaultAudioFilename("../../media/music/benni_entertainer_test.ogg")
//, DefaultAudioFilename("../../media/music/benni_madworld_test.ogg")
//, DefaultAudioFilename("../../media/music/benni_tonleiter.ogg")
, DefaultSkydomeTexture("../../media/skydome/iras.jpg")
, DefaultIntroTexture("../../media/background/loadscreen.jpg")
, ScreenSize(0,0)
, ScreenRect(0,0,0,0)
, Wallpaper(0)
, FFT_Size(4*1024)
, FFT_MatrixRows(100)
, FFT_MatrixCols(250)
, MeshSize(1000,250,1000)
, Transform( FFT_Size )
, FFT_Range(0,150)
, FFT_Threshold(50)
//, FFT_Matrix( FFT_MatrixRows, FFT_MatrixCols )
, FFT_SceneNode(0)
, ui_WireframeButton(0)
, ui_AxisMode(0)
, ui_MeshSizeX(0)
, ui_MeshSizeY(0)
, ui_MeshSizeZ(0)
, ui_MatrixCols(0)
, ui_MatrixRows(0)
, ui_DecibelMin(0)
, ui_DecibelMax(0)
, ui_DecibelThreshold(0)
, ui_FFT_Size(0)
{
	if (!setup())
	{
		printf("Could not create Application\n");
		exit(-2);
	}

	setupGUI();

	player.play();
}

Application::~Application()
{

}

bool Application::setup()
{
	dbPRINT( "Application::setup()\n" )

	if (!Device)
		return false;

	Device->setWindowCaption( AppTitle.c_str() );
    Device->setResizable( true );
	Device->setEventReceiver(this);

	ITimer* timer = Device->getTimer();
	u32 timeNow = timer->getRealTime();
	srand( timeNow );

	video::IVideoDriver* driver = Device->getVideoDriver();
	gui::IGUIEnvironment* env = Device->getGUIEnvironment();
	scene::ISceneManager* smgr = Device->getSceneManager();

	ScreenSize = driver->getScreenSize();
	ScreenRect = env->getRootGUIElement()->getAbsolutePosition();

	/// Intro
	gui::IGUIFont* font = env->getBuiltInFont();
	driver->setTextureCreationFlag( video::ETCF_ALLOW_NON_POWER_2, true );
	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, false );
	Wallpaper = driver->getTexture( DefaultIntroTexture );
	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, true );
	driver->beginScene( true, true, video::SColor(255,0,0,0) );
		drawLoadingScreen( driver, Wallpaper, L"loading...", font, 0xffffffff);
	driver->endScene();
	driver->removeTexture( Wallpaper );
	Wallpaper = 0;

	/// Camera
	scene::ICameraSceneNode* camera = smgr->getActiveCamera();
	if (!camera)
	{
        camera = Camera::createFPS( smgr, 0.1f, 100.f );
	}

	if (camera)
	{
        camera->setPosition( core::vector3df(0.5f*MeshSize.X,1.0f*MeshSize.Y,-0.28f*MeshSize.X) );
        camera->setTarget( core::vector3df(0.5f*MeshSize.X,0.0f,0.3f*MeshSize.Z) );
        camera->setNearValue( 0.1f );
        camera->setFarValue( 10000.f );
        camera->setFOV( core::PI/2.1f );
	}

	/// Skydome
	video::ITexture* skyDomeTex = driver->getTexture( DefaultSkydomeTexture );
	scene::ISceneNode* skyDomeNode = smgr->addSkyDomeSceneNode(
		skyDomeTex, 32,16, 0.99f, 2.0f, 1000.0f, smgr->getRootSceneNode(), -1);
	if (skyDomeNode)
	{
		skyDomeNode->setRotation( core::vector3df(0,180,0) );
	}

	/// +++ WAV ColorGradient +++
	WAV_Gradient.addColor( video::SColor(255,0,0,0), 0.00f );
	WAV_Gradient.addColor( video::SColor(255,0,0,255), 0.25f );
	WAV_Gradient.addColor( video::SColor(255,0,255,0), 0.50f );
	WAV_Gradient.addColor( video::SColor(255,255,255,0), 0.75f );
	WAV_Gradient.addColor( video::SColor(255,255,0,0), 1.00f );
	WAV_Gradient.setTableSize( 128 );
	WAV_Gradient.updateTable();

	/// +++ FFT ColorGradient +++
	FFT_Gradient.addColor( video::SColor(255,0,0,0), 0.00f );
//	FFT_Gradient.addColor( video::SColor(255,25,25,25), 0.10f );
//	FFT_Gradient.addColor( video::SColor(255,50,50,50), 0.20f );
//	FFT_Gradient.addColor( video::SColor(255,75,75,75), 0.30f );
//	FFT_Gradient.addColor( video::SColor(255,100,100,100), 0.40f );
	FFT_Gradient.addColor( video::SColor(255,255,255,255), 0.5f );
	FFT_Gradient.addColor( video::SColor(255,0,0,255), 0.70f );
	FFT_Gradient.addColor( video::SColor(255,0,200,0), .80f );
	FFT_Gradient.addColor( video::SColor(255,255,255,0), .90f );
//	FFT_Gradient.addColor( video::SColor(255,255,255,0), .95f );
	FFT_Gradient.addColor( video::SColor(255,255,0,0), 1.0f );
	FFT_Gradient.setTableSize( 1024 );
	FFT_Gradient.updateTable();


	/// +++ IO-Buffer +++
	FFT_Input.reallocate( FFT_Size );
	FFT_Input.set_used( FFT_Size );

	FFT_Output.reallocate( FFT_MatrixCols );
	FFT_Output.set_used( FFT_MatrixCols );

	FFT_Matrix.resize( FFT_MatrixRows, FFT_MatrixCols );
	FFT_Matrix.fill( 0 );

	/// +++ Matrix3DSceneNode +++
	FFT_SceneNode = new scene::CMatrixSceneNode(
		scene::EPT_TRIANGLES, MeshSize, &FFT_Gradient, &FFT_Matrix,
		smgr, smgr->getRootSceneNode(), -1, core::vector3df(0,0,0) );

	if (!FFT_SceneNode)
	{
		dbPRINT("Could not create CMatrixSceneNode\n");
	}
	else
	{
		FFT_SceneNode->setMaterialFlag( video::EMF_LIGHTING, false );
		// FFT_SceneNode->setDebugDataVisible( scene::EDS_BBOX );

		createLinearAxis( XAxis, FFT_MatrixCols );
		FFT_SceneNode->setUserXAxisValues( &XAxis );
	}

	/// disable camera event-receiver on start
	{
		scene::ICameraSceneNode* camera = smgr->getActiveCamera();
		if (camera)
		{
			camera->setInputReceiverEnabled( false );
		}
	}

	return true;
}

bool Application::setupGUI()
{
	dbPRINT( "Application::setupGUI()\n" )

//	gui::IGUIWindow* win = createWindow( env, L"Realtime-Monitor for AudioSignals",
//		100,100, ScreenSize.Width-200,ScreenSize.Height-200);
//
//	gui::CGUIAudioDisplaySFML* rtmon = new gui::CGUIAudioDisplaySFML(
//		44100, 4*1024, env, win, -1, win->getClientRect());
//
	gui::IGUIEnvironment* env = Device->getGUIEnvironment();
	if (!env)
		return false;

	video::SColor fgColor(255,255,255,255);
	video::SColor bgColor(160,0,0,0);

	s32 x,y,dx,dy;

	/// Wireframe Button
	x = 10;
	y = 10;
	dx = 48;
	dy = 48;
	ui_WireframeButton = env->addButton(core::recti(x,y,x+dx-1,y+dy-1),
		env->getRootGUIElement(), -1, L"Wireframe", L"toggle wireframe");

	/// HelpText
	x = (s32)ScreenSize.Width-1;
	y = 0;
	gui::IGUIStaticText* helpText = createLabel( env, env->getRootGUIElement(),
		L" Help:\n" \
		L" _________________________________________\n" \
		L" SPACE  - Enable / Disable camera movement\n" \
		L" WASD    - Move camera\n" \
		L" PRINT    - Create and save screenshot\n" \
		L" ESCAPE - Exit program", x, y, 1,-1, 5, false, true, true, 0, -1 );

	helpText->setTextAlignment( gui::EGUIA_UPPERLEFT, gui::EGUIA_CENTER );
	helpText->setOverrideColor( fgColor );
	helpText->setBackgroundColor( bgColor );

	/// Labels
	x = 100;
	y = 100;
	dx = 100;
	dy = 22;
	gui::IGUIStaticText* label;

	label = createLabel( env, env->getRootGUIElement(), L"Mesh Size X", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"Mesh Size Y", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"Mesh Size Z", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"Matrix Cols [x]", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"Matrix Rows [y]", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"Decibel Min", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"Decibel Max", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"Decibel Threshold", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"FFT Size", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	label = createLabel( env, env->getRootGUIElement(), L"X-Axis Mode", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;

	/// Inputs
	y = 100;
	x = 105;
	dx = 100;
	dy = 22;
	ui_MeshSizeX = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MeshSizeX->setDecimalPlaces( 0 );
	ui_MeshSizeX->setStepSize( 10 );
	ui_MeshSizeX->setRange( 2, 65536 );
	ui_MeshSizeX->setValue( MeshSize.X );
	y += dy;

	ui_MeshSizeY = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MeshSizeY->setDecimalPlaces( 0 );
	ui_MeshSizeY->setStepSize( 10 );
	ui_MeshSizeY->setRange( 2, 65536 );
	ui_MeshSizeY->setValue( MeshSize.Y );
	y += dy;

	ui_MeshSizeZ = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MeshSizeZ->setDecimalPlaces( 0 );
	ui_MeshSizeZ->setStepSize( 10 );
	ui_MeshSizeZ->setRange( 2, 65536 );
	ui_MeshSizeZ->setValue( MeshSize.Z );
	y += dy;

	ui_MatrixCols = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MatrixCols->setDecimalPlaces( 0 );
	ui_MatrixCols->setStepSize( 10 );
	ui_MatrixCols->setRange( 2, 65536 );
	ui_MatrixCols->setValue( FFT_MatrixCols );
	y += dy;

	ui_MatrixRows = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MatrixRows->setDecimalPlaces( 0 );
	ui_MatrixRows->setStepSize( 16 );
	ui_MatrixRows->setRange( 1, 10000 );
	ui_MatrixRows->setValue( FFT_MatrixRows );
	y += dy;

	ui_DecibelMin = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_DecibelMin->setDecimalPlaces( 0 );
	ui_DecibelMin->setStepSize( 10 );
	ui_DecibelMin->setRange( 0, 80 );
	ui_DecibelMin->setValue( FFT_Range.Min );
	y += dy;

	ui_DecibelMax = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_DecibelMax->setDecimalPlaces( 0 );
	ui_DecibelMax->setStepSize( 10 );
	ui_DecibelMax->setRange( 1, 10000 );
	ui_DecibelMax->setValue( FFT_Range.Max );
	y += dy;

	ui_DecibelThreshold = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_DecibelThreshold->setDecimalPlaces( 0 );
	ui_DecibelThreshold->setStepSize( 10 );
	ui_DecibelThreshold->setRange( FFT_Range.Min, FFT_Range.Max );
	ui_DecibelThreshold->setValue( FFT_Threshold );
	y += dy;

	ui_FFT_Size = env->addComboBox( core::recti(x,y,x+dx-1, y+dy-1), env->getRootGUIElement(), -1);
	ui_FFT_Size->addItem(L"1");
	ui_FFT_Size->addItem(L"2");
	ui_FFT_Size->addItem(L"4");
	ui_FFT_Size->addItem(L"8");
	ui_FFT_Size->addItem(L"16");
	ui_FFT_Size->addItem(L"32");
	ui_FFT_Size->addItem(L"64");
	ui_FFT_Size->addItem(L"128");
	ui_FFT_Size->addItem(L"256");
	ui_FFT_Size->addItem(L"512");
	ui_FFT_Size->addItem(L"1024");
	ui_FFT_Size->addItem(L"2048");
	ui_FFT_Size->addItem(L"4096");
	ui_FFT_Size->addItem(L"8192");
	ui_FFT_Size->addItem(L"16384");
	ui_FFT_Size->addItem(L"32768");
	ui_FFT_Size->addItem(L"65536");
	ui_FFT_Size->setSelected(12);
	y += dy;

	ui_AxisMode = env->addComboBox( core::recti(x,y,x+dx-1, y+dy-1), env->getRootGUIElement(), -1);
	ui_AxisMode->addItem(L"Linear");
	ui_AxisMode->addItem(L"Logarithmic 10");
	ui_AxisMode->setSelected(0);
	y += dy;

	/// GUI AudioPlayer
	gui::IGUIWindow* playerWindow = env->addWindow(
		core::recti( 50, 3*ScreenSize.Height/4, ScreenSize.Width-50, ScreenSize.Height-10),
		false, L"GUI AudioPlayer (SFML API)", env->getRootGUIElement(), -1);

	gui::CGUIAudioPlayer* playerPanel = new gui::CGUIAudioPlayer(
		0, env, playerWindow, -1, playerWindow->getClientRect() );

	player.loadFile( DefaultAudioFilename );

	playerPanel->setPlayer( &player );
	return true;
}

/// run()

bool Application::run()
{
	dbPRINT( "Application::run()\n" )

	if (!Device)
		return false;

	video::IVideoDriver* driver = Device->getVideoDriver();
	gui::IGUIEnvironment* env = Device->getGUIEnvironment();
	scene::ISceneManager* smgr = Device->getSceneManager();
	ITimer* timer = Device->getTimer();
	u32 timeNow = timer->getRealTime();
	u32 renderTime = timeNow;
	u32 timeLastWindowTitleUpate = 0;
	u32 timeWaitWindowTitleUpate = 250;

    while (Device->run())
    {
//	#ifdef _IRR_COMPILE_WITH_FLTK_
//		Fl::check();
//	#endif // _IRR_COMPILE_WITH_FLTK_

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
        if (Device->isWindowActive())
        {
			u32 ChannelCount = player.getChannelCount();

			/// get current PlayPosition
			u32 PlayPosition = player.getPosition();

			// decrease time to be sync with audio
			if (PlayPosition > renderTime * ChannelCount )
				PlayPosition -= renderTime * ChannelCount;


			/// if window is active ( can be minimized but still active )
			if (Device->isWindowFocused())
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
				if (player.isPlaying())
				{
				/// fill SampleBuffer for FFT
				sfx::fillSampleBuffer<s16>( FFT_Input, player.getSoundBuffer(), 0, PlayPosition, FFT_Input.size() );

				/// do FourierTransform
				Transform.setInputData<s16>( FFT_Input );
				Transform.fft();
				//Transform.getPowerSpectrumAsDecibels<f32>( FFT_Output );
				Transform.getScaledPowerSpectrumAsDecibelsThresholdFast<f32>( FFT_Output, FFT_Range.Min, FFT_Range.Max, FFT_Threshold );

				/// shift Matrix
				FFT_Matrix.shiftRow();

				/// fill Matrix with new values from FourierTransform
				const u32 i_max = core::min_<u32>( FFT_Output.size(), FFT_Matrix.getCols() );
				for (u32 i=0; i<i_max; i++)
				{
					FFT_Matrix.setElement( 0, i, FFT_Output[i] );
				}

				/// create Mesh
				if (FFT_SceneNode)
					FFT_SceneNode->createMesh();

				}

				/// glBegin()
				driver->beginScene( true, true, video::SColor(255,0,0,0) );

//					/// draw Wallpaper
//					audioPlayer->drawWallpaper( tex_spectrum ); // or use skyDome now

					/// draw SceneManager
					smgr->drawAll();

					/// set transformation-matrices for native OpenGL commands
					scene::ICameraSceneNode* camera = smgr->getActiveCamera();
					if (camera)
					{
					driver->setTransform( video::ETS_WORLD, core::IdentityMatrix );
					driver->setTransform( video::ETS_VIEW, camera->getViewMatrix() );
					driver->setTransform( video::ETS_PROJECTION, camera->getProjectionMatrix() );
					}

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
//					sfx::createFilledPath(
//						&FFT_FrontMesh,
//						core::dimension2df(MeshSize.X, MeshSize.Y),
//						FFT_Output,
//						&FFT_Gradient,
//						core::vector3df(0,0,0) );
//
//					driver->setMaterial( FFT_FrontMesh.getMaterial() );
//
//					video::drawMeshBufferEx( driver, &FFT_FrontMesh, scene::EPT_TRIANGLES );

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
					txt += L" | "; txt += driver->getFPS();
					txt += L" | "; txt += driver->getVendorInfo();
					txt += L" | "; txt += driver->getName();
					txt += L" | "; txt += video::IImage::getBitsPerPixelFromFormat( driver->getColorFormat() );
					txt += L" BPP, Tris "; txt += driver->getPrimitiveCountDrawn();
					txt += L" / "; txt += driver->getMaximalPrimitiveCount(); txt += L", ";

					scene::ICameraSceneNode* cam = smgr->getActiveCamera();
					if (cam)
					{
						const core::vector3df& pos = cam->getAbsolutePosition();
						txt += L"cam(";
						txt += core::round32(pos.X); txt += L",";
						txt += core::round32(pos.Y); txt += L",";
						txt += core::round32(pos.Z); txt += L"), ";

						const core::vector3df& eye = cam->getTarget();
						txt += L"eye(";
						txt += core::round32(eye.X); txt += L",";
						txt += core::round32(eye.Y); txt += L",";
						txt += core::round32(eye.Z); txt += L")";
//						txt += L", near(";
//						txt += cam->getNearValue();
//						txt += L"), far(";
//						txt += core::round32(cam->getFarValue() ); txt += L")";
//						txt += L" )";
					}

					Device->setWindowCaption( txt.c_str() );

					timeLastWindowTitleUpate = timer->getRealTime();
				}
			}
			else
			{

			}
        }
        else
        {
            Device->yield();
        }
    }

	Device->drop();
	return true;
}

/// event-handler
void Application::OnKeyUp_Espace()
{
	exit(0); //We exit
}

/// event-handler
void Application::OnKeyUp_Print()
{
	if (!Device)
		return;

	video::IVideoDriver* driver = Device->getVideoDriver();
	ITimer* timer = Device->getTimer();

	if (!driver || !timer)
		return;

	video::IImage* screenshot = driver->createScreenShot();
	if (!screenshot)
		return;

	/// fill alpha-channel with 255 ( make it opaque )
	const core::dimension2du imgSize;
	for (u32 y=0; y<imgSize.Height; y++)
	{
		for (u32 x=0; x<imgSize.Width; x++)
		{
			video::SColor color = screenshot->getPixel(x,y);
			color.setAlpha( 255 );
			screenshot->setPixel( x,y,color, false);
		}
	}

	/// create filename
	core::stringc name = "./AudioAnimator3d_screen_";
	name += currentDateTimeString( timer );
	name += ".png";

	/// save screenshot
	if (!driver->writeImageToFile( screenshot, io::path(name) ) )
	{
		dbERROR( "Could not save image %s\n", name.c_str() )
	}

	/// delete
	screenshot->drop();
}

/// event-handler
void Application::OnKeyUp_Space()
{
	if (!Device)
		return;

	scene::ISceneManager* smgr = Device->getSceneManager();
	if (!smgr)
		return;

	scene::ICameraSceneNode* camera = smgr->getActiveCamera();
	if (!camera)
		return;

	camera->setInputReceiverEnabled( !camera->isInputReceiverEnabled() );
}

/// OnEvent
void Application::OnButton_Wireframe()
{
	if (!Device)
		return;

	scene::ISceneManager* smgr = Device->getSceneManager();
	if (!smgr)
		return;

	scene::ISceneNode* rootNode = smgr->getRootSceneNode();
	if (!rootNode)
		return;

	const core::list<scene::ISceneNode*>& children = rootNode->getChildren();

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
}

/// event-handler
void Application::OnChangedSpinBox_MeshSizeX( f32 value )
{
	MeshSize.X = core::clamp<f32>( value, 1.0f, 10000.0f );

	if (FFT_SceneNode)
	{
		FFT_SceneNode->setMeshSize( MeshSize );
	}

	if (ui_MeshSizeX)
	{
		ui_MeshSizeX->setValue( MeshSize.X );
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_MeshSizeX );
		}
	}
}

/// event-handler
void Application::OnChangedSpinBox_MeshSizeY( f32 value )
{
	MeshSize.Y = core::clamp<f32>( value, 1.0f, 10000.0f );

	if (FFT_SceneNode)
	{
		FFT_SceneNode->setMeshSize( MeshSize );
	}

	if (ui_MeshSizeY)
	{
		ui_MeshSizeY->setValue( MeshSize.Y );
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_MeshSizeY );
		}
	}
}

/// event-handler
void Application::OnChangedSpinBox_MeshSizeZ( f32 value )
{
	MeshSize.Z = core::clamp<f32>( value, 1.0f, 10000.0f );

	if (FFT_SceneNode)
	{
		FFT_SceneNode->setMeshSize( MeshSize );
	}

	if (ui_MeshSizeZ)
	{
		ui_MeshSizeZ->setValue( MeshSize.Z );
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_MeshSizeZ );
		}
	}
}

/// event-handler
void Application::OnChangedSpinBox_DecibelMin( f32 value )
{
	FFT_Range.Min = core::clamp<f32>( value, 0.0f, FFT_Range.Max );
	FFT_Threshold = core::clamp<f32>( FFT_Threshold, FFT_Range.Min, FFT_Range.Max );

	if (ui_DecibelMin)
	{
		ui_DecibelMin->setValue( FFT_Range.Min );
	}

	if (ui_DecibelThreshold)
	{
		ui_DecibelThreshold->setValue( FFT_Threshold );
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_DecibelMin );
		}
	}
}

/// event-handler
void Application::OnChangedSpinBox_DecibelMax( f32 value )
{
	FFT_Range.Max = core::clamp<f32>( value, FFT_Range.Min, 1000.0f );
	FFT_Threshold = core::clamp<f32>( FFT_Threshold, FFT_Range.Min, FFT_Range.Max );

	if (ui_DecibelMax)
	{
		ui_DecibelMax->setValue( FFT_Range.Max );
	}

	if (ui_DecibelThreshold)
	{
		ui_DecibelThreshold->setValue( FFT_Threshold );
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_DecibelMax );
		}
	}
}

/// event-handler
void Application::OnChangedSpinBox_DecibelThreshold( f32 value )
{
	FFT_Threshold = core::clamp<f32>( value, FFT_Range.Min, FFT_Range.Max );

	if (ui_DecibelThreshold)
	{
		ui_DecibelThreshold->setValue( FFT_Threshold );
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_DecibelThreshold );
		}
	}
}

/// event-handler
void Application::OnChangedSpinBox_MatrixCols( f32 value )
{
	u32 cols = core::clamp<u32>( (u32)value, 2, FFT_Size);

	FFT_Matrix.resize( FFT_Matrix.getRows(), cols );

	FFT_Output.reallocate( FFT_Matrix.getCols() );
	FFT_Output.set_used( FFT_Matrix.getCols() );

	if (ui_MatrixCols)
	{
		ui_MatrixCols->setValue( (f32)FFT_Matrix.getCols() );
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_MatrixCols );
		}
	}
}

/// event-handler
void Application::OnChangedSpinBox_MatrixRows( f32 value )
{
	u32 rows = core::clamp<u32>( (u32)value, 1, 1024);

	FFT_Matrix.resize( rows, FFT_Matrix.getCols() );

	if (ui_MatrixRows)
	{
		ui_MatrixRows->setValue( (f32)FFT_Matrix.getRows() );
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_MatrixRows );
		}
	}
}

/// event-handler
void Application::OnChangedComboBox_FFTSize( s32 selected )
{
	if (!ui_FFT_Size)
		return;

	if (selected >= 0)
	{
		u32 value = core::Math::pow( 2, selected + 1 );
		FFT_Size = value;

		Transform.resize( FFT_Size );

		FFT_Size = Transform.size();

		FFT_Input.reallocate( FFT_Size );
		FFT_Input.set_used( FFT_Size );

		dbPRINT("Changed FFT-Size to %d\n", Transform.size() )
	//					FFT_Size = value;
	//
	//					FFT_Input.reallocate( Transform.size() );
	//					FFT_Input.set_used( Transform.size() );
	//					if (FFT_Output.size() > FFT_Input.size())
	//					{
	//						FFT_Output.reallocate( Transform.size() );
	//						FFT_Output.set_used( )
	}

	if (Device)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();
		if (env)
		{
			env->removeFocus( ui_FFT_Size );
		}
	}
}

/// event-handler
void Application::OnChangedComboBox_AxisMode( s32 selected )
{
	if (selected == 0)
	{
		createLinearAxis( XAxis, FFT_Matrix.getCols() );
	}
	else if ( selected == 1 )
	{
		createLogarithmicAxis( XAxis, FFT_Matrix.getCols(), 10, player.getSampleRate(), FFT_Size );
	}
}



/// OnEvent()

bool Application::OnEvent(const SEvent &event)
{
	/// handle GUI events
	if (event.EventType == EET_GUI_EVENT)
	{
		gui::IGUIEnvironment* env = Device->getGUIEnvironment();

		gui::IGUIElement* caller = event.GUIEvent.Caller;

		/// window

		if (event.GUIEvent.EventType == gui::EGET_ELEMENT_CLOSED)
		{
			if (caller == env->getRootGUIElement())
			{
				return true;
			}
		}

		/// buttons
		if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED)
		{
			/// handle wireframe-button
			if (caller == ui_WireframeButton)
			{
				OnButton_Wireframe();
				return true;
			}

			/// handle other buttons
		}

		/// combobox
		if ( event.GUIEvent.EventType == gui::EGET_COMBO_BOX_CHANGED )
		{
			const s32 selected = ((gui::IGUIComboBox*)caller)->getSelected();

			/// FFT-Size
			if (caller == ui_FFT_Size)
			{
				OnChangedComboBox_FFTSize( selected );

				/// need to update the axis
				if (ui_AxisMode)
				{
					OnChangedComboBox_AxisMode( ui_AxisMode->getSelected() );
				}
			}
			/// AxisMode
			if (caller == ui_AxisMode)
			{
				OnChangedComboBox_AxisMode( selected );
			}
		}

		/// spinbox
		if ( event.GUIEvent.EventType == gui::EGET_SPINBOX_CHANGED)
		{
			gui::IGUIEnvironment* env = Device->getGUIEnvironment();

			const f32 value = ((gui::IGUISpinBox*)caller)->getValue();

			if (caller == ui_MeshSizeX)
			{
				OnChangedSpinBox_MeshSizeX( value );
			}
			if (caller == ui_MeshSizeY)
			{
				OnChangedSpinBox_MeshSizeY( value );
			}
			if (caller == ui_MeshSizeZ)
			{
				OnChangedSpinBox_MeshSizeZ( value );
			}
			if (caller == ui_DecibelMin)
			{
				OnChangedSpinBox_DecibelMin( value );
			}
			if (caller == ui_DecibelMax)
			{
				OnChangedSpinBox_DecibelMax( value );
			}
			if (caller == ui_DecibelThreshold)
			{
				OnChangedSpinBox_DecibelThreshold( value );
			}
			if (caller == ui_MatrixRows)
			{
				OnChangedSpinBox_MatrixRows( value );

				/// need to update the axis
				if (ui_AxisMode)
				{
					OnChangedComboBox_AxisMode( ui_AxisMode->getSelected() );
				}
			}
			if (caller == ui_MatrixCols)
			{
				OnChangedSpinBox_MatrixCols( value );

				/// need to update the axis
				if (ui_AxisMode)
				{
					OnChangedComboBox_AxisMode( ui_AxisMode->getSelected() );
				}

			}
		}
	}

	if(event.EventType == EET_KEY_INPUT_EVENT)
	{
		/// exit program if escape key hit
		if(event.KeyInput.Key == KEY_ESCAPE)
		{
			OnKeyUp_Espace();
			return true;
		}

		/// if print-screen key pressed ( create screenshot image with unique name ( timer based ) and save to hdd )
		if ( (event.KeyInput.Key == KEY_PRINT || event.KeyInput.Key == KEY_SNAPSHOT) && !event.KeyInput.PressedDown)
		{
			OnKeyUp_Print();
			return true;
		}

		/// space key - enable/disable camera eventreceiver
		if (event.KeyInput.Key == KEY_SPACE && !event.KeyInput.PressedDown)
		{
			OnKeyUp_Space();
			return true;
		}
	}
	return false;
}

} // end namespace irr
