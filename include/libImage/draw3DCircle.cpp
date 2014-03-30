// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "draw3DCircle.h"

namespace irr
{

/// Circle

void draw3DCircleXY( video::IVideoDriver* driver, const core::vector3df& pos,
	f32 radius, u32 tesselation, bool filled, const video::SColor& color )
{
	if (!driver)
		return;

	scene::SMeshBuffer buffer;

	// material
	buffer.Material.MaterialType = video::EMT_SOLID;
	buffer.Material.Lighting = false;
	buffer.Material.FogEnable = false;

	if (color.getAlpha()<255)
		buffer.Material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;

	// bbox
	buffer.BoundingBox.reset(
		core::aabbox3df(
			pos.X-radius, pos.Y-radius, pos.Z,
			pos.X+radius, pos.Y+radius, pos.Z ) );

	// memory
	if (filled)	// draw as EPT_TRIANGLES
	{
		buffer.Vertices.reallocate( tesselation+1 );
		buffer.Vertices.set_used( 0 );
		buffer.Indices.reallocate( 3*tesselation );
		buffer.Indices.set_used( 0 );
		//buffer.PrimitiveType = EPT_TRIANGLES;

		// push center vertex
		buffer.Vertices.push_back( video::S3DVertex(pos.X,pos.Y,pos.Z,0,0,-1,color,0.5f,0.5f) );
	}
	else	// draw as EPT_LINE_LOOP
	{
		buffer.Vertices.reallocate( tesselation );
		buffer.Vertices.set_used( 0 );
		buffer.Indices.reallocate( tesselation );
		buffer.Indices.set_used( 0 );
		//buffer.PrimitiveType = EPT_LINE_LOOP;
	}

	// tables
	core::CSinTablef sinTable( tesselation );
	core::CCosTablef cosTable( tesselation );


	// fill vertices and indices for (tesselation-1) x segments ( either a triangle or only one vertex )
	for (u32 i=0; i<tesselation; ++i)
	{
		const f32 x = pos.X-radius * sinTable[i];
		const f32 y = pos.Y+radius * cosTable[i];
		const f32& z = pos.Z;
		const f32 u = 0.5f-0.5f*sinTable[i];
		const f32 v = 0.5f-0.5f*cosTable[i];
		buffer.Vertices.push_back( video::S3DVertex(x,y,z,0,0,-1,color,u,v) );

		if (filled)
		{
			buffer.Indices.push_back(0);
			u32 j=i+2;
			if (j>=tesselation+1)
				j=1;
			buffer.Indices.push_back(j);
			buffer.Indices.push_back(i+1);
		}
		else
		{
			buffer.Indices.push_back(i);
		}
	}

	driver->setMaterial( buffer.Material );

	if (filled)
	{
		driver->drawVertexPrimitiveList(
			buffer.getVertices(),
			buffer.getVertexCount(),
			buffer.getIndices(),
			buffer.getIndexCount()/3,
			video::EVT_STANDARD,
			scene::EPT_TRIANGLES,
			video::EIT_16BIT );
	}
	else
	{
		driver->drawVertexPrimitiveList(
			buffer.getVertices(),
			buffer.getVertexCount(),
			buffer.getIndices(),
			buffer.getIndexCount()-1,
			video::EVT_STANDARD,
			scene::EPT_LINE_LOOP,
			video::EIT_16BIT );
	}
}

} // end namespace irr
