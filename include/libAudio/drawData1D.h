// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_DRAW_FOURIER_DATA_H__
#define __IRR_EXT_DRAW_FOURIER_DATA_H__

#include <irrlicht.h>

#include <libImage/renderByPrimitiveType.h>
#include <libAudio/drawGrid.h>

namespace irr
{

template <class T>
void drawData1D(
	video::IVideoDriver* driver,
	const core::array<T>& points,
	const video::SColor& color = video::SColor(200,255,128,30),
	scene::E_PRIMITIVE_TYPE pType = scene::EPT_TRIANGLES,
	const core::rect<T>& zoomRect = core::rect<T>(0,0,0,0),
	E_AXIS_SCALE xScaling = EAS_LINEAR,
	E_AXIS_SCALE yScaling = EAS_LINEAR )
{
	// exit condition
	if (!driver)
		return;

	const u32& pointCount = points.size();

	// exit condition
	if ( pointCount < 2 )
		return;

	scene::SMeshBuffer buffer;

	/**
		Material
	*/

	buffer.Material.MaterialType = video::EMT_SOLID;
	if (color.getAlpha()<255)
		buffer.Material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;

	buffer.Material.Lighting = false;
	buffer.Material.FogEnable = false;

	/**
		Allocate memory for vertices and indices
	*/

	buffer.Vertices.reallocate( 4*(pointCount-1) );
	buffer.Vertices.set_used( 0 );
	buffer.Indices.reallocate( 6*(pointCount-1) );
	buffer.Indices.set_used( 0 );

	/**
		get Minimum and Maximum values from dataSet
	*/

	f32 data_y_min = FLT_MAX;
	f32 data_y_max = FLT_MIN;

	for (u32 i=0; i<pointCount; i++)
	{
		// minimum
		if (data_y_min > points[i])
			data_y_min = points[i];

		// maximum
		if (data_y_max < points[i])
			data_y_max = points[i];
	}

	// for color-gradient
//	const f32 data_y_range = core::abs_<f32>( data_y_max - data_y_min );
//	f32 data_y_range_inv = 1.0f;
//	if ( !core::equals( data_y_range, 0.0f) )
//	{
//		data_y_range_inv = core::reciprocal( data_y_range );
//	}

	const f32 step_x = core::reciprocal( (f32)(pointCount-1) );

	/**
		add Vertices and Indices
	*/

	for (u32 i=0; i<pointCount-1; i++)
	{
		const f32 y1 = points[i];
		const f32 y2 = points[i+1];

		video::SColor color1 = color;
		video::SColor color2 = color;

//		if (gradient)
//		{
//			f32 t1 = core::abs_<f32>(y1-min_y)*rec_diff_y;
//			f32 t2 = core::abs_<f32>(y2-min_y)*rec_diff_y;
//			color1 = gradient->getColor( t1 );
//			color2 = gradient->getColor( t2 );
//		}

		video::S3DVertex A,B,C,D;
		A.Pos = core::vector3df((f32)i*step_x,0.f,0.f);
		A.Normal = core::vector3df( 0,0,-1);
		A.Color = color;
		A.TCoords = core::vector2df(0,0);
		B.Pos = core::vector3df((f32)i*step_x,y1,0.f);
		B.Normal = core::vector3df(0,0,-1);
		B.Color = color1;
		B.TCoords = core::vector2df(0,0);
		C.Pos = core::vector3df((f32)(i+1)*step_x,y2,0.f);
		C.Normal = core::vector3df(0,0,-1);
		C.Color = color2;
		C.TCoords = core::vector2df(0,0);
		D.Pos = core::vector3df((f32)(i+1)*step_x,0.f,0.f);
		D.Normal = core::vector3df(0,0,-1);
		D.Color = color;
		D.TCoords = core::vector2df(0,0);

		const u32& vCount = buffer.Vertices.size();
		buffer.Vertices.push_back( A );
		buffer.Vertices.push_back( B );
		buffer.Vertices.push_back( C );
		buffer.Vertices.push_back( D );

		buffer.Indices.push_back( vCount );
		buffer.Indices.push_back( vCount+1 );
		buffer.Indices.push_back( vCount+2 );
		buffer.Indices.push_back( vCount );
		buffer.Indices.push_back( vCount+2 );
		buffer.Indices.push_back( vCount+3 );
	}

	buffer.recalculateBoundingBox();

	driver->setMaterial( buffer.Material );

	video::drawMeshBufferEx( driver, &buffer, pType, core::vector3df(0,0,0) );
}


} // end namespace irr



#endif // __IRR_EXT_DRAW_3D_CIRCLE_H__
