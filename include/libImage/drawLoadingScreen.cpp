// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "drawLoadingScreen.h"

namespace irr
{

namespace video
{

//
//bool drawLoadingScreen( video::IVideoDriver* driver,
//			video::ITexture* background_tex,
//			const core::stringw& text,
//			gui::IGUIFont* text_font,
//			const video::SColor& text_color)
//{
//	if (!driver)
//		return false;
//
//	core::dimension2du screen = driver->getScreenSize();
//
//	if (background_tex)
//	{
//		const core::dimension2du& tex_size = background_tex->getSize();
//
//		f32 aspect = (f32)tex_size.Height / (f32)tex_size.Width;
//
//		s32 height = aspect * screen.Width;
//
//		s32 y = ( (s32)screen.Height - height ) / 2;
//
//		driver->draw2DImage( background_tex,
//			core::recti( 0, y, screen.Width - 1, y + height ),
//			core::recti( core::position2di(0,0), background_tex->getSize()) );
//	}
//
//	if (text_font)
//	{
//		text_font->draw( text, core::recti(core::position2di(0,0), screen), text_color, true, true);
//	}
//
//	return true;
//}



} // end namespace video

} // end namespace irr
