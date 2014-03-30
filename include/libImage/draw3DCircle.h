// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_DRAW_3D_CIRCLE_H__
#define __IRR_EXT_DRAW_3D_CIRCLE_H__

#include <irrlicht.h>

#include <libImage/sinCosTable.h>

namespace irr
{

/// Circle

void draw3DCircleXY(
	video::IVideoDriver* driver,
	const core::vector3df& pos,
	f32 radius,
	u32 tesselation,
	bool filled,
	const video::SColor& color );

} // end namespace irr



#endif // __IRR_EXT_DRAW_3D_CIRCLE_H__
