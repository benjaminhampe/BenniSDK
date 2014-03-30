#include "svgLib.h"

#include <irrlicht.h>

namespace irr
{
namespace core
{

core::stringc toString( const core::recti& rectangle, const core::stringc& name )
{
	core::stringc txt = name;
	if (name.size()==0)
	{
		txt += "recti";
	}
	txt += " = { ";
    txt += rectangle.UpperLeftCorner.X;
    txt += ", ";
    txt += rectangle.UpperLeftCorner.Y;
    txt += ", ";
    txt += rectangle.LowerRightCorner.X;
    txt += ", ";
    txt += rectangle.LowerRightCorner.Y;
    txt += " };";
    return txt;
}

core::stringc toString( const core::rectf& rectangle, const core::stringc& name )
{
	core::stringc txt = name;
	if (name.size()==0)
	{
		txt += "rectf";
	}
	txt += " = { ";
    txt += rectangle.UpperLeftCorner.X;
    txt += ", ";
    txt += rectangle.UpperLeftCorner.Y;
    txt += ", ";
    txt += rectangle.LowerRightCorner.X;
    txt += ", ";
    txt += rectangle.LowerRightCorner.Y;
    txt += " };";
    return txt;
}

core::vector2df transformPoint2f( const core::vector2df& v_src, const core::rectf& r_src, const core::rectf& r_dst )
{
	core::vector2df	v_dst(0,0);

	// normalize 0..1
	v_dst.X = (v_src.X - r_src.UpperLeftCorner.X) / r_src.getWidth();
	v_dst.Y = (v_src.Y - r_src.UpperLeftCorner.Y) / r_src.getHeight();

	// scale
	v_dst.X *= r_dst.getWidth();
	v_dst.Y *= r_dst.getHeight();

	//translate
	v_dst.X += r_dst.UpperLeftCorner.X;
	v_dst.Y += r_dst.UpperLeftCorner.Y;

	return v_dst;
}

core::stringc CSS_Attrib( const core::stringc& key, const core::stringc& value )
{
	core::stringc txt = key;
	txt += ":";
	txt += value;
	txt += "; ";
	return txt;
}

core::stringc XML_Attrib( const core::stringc& key, const core::stringc& value )
{
	core::stringc txt = key;
	txt += "=\"";
	txt += value;
	txt += "\" ";
	return txt;
}

core::stringc SVG_Color( const video::SColor& color )
{
	core::stringc txt;
	if (color.getAlpha()==0)
	{
		txt = "none";
	}
	else
	{
		txt = "rgb(";
		txt += color.getRed();
		txt += ",";
		txt += color.getGreen();
		txt += ",";
		txt += color.getBlue();
		txt += ")";
	}
	return txt;
}

core::stringc SVG_Opacity( const video::SColor& color )
{
	f32 opacity = core::clamp<f32>( (f32)color.getAlpha()/255.0f, 0.0f, 1.0f );
	core::stringc txt("");
	txt += opacity;
	return txt;
}

core::stringc SVG_DashArray( const u16& stipple )
{
	core::stringc txt("");
	if (stipple == 0xffff)
	{
		txt = "none";
	}
	else
	{
		u32 count = 1;
		bool lastBit = (stipple & (1<<15)) > 0;
		bool needColon = false;
		for (u32 i=1; i<16; i++)
		{
			bool bit = (stipple & (1<<(15-i))) > 0;
			if (i<15 && (lastBit == bit))
			{
				count++;
			}
			else
			{
				if (needColon)
				{
					txt += ",";
				}

				if (!needColon)
				{
					needColon = true;
				}

				txt += (s32)count;
				lastBit = bit;
				count = 1;
			}
		}
	}
	return txt;
}

core::stringc SVG_Stroke( const video::SColor& lineColor, const f32& lineWidth, const u16& lineStipple)
{
	core::stringc txt = "stroke:";

	if (lineColor.getAlpha() == 0)
	{
		txt += "none;";
		return txt;
	}

	txt += SVG_Color( lineColor );
	txt += "; ";
	txt += CSS_Attrib( "stroke-width", core::stringc( lineWidth ) );

	if (lineColor.getAlpha()<255)
	{
		txt += CSS_Attrib( "stroke-opacity", SVG_Opacity( lineColor ) );
	}

	if (lineStipple != 0xffff)
	{
		txt += CSS_Attrib( "stroke-dasharray", SVG_DashArray( lineStipple ) );
	}

	return txt;
}

core::stringc SVG_Fill( const video::SColor& fillColor )
{
	core::stringc txt = "fill:";

	if (fillColor.getAlpha() == 0)
	{
		txt += "none";
	}

	if (fillColor.getAlpha()<255)
	{
		txt += CSS_Attrib( "fill-opacity", SVG_Opacity( fillColor ) );
	}

	return txt;
}

core::stringc SVG_Font( const core::stringc& font, f32 size, u32 weight, u32 style )
{
	core::stringc txt = "";

	return txt;
}

core::stringc SVG_Tag( const core::dimension2du& svg_size, const core::rectf& viewbox, const core::stringc& style )
{
	core::stringc txt = "<svg ";
	//txt += XML_Attrib( "x", core::stringc( pos.X ));
	//txt += XML_Attrib( "y", core::stringc( pos.Y ));
    txt += XML_Attrib( "width", core::stringc( svg_size.Width ));
    txt += XML_Attrib( "height", core::stringc( svg_size.Height ));
    txt += XML_Attrib( "xmlns", "http://www.w3.org/2000/svg" );
    txt += XML_Attrib( "version", "1.1" );

	core::stringc tmp = ""; tmp += viewbox.UpperLeftCorner.X;
    tmp += " "; tmp += viewbox.UpperLeftCorner.Y;
    tmp += " "; tmp += viewbox.LowerRightCorner.X;
    tmp += " "; tmp += viewbox.LowerRightCorner.Y;
    txt += XML_Attrib( "viewbox", tmp );

	if (style.size() > 0)
	{
		txt += XML_Attrib( "style", style );
	}
	txt += ">";

    return txt;
}

core::stringc SVG_Comment( const core::stringw& comment )
{
	core::stringc txt = "<!-- ";
	txt += comment;
	txt += " -->";
	return txt;
}

core::stringc SVG_Title( const core::stringw& title )
{
	core::stringc txt = "<title>";
	txt += title;
	txt += "</title>";
	return txt;
}

core::stringc SVG_Description( const core::stringw& description )
{
	core::stringc txt = "<desc>";
	txt += description;
	txt += "</desc>";
	return txt;
}

core::stringc SVG_Line( const core::stringc& id, const core::rectf& pos, const core::stringc& style )
{
	core::stringc txt = "<line ";
	txt += XML_Attrib( "class", id );
	txt += XML_Attrib( "x1", core::stringc( pos.UpperLeftCorner.X ) );
	txt += XML_Attrib( "y1", core::stringc( pos.UpperLeftCorner.Y ) );
	txt += XML_Attrib( "x2", core::stringc( pos.LowerRightCorner.X ) );
	txt += XML_Attrib( "y2", core::stringc( pos.LowerRightCorner.Y ) );
	if ( style.size() > 0)
	{
		txt += XML_Attrib( "style", style );
	}
	txt += "/>";
	return txt;
}

core::stringc SVG_Rect( const core::stringc& id, const core::rectf& pos, const core::vector2df& roundings, const core::stringc& style )
{
	core::stringc txt = "<rect ";
	txt += XML_Attrib( "class", id );
	txt += XML_Attrib( "x", core::stringc( pos.UpperLeftCorner.X ) );
	txt += XML_Attrib( "y", core::stringc( pos.UpperLeftCorner.Y ) );
	txt += XML_Attrib( "width", core::stringc( pos.getWidth() ) );
	txt += XML_Attrib( "height", core::stringc( pos.getHeight() ) );

	if (roundings.X >= 0.0f)
		txt += XML_Attrib( "rx", core::stringc( roundings.X ) );
	if (roundings.Y >= 0.0f)
		txt += XML_Attrib( "ry", core::stringc( roundings.Y ) );

	if ( style.size() > 0)
	{
		txt += XML_Attrib( "style", style );
	}
	txt += "/>";
	return txt;
}

core::stringc SVG_Circle( const core::stringc& id, const core::vector2df& center, f32 radius, const core::stringc& style )
{
	core::stringc txt = "<circle ";
	txt += XML_Attrib( "class", id );
	txt += XML_Attrib( "cx", core::stringc( center.X ) );
	txt += XML_Attrib( "cy", core::stringc( center.Y ) );
	txt += XML_Attrib( "r", core::stringc( radius ) );
	if ( style.size() > 0)
	{
		txt += XML_Attrib( "style", style );
	}
	txt += "/>";
	return txt;
}

core::stringc SVG_Ellipse( const core::stringc& id, const core::vector2df& center, const core::vector2df& radius, const core::stringc& style )
{
	core::stringc txt = "<ellipse ";
	txt += XML_Attrib( "class", id );
	txt += XML_Attrib( "cx", core::stringc( center.X ) );
	txt += XML_Attrib( "cy", core::stringc( center.Y ) );
	txt += XML_Attrib( "rx", core::stringc( radius.X ) );
	txt += XML_Attrib( "ry", core::stringc( radius.Y ) );
	if ( style.size() > 0)
	{
		txt += XML_Attrib( "style", style );
	}
	txt += "/>";
	return txt;
}


core::stringc SVG_Polyline( const core::stringc& id, const core::array<core::vector2df>& points, const core::stringc& style )
{
	core::stringc txt = "<polyline ";
	txt += XML_Attrib( "class", id );

	core::stringc tmp = "";
	for (u32 i=0; i<points.size(); i++)
	{
		tmp += points[i].X;
		tmp += ",";
		tmp += points[i].Y;
		if (i<points.size()-1)
		{
			tmp += " ";
		}
	}

	txt += XML_Attrib( "points", tmp );
	if ( style.size() > 0)
	{
		txt += XML_Attrib( "style", style );
	}
	txt += "/>";
	return txt;
}

core::stringc SVG_Polygon( const core::stringc& id, const core::array<core::vector2df>& points, const core::stringc& style )
{
	core::stringc txt = "<polygon ";
	txt += XML_Attrib( "class", id );

	core::stringc tmp = "";
	for (u32 i=0; i<points.size(); i++)
	{
		tmp += points[i].X;
		tmp += ",";
		tmp += points[i].Y;
		if ( i < points.size()-1 )
		{
			tmp += " ";
		}
	}

	txt += XML_Attrib( "points", tmp );
	if ( style.size() > 0)
	{
		txt += XML_Attrib( "style", style );
	}
	txt += "/>";
	return txt;
}

core::stringc SVG_Text( const core::stringc& id, const core::vector2df& pos, const core::stringw& text, s32 hAlign /* -1 left; 0 center; 1 right; */, s32 vAlign /* -1 top; 0 middle; 1 bottom; */, const core::stringc& style )
{
	core::stringc txt = "<text ";
	txt += XML_Attrib( "class", id );
	txt += XML_Attrib( "x", core::stringc( pos.X ) );
	txt += XML_Attrib( "y", core::stringc( pos.X ) );
	txt += XML_Attrib( "dx", "0" );

	if (vAlign == -1)
	{
		txt += XML_Attrib( "dy", "0.8em" );
	}
	else if  (vAlign == 0)
	{
		txt += XML_Attrib( "dy", "0.4em" );
	}
	else if (vAlign == 1)
	{
		txt += XML_Attrib( "dy", "0" );
	}

	if (hAlign == -1)
	{
		txt += XML_Attrib( "text-anchor", "start" );
	}
	else if  (hAlign == 0)
	{
		txt += XML_Attrib( "text-anchor", "middle" );
	}
	else if (hAlign == 1)
	{
		txt += XML_Attrib( "text-anchor", "end" );
	}

	if ( style.size() > 0)
	{
		txt += XML_Attrib( "style", style );
	}

	txt += ">";
	txt += text;
	txt += "</text>";
	return txt;
}







//core::stringc CSS_Attrib( const core::array<core::stringc>& keys, const core::array<core::stringc>& values )
//{
//	core::stringc txt = "";
//
//	u32 count = core::min_<u32>( keys.size(), values.size() );
//
//	for ( u32 i=0; i<count; i++ )
//	{
//		txt += keys[i];
//		txt += ":";
//		txt += values[i];
//		txt += ";";
//
//		if ( i < count-1 )
//			txt += " ";
//	}
//
//	return txt;
//}
//
//core::stringc XML_Attrib( const core::array<core::stringc>& keys, const core::array<core::stringc>& values )
//{
//	core::stringc txt = "";
//
//	u32 count = core::min_<u32>( keys.size(), values.size() );
//
//	for ( u32 i=0; i<count; i++ )
//	{
//		txt += keys[i];
//		txt += "=\"";
//		txt += values[i];
//		txt += "\"";
//
//		if ( i < count-1 )
//			txt += " ";
//	}
//
//	return txt;
//}




//core::stringc SVG_Stroke( const video::SColor& lineColor, const f32& lineWidth, const u16& lineStipple);
//
//core::stringc SVG_Fill( const video::SColor& fillColor );
//
//core::stringc SVG_Font( const core::stringc& font, f32 size, u32 weight = 400, u32 style = 0 );
//
//core::stringc SVG_Tag( const core::dimension2df& svg_size, const core::rectf& viewbox );
//
//core::stringc SVG_Title( const core::stringw& title );
//
//core::stringc SVG_Description( const core::stringw& description );
//
//core::stringc SVG_Line( const core::stringc& id, const core::rectf& pos, const core::stringc& style );
//
//core::stringc SVG_Rect( const core::stringc& id, const core::rectf& pos, const core::vector2df& roundings, const core::stringc& style );
//
//// core::stringc SVG_RoundRect( const core::stringc& id, const core::rectf& pos, const core::vector2df& roundings, const core::stringc& style );
//
//core::stringc SVG_Circle( const core::stringc& id, const core::vector2df& center, f32 radius, const core::stringc& style );
//
//core::stringc SVG_Ellipse( const core::stringc& id, const core::vector2df& center, const core::vector2df& radius, const core::stringc& style );
//
//core::stringc SVG_Polyline( const core::stringc& id, const core::array<core::vector2df>& points, const core::stringc& style );
//
//core::stringc SVG_Polygon( const core::stringc& id, const core::array<core::vector2df>& points, const core::stringc& style );
//
//core::stringc SVG_Text( const core::stringc& id, const core::vector2df& pos, const core::stringw& txt, s32 hAlign /* -1 left; 0 center; 1 right; */, s32 vAlign /* -1 top; 0 middle; 1 bottom; */, const core::stringc& style );






//
//
//
//
//
//
//
//
//
//
//
////! write SVG-Image to XML-Stream
//bool writeSVG_Tag( io::IXMLWriter* xml,
//                    const core::position2df& pos,
//                         const core::dimension2df& size,
//                         const core::rectf& viewbox)
//{
//    if (!xml)
//        return false;
//    core::array<core::stringw> attrNames;
//    core::array<core::stringw> attrValues;
//    attrNames.clear();
//    attrValues.clear();
//    attrNames.push_back(L"x"); attrValues.push_back(core::stringw(pos.X)); // attrValues.push_back(L"100%");
//    attrNames.push_back(L"y"); attrValues.push_back(core::stringw(pos.Y)); // attrValues.push_back(L"100%");
//    attrNames.push_back(L"width"); attrValues.push_back(core::stringw(size.Width)); // attrValues.push_back(L"100%");
//    attrNames.push_back(L"height"); attrValues.push_back(core::stringw(size.Height)); // attrValues.push_back(L"100%");
//
//    core::stringw s(L"");
//    s+=viewbox.UpperLeftCorner.X; s+=L" ";
//    s+=viewbox.UpperLeftCorner.Y; s+=L" ";
//    s+=viewbox.LowerRightCorner.X; s+=L" ";
//    s+=viewbox.LowerRightCorner.Y;
//    attrNames.push_back(L"viewbox"); attrValues.push_back(s); // attrValues.push_back(L"100%");
//    attrNames.push_back(L"xmlns"); attrValues.push_back(L"http://www.w3.org/2000/svg");
//    attrNames.push_back(L"version"); attrValues.push_back(L"1.1");
////  attrNames.push_back(L"preserveAspectRatio"); attrValues.push_back(L"xMinYMid");
//    xml->writeElement(L"svg", false, attrNames, attrValues);
//    xml->writeLineBreak();
//    return true;
//}
//
//
//bool writeStyle(
//    io::IXMLWriter* xml,
//    const core::stringw& style )
//{
//	if (!xml)
//		return false;
//
//	xml->writeElement(L"style", false, L"type", L"text/css");
//	xml->writeLineBreak();
//	xml->writeText( L"text { color:white; stroke:none; fill:white; font-family:Verdana; font-size:14; line-height:14; font-weight:normal; }" );
//	xml->writeLineBreak();
//	xml->writeClosingTag( L"style" );
//	xml->writeLineBreak();
//}
//
//
//// <title>Beispiel eingeschränkte Transformation</title>
//bool writeTitle(
//	io::IXMLWriter* xml,
//	const core::stringw& title )
//{
//	if (!xml)
//		return false;
//
//	xml->writeElement(L"title", false );
//	xml->writeText( title.c_str() );
//	xml->writeClosingTag(L"title");
//	xml->writeLineBreak();
//	return true;
//}
//
//// <desc>Beispiel eingeschränkte Transformation</desc>
//bool writeDescription(
//	io::IXMLWriter* xml,
//	const core::stringw& description )
//{
//	if (!xml)
//		return false;
//
//	xml->writeElement(L"desc", false );
//	xml->writeText( description.c_str() );
//	xml->writeClosingTag(L"desc");
//	xml->writeLineBreak();
//
//	return true;
//}
//
//
//// <g transform="translate(50,50)">
//bool writeGroupTranslate( io::IXMLWriter* xml, f32 tx, f32 ty )
//{
//    if (!xml)
//    {
//        return false;
//    }
//
//    core::stringw txt(L"translate("); txt+=tx; txt+=L","; txt+=ty; txt+=L")";
//    xml->writeElement(L"g", false, L"transform",  txt.c_str());
//    xml->writeLineBreak();
//    return true;
//}
//
//// <g transform="rotate(50,50)">
//bool writeGroupRotate( io::IXMLWriter* xml, f32 phiCW )
//{
//    if (!xml)
//    {
//        return false;
//    }
//
//    core::stringw txt(L"rotate("); txt+=phiCW; txt+=L")";
//    xml->writeElement(L"g", false, L"transform",  txt.c_str());
//    xml->writeLineBreak();
//    return true;
//}
//
//// <g transform="scale(50,50)">
//bool writeGroupScale( io::IXMLWriter* xml, f32 sx, f32 sy )
//{
//    if (!xml)
//    {
//        return false;
//    }
//    core::stringw txt(L"scale("); txt+=sx; txt+=L","; txt+=sy; txt+=L")";
//    xml->writeElement(L"g", false, L"transform",  txt.c_str());
//    xml->writeLineBreak();
//    return true;
//}
//
//bool writeGroupStyle( io::IXMLWriter* xml, const core::stringw& style )
//{
//    if (!xml)
//    {
//        return false;
//    }
//    xml->writeElement(L"g", false, L"style", style.c_str());
//    xml->writeLineBreak();
//    return true;
//}
//
//bool writeGroupStyle( io::IXMLWriter* xml, const core::array<core::stringw>& attrNames, const core::array<core::stringw>& attrValues )
//{
//    if (!xml)
//    {
//        return false;
//    }
//    core::stringw style = L"";
//    u32 attrCount = core::max_(attrNames.size(), attrValues.size());
//    for (u32 i = 0; i < attrCount; ++i)
//    {
//        style+=attrNames[i];
//        style+=":";
//        style+=attrValues[i];
//        if (i<attrCount-1)
//            style+=L"; ";
//    }
//    xml->writeElement(L"g", false, L"style", style.c_str());
//    xml->writeLineBreak();
//    return true;
//}
//
//
//bool writeLine(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::rectf& pos,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
//
//	AttribNames.push_back( L"x1" ); AttribValues.push_back( core::stringw( pos.UpperLeftCorner.X ) );
//	AttribNames.push_back( L"y1" ); AttribValues.push_back( core::stringw( pos.UpperLeftCorner.Y ) );
//	AttribNames.push_back( L"x2" ); AttribValues.push_back( core::stringw( pos.LowerRightCorner.X ) );
//	AttribNames.push_back( L"y2" ); AttribValues.push_back( core::stringw( pos.LowerRightCorner.Y ) );
//	AttribNames.push_back( L"style" ); AttribValues.push_back( core::stringw( style ) );
//	xml->writeElement(L"line", true, AttribNames, AttribValues );
//	xml->writeLineBreak();
//	return true;
//}
//
//// <rect x="1" y="1" width="1198" height="398" fill="none" stroke="blue" stroke-width="2"/>
//bool writeRect(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::rectf& pos,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
//
//	AttribNames.push_back( L"x" ); AttribValues.push_back( core::stringw( pos.UpperLeftCorner.X ) );
//	AttribNames.push_back( L"y" ); AttribValues.push_back( core::stringw( pos.UpperLeftCorner.Y ) );
//	AttribNames.push_back( L"width" ); AttribValues.push_back( core::stringw( pos.getWidth() ) );
//	AttribNames.push_back( L"height" ); AttribValues.push_back( core::stringw( pos.getHeight() ) );
//	AttribNames.push_back( L"style" ); AttribValues.push_back( core::stringw( style ) );
//	xml->writeElement(L"rect", true, AttribNames, AttribValues );
//	xml->writeLineBreak();
//	return true;
//}
//
//// <rect x="1" y="1" width="1198" height="398" fill="none" stroke="blue" stroke-width="2"/>
//bool writeRect(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::rectf& pos,
//	const video::SColor& fillColor,
//	const video::SColor& lineColor,
//	const f32& lineWidth,
//	const u16& lineStipple)
//{
//	if (!xml)
//		return false;
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
//
//	AttribNames.push_back( L"x" ); AttribValues.push_back( core::stringw( pos.UpperLeftCorner.X ) );
//	AttribNames.push_back( L"y" ); AttribValues.push_back( core::stringw( pos.UpperLeftCorner.Y ) );
//	AttribNames.push_back( L"width" ); AttribValues.push_back( core::stringw( pos.getWidth() ) );
//	AttribNames.push_back( L"height" ); AttribValues.push_back( core::stringw( pos.getHeight() ) );
//
//	AttribNames.push_back( L"fill" ); AttribValues.push_back( core::stringw( SVG_Color(fillColor) ) );
//	if (fillColor.getAlpha()<255)
//	{
//		AttribNames.push_back( L"fill-opacity" ); AttribValues.push_back( core::stringw( SVG_Opacity(fillColor) ) );
//	}
//
//	AttribNames.push_back( L"stroke" ); AttribValues.push_back( core::stringw( SVG_Color(lineColor) ) );
//	if (lineColor.getAlpha()<255)
//	{
//		AttribNames.push_back( L"stroke-opacity" ); AttribValues.push_back( core::stringw( SVG_Opacity(lineColor) ) );
//	}
//	if (lineColor.getAlpha()>0)
//	{
//		core::stringw txt(L"");
//		txt += lineWidth;
//		AttribNames.push_back( L"stroke-width" ); AttribValues.push_back( txt );
//
//		if (lineStipple != 0xffff)
//		{
//			core::stringw txt(L"");
//			txt += lineWidth;
//			AttribNames.push_back( L"stroke-dasharray" ); AttribValues.push_back( SVG_DashArray( lineStipple ) );
//		}
//	}
//
//	xml->writeElement(L"rect", true, AttribNames, AttribValues );
//	xml->writeLineBreak();
//	return true;
//}
//
//// <rect x="1" y="1" width="1198" height="398" fill="none" stroke="blue" stroke-width="2"/>
//bool writeRoundRect(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::rectf& pos,
//	const core::vector2df& roundings,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
//
//	AttribNames.push_back( L"x" ); AttribValues.push_back( core::stringw( pos.UpperLeftCorner.X ) );
//	AttribNames.push_back( L"y" ); AttribValues.push_back( core::stringw( pos.UpperLeftCorner.Y ) );
//	AttribNames.push_back( L"width" ); AttribValues.push_back( core::stringw( pos.getWidth() ) );
//	AttribNames.push_back( L"height" ); AttribValues.push_back( core::stringw( pos.getHeight() ) );
//	AttribNames.push_back( L"rx" ); AttribValues.push_back( core::stringw( roundings.X ) );
//	AttribNames.push_back( L"ry" ); AttribValues.push_back( core::stringw( roundings.Y ) );
//	AttribNames.push_back( L"style" ); AttribValues.push_back( core::stringw( style ) );
//	xml->writeElement(L"rect", true, AttribNames, AttribValues );
//	xml->writeLineBreak();
//	return true;
//}
//
//
//// <circle cx="600" cy="200" r="100" fill="red" stroke="blue" stroke-width="10"  />
//bool writeCircle(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::vector2df& center,
//	f32 radius,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
//	AttribNames.push_back( L"cx" ); AttribValues.push_back( core::stringw( center.X ) );
//	AttribNames.push_back( L"cy" ); AttribValues.push_back( core::stringw( center.Y ) );
//	AttribNames.push_back( L"r" ); AttribValues.push_back( core::stringw( radius ) );
//	AttribNames.push_back( L"style" ); AttribValues.push_back( core::stringw( style ) );
//	xml->writeElement(L"circle", true, AttribNames, AttribValues );
//	xml->writeLineBreak();
//	return true;
//}
//
//// <ellipse transform="translate(900 200) rotate(-30)" rx="250" ry="100"
////		fill="none" stroke="blue" stroke-width="20"  />
//bool writeEllipse(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::vector2df& center,
//	const core::vector2df& radius,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
//	AttribNames.push_back( L"cx" ); AttribValues.push_back( core::stringw( center.X ) );
//	AttribNames.push_back( L"cy" ); AttribValues.push_back( core::stringw( center.Y ) );
//	AttribNames.push_back( L"rx" ); AttribValues.push_back( core::stringw( radius.X ) );
//	AttribNames.push_back( L"ry" ); AttribValues.push_back( core::stringw( radius.Y ) );
//	AttribNames.push_back( L"style" ); AttribValues.push_back( core::stringw( style ) );
//	xml->writeElement(L"ellipse", true, AttribNames, AttribValues );
//	xml->writeLineBreak();
//	return true;
//}
//
//
////  <polyline fill="none" stroke="blue" stroke-width="10"
////            points="50,375
////                    150,375 150,325 250,325 250,375
////                    350,375 350,250 450,250 450,375
////                    550,375 550,175 650,175 650,375
////                    750,375 750,100 850,100 850,375
////                    950,375 950,25 1050,25 1050,375
////                    1150,375" />
//
//bool writePolyline(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::array<core::vector2df>& points,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	core::stringw data;
//	for (u32 i=0; i<points.size(); i++)
//	{
//		data += points[i].X;
//		data += L",";
//		data += points[i].Y;
//		if (i<points.size()-1)
//			data += L" ";
//	}
//
//	xml->writeElement(L"polyline", true, L"points", data.c_str(), L"style", core::stringw( style ).c_str() );
//	xml->writeLineBreak();
//
//	return true;
//}
//
////	<polygon fill="red" stroke="blue" stroke-width="10"
////            points="350,75  379,161 469,161 397,215
////                    423,301 350,250 277,301 303,215
////                    231,161 321,161" />
//
//bool writePolygon(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::array<core::vector2df>& points,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	core::stringw data;
//	for (u32 i=0; i<points.size(); i++)
//	{
//		data += points[i].X;
//		data += L",";
//		data += points[i].Y;
//		if (i<points.size()-1)
//			data += L" ";
//	}
//
//	xml->writeElement(L"polygon", true, L"points", data.c_str(), L"style", core::stringw( style ).c_str() );
//	xml->writeLineBreak();
//
//	return true;
//}
//
//bool writeText(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::vector2df& pos,
//	const core::stringw& txt,
//	s32 hAlign, /* -1 left; 0 center; 1 right; */
//	s32 vAlign, /* -1 top; 0 middle; 1 bottom; */
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//
//
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
//	AttribNames.push_back( L"x" ); AttribValues.push_back( core::stringw( pos.X ) );
//	AttribNames.push_back( L"y" ); AttribValues.push_back( core::stringw( pos.Y ) );
//	AttribNames.push_back( L"dx" ); AttribValues.push_back( L"0" );
//
//	if (vAlign == -1)
//	{
//		AttribNames.push_back( L"dy" ); AttribValues.push_back( L"0.8em" );
//	}
//	else if  (vAlign == 0)
//	{
//		AttribNames.push_back( L"dy" ); AttribValues.push_back( L"0.4em" );
//	}
//	else if (vAlign == 1)
//	{
//		AttribNames.push_back( L"dy" ); AttribValues.push_back( L"0" );
//	}
//
//	if (hAlign == -1)
//	{
//		AttribNames.push_back( L"text-anchor" ); AttribValues.push_back( L"start" );
//	}
//	else if  (hAlign == 0)
//	{
//		AttribNames.push_back( L"text-anchor" ); AttribValues.push_back( L"middle" );
//	}
//	else if (hAlign == 1)
//	{
//		AttribNames.push_back( L"text-anchor" ); AttribValues.push_back( L"end" );
//	}
//
//	AttribNames.push_back( L"style" ); AttribValues.push_back( core::stringw( style ) );
//
//	xml->writeElement( L"text", false, AttribNames, AttribValues );
//	xml->writeText( txt.c_str() );
//	xml->writeClosingTag( L"text" );
//	xml->writeLineBreak();
//
//	return true;
//}
//
//bool writeTextOld(
//	io::IXMLWriter* xml,
//    const core::stringc& id,
//	const core::vector2df& pos,
//	const core::stringw& txt,
//	s32 hAlign,
//	s32 vAlign,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	f32 scale = 100.0f/297.0f;
//
//	core::stringw h = L"translate(";
//	h += pos.X;
//	h += L",";
//	h += pos.Y;
//	h += L") scale(";
//	h += scale;
//	h += ")";
//
//	xml->writeElement(L"g", false, L"transform", h.c_str() );
//	xml->writeLineBreak();
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
////	AttribNames.push_back( L"x" ); AttribValues.push_back( core::stringw( pos.X ) );
////	AttribNames.push_back( L"y" ); AttribValues.push_back( core::stringw( pos.Y ) );
////	AttribNames.push_back( L"width" ); AttribValues.push_back( L"100" );
////	AttribNames.push_back( L"height" ); AttribValues.push_back( L"100" );
//	AttribNames.push_back( L"style" ); AttribValues.push_back( core::stringw( style ) );
//
////	xml->writeElement( L"text", false, AttribNames, AttribValues );
//
//	xml->writeElement( L"text", false );
//	xml->writeLineBreak();
//
//	xml->writeElement( L"tspan", false, L"dx", L"0", L"dy", L"10", L"style", core::stringw( style ).c_str() );
//
////	xml->writeLineBreak();
//
////	xml->writeElement(L"g", false, L"transform", L"translate(0px,24px)" );
////	xml->writeLineBreak();
//
////	xml->writeElement(L"tspan", false );
//	xml->writeText( txt.c_str() );
//
//	xml->writeClosingTag( L"tspan" );
//	xml->writeLineBreak();
//
//	xml->writeClosingTag( L"text" );
//	xml->writeLineBreak();
//
//	xml->writeClosingTag( L"g" );
//	xml->writeLineBreak();
//	return true;
//}




} // end namespace core


} // end namespace irr






//<?xml version="1.0" standalone="no"?>
//<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
//  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
//<svg width="400px" height="200px" viewBox="0 0 4000 2000"
//     xmlns="http://www.w3.org/2000/svg" version="1.1">
//  <title>Example Units</title>
//  <desc>Illustrates various units options</desc>
//
//  <!-- Frame the picture -->
//  <rect x="5" y="5" width="3990" height="1990"
//        fill="none" stroke="blue" stroke-width="10"/>
//
//  <g fill="blue" stroke="red" font-family="Verdana" font-size="150">
//    <!-- Absolute unit specifiers -->
//    <g transform="translate(400,0)">
//      <text x="-50" y="300" fill="black" stroke="none">Abs. units:</text>
//      <rect x="0" y="400" width="4in" height="2in" stroke-width=".4in"/>
//      <rect x="0" y="750" width="384" height="192" stroke-width="38.4"/>
//      <g transform="scale(2)">
//        <rect x="0" y="600" width="4in" height="2in" stroke-width=".4in"/>
//      </g>
//    </g>
//
//    <!-- Relative unit specifiers -->
//    <g transform="translate(1600,0)">
//      <text x="-50" y="300" fill="black" stroke="none">Rel. units:</text>
//      <rect x="0" y="400" width="2.5em" height="1.25em" stroke-width=".25em"/>
//      <rect x="0" y="750" width="375" height="187.5" stroke-width="37.5"/>
//      <g transform="scale(2)">
//        <rect x="0" y="600" width="2.5em" height="1.25em" stroke-width=".25em"/>
//      </g>
//    </g>
//
//    <!-- Percentages -->
//    <g transform="translate(2800,0)">
//      <text x="-50" y="300" fill="black" stroke="none">Percentages:</text>
//      <rect x="0" y="400" width="10%" height="10%" stroke-width="1%"/>
//      <rect x="0" y="750" width="400" height="200" stroke-width="31.62"/>
//      <g transform="scale(2)">
//        <rect x="0" y="600" width="10%" height="10%" stroke-width="1%"/>
//      </g>
//    </g>
//  </g>
//</svg>

//    <path id="MyPath"
//          d="M 100 125
//             C 150 125 250 175 300 175
//             C 350 175 450 125 500 125
//             C 550 125 650 175 700 175
//             C 750 175 850 125 900 125" />

//bool writePath(
//	io::IXMLWriter* xml,
//  const core::stringc& id,
//	const core::array<core::vector2df>& points,
//	const core::stringc& style )
//{
//	if (!xml)
//		return false;
//
//	core::array<core::stringw> AttribNames;
//	core::array<core::stringw> AttribValues;
//
//	AttribNames.push_back( L"x" ); AttribValues.push_back( pos.UpperLeftCorner.X );
//	AttribNames.push_back( L"y" ); AttribValues.push_back( pos.UpperLeftCorner.Y );
//	AttribNames.push_back( L"width" ); AttribValues.push_back( pos.getWidth() );
//	AttribNames.push_back( L"height" ); AttribValues.push_back( pos.getHeight() );
//	AttribNames.push_back( L"style" ); AttribValues.push_back( style );
//	xml->writeElement(L"rect", true, AttribNames, AttribValues );
//	xml->writeLineBreak();
//
//	return true;
//}


//<g transform="translate(-10,-20)">
//  <g transform="scale(2)">
//    <g transform="rotate(45)">
//      <g transform="translate(5,10)">
//        <!-- graphics elements go here -->
//      </g>
//    </g>
//  </g>
//</g>


//‘letter-spacing’Value:  	normal | <length> | inherit
//Initial:  	normal
//‘text-anchor’Value:  	start | middle | end | inherit
//Initial:  	start
//‘dominant-baseline’Value:  	auto | use-script | no-change | reset-size | ideographic | alphabetic | hanging | mathematical | central | middle | text-after-edge | text-before-edge |  inherit
//Initial:  	auto
//‘alignment-baseline’Value:  	auto | baseline | before-edge | text-before-edge | middle | central | after-edge | text-after-edge | ideographic | alphabetic | hanging | mathematical |  inherit
//Initial:  	auto
//‘baseline-shift’Value:  	baseline | sub | super | <percentage> | <length> |  inherit
//Initial:  	baseline
//‘font-family’Value:  	[[ <family-name> |
// <generic-family> ],]* [<family-name> |
// <generic-family>] | inherit
//Initial:  	depends on user agent
//‘font-style’Value:  	normal | italic | oblique | inherit
//Initial:  	normal
//‘font-variant’Value:  	normal | small-caps | inherit
//Initial:  	normal
//‘font-weight’Value:  	normal | bold | bolder | lighter | 100 | 200 | 300
// | 400 | 500 | 600 | 700 | 800 | 900 | inherit
//Initial:  	normal
//‘font-stretch’Value:  	normal | wider | narrower |
// ultra-condensed | extra-condensed |
// condensed | semi-condensed |
// semi-expanded | expanded |
// extra-expanded | ultra-expanded | inherit
//Initial:  	normal
//‘font-size’Value:  	<absolute-size> | <relative-size> |
// <length> | <percentage> | inherit
//Initial:  	medium
//‘font-size-adjust’Value:  	<number> | none | inherit
//Initial:  	none
//‘font’Value:  	[ [ <'font-style'> || <'font-variant'> || <'font-weight'> ]?
// <'font-size'> [ / <'line-height'> ]? <'font-family'> ] |
// caption | icon | menu | message-box |
// small-caption | status-bar | inherit
//Initial:  	see individual properties
//‘kerning’Value:  	auto | <length> | inherit
//Initial:  	auto
//‘letter-spacing’Value:  	normal | <length> | inherit
//Initial:  	normal
//‘word-spacing’Value:  	normal | <length> | inherit
//Initial:  	normal
//‘text-decoration’Value:  	none | [ underline || overline || line-through || blink ] | inherit
//Initial:  	none
//‘text-along-a-path’ EXAMPLE
//<svg xmlns="http://www.w3.org/2000/svg"
//     xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1">
//  <g transform="rotate(45)">
//    <defs>
//      <path id="path1" transform="scale(2)" d="..." fill="none" stroke="red"/>
//    </defs>
//    <text>
//      <textPath xlink:href="#path1">Text along path1</textPath>
//    </text>
//  </g>
//</svg>

//    <text x="20" y="10">
//        <tspan>tspan line 1</tspan>
//        <tspan dx="30" dy="10">tspan line 2</tspan>
//    </text>
