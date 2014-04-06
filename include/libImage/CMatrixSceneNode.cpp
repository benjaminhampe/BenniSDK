// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CMatrixSceneNode.h"

namespace irr
{
namespace scene
{

//! default constructor - value ctr ( by render-shape, shape-size, color-gradient, matrix-data, irr::scene::ISceneNode-interface )
CMatrixSceneNode::CMatrixSceneNode(
	scene::E_PRIMITIVE_TYPE shapeType,
	const core::vector3df& meshSize,
	video::IColorGradient* colorGradient,
	core::CMatrix<f32>* matrixData,
	ISceneManager* smgr, ISceneNode* parent, s32 id, const core::vector3df& position,
	const core::vector3df& rotation, const core::vector3df& scale )

: ISceneNode( parent, smgr, id, position, rotation, scale)
, PrimitiveType( shapeType )
, MeshSize(meshSize)
, ColorGradient( colorGradient )
, Data( matrixData )
, BoundingBox( core::aabbox3df( core::vector3df(0,0,0), meshSize ))
, XAxis(0)
{
	dbPRINT( "CMatrixSceneNode::CMatrixSceneNode()\n" );

//	if (Data)
//		Data->grab();
//
//	if (ColorGradient)
//		ColorGradient->grab();

	Material.MaterialType = video::EMT_SOLID;
	Material.PointCloud = false;
	Material.Thickness = 1.0f;
	Material.Lighting = false;
	Material.Wireframe = false;
	Material.FogEnable = false;

	if (ColorGradient)
		Material.MaterialType = ColorGradient->getMaterialType();

	BoundingBox.reset( MeshSize );

	createMesh();
}

//! destructor
CMatrixSceneNode::~CMatrixSceneNode()
{
	dbPRINT( "CMatrixSceneNode::~CMatrixSceneNode()\n" );
	Vertices.clear();
	Indices.clear();
}

void CMatrixSceneNode::render()
{
	if (!IsVisible)
		return;

	video::IVideoDriver* driver = SceneManager->getVideoDriver();
    core::matrix4 oldWorldMatrix = driver->getTransform(video::ETS_WORLD);
    driver->setTransform(video::ETS_WORLD, getAbsoluteTransformation());
	driver->setMaterial( Material );
	video::drawElements( driver, Vertices, Indices, PrimitiveType );

	video::drawMeshBufferEx( driver, &FrontBuffer, scene::EPT_TRIANGLES );

	if (DebugDataVisible & EDS_BBOX)
	{
		video::SMaterial debugMaterial;
		debugMaterial.MaterialType = video::EMT_SOLID;
		debugMaterial.Lighting = false;
		debugMaterial.FogEnable = false;
		debugMaterial.Wireframe = false;

		driver->draw3DBox( BoundingBox );
	}

	driver->setTransform(video::ETS_WORLD, oldWorldMatrix);
}

//! create mesh by PrimtitiveType
bool CMatrixSceneNode::createMesh()
{
//	dbPRINT( "CMatrixSceneNode::createMesh()\n" );
	if (!Data || Data->empty())
		return false;

	bool result = false;

	if (PrimitiveType == EPT_TRIANGLES)
	{
		result = createMeshAsTriangles();
	}

	createFrontBuffer();
//
//	switch (PrimitiveType)
//	{
//		case EPT_POINTS: //result = createMeshAsPointCloud(); break;
//		case EPT_POINT_SPRITES: //result = createMeshAsTexturedBillboards(); break;
//		case EPT_LINES: result = createMeshAsLines(); break;
//		case EPT_LINE_STRIP: result = createMeshAsLineStrips(); break;
//		case EPT_LINE_LOOP: result = createMeshAsLineStrips(); break;
//		//case EPT_TRIANGLES: result = createMeshAsTriangles(); break;
//		case EPT_TRIANGLES: result = createMeshAsTriangles(); break;
//		case EPT_TRIANGLE_FAN: result = createMeshAsTriangles(); break;
//		case EPT_POLYGON: result = createMeshAsTriangles(); break;
//		case EPT_QUADS:	result = createMeshAsTrianglesWithTextureAtlas(); break;
//		case EPT_QUAD_STRIP: result = createMeshAsTexturedBillboards(); break;
//		default: break;
//	}
	return result;
}

void CMatrixSceneNode::createFrontBuffer()
{
	if (!Data)
		return;

	const u32 pointCount = Data->getCols();

	FrontBuffer.Vertices.reallocate( 4*(pointCount-1) );
	FrontBuffer.Indices.reallocate( 6*(pointCount-1) );
	FrontBuffer.Vertices.set_used( 0 );
	FrontBuffer.Indices.set_used( 0 );
	//FrontBuffer.Material = Material;

	const f32 x_scale = MeshSize.X * core::reciprocal( (f32)(pointCount-1) );
	const f32 y_scale = MeshSize.Y;

	video::SColor bgColor = video::SColor(255,255,0,255);
	if (ColorGradient)
		bgColor = ColorGradient->getColor( 0.0f );

	video::SColor c1 = bgColor;
	video::SColor c2 = bgColor;
	f32 y1 = 0.0f;
	f32 y2 = 0.0f;

	for (u32 i=0; i<pointCount-1; i++)
	{
		y1 = Data->getElement(0, i);
		y2 = Data->getElement(0, i+1);

		if (ColorGradient)
		{
			c1 = ColorGradient->getColor( y1 );
			c2 = ColorGradient->getColor( y2 );
		}

		video::S3DVertex A( 0,0,0,			0,0,-1, bgColor, 0,0);
		video::S3DVertex B( 0,y_scale*y1,0,	0,0,-1, c1, 	 0,0);
		video::S3DVertex C( 0,y_scale*y2,0,	0,0,-1, c2, 	 0,0);
		video::S3DVertex D( 0,0,0,			0,0,-1, bgColor, 0,0);

		if (XAxis)
		{
			A.Pos.X = ((*XAxis)[i]) * MeshSize.X;
			C.Pos.X = ((*XAxis)[i+1]) * MeshSize.X;
		}
		else
		{
			A.Pos.X = x_scale*(f32)i;
			C.Pos.X = x_scale*(f32)(i+1);
		}

		B.Pos.X = A.Pos.X;
		D.Pos.X = C.Pos.X;

		const u32 vCount = FrontBuffer.Vertices.size();
		FrontBuffer.Vertices.push_back( A );
		FrontBuffer.Vertices.push_back( B );
		FrontBuffer.Vertices.push_back( C );
		FrontBuffer.Vertices.push_back( D );

		FrontBuffer.Indices.push_back( vCount );
		FrontBuffer.Indices.push_back( vCount+1 );
		FrontBuffer.Indices.push_back( vCount+2 );
		FrontBuffer.Indices.push_back( vCount );
		FrontBuffer.Indices.push_back( vCount+2 );
		FrontBuffer.Indices.push_back( vCount+3 );
	}

	FrontBuffer.recalculateBoundingBox();
}

/// EPT_TRIANGLES
bool CMatrixSceneNode :: createMeshAsTriangles()
{
//	dbPRINT("CMatrixSceneNode::createMeshAsTriangles()\n")
	if (!Data)
		return false;

	if (Data->empty())
		return false;

	const u32 r = Data->getRows();
	const u32 c = Data->getCols();

	if ((r==0) || (c==0))
	{
		dbPRINT("r==0 && c==0\n");
		return false;
	}

	Vertices.reallocate( 4*(r-1)*(c-1) );
	Vertices.set_used( 0 );

	Indices.reallocate( 6*(r-1)*(c-1) );
	Indices.set_used( 0 );

	BoundingBox.reset( MeshSize );

	const f32 step_x = MeshSize.X / (f32)(c-1);
	const f32 step_y = MeshSize.Y;
	const f32 step_z = MeshSize.Z / (f32)(r-1);

	f32 y1, y2, y3, y4;
	f32 t1, t2, t3, t4;
	video::SColor c1, c2, c3, c4;
	c1 = video::SColor(255,100,100,255);
	c2 = c1;
	c3 = c1;
	c4 = c1;

	/// loop rows ( y-direction )
	for (u32 y=0; y<r-1; y++)
	{
		/// loop columns ( x-direction )
		for (u32 x=0; x<c-1; x++)
		{
			y1 = Data->getElement(y,x); // normalized y-height to range 0..1, needed for color-gradient
			y2 = Data->getElement(y+1,x);	// normalized y-height to range 0..1, needed for color-gradient
			y3 = Data->getElement(y+1,x+1);	// normalized y-height to range 0..1, needed for color-gradient
			y4 = Data->getElement(y,x+1);	// normalized y-height to range 0..1, needed for color-gradient

			if (ColorGradient)
			{
				// t1 = (y1-data_min_max_.X)*data_range_inv_; // normalized y-height to range 0..1, needed for color-gradient
				// t2 = (y2-data_min_max_.X)*data_range_inv_;	// normalized y-height to range 0..1, needed for color-gradient
				// t3 = (y3-data_min_max_.X)*data_range_inv_;	// normalized y-height to range 0..1, needed for color-gradient
				// t4 = (y4-data_min_max_.X)*data_range_inv_;	// normalized y-height to range 0..1, needed for color-gradient

				c1 = ColorGradient->getColor(y1);// t1);
				c2 = ColorGradient->getColor(y2);// t2);
				c3 = ColorGradient->getColor(y3);// t3);
				c4 = ColorGradient->getColor(y4);// t4);
			}

			video::S3DVertex A( 0,y1*step_y,(f32)y*step_z, 0,0,-1, c1, 0,0);
			video::S3DVertex B( 0,y2*step_y,(f32)(y+1)*step_z, 0,0,-1, c2, 0,0);
			video::S3DVertex C( 0,y3*step_y,(f32)(y+1)*step_z, 0,0,-1, c3, 0,0);
			video::S3DVertex D( 0,y4*step_y,(f32)y*step_z, 0,0,-1, c4, 0,0);

			if (XAxis)
			{
				A.Pos.X = ((*XAxis)[x]) * MeshSize.X;
				C.Pos.X = ((*XAxis)[x+1]) * MeshSize.X;
			}
			else
			{
				A.Pos.X = (f32)x*step_x;
				C.Pos.X = (f32)(x+1)*step_x;
			}

			B.Pos.X = A.Pos.X;
			D.Pos.X = C.Pos.X;

			u32 k = Vertices.size();

			Vertices.push_back( A );
			Vertices.push_back( B );
			Vertices.push_back( C );
			Vertices.push_back( D );

			Indices.push_back( k );
			Indices.push_back( k+1 );
			Indices.push_back( k+2 );
			Indices.push_back( k );
			Indices.push_back( k+2 );
			Indices.push_back( k+3 );
		}
	}
//	dbPRINT("OK\n")
	return true;
}
//
///// EPT_TRIANGLE_FAN
//bool CMatrixSceneNode :: createMeshAsLogarithmicTriangles()
//{
//	if (!Data)
//		return false;
//
//	const u32 r = Data->getRows();
//	const u32 c = Data->getCols();
//
//	if ((r==0) || (c==0))
//		return false;
//
//	Vertices.reallocate( 4*(r-1)*(c-1) );
//	Vertices.set_used( 0 );
//
//	Indices.reallocate( 6*(r-1)*(c-1) );
//	Indices.set_used( 0 );
//
//	BoundingBox.reset(MeshSize);
//
//	const f32 step_x = MeshSize.X / (f32)(c-1);
//	const f32 step_y = MeshSize.Y;
//	const f32 step_z = MeshSize.Z / (f32)(r-1);
//
//	f32 y1, y2, y3, y4;
//	//f32 t1, t2, t3, t4;
//	video::SColor c1, c2, c3, c4;
//	c1 = video::SColor(255,100,100,255);
//	c2 = c1;
//	c3 = c1;
//	c4 = c1;
//
//	/// loop rows ( y-direction )
//	for (u32 y=0; y<r-1; y++)
//	{
//		/// loop columns ( x-direction )
//		for (u32 x=0; x<c-1; x++)
//		{
//			y1 = Data->getElement(y,x); // normalized y-height to range 0..1, needed for color-gradient
//			y2 = Data->getElement(y+1,x);	// normalized y-height to range 0..1, needed for color-gradient
//			y3 = Data->getElement(y+1,x+1);	// normalized y-height to range 0..1, needed for color-gradient
//			y4 = Data->getElement(y,x+1);	// normalized y-height to range 0..1, needed for color-gradient
//
//			if (ColorGradient)
//			{
//				// t1 = (y1-data_min_max_.X)*data_range_inv_; // normalized y-height to range 0..1, needed for color-gradient
//				// t2 = (y2-data_min_max_.X)*data_range_inv_;	// normalized y-height to range 0..1, needed for color-gradient
//				// t3 = (y3-data_min_max_.X)*data_range_inv_;	// normalized y-height to range 0..1, needed for color-gradient
//				// t4 = (y4-data_min_max_.X)*data_range_inv_;	// normalized y-height to range 0..1, needed for color-gradient
//
//				c1 = ColorGradient->getColor(y1); // t1);
//				c2 = ColorGradient->getColor(y2); // t2);
//				c3 = ColorGradient->getColor(y3); // t3);
//				c4 = ColorGradient->getColor(y4); // t4);
//			}
//
//			f32 x1 = core::clamp<f32>(1.0f-(f32)(x+1)/(f32)c, 0.0f, 1.0f );
//			f32 x2 = core::clamp<f32>(1.0f-(f32)(x+2)/(f32)c, 0.0f, 1.0f );
//
//			u32 k = Vertices.size();
//
//			Vertices.push_back( video::S3DVertex( (f32)x1*MeshSize.X,y1*step_y,(f32)y*step_z, 0,0,-1, c1, 0,0) );
//			Vertices.push_back( video::S3DVertex( (f32)x1*MeshSize.X,y2*step_y,(f32)(y+1)*step_z, 0,0,-1, c2, 0,0) );
//			Vertices.push_back( video::S3DVertex( (f32)x2*MeshSize.X,y3*step_y,(f32)(y+1)*step_z, 0,0,-1, c3, 0,0) );
//			Vertices.push_back( video::S3DVertex( (f32)x2*MeshSize.X,y4*step_y,(f32)y*step_z, 0,0,-1, c4, 0,0) );
//
//			Indices.push_back( k );
//			Indices.push_back( k+1 );
//			Indices.push_back( k+2 );
//			Indices.push_back( k );
//			Indices.push_back( k+2 );
//			Indices.push_back( k+3 );
//		}
//	}
//	return true;
//}

/// EPT_LINE_STRIP
bool CMatrixSceneNode::createMeshAsLineStrips()
{
	if (!Data)
		return false;

	const u32& rows = Data->getRows();
	const u32& cols = Data->getCols();

	if ((rows==0) || (cols==0))
		return false;

	Vertices.reallocate( rows*cols*4 );
	Vertices.set_used( 0 );
	Indices.reallocate( rows*cols*6 );
	Indices.set_used( 0 );

	Material.PointCloud = true;
	Material.Thickness = 3.55f;

	BoundingBox.reset( MeshSize );

	const f32 step_x = MeshSize.X / (f32)(cols-1);
	const f32 step_y = MeshSize.Y;
	const f32 step_z = MeshSize.Z / (f32)(rows-1);

	/// loop columns ( x-direction )
	video::SColor c0 = 0xffffffff;

	for (u32 x=0; x<cols; x++)
	{
		for (u32 y=0; y<rows-1; y++)
		{
			const f32 y0 = Data->getElement(y,x);

			if (ColorGradient)
			{
				c0 = ColorGradient->getColor( y0 );
			}

			const u32 k = Vertices.size();

			Vertices.push_back( video::S3DVertex( (f32)x*step_x, y0*step_y, (f32)y*step_z, 0,0,-1, c0, 0,0) );
			Indices.push_back( k );
		}
	}

	return true;
}
//
///// EPT_LINES
//bool CMatrixSceneNode::createMeshAsLines()
//{
//	if (!Data)
//		return false;
//
//	const u32& rows = Data->getRows();
//	const u32& cols = Data->getCols();
//
//	if ((rows==0) || (cols==0))
//		return false;
//
//	Vertices.reallocate( rows*cols*4 );
//	Vertices.set_used( 0 );
//	Indices.reallocate( rows*cols*6 );
//	Indices.set_used( 0 );
//
//	Material.MaterialType = video::EMT_SOLID;
//	// Material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
//	// Material.MaterialType = (ColorGradient)?ColorGradient->getMaterialType():video::EMT_SOLID;
//	Material.Thickness = 1.5f;
//	Material.Lighting = false;
//	Material.Wireframe = false;
//	Material.FogEnable = false;
//
//	const core::vector2df DataMinMax(0,160); // = Data->getMinMax();
////	const f32 DataRange = DataMinMax.Y-DataMinMax.X;
//	const f32 DataRangeInv = core::reciprocal( 160.0f /* in dB */ );
//	const core::vector3df& MeshExtent = BoundingBox.getExtent();
//	const f32 step_x = MeshExtent.X / (f32)cols;
//	const f32 step_y = 1.0f;//MeshExtent.Y / DataRange;
//	const f32 step_z = MeshExtent.Z / (f32)rows;
//
//
//
//	/// loop rows ( y-direction )
////	for (u32 y=0; y<rows; y++)
////	{
////		for (u32 x=0; x<cols-1; x++)
////		{
////			const f32 y0 = Data->getElement(y,x);
////			const f32 y1 = Data->getElement(y,x+1);
////			const f32 t0 = (y0 - DataMinMax.X) * DataRangeInv;
////			const f32 t1 = (y1 - DataMinMax.X) * DataRangeInv;
////			const video::SColor c0 = (ColorGradient)?ColorGradient->getColor( t0 ):0xffffffff;
////			const video::SColor c1 = (ColorGradient)?ColorGradient->getColor( t1 ):0xffffffff;
////
////			const u32 k = Vertices.size();
////
////			Vertices.push_back( video::S3DVertex( (f32)x*step_x, y0*step_y, (f32)y*step_z, 0,0,-1, c0, 0,0) );
////			Vertices.push_back( video::S3DVertex( (f32)(x+1)*step_x, y1*step_y, (f32)y*step_z, 0,0,-1, c1, 0,0) );
////			Indices.push_back( k );
////			Indices.push_back( k+1 );
////		}
////	}
//
//	video::SColor c1, c2;
//
//	/// loop columns ( x-direction )
//	for (u32 x=0; x<cols; x++)
//	{
//		for (u32 y=0; y<rows-1; y++)
//		{
//			const f32 y1 = Data->getElement(y,x);
//			const f32 y2 = Data->getElement(y+1,x);
////			const f32 t0 = (y0 - DataMinMax.X) * DataRangeInv;
////			const f32 t1 = (y1 - DataMinMax.X) * DataRangeInv;
////			const video::SColor c0 = (ColorGradient)?ColorGradient->getColor( t0 ):0xffffffff;
////			const video::SColor c1 = (ColorGradient)?ColorGradient->getColor( t1 ):0xffffffff;
//
//			const f32 t1 = (y1-DataMinMax.X)*DataRangeInv; // normalized y-height to range 0..1, needed for color-gradient
//			const f32 t2 = (y2-DataMinMax.X)*DataRangeInv;	// normalized y-height to range 0..1, needed for color-gradient
//
//			u32 cf = core::s32_clamp( (s32)(t1*255.0f), 0, 255);
//			c1 = video::SColor( 255, cf,cf,255 );
//			cf = core::s32_clamp( (s32)(t2*255.0f), 0, 255);
//			c2 = video::SColor( 255, cf,cf,255 );
////			cf = core::s32_clamp( (s32)(t3*255.0f), 0, 255);
////			c3 = video::SColor( 255, cf,cf,255 );
////			cf = core::s32_clamp( (s32)(t4*255.0f), 0, 255);
////			c4 = video::SColor( 255, cf,cf,255 );
//
//			const u32 k = Vertices.size();
//
//			Vertices.push_back( video::S3DVertex( (f32)x*step_x, y1*step_y, (f32)y*step_z, 0,0,-1, c1, 0,0) );
//			Vertices.push_back( video::S3DVertex( (f32)x*step_x, y2*step_y, (f32)(y+1)*step_z, 0,0,-1, c2, 0,0) );
//			Indices.push_back( k );
//			Indices.push_back( k+1 );
//		}
//	}
//
//	return true;
//}
//
///// EPT_POINTS
//bool CMatrixSceneNode::createMeshAsPointCloud()
//{
//	return false;
//}
//
//
///// EPT_QUADS
//bool CMatrixSceneNode::createMeshAsTrianglesWithTextureAtlas( const core::dimension2du& tiles_size )
//{
//	return false;
//}
//
///// EPT_POINT_SPRITE
//bool CMatrixSceneNode::createMeshAsTexturedBillboards( f32 billboard_width )
//{
//	return false;
//}



} // end namespace scene
} // end namespace irr
