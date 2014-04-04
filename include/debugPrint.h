// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_DEBUG_PRINT_H__
#define __IRR_EXT_DEBUG_PRINT_H__

//#include <cstdlib>
#include <cstdio>
#include <cstdarg>

/// debugPrint
#if defined(_DEBUG) || defined(DEBUG)
	#define dbPRINT( args... ) { printf( args ); fflush(stdout); }
	#define dbERROR( args... ) { fprintf( stderr, args ); fflush(stderr); fprintf( stdout, args ); fflush(stdout); }
#else
	#define dbPRINT( args... )
	#define dbERROR( args... ) { fprintf( stdout, args ); fflush(stdout); }
#endif

#endif // __IRR_EXT_DEBUG_PRINT_H__
