// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CXMLWriterUTF8.h"
#include <wchar.h>
#include <IrrCompileConfig.h>

#include "../core/irrUString.h"

namespace irr
{
namespace io
{


//! Constructor
CXMLWriterUTF8::CXMLWriterUTF8(IWriteFile* file)
: File(file), Tabs(0), TextWrittenLast(false)
{
	#ifdef _DEBUG
	setDebugName("CXMLWriterUTF8");
	#endif

	if (File)
		File->grab();
}



//! Destructor
CXMLWriterUTF8::~CXMLWriterUTF8()
{
	// const c8* const htmlEnd = "</body></html>\n";

	if (File)
		File->drop();
}



//! Writes a xml 1.0 header like <?xml version="1.0"?>
void CXMLWriterUTF8::writeXMLHeader()
{
	if (!File)
		return;

	// Write the UTF-8 byte-order mark.  This lets text editors know that we are saving as UTF-8.
	File->write(core::unicode::BOM_ENCODE_UTF8, core::unicode::BOM_ENCODE_UTF8_LEN);

	const c8* const p = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	File->write(p, strlen(p));

	writeLineBreak();
	TextWrittenLast = false;
}

//! Writes a xml 1.0 header like <?xml version="1.0"?>
void CXMLWriterUTF8::writeHtmlHeader()
{
	if (!File)
		return;

	// Write the UTF-8 byte-order mark.  This lets text editors know that we are saving as UTF-8.
	File->write(core::unicode::BOM_ENCODE_UTF8, core::unicode::BOM_ENCODE_UTF8_LEN);

	const c8* const htmlStart = "<html>\n<body>\n<style>\n"
	"  circle { stroke: #006600; fill: #00cc00; }\n"
	"  rect.red { stroke: #006600; fill: #00cc00; }\n"
	"  rect.full { stroke: #006600; fill: #00cc00; }\n"
	"  rect.canvas { stroke: #006600; fill: #00cc00; }\n"
	"</style>\n";

	File->write(htmlStart, strlen(htmlStart));

	writeLineBreak();
	TextWrittenLast = false;
}



//! Writes an xml element with maximal 5 attributes
void CXMLWriterUTF8::writeElement(const wchar_t* name, bool empty,
	const wchar_t* attr1Name, const wchar_t* attr1Value,
	const wchar_t* attr2Name, const wchar_t* attr2Value,
	const wchar_t* attr3Name, const wchar_t* attr3Value,
	const wchar_t* attr4Name, const wchar_t* attr4Value,
	const wchar_t* attr5Name, const wchar_t* attr5Value)
{
	if (!File || !name)
		return;

	core::ustring converter;

	if (Tabs > 0)
	{
		for (int i=0; i<Tabs; ++i)
			File->write("\t", 1);
	}

	// write name

	File->write("<", 1);
	converter = name;
	core::stringc name_s = converter.toUTF8_s();
	File->write(name_s.c_str(), name_s.size());

	// write attributes

	writeAttribute(attr1Name, attr1Value);
	writeAttribute(attr2Name, attr2Value);
	writeAttribute(attr3Name, attr3Value);
	writeAttribute(attr4Name, attr4Value);
	writeAttribute(attr5Name, attr5Value);

	// write closing tag
	if (empty)
		File->write(" />", 3);
	else
	{
		File->write(">", 1);
		++Tabs;
	}

	TextWrittenLast = false;
}

//! Writes an xml element with any number of attributes
void CXMLWriterUTF8::writeElement(const wchar_t* name, bool empty,
				  core::array<core::stringw> &names,
				  core::array<core::stringw> &values)
{
	if (!File || !name)
		return;

	core::ustring converter;

	if (Tabs > 0)
	{
		for (int i=0; i<Tabs; ++i)
			File->write("\t", 1);
	}

	// write name

	File->write("<", 1);
	converter = name;
	core::stringc name_s = converter.toUTF8_s();
	File->write(name_s.c_str(), name_s.size());

	// write attributes
	u32 i=0;
	for (; i < names.size() && i < values.size(); ++i)
		writeAttribute(names[i].c_str(), values[i].c_str());

	// write closing tag
	if (empty)
		File->write(" />", 3);
	else
	{
		File->write(">", 1);
		++Tabs;
	}

	TextWrittenLast = false;
}


void CXMLWriterUTF8::writeAttribute(const wchar_t* name, const wchar_t* value)
{
	if (!name || !value)
		return;

	core::ustring converter;

	File->write(" ", 1);

	converter = name;
	core::stringc name_s = converter.toUTF8_s();
	File->write(name_s.c_str(), name_s.size());

	File->write("=\"", 2);
	writeText(value);
	File->write("\"", 1);
}


//! Writes a comment into the xml file
void CXMLWriterUTF8::writeComment(const wchar_t* comment)
{
	if (!File || !comment)
		return;

	File->write("<!--", 4);
	writeText(comment);
	File->write("-->", 3);
}


//! Writes the closing tag for an element. Like </foo>
void CXMLWriterUTF8::writeClosingTag(const wchar_t* name)
{
	if (!File || !name)
		return;

	--Tabs;

	if (Tabs > 0 && !TextWrittenLast)
	{
		for (int i=0; i<Tabs; ++i)
			File->write("\t", 1);
	}

	core::ustring converter;

	File->write("</", 2);

	converter = name;
	core::stringc name_s = converter.toUTF8_s();
	File->write(name_s.c_str(), name_s.size());

	File->write(">", 1);
	TextWrittenLast = false;
}



const CXMLWriterUTF8::XMLSpecialCharacters XMLWSChar[] =
{
	{ L'&', L"&amp;" },
	{ L'<', L"&lt;" },
	{ L'>', L"&gt;" },
	{ L'"', L"&quot;" },
	{ L'\0', 0 }
};


//! Writes a text into the file. All occurrences of special characters like
//! & (&amp;), < (&lt;), > (&gt;), and " (&quot;) are automaticly replaced.
void CXMLWriterUTF8::writeText(const wchar_t* text)
{
	if (!File || !text)
		return;

	// TODO: we have to get rid of that reserve call as well as it slows down xml-writing seriously.
	// Making a member-variable would work, but a lot of memory would stay around after writing.
	// So the correct solution is probably using fixed block here and always write when that is full.
	core::stringw s;
	s.reserve(wcslen(text)+1);
	const wchar_t* p = text;

	while(*p)
	{
		// check if it is matching
		bool found = false;
		for (s32 i=0; XMLWSChar[i].Character != '\0'; ++i)
			if (*p == XMLWSChar[i].Character)
			{
				s.append(XMLWSChar[i].Symbol);
				found = true;
				break;
			}

		if (!found)
			s.append(*p);
		++p;
	}

	// write new string
	core::ustring converter(s);
	core::stringc s_s = converter.toUTF8_s();
	File->write(s_s.c_str(), s_s.size());
	TextWrittenLast = true;
}


//! Writes a line break
void CXMLWriterUTF8::writeLineBreak()
{
	if (!File)
		return;

#if defined(_IRR_OSX_PLATFORM_)
	File->write("\r", 1);
#elif defined(_IRR_WINDOWS_API_)
	File->write("\r\n", 2);
#else
	File->write("\n", 1);
#endif

}


} // end namespace irr
} // end namespace io

