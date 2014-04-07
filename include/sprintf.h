// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_SPRINTF_H__
#define __IRR_EXT_SPRINTF_H__

#include <irrlicht.h>

namespace irr{
namespace core{

	// format value
	stringc sprintf( const char* formatString, ...);

	// format value
	stringw wsprintf( const wchar_t* formatString, ...);

} // end namespace core
} // end namespace irr

#endif // __IRR_EXT_SPRINTF_H__

