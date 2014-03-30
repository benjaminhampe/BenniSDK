/// Copyright (C) 2002-2013 Benjamin Hampe
/// This file is part of the "irrlicht-engine"
/// For conditions of distribution and use, see copyright notice in irrlicht.h

/// definitions

// Irrlicht Engine
#include <irrlicht.h>

// hampe
#include "src/AudioAnimator3d_App.h"

/// implementation

#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/os.cpp>

using namespace irr;

s32 main( s32 argc, c8** argv)
{
	/// we like to have some choice

	//video::E_DRIVER_TYPE myDriverType = driverChoiceConsole(true);

	/// create NullDevice

	SIrrlichtCreationParameters params;
	params.LoggingLevel = ELL_NONE;
	params.DriverType = video::EDT_NULL;

	IrrlichtDevice* nulldev = createDeviceEx( params );
    if (nulldev)
    {
		video::IVideoModeList* videoModes = nulldev->getVideoModeList();
        params.WindowSize = videoModes->getDesktopResolution();
        params.WindowSize -= core::dimension2du(100,100);
        params.Bits = videoModes->getDesktopDepth();

		nulldev->drop();
    }
    else
	{
		printf("Could not create Null device\n");
		exit(-1);
	}

	/// create OpenGL Device

	params.LoggingLevel = ELL_INFORMATION;
	params.DriverType = video::EDT_OPENGL;
	params.AntiAlias = video::EAAM_QUALITY;
	params.EventReceiver = 0;
	params.HighPrecisionFPU = true;
	params.Doublebuffer = true;
	params.Vsync = false;
	params.Fullscreen = false;
	// params.ZBufferBits = 32;
	// params.Stencilbuffer = true;
	// params.WithAlphaChannel = false;

	IrrlichtDevice* device = createDeviceEx( params );
	if (!device)
	{
		printf("Could not create OpenGL device\n");
		exit(-2);
	}

    device->setResizable( true );

	IRunable* runable = 0;

	ICustomRenderer* customRenderer = 0;


	/// create App, this app can be hidden, by simply commenting out the next line

	AudioAnimator3d app0( device );

	runable = &app0;

	customRenderer = &app0;



	/// some pointers and variables

	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	video::IVideoDriver* driver = device->getVideoDriver();

	core::dimension2du screen = driver->getScreenSize();

	scene::ISceneManager* smgr = device->getSceneManager();

	ITimer* timer = device->getTimer();

	u32 timeLastWindowTitleUpate(0);

	u32 timeWaitWindowTitleUpate(500);
//
//	/// testing CGUIImageViewer
//
//	gui::IGUIWindow* win0 = env->addWindow(
//		core::recti( 100, 100, screen.Width-300, screen.Height-300 ),
//		false, L"CGUIImageViewer", env->getRootGUIElement(), -1 );
//
//	gui::CGUIImageViewer* viewer0 = new gui::CGUIImageViewer(
//		env, win0, -1, win0->getClientRect() );
//
//
//	/// testing CGUIDataPlot
//
//	gui::IGUIWindow* win1 = env->addWindow(
//		core::recti( 200, 200, screen.Width-200, screen.Height-200 ),
//		false, L"CGUIDataPlot", env->getRootGUIElement(), -1 );
//
//	gui::CGUIDataPlot* viewer1 = new gui::CGUIDataPlot(
//		env, win1, -1, win1->getClientRect() );


	/// main loop

    while (device->run())
    {
		/// Resize-Event

      	if (screen != driver->getScreenSize() )
		{
			screen = driver->getScreenSize();

			scene::ICameraSceneNode* camera = smgr->getActiveCamera();
			if (camera)
			{
				f32 aspect = (f32)screen.Width / (f32)screen.Height;
				camera->setAspectRatio( aspect );
			}
		}

		/// run injected Application class
		if (runable)
			runable->run();

		/// if window is active ( can be minimized but still active )
        if (device->isWindowActive())
        {

			/// if window is active ( can be minimized but still active )
			if (device->isWindowFocused())
			{

				/// render all

				driver->beginScene( true, true, video::SColor(255,0,0,0) );

				smgr->drawAll();

				// render, if exist
				if (customRenderer)
					customRenderer->render();

				env->drawAll();

				driver->endScene();


				/// window-title update

				if ( timer->getRealTime() - timeLastWindowTitleUpate > timeWaitWindowTitleUpate )
				{
					core::stringw txt = MY_TITLE;
					txt += L" | fps( ";
					txt += driver->getFPS();
					txt += L" ), poly( ";
					txt += driver->getPrimitiveCountDrawn(); txt += L" / ";
					txt += driver->getMaximalPrimitiveCount(); txt += L" ), ";

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
				device->yield();
			}
		}
        else
        {
            device->yield();
        }
    }

	if (device)
		device->drop();

	return 0;
}
