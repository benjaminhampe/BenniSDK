/**
 * Copyright (C) 2002-2014 by Benjamin Hampe <benjaminhampe@gmx.de>
 *
 * @page AudioAnimator3d
 *
 * @brief This program creates an animated 3d power-spectrum from audio-files
 *
 * For conditions of distribution and use, see copyright notice in irrlicht.h
 * idea found at http://www.youtube.com/watch?v=gbuZVcw3ZiM
 * no copyright infringement intended i just like working with audio-data
 *
 * to compile this program u need fftw3, SFML2.1, freetype and Irrlicht
 *
 * tested on Windows7 32-bit Professional
 * tested on Windows7 64-bit Home Premium
 *
 * test system 1: Core2Duo E6600 2x 2,4 GHz, 2GiB RAM, Ati HD5450 512MiByte VRAM
 * test system 2: Core i5 2x 2,53 GHz, 4GiB RAM, Geforce 310M 1024MiByte VRAM + Intel Graphics Hybrid
 */

#include "AudioAnimator3d_App.h"

#include <../source/Irrlicht/os.cpp>
#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CMeshSceneNode.cpp>
#include <../source/Irrlicht/CShadowVolumeSceneNode.cpp>

using namespace irr;

s32 main( s32 argc, c8** argv)
{
	//video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;//driverChoiceConsole(true);

	IrrlichtDevice* device = createOpenGlDevice( -100, -200, 16, false, true );
	if (!device)
	{
		dbPRINT("Could not create Irrlicht-Device\n")
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
