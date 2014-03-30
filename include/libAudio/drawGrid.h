// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_DRAW_GRID_H__
#define __IRR_EXT_DRAW_GRID_H__

#include <irrlicht.h>

#include <libImage/renderByPrimitiveType.h>

namespace irr
{

// create filled path between data-points and y=0 ( x-axis )
enum E_AXIS_SCALE
{
	EAS_LINEAR=0,
	EAS_LOG10,
	EAS_LOG2,
	EAS_COUNT
};

struct SGrid
{

};

// render as EPT_LINES

void drawGrid(
	video::IVideoDriver* driver,
	const core::rectf& pos = core::rectf(0,0,1,1),
	E_AXIS_SCALE xScaling = EAS_LOG10,
	E_AXIS_SCALE yScaling = EAS_LOG10,
	const video::SColor& color = video::SColor(255,100,100,100) )
{
	scene::SMeshBuffer buffer;

	// exit condition
	if (!driver)
		return;

	/**
		Material
	*/

	buffer.Material.MaterialType = video::EMT_SOLID;
//	if (color.getAlpha()<255)
//		buffer.Material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;

	buffer.Material.Lighting = false;
	buffer.Material.FogEnable = false;

	/**
		Allocate memory for vertices and indices
	*/

//	buffer.Vertices.reallocate( 4*(pointCount-1) );
	buffer.Vertices.set_used( 0 );
//	buffer.Indices.reallocate( 6*(pointCount-1) );
	buffer.Indices.set_used( 0 );


	const f32 x_min = pos.UpperLeftCorner.X;
	const f32 y_min = pos.UpperLeftCorner.Y;
	const f32 x_max = pos.LowerRightCorner.X;
	const f32 y_max = pos.LowerRightCorner.Y;

	/**
		add vertical lines ( x = const per line )
	*/

	if ( xScaling == EAS_LINEAR )
	{
		const u32 x_count = 50;
		const f32 x_range = x_max-x_min;
		const f32 x_step = x_range / (f32)x_count;

		for ( u32 i = 0; i < x_count; i++ )
		{
			const f32 x = x_step*i;
			video::S3DVertex A( x,y_min,0.f, 0,0,-1, color, 0,0);
			video::S3DVertex B( x,y_max,0.f, 0,0,-1, color, 0,0);

			u32 vCount = buffer.getVertexCount();
			buffer.Vertices.push_back( A );
			buffer.Vertices.push_back( B );
			buffer.Indices.push_back( vCount );
			buffer.Indices.push_back( vCount+1 );
		}
	}
	else if ( xScaling == EAS_LOG10 )
	{
		const u32 x_count = 10;
		const f32 x_step = (x_max-x_min) / (f32)x_count;

		for ( u32 i = 0; i < x_count; i++ )
		{
			const f32 x = x_step*i;
			video::S3DVertex A( x,y_min,0.f, 0,0,-1, color, 0,0);
			video::S3DVertex B( x,y_max,0.f, 0,0,-1, color, 0,0);

			u32 vCount = buffer.getVertexCount();
			buffer.Vertices.push_back( A );
			buffer.Vertices.push_back( B );
			buffer.Indices.push_back( vCount );
			buffer.Indices.push_back( vCount+1 );
		}
	}
	else if ( xScaling == EAS_LOG2 )
	{
		const u32 x_count = 10;
		const f32 x_step = (x_max-x_min) / (f32)x_count;

		for ( u32 i = 0; i < x_count; i++ )
		{
			const f32 x = x_step*i;
			video::S3DVertex A( x,y_min,0.f, 0,0,-1, color, 0,0);
			video::S3DVertex B( x,y_max,0.f, 0,0,-1, color, 0,0);

			u32 vCount = buffer.getVertexCount();
			buffer.Vertices.push_back( A );
			buffer.Vertices.push_back( B );
			buffer.Indices.push_back( vCount );
			buffer.Indices.push_back( vCount+1 );
		}
	}
	/**
		add horizontal lines ( y = const per line )
	*/

	if ( yScaling == EAS_LINEAR )
	{
		u32 y_count = (u32)core::round32(y_max-y_min);
		if ( y_count > 100 )
			y_count = (u32)core::round32(0.1f*(y_max-y_min));

		const f32 y_step = (y_max-y_min) / (f32)y_count;

		for ( u32 i = 0; i < y_count; i++ )
		{
			const f32 y = y_step*i;
			video::S3DVertex A( x_min,y,0, 0,0,-1, color, 0,0);
			video::S3DVertex B( x_max,y,0, 0,0,-1, color, 0,0);

			u32 vCount = buffer.getVertexCount();
			buffer.Vertices.push_back( A );
			buffer.Vertices.push_back( B );
			buffer.Indices.push_back( vCount );
			buffer.Indices.push_back( vCount+1 );
		}
	}
	else if ( yScaling == EAS_LOG10 )
	{
		const u32 y_count = 10;
		const f32 y_step = (y_max-y_min) / (f32)y_count;

		for ( u32 i = 0; i < y_count; i++ )
		{
			const f32 y = y_step*i;
			video::S3DVertex A( x_min,y,0, 0,0,-1, color, 0,0);
			video::S3DVertex B( x_max,y,0, 0,0,-1, color, 0,0);

			u32 vCount = buffer.getVertexCount();
			buffer.Vertices.push_back( A );
			buffer.Vertices.push_back( B );
			buffer.Indices.push_back( vCount );
			buffer.Indices.push_back( vCount+1 );
		}
	}
	else if ( yScaling == EAS_LOG2 )
	{
		const u32 y_count = 10;
		const f32 y_step = (y_max-y_min) / (f32)y_count;

		for ( u32 i = 0; i < y_count; i++ )
		{
			const f32 y = y_step*i;
			video::S3DVertex A( x_min,y,0, 0,0,-1, color, 0,0);
			video::S3DVertex B( x_max,y,0, 0,0,-1, color, 0,0);

			u32 vCount = buffer.getVertexCount();
			buffer.Vertices.push_back( A );
			buffer.Vertices.push_back( B );
			buffer.Indices.push_back( vCount );
			buffer.Indices.push_back( vCount+1 );
		}
	}

	buffer.recalculateBoundingBox();

	driver->setMaterial( buffer.Material );

	video::drawMeshBufferEx( driver, &buffer, scene::EPT_LINES, core::vector3df(0,0,0) );
}


} // end namespace irr



#endif // __IRR_EXT_DRAW_GRID_H__
