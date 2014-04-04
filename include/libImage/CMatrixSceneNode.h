// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXTENSION_DYNAMIC_MATRIX_SCENE_NODE_H__
#define __IRR_EXTENSION_DYNAMIC_MATRIX_SCENE_NODE_H__

#include "../CompileConfig.h"

#include <../source/Irrlicht/os.h>
#include <../source/Irrlicht/CImage.h>

#include <cmath>

// The matrix holds the actual heightdata
// it is stored as pointer, so the scenenode does not create copies and wastes space

#include "CMatrix.h"

// uses function drawMeshBufferEx() to render depending on E_PRIMITIVE_TYPE
#include "renderByPrimitiveType.h"

// uses IColorGradient for coloring the vertices
// depending on height and min/max-height, if needed
#include "IColorGradient.h"

namespace irr
{
namespace scene
{

class CMatrixSceneNode : public ISceneNode
{
private:
	E_PRIMITIVE_TYPE PrimitiveType;
	core::vector3df MeshSize;
	video::IColorGradient* ColorGradient;
	core::CMatrix<f32>* Data; // Data should be normalized
	//core::vector2df DataMinMax;
	core::aabbox3df BoundingBox;
	video::SMaterial Material;
	core::array<video::S3DVertex> Vertices;
	core::array<s32> Indices;

public:
	///@brief constructor
	/**
	**/
	CMatrixSceneNode(
		E_PRIMITIVE_TYPE shapeType, /* default = EPT_TRIANGLES */
		const core::vector3df& meshSize,
		video::IColorGradient* colorGradient,
		core::CMatrix<f32>* matrixData,
		ISceneManager* smgr, ISceneNode* parent = 0, s32 id=-1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f) );

	///@brief destructor
	virtual ~CMatrixSceneNode();

//	///@brief
//	void clear();

	///@brief create mesh
	bool createMesh();
	bool createMeshAsTriangles();
	bool createMeshAsLogarithmicTriangles();
	bool createMeshAsLineStrips();
//	bool createMeshAsPointCloud();
//	bool createMeshAsLines();
//	bool createMeshAsTexturedBillboards( f32 billboard_width = core::PI );
//	bool createMeshAsTrianglesWithTextureAtlas( const core::dimension2du& tiles_size = core::dimension2du(64,64) );

	virtual core::stringc getInfoString() const
	{
		core::stringc s = "DataMinMax = ( 0,0 )\n";
		s += "MeshSize = ( ";
		s += MeshSize.X; s+=",";
		s += MeshSize.Y; s+=",";
		s += MeshSize.Z; s+=")\n";
		return s;
	}

	///@brief implement interface ISceneNode
	virtual void render();

	///@brief implement interface ISceneNode
	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return BoundingBox;
	}

	///@brief implement interface ISceneNode
	virtual u32 getMaterialCount() const
	{
		return 1;
	}

	///@brief implement interface ISceneNode
	virtual video::SMaterial& getMaterial(u32 num)
	{
		return Material;
	}

	///@brief implement interface ISceneNode
	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);
		ISceneNode::OnRegisterSceneNode();
	}

	///@brief Get MeshSize
	virtual core::vector3df getMeshSize( ) const
	{
		return MeshSize;
	}

	virtual void setMeshSize( const core::vector3df& size )  // inline
	{
		if ((size.X > 0.0f) && (size.Y > 0.0f) && (size.Z > 0.0f))
		{
			MeshSize = size;
		}
	}
	virtual video::IColorGradient* getColorGradient( )
	{
		return ColorGradient;
	}

	virtual void setColorGradient( video::IColorGradient* gradient )
	{
		// if (ColorGradient)
		//	ColorGradient->drop();

		ColorGradient = gradient;

		//if (ColorGradient)
		//	ColorGradient->grab();
	}

	virtual u32 getRows() const
	{
		if (!Data)
			return 0;
		else
			return Data->getRows();
	}

	virtual u32 getCols() const
	{
		if (!Data)
			return 0;
		else
			return Data->getCols();
	}

	virtual E_PRIMITIVE_TYPE getPrimitiveType( ) const
	{
		return PrimitiveType;
	}

	virtual void setPrimitiveType( E_PRIMITIVE_TYPE primType )
	{
		PrimitiveType = primType;
	}

	virtual core::CMatrix<f32>* getData()
	{
		return Data;
	}

	virtual bool setData( core::CMatrix<f32>* mat )
	{
		if (Data)
			Data->drop();

		Data = mat;

		if ( !mat )
			return false;

		createMesh();

		return true;
	}

};

} // end namespace scene
} // end namespace irr


#endif // __IRR_C_DYNAMIC_EQ_SCENE_NODE_H__
