// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_AUDIO_LIB_H__
#define __IRR_EXT_AUDIO_LIB_H__

#include "CADSREnvelope.h"
#include "CCombinedOscillator.h"
#include "CPulseOscillator.h"
#include "CSawOscillator.h"
#include "CSineOscillator.h"
#include "CTriangleOscillator.h"

/// fftw
#include "FourierTransformRtoC.h"

/// goertzel algorithm for single-tone detection
#include "goertzel.h"

/// portmidi
#include "CMidi.h"
#include "CGUIMasterKeyboard.h"

/// sfml
#include "CAudioPlayerSFML.h"
#include "CGUIAudioPlayer.h"

#endif // __IRR_EXT_AUDIO_LIB_H__
