// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_COMPILE_CONFIG_H_INCLUDED__
#define __IRR_EXT_COMPILE_CONFIG_H_INCLUDED__

/// MyDefines

//#ifndef _IRR_COMPILE_WITH_FLTK_
//#define _IRR_COMPILE_WITH_FLTK_
//#endif

#ifndef _IRR_COMPILE_WITH_FFTW3_
#define _IRR_COMPILE_WITH_FFTW3_
#endif

#ifndef _IRR_COMPILE_WITH_SFML_AUDIO_
#define _IRR_COMPILE_WITH_SFML_AUDIO_
#endif

//#ifndef _IRR_COMPILE_WITH_PORTMIDI_
//#define _IRR_COMPILE_WITH_PORTMIDI_
//#endif

//#ifndef _IRR_COMPILE_WITH_FREETYPE2_
//#define _IRR_COMPILE_WITH_FREETYPE2_
//#endif

#ifndef _IRR_COMPILE_WITH_XPM_LOADER_
#define _IRR_COMPILE_WITH_XPM_LOADER_
#endif

#ifndef _IRR_COMPILE_WITH_XPM_WRITER_
#define _IRR_COMPILE_WITH_XPM_WRITER_
#endif

//#ifndef _IRR_COMPILE_WITH_XPM_LOADER_DEBUG_OUTPUT_
//#define _IRR_COMPILE_WITH_XPM_LOADER_DEBUG_OUTPUT_
//#endif

//#ifndef _IRR_COMPILE_WITH_WX_300_
//#define _IRR_COMPILE_WITH_WX_300_
//#endif

/// Includes

// LibC
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cwchar>
#include <ctime>
#include <cmath>
#include <getopt.h>

#include <debugPrint.h>

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

#include <irrlicht.h>
#include <driverChoice.h>
#include <../source/Irrlicht/os.h>
#include <../source/Irrlicht/CImage.h>
#include <../source/Irrlicht/CColorConverter.h>
#include <../source/Irrlicht/CMeshSceneNode.h>
#include <../source/Irrlicht/CShadowVolumeSceneNode.h>

// OpenGL API
#include <GL/gl.h>
#include <GL/glu.h>

#if defined(__GXX_EXPERIMENTAL_CXX0X) || __cplusplus >= 201103L
	#include <functional>
#endif

#ifdef _IRR_COMPILE_WITH_FFTW3_
#include <fftw3.h>
#endif // _IRR_COMPILE_WITH_FFTW3_

#ifdef _IRR_COMPILE_WITH_SFML_AUDIO_
#include <SFML/Audio.hpp>
#endif // _IRR_COMPILE_WITH_SFML_AUDIO_

#ifdef _IRR_COMPILE_WITH_FREETYPE2_
#include <irrlicht.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#endif // _IRR_COMPILE_WITH_FREETYPE2_

#ifdef _IRR_COMPILE_WITH_PORTMIDI_
    #include <portmidi.h>
#endif // _IRR_COMPILE_WITH_PORTMIDI_

#ifdef _IRR_COMPILE_WITH_WX_300_
	#include <wx/wx.h>
	#include <wx/artprov.h>
	#include <wx/xrc/xmlres.h>
	#include <wx/panel.h>
	#include <wx/gdicmn.h>
	#include <wx/font.h>
	#include <wx/colour.h>
	#include <wx/settings.h>
	#include <wx/string.h>
	#include <wx/bitmap.h>
	#include <wx/image.h>
	#include <wx/icon.h>
	#include <wx/notebook.h>
	#include <wx/sizer.h>
#endif

//#ifndef __IRR_EXT_INCLUDE_SOURCE_FILES__
//#define __IRR_EXT_INCLUDE_SOURCE_FILES__
//	#include <../source/Irrlicht/os.cpp>
//	#include <../source/Irrlicht/CImage.cpp>
//	#include <../source/Irrlicht/CColorConverter.cpp>
//	#include <../source/Irrlicht/CMeshSceneNode.cpp>
//	#include <../source/Irrlicht/CShadowVolumeSceneNode.cpp>
//#endif // __IRR_EXT_INCLUDE_SOURCE_FILES__

inline void PRINT_INFO()
{
#if defined(__x86_64__) || defined(_M_X64)
	dbPRINT("Architecture = x86 64-Bit\n");
#elif defined(__i386) || defined(_M_IX86)
	dbPRINT("Architecture = x86 32-Bit\n");
#else
	PRINT("Architecture = unknown\n");
#endif
}

#endif // __IRR_EXT_COMPILE_CONFIG_H_INCLUDED__
