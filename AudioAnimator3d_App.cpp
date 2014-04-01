// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "AudioAnimator3d_App.h"

namespace irr
{

Application::Application( IrrlichtDevice* Device )
: Device(Device)
, AppTitle(L"AudioPlayer with 3d animated FFT-PowerSpectrum (c) 2013 by BenjaminHampe@gmx.de")
, DefaultAudioFilename("../../media/music/razor.ogg")
, DefaultSkydomeTexture("../../media/skydome/iras.jpg")
, DefaultIntroTexture("../../media/background/loadscreen.jpg")
, ScreenSize(0,0)
, ScreenRect(0,0,0,0)
, Wallpaper(0)
, FFT_Size(2*1024)
, FFT_MatrixRows(50)
, FFT_MatrixCols(100)
, MeshSize(1024,256,1024)
, Transform( FFT_Size )
//, FFT_Matrix( FFT_MatrixRows, FFT_MatrixCols )
, FFT_SceneNode(0)
, WireframeButton(0)
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
        camera->setPosition( core::vector3df(0.5f*MeshSize.X,2.0f*MeshSize.Y,-0.25f*MeshSize.X) );
        // camera->setRotation( core::vector3df(0,528,0) );
        camera->setTarget( core::vector3df(0.5f*MeshSize.X,0.0f,0.25f*MeshSize.Z) );
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


	/// +++ FFT IO-Buffer +++
	FFT_Input.reallocate( FFT_Size );
	FFT_Input.set_used( FFT_Size );
	FFT_Output.reallocate( FFT_MatrixCols );
	FFT_Output.set_used( FFT_MatrixCols );
	FFT_Matrix.resize( FFT_MatrixRows, FFT_MatrixCols );
	FFT_Matrix.fill( 0 );
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
	}

	/// +++ FFT FrontMeshBuffer +++
	FFT_FrontMesh.Vertices.reallocate( 4*(FFT_Output.size()-1) );
	FFT_FrontMesh.Indices.reallocate( 6*(FFT_Output.size()-1) );
	FFT_FrontMesh.Vertices.set_used( 0 );
	FFT_FrontMesh.Indices.set_used( 0 );
	FFT_FrontMesh.Material.MaterialType = video::EMT_SOLID;
	FFT_FrontMesh.Material.Lighting = false;
	FFT_FrontMesh.Material.Wireframe = false;
	FFT_FrontMesh.Material.FogEnable = false;
	dbPRINT("setup() OK\n")

	return true;
}

bool Application::setupGUI()
{
//	gui::IGUIWindow* win = createWindow( env, L"Realtime-Monitor for AudioSignals",
//		100,100, ScreenSize.Width-200,ScreenSize.Height-200);
//
//	gui::CGUIAudioDisplaySFML* rtmon = new gui::CGUIAudioDisplaySFML(
//		44100, 4*1024, env, win, -1, win->getClientRect());
//
	gui::IGUIEnvironment* env = Device->getGUIEnvironment();
	if (!env)
		return false;

	WireframeButton = env->addButton(core::recti(5,5,5+48,5+48), env->getRootGUIElement(), -1, L"Wireframe", L"toggle wireframe");

	s32 x = 100;
	s32 y = 100;
	s32 dx = 100;
	s32 dy = 22;

	gui::IGUIStaticText* label;
	video::SColor fgColor(255,255,255,255);
	video::SColor bgColor(200,0,0,0);

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

	label = createLabel( env, env->getRootGUIElement(), L"FFT Size", x,y, 1,-1,5);
	label->setOverrideColor( fgColor );
	label->setBackgroundColor( bgColor );
	y += dy;


	y = 100;
	x = 105;

	ui_MeshSizeX = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MeshSizeX->setDecimalPlaces( 0 );
	ui_MeshSizeX->setStepSize( 1 );
	ui_MeshSizeX->setRange( 2, 65536 );
	ui_MeshSizeX->setValue( Transform.size() );
	y += dy;

	ui_MeshSizeY = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MeshSizeY->setDecimalPlaces( 0 );
	ui_MeshSizeY->setStepSize( 1 );
	ui_MeshSizeY->setRange( 2, 65536 );
	ui_MeshSizeY->setValue( Transform.size() );
	y += dy;

	ui_MeshSizeZ = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MeshSizeZ->setDecimalPlaces( 0 );
	ui_MeshSizeZ->setStepSize( 1 );
	ui_MeshSizeZ->setRange( 2, 65536 );
	ui_MeshSizeZ->setValue( Transform.size() );
	y += dy;

	ui_MatrixCols = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MatrixCols->setDecimalPlaces( 0 );
	ui_MatrixCols->setStepSize( 1 );
	ui_MatrixCols->setRange( 2, 65536 );
	ui_MatrixCols->setValue( Transform.size() );
	y += dy;

	ui_MatrixRows = env->addSpinBox( L"", core::recti(x,y,x+dx-1, y+dy-1), true, env->getRootGUIElement(), -1);
	ui_MatrixRows->setDecimalPlaces( 0 );
	ui_MatrixRows->setStepSize( 1 );
	ui_MatrixRows->setRange( 2, Transform.size() );
	ui_MatrixRows->setValue( FFT_Output.size() );
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
	ui_FFT_Size->setSelected(11);
	y += dy;

	gui::IGUIWindow* playerWindow = env->addWindow(
		core::recti( 50, 3*ScreenSize.Height/4, ScreenSize.Width-50, ScreenSize.Height-10),
		false, L"GUI AudioPlayer (SFML API)", env->getRootGUIElement(), -1);

	gui::CGUIAudioPlayer* playerPanel = new gui::CGUIAudioPlayer(
		&player, env, playerWindow, -1, playerWindow->getClientRect() );

	player.loadFile( DefaultAudioFilename );

	player.play();
	dbPRINT("setupGUI() OK\n")
	return true;
}

/// run()

bool Application::run()
{
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
				if (FFT_SceneNode)
					FFT_SceneNode->createMesh();

				/// glBegin()
				driver->beginScene( true, true, video::SColor(255,0,0,0) );

//					/// draw Wallpaper
//					audioPlayer->drawWallpaper( tex_spectrum ); // or use skyDome now

					/// draw SceneManager
					smgr->drawAll();

					/// draw few animated FFT Paths
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

/// OnEvent()

bool Application::OnEvent(const SEvent &event)
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

} // end namespace irr
