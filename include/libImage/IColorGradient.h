// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_ICOLOR_GRADIENT_H__
#define __IRR_EXT_ICOLOR_GRADIENT_H__

#include <irrTypes.h>
#include <irrMath.h>
#include <irrArray.h>
#include <irrString.h>
#include <IReferenceCounted.h>

#include <SColor.h>
#include <IImage.h>
#include <../source/Irrlicht/CImage.h>

#include <debugPrint.h>

namespace irr
{
namespace video
{
	/// @brief convert SColor to formatted string
	REALINLINE core::stringc toString( const SColor& color )
	{
		core::stringc s("SColor(");
		s += color.getAlpha();
		s += ",";
		s += color.getRed();
		s += ",";
		s += color.getGreen();
		s += ",";
		s += color.getBlue();
		s += ")";
		return s;
	}

	/// @brief convert SColorf to formatted string
	REALINLINE core::stringc toString( const SColorf& color )
	{
		core::stringc s("SColorf(");
		s += color.getAlpha();
		s += ",";
		s += color.getRed();
		s += ",";
		s += color.getGreen();
		s += ",";
		s += color.getBlue();
		s += ")";
		return s;
	}

	/// @class IColorGradient ( pure virtual/abstract methods -> interface definition )
	class IColorGradient : public IReferenceCounted
	{
	public:

		/// @class MyColorStruct
		struct MyColorStruct
		{
			SColorf Color;	// 128-bit color for better interplation quality
			f32 Position;	// in range 0..1

			/// @brief default constructor
			MyColorStruct( )
				: Color(SColorf(0,0,0,0)), Position(0)
			{}

			/// @brief value constructor
			MyColorStruct( const SColor& color, f32 t )
				: Color(SColorf(color)), Position(t)
			{}

			/// @brief value constructor
			MyColorStruct( const SColorf& color, f32 t )
				: Color(color), Position(t)
			{}

			/// @brief copy constructor
			MyColorStruct( const MyColorStruct& other)
				: Color(SColorf(0,0,0,0)), Position(0.0f)
			{
				if ( this != &other )
				{
					Color = (other.Color);
					Position = (other.Position);
				}
			}

			/// @brief copy operator
			MyColorStruct& operator= ( const MyColorStruct& other )
			{
				if ( this == &other )
					return *this;

				Color = (other.Color);
				Position = (other.Position);
				return *this;
			}

			/// @brief multiplies color times t in range 0..1
			virtual MyColorStruct& operator* ( f32 value )
			{
				value = core::clamp<f32>( value, 0.0f, 1.0f );
				Color.set(
					core::clamp<f32>( Color.getAlpha() * value, 0.f, 1.f ),
					core::clamp<f32>( Color.getRed() * value, 0.f, 1.f ),
					core::clamp<f32>( Color.getGreen() * value, 0.f, 1.f ),
					core::clamp<f32>( Color.getBlue() * value, 0.f, 1.f ) );
				return *this;
			}

			/// @brief lower than operator, only operating on t
			virtual bool operator< ( const MyColorStruct& other ) const
			{
				if ( this == &other )
					return false;

				if ( Position < other.Position )
					return true;
				else
					return false;
			}

			/// @brief greater than operator, only operating on t
			virtual bool operator> ( const MyColorStruct& other ) const
			{
				if ( this == &other )
					return false;

				if ( Position < other.Position )
					return true;
				else
					return false;
			}
		};

	protected:
		core::stringc Name;

		core::array<MyColorStruct> Colors;

	public:
		/// @brief get formatted self-decription ( text as stringc )
		virtual core::stringc toString() const = 0;

		/// return interpolated color t in range [0,1]
		virtual SColor getColor( f32 t ) const = 0;

		/// return interpolated color t in range [0,1]
		virtual SColorf getColorf( f32 t ) const = 0;

		/// @brief load from xml file-stream
		virtual bool read ( io::IXMLReader* pReader ) = 0;

		/// @brief save to xml file-stream
		virtual bool write ( io::IXMLWriter* pWriter ) const = 0;

		/// @brief value constructor ( default )
		explicit IColorGradient( core::stringc name = "", u32 color_count = 0 )
			: Name(name)
		{
			Colors.reallocate( color_count );
			Colors.set_used( 0 );

			const MyColorStruct zeroColor( SColorf(0,0,0,0), 0.0f );

			for (u32 i=0; i<color_count; i++)
			{
				Colors.push_back( zeroColor );
			}
		}

		/// @brief value destructor, erases used memory for stop-color-array
		virtual ~IColorGradient()
		{
			Colors.clear();
		}

		/// @brief set basename of color-gradient
		virtual void setName( const core::stringc& name )
		{
			Name = name;
		}

		/// @brief get basename of color-gradient
		virtual core::stringc getName( ) const
		{
			return Name;
		}

		/// @brief erase whole allocated memory by clearing the arrays
		virtual void clear()
		{
			Colors.clear();
		}

		/// @brief get size of stop-color-array
		virtual u32 getColorCount() const
		{
			return Colors.size();
		}

		/// @brief get size of stop-color-array
		virtual void setColorCount( u32 color_count, bool bFillZero = false )
		{
			Colors.reallocate( color_count );
			Colors.set_used( 0 );

//			/// erase content without reallocation
//			if (bFillZero && color)
//			{
			const MyColorStruct zeroColor( SColorf(0,0,0,0), 0.0f );

			for (u32 i=0; i<color_count; i++)
			{
				Colors.push_back( zeroColor );
			}
//			}
		}

		/// @brief management of stop-color-array
		virtual void addColor( const SColor& stopColor, f32 t = 0.5f )
		{
			return addColor( SColorf(stopColor), t);
		}

		/// @brief management of stop-color-array
		virtual void addColor( const SColorf& stopColor, f32 t = 0.5f )
		{
			Colors.push_back( MyColorStruct( stopColor, core::clamp<f32>(t,0.f,1.f) ) );
		}

		/// @brief management of stop-color-array
		virtual bool setColor( u32 index, const SColor& stopColor, f32 t = 0.5f )
		{	const u32 c = getColorCount();
			if (index >= c ) return false;
			Colors[index].Color = SColorf(stopColor);
			Colors[index].Position = core::clamp<f32>( t, 0.0f, 1.0f );
			return true;
		}

		/// @brief management of stop-color-array
		virtual bool setColor( u32 index, const SColorf& stopColor, f32 t = 0.5f )
		{	const u32 c = getColorCount();
			if (index >= c) return false;
			Colors[index].Color = stopColor;
			Colors[index].Position = core::clamp<f32>( t, 0.0f, 1.0f );
			return true;
		}

		/// @brief get solid or transparency material
		virtual bool isTransparent( ) const
		{
			bool bTransparent = false;
			u32 i=0;
			u32 c=getColorCount();

			while (i<c)
			{
				if (Colors[i].Color.getAlpha() < 1.0f) // not 255
				{
					bTransparent = true;
					break;
				}
				i++; // dont ever forget again foo
			}
			return bTransparent;
		}

		/// @brief get solid or transparent material-type
		virtual E_MATERIAL_TYPE getMaterialType( ) const
		{	if (isTransparent())
				return EMT_TRANSPARENT_ALPHA_CHANNEL;
			else
				return EMT_SOLID;
		}

		/// @brief get solid or transparent color-format
		virtual ECOLOR_FORMAT getColorFormat( ) const
		{	if (isTransparent())
				return ECF_A8R8G8B8;
			else
				return ECF_R8G8B8;
		}

		/// @brief create image from colorgradient
		virtual IImage* createImage(
			u32 w,
			u32 h,
			bool bVertical = false ) const
		{
			dbPRINT( "IColorGradient::createImage()\n" )

			if (w==0 || h==0)
			{
				dbERROR( "Can't create Image of size zero.\n" )
				return 0;
			}

			IImage* img = (IImage*)new CImage( this->getColorFormat(), core::dimension2du(w,h) );
			if (!img)
			{
				dbERROR( "Could not create CImage\n" )
				return 0;
			}

			const core::dimension2du& ImageSize = img->getDimension();

			// vertical filling
			if ( bVertical )
			{
				const f32 fy = 1.0f / (f32)h;

				for (u32 y=0; y<ImageSize.Height; y++)
				{
					SColor color = getColor( fy*y );

					for (u32 x=0; x<ImageSize.Width; x++)
					{
						img->setPixel( x,y,color );
					}
				}

			}
			// horizontal filling
			else
			{
				const f32 fx = 1.0f / (f32)w ;

				for (u32 x=0; x<ImageSize.Width; x++)
				{
					SColor color = getColor( fx*x );

					for (u32 y=0; y<ImageSize.Height; y++)
					{
						img->setPixel( x,y,color );
					}
				}
			}

			return img;
		}

		/// @brief create texture from colorgradient
		virtual ITexture* createTexture(
			video::IVideoDriver* driver,
			u32 w,
			u32 h,
			bool bVertical = false,
			const io::path& name = "" ) const
		{
			dbPRINT( "createTexture()\n" );

			if (!driver)
			{
				dbERROR( "Can't create Texture without IVideoDriver (invalid pointer)\n" )
				return 0;
			}

			if (w==0 || h==0)
			{
				dbERROR( "Can't create Texture of size zero\n" )
				return 0;
			}

			IImage* img = createImage(w,h,bVertical);
			if (!img)
			{
				dbERROR( "Could not create Image\n")
				return 0;
			}

			io::path TexName = name;
			if (TexName.size() == 0)
			{
				TexName = getName();
				TexName += io::path("_");
				if (bVertical)
					TexName += io::path("v");
				else
					TexName += io::path("h");
				TexName += io::path("_");
				TexName += (s32)w;
				TexName += io::path("_x_");
				TexName += (s32)h;
			}

			bool bMipMap = driver->getTextureCreationFlag( ETCF_CREATE_MIP_MAPS );
			driver->setTextureCreationFlag( ETCF_CREATE_MIP_MAPS, false );
			ITexture* tex = driver->addTexture( TexName, img, 0);
			driver->setTextureCreationFlag( ETCF_CREATE_MIP_MAPS, bMipMap );
			img->drop();

			if (!tex)
			{
				dbERROR( "Could not create Texture\n")
				return 0;
			}

			return tex;
		}

		/// @brief create texture from colorgradient
		virtual bool saveImage(
			IVideoDriver* driver,
			const io::path& filename = "test_colorgradient.png",
			u32 w = 1024, u32 h = 1024, bool bVertical = false )
		{
			if (!driver)
			{
				dbERROR( "Invalid pointer to IVieoDriver\n")
				return false;
			}

			IImage* img = createImage( w, h, bVertical );
			if (!img)
			{
				dbERROR( "Could not create image from ColorGradient\n")
				return false;
			}

			if (!driver || !driver->writeImageToFile(img, filename ) )
			{
				dbERROR( "Could not save image from ColorGradient to file\n")
				return false;
			}

			return true;
		}


	};


} // end namespace video

} // end namespace irr

#endif // ICOLORGRADIENT_H
