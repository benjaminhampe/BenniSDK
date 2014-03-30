// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXTENSION_SVG_UTILS_H__
#define __IRR_EXTENSION_SVG_UTILS_H__

#include <irrlicht.h>

namespace irr
{
namespace core
{
core::stringc toString( const core::recti& rectangle, const core::stringc& name = "");

core::stringc toString( const core::rectf& rectangle, const core::stringc& name = "");

core::vector2df transformPoint2f( const core::vector2df& v_src, const core::rectf& r_src, const core::rectf& r_dst );

// can be classes, but why

core::stringc CSS_Attrib( const core::stringc& key, const core::stringc& value );

core::stringc XML_Attrib( const core::stringc& key, const core::stringc& value );

core::stringc SVG_Color( const video::SColor& color);

core::stringc SVG_Opacity( const video::SColor& color);

core::stringc SVG_DashArray( const u16& stipple );

//

//! struct that holds a pair key ( unique name ) + value
class CSS_Class
{
public:
	core::stringc Key;

	core::stringc Value;

	CSS_Class( )
		: Key(""), Value("")
	{
		// empty
	}

	CSS_Class( const core::stringc& key, const core::stringc& value )
		: Key(key), Value(value)
	{
		// empty
	}

	virtual ~CSS_Class( )
	{
		// empty
	}
};

class CSS_ClassManager : public IReferenceCounted
{
private:
	core::array<CSS_Class> Classes;
public:
	CSS_ClassManager( u32 count = 100 )
	{
		Classes.clear();

		if ( count > 0 )
		{
			Classes.reallocate( count );
			Classes.set_used(0);
		}
	}

	virtual ~CSS_ClassManager() {}

	virtual void clear() { Classes.clear(); }

	virtual core::stringc toString() const
	{
		core::stringc txt = "<style type=\"text/css\">\n";
		txt += "<![CDATA[\n";

		for (u32 i=0; i<Classes.size(); i++)
		{
			txt += Classes[i].Key;
			txt += "\t\t{ ";
			txt += Classes[i].Value;
			txt += " }\n";
		}

		txt += "]]>\n";
		txt += "</style>";

		return txt;
	}

	virtual u32 getClassCount() const
	{
		return Classes.size();
	}

	virtual CSS_Class& getClass( u32 i )
	{
		_IRR_DEBUG_BREAK_IF( i >= Classes.size() );
		return Classes[i];
	}

	virtual const CSS_Class& getClass( u32 i ) const
	{
		_IRR_DEBUG_BREAK_IF( i >= Classes.size() );
		return Classes[i];
	}

	virtual u32 addClass( const CSS_Class& other )
	{
		u32 index = Classes.size();
		Classes.push_back( other );
		return index;
	}

	virtual u32 addClass( const core::stringc& key = "classname", const core::stringc& value = "" )
	{
		u32 index = Classes.size();
		Classes.push_back( CSS_Class( key, value) );
		return index;
	}

	//	virtual bool write( FILE* io_handle ) const;
	//
	//	virtual bool save( const path& filename );
	//
	//	virtual bool load( const path& filename );

};


/// creates strings with SVG definitions for writing to any stream



core::stringc SVG_Stroke( const video::SColor& lineColor, const f32& lineWidth, const u16& lineStipple);

core::stringc SVG_Fill( const video::SColor& fillColor );

core::stringc SVG_Font( const core::stringc& font, f32 size, u32 weight = 400, u32 style = 0 );

core::stringc SVG_Tag( const core::dimension2du& svg_size, const core::rectf& viewbox, const core::stringc& style = "" );

core::stringc SVG_Comment( const core::stringw& comment );

core::stringc SVG_Title( const core::stringw& title );

core::stringc SVG_Description( const core::stringw& description );

core::stringc SVG_Line( const core::stringc& id, const core::rectf& pos, const core::stringc& style );

core::stringc SVG_Rect( const core::stringc& id, const core::rectf& pos, const core::vector2df& roundings, const core::stringc& style );

// core::stringc SVG_RoundRect( const core::stringc& id, const core::rectf& pos, const core::vector2df& roundings, const core::stringc& style );

core::stringc SVG_Circle( const core::stringc& id, const core::vector2df& center, f32 radius, const core::stringc& style );

core::stringc SVG_Ellipse( const core::stringc& id, const core::vector2df& center, const core::vector2df& radius, const core::stringc& style );

core::stringc SVG_Polyline( const core::stringc& id, const core::array<core::vector2df>& points, const core::stringc& style );

core::stringc SVG_Polygon( const core::stringc& id, const core::array<core::vector2df>& points, const core::stringc& style );

core::stringc SVG_Text( const core::stringc& id, const core::vector2df& pos, const core::stringw& text, s32 hAlign /* -1 left; 0 center; 1 right; */, s32 vAlign /* -1 top; 0 middle; 1 bottom; */, const core::stringc& style );

} // end namespace core

} // end namespace irr

//  <defs>
//    <style type="text/css">
//    <![CDATA[
//      text {font-family:Verdana,sans-serif; font-size:12px;}
//    ]]>
//    </style>
//  </defs>
//
//	typedef std::map<String,String> ClassCollectionType;
//	typedef core::array<String> ShapeCollectionType;
//private:
//	// std::map<String, String> Classes;
//	ClassCollectionType Classes;
//	ShapeCollectionType Shapes;
//	core::rectf viewport;
//	ILogger* Logger;
//
//public:
//	CSVGMetaFile( ILogger* logger = 0 )
//	: viewport(-100,-100,100,100), Logger(logger)
//	{
//		if (Logger)
//		{
//			Logger->log( "CSVGMetaFile" );
//		}
//
//		Classes.clear();
//
//		// iterate over elements and print to ILogger
//		if (Logger)
//		{
//			String txt = "class map has size = ";
//			txt += Classes.size();
//			Logger->log( txt.c_str(), ELL_INFORMATION );
//			std::cout<<txt.c_str()<<std::endl;
//			//typedef std::map<Stringc,Stringc>::const_iterator my_class_iter;
//			// my_class_iter it = Classes.begin();
//			for ( auto it = Classes.begin(); it != Classes.end(); it++)
//			// for ( my_class_iter it = Classes.begin(); it != Classes.end(); it++)
//			// for ( std::map<Stringc,Stringc>::const_iterator it = Classes.begin(); it != Classes.end(); it++)
//			{
//				std::pair<String,String> paar = (*it);
//				txt = "pair { key = ";
//				txt += paar.first.c_str();
//				txt += ", value = ";
//				txt += paar.second.c_str();
//				txt += "}";
//				Logger->log( txt.c_str(), ELL_INFORMATION );
//				std::cout<<String(txt.c_str()).c_str()<<std::endl;
//			}
//		}
//
//	}
//
//	~CSVGMetaFile()
//	{
//		Classes.clear();
//	}


/// writes to XML stream
//
//bool writeSVG_Tag( io::IXMLWriter* xml, const core::position2df& pos, const core::dimension2df& size, const core::rectf& viewbox );
//
//bool writeStyle( io::IXMLWriter* xml, const core::stringw& style );
//
//bool writeTitle( io::IXMLWriter* xml, const core::stringw& title );
//
//bool writeDescription(	io::IXMLWriter* xml, const core::stringw& description );
//
//bool writeGroupTranslate( io::IXMLWriter* xml, f32 tx, f32 ty );
//
//bool writeGroupRotate( io::IXMLWriter* xml, f32 phiCW );
//
//bool writeGroupScale( io::IXMLWriter* xml, f32 sx, f32 sy );
//
//bool writeGroupStyle( io::IXMLWriter* xml, const core::stringw& style );
//
//bool writeGroupStyle( io::IXMLWriter* xml, const core::array<core::stringw>& attrNames, const core::array<core::stringw>& attrValues );
//
//bool writeLine( io::IXMLWriter* xml, const core::stringc& id, const core::rectf& pos, const core::stringc& style );
//
//bool writeRect( io::IXMLWriter* xml, const core::stringc& id, const core::rectf& pos, const core::stringc& style );
//
//bool writeRect( io::IXMLWriter* xml, const core::stringc& id, const core::rectf& pos, const video::SColor& fillColor, const video::SColor& lineColor, const f32& lineWidth,	const u16& lineStipple);
//
//bool writeRoundRect( io::IXMLWriter* xml, const core::stringc& id, const core::rectf& pos, const core::vector2df& roundings, const core::stringc& style );
//
//bool writeCircle( io::IXMLWriter* xml, const core::stringc& id, const core::vector2df& center, f32 radius, const core::stringc& style );
//
//bool writeEllipse( io::IXMLWriter* xml, const core::stringc& id, const core::vector2df& center, const core::vector2df& radius, const core::stringc& style );
//
//bool writePolyline( io::IXMLWriter* xml, const core::stringc& id, const core::array<core::vector2df>& points, const core::stringc& style );
//
//bool writePolygon( io::IXMLWriter* xml, const core::stringc& id, const core::array<core::vector2df>& points, const core::stringc& style );
//
//bool writeText( io::IXMLWriter* xml, const core::stringc& id, const core::vector2df& pos, const core::stringw& txt, s32 hAlign /* -1 left; 0 center; 1 right; */, s32 vAlign /* -1 top; 0 middle; 1 bottom; */, const core::stringc& style );
//
//bool writeTextOld( io::IXMLWriter* xml, const core::stringc& id, const core::vector2df& pos, const core::stringw& txt, s32 hAlign, s32 vAlign,const core::stringc& style );


#endif	//
