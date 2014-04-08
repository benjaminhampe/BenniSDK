// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_DEBUG_PRINT_H__
#define __IRR_EXT_DEBUG_PRINT_H__

#include <cstdio>
#include <cstdarg>

/// debugPrint
#if defined(_DEBUG) || defined(DEBUG)
	#ifdef _IRR_COMPILE_WITH_FLTK_
		#define dbPRINT( args... ) \
		{ \
			fprintf( stdout, args ); \
			fflush( stdout ); \
		}

		#define dbERROR( args... ) \
		{ \
			fprintf( stderr, args ); \
			fflush( stderr ); \
			fl_alert( args ); \
		}
	#else
		#define dbPRINT( args... ) \
		{ \
			fprintf( stdout, args ); \
			fflush( stdout ); \
		}

		#define dbERROR( args... ) \
		{ \
			fprintf( stderr, args ); \
			fflush( stderr ); \
		}
	#endif // _IRR_COMPILE_WITH_FLTK_
#else
	#define dbPRINT( args... )
	#ifdef _IRR_COMPILE_WITH_FLTK_
		#define dbERROR( args... ) \
		{ \
			fl_alert( args ); \
		}
	#else
		#define dbERROR( args... )
	#endif // _IRR_COMPILE_WITH_FLTK_
#endif

#endif // __IRR_EXT_DEBUG_PRINT_H__
