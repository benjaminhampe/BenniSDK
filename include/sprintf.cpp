// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "sprintf.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>	// for static sprintf and internal printf
#include <cmath>
#include <ctime>
#include <wchar.h>

#ifdef _DEBUG
	#include <../source/Irrlicht/os.h>
#endif // _DEBUG


namespace irr
{
namespace core
{

stringc sprintf(const char* formatString, ...)
{
	va_list p;
	va_start(p,formatString);
	char tmp[1024];
#if defined(_MSC_VER)
	vsnprintf(&(tmp[0]), 1024, formatString, p);
#elif defined(__GNUC__)
	vsnprintf(&tmp[0], 1024, formatString, p);
#else

#endif
	va_end(p);
	return stringc(tmp);
}

/// function

stringw wsprintf(const wchar_t* formatString, ...)
{
	va_list p;
	va_start(p,formatString);
	wchar_t tmp[1024];
#if defined(_MSC_VER)
	_vsnwprintf(&(tmp[0]), 1024, formatString, p);
#elif defined(__GNUC__)
#if defined(_IRR_WINDOWS_)
	vsnwprintf(&tmp[0], 1024, formatString, p);
#else
	vswprintf(&tmp[0], 1024, formatString, p);
#endif
#else

#endif
	va_end(p);
	return stringw(tmp);
}

} // end namspace core

} // end namspace irr



