// Copyright (C) 2008-2012 Benjamin Hampe
// This file uses parts of the "Irrlicht Engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_C_MIDI_TOOLS_H_INCLUDED__
#define __IRR_C_MIDI_TOOLS_H_INCLUDED__

#include "../CompileConfig.h"

namespace irr{

class CMidiTools
{
public:
	virtual core::stringc getNameFromNote( s32 midiNote, bool major = true) const;
	virtual s32 getNoteFromName( const irr::core::stringc& name ) const;
	virtual bool isBlackKey( s32 midiNote ) const;
	virtual bool isBlackKey( const irr::core::stringc& name ) const;
	virtual bool isWhiteKey( s32 midiNote ) const;
	virtual bool isWhiteKey( const irr::core::stringc& name ) const;
	virtual s32 getNoteCount( s32 startNote, s32 endNote ) const;
	virtual s32 getNoteCount( const irr::core::stringc& startNote, const irr::core::stringc& endNote ) const;
	virtual f32 getFrequencyFromNote( s32 midiNote ) const;
	virtual s32	getNoteFromFrequency( f32 frequency ) const;
	virtual s32 getWhiteKeyCount( s32 startNote, s32 endNote ) const;
	virtual s32 getWhiteKeyCount( const irr::core::stringc& startNote, const irr::core::stringc& endNote ) const;
	virtual s32 getBlackKeyCount( s32 startNote, s32 endNote ) const;
	virtual s32 getBlackKeyCount( const irr::core::stringc& startNote, const irr::core::stringc& endNote ) const;
};

void Test(ITimer* timer, ILogger* logger);

} // end namespace irr

#endif // __IRR_C_MIDI_TOOLS_H_INCLUDED__
