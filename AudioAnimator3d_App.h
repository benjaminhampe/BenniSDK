// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef AUDIOANIMATOR3D_APP_H
#define AUDIOANIMATOR3D_APP_H

#include <CompileConfig.h>
#include <debugPrint.h>
#include <core.h>

#include <libImage/CMath.h>
#include <libImage/CMatrix.h>
#include <libImage/CMatrixSceneNode.h>
#include <libImage/CLinearColorGradientTable.h>

#include <libAudio/FourierTransformRtoC.h>
#include <libAudio/irrAudio.h>
#include <libAudio/IAudioPlayer.h>
#include <libAudio/CAudioPlayerSFML.h>
#include <libAudio/CGUIAudioPlayer.h>

namespace irr
{

class Application : public IEventReceiver
{
private:
	IrrlichtDevice* Device;
	core::dimension2du ScreenSize;
	core::recti ScreenRect;
	core::stringw AppTitle;
	core::stringc DefaultAudioFilename;
	core::stringc DefaultSkydomeTexture;
	core::stringc DefaultIntroTexture;

	video::ITexture* Wallpaper = 0;

	CAudioPlayerSFML player;
	video::CLinearColorGradientTable WAV_Gradient;
	video::CLinearColorGradientTable FFT_Gradient;
	u32 FFT_Size;
	u32 FFT_MatrixRows;
	u32 FFT_MatrixCols;
	core::vector3df MeshSize;
	core::FourierTransformRtoC Transform;
	core::SRangef FFT_Range;
	f32 FFT_Threshold;
	core::array<s16> FFT_Input; //! depends on FFT_Size
	core::array<f32> FFT_Output; //! depends on FFT_MatrixCols
	core::CMatrix<f32> FFT_Matrix; //! depends on FFT_MatrixCols and FFT_MatrixRows
	scene::CMatrixSceneNode* FFT_SceneNode;
	core::array<f32> XAxis;

	/// +++ create UI controls +++
	gui::IGUIButton* ui_WireframeButton;
	gui::IGUIComboBox* ui_AxisMode;
	gui::IGUISpinBox* ui_MeshSizeX;
	gui::IGUISpinBox* ui_MeshSizeY;
	gui::IGUISpinBox* ui_MeshSizeZ;
	gui::IGUISpinBox* ui_MatrixCols;
	gui::IGUISpinBox* ui_MatrixRows;
	gui::IGUISpinBox* ui_DecibelMin;
	gui::IGUISpinBox* ui_DecibelMax;
	gui::IGUISpinBox* ui_DecibelThreshold;
	gui::IGUIComboBox* ui_FFT_Size;

	gui::IGUIStaticText* DebugBox;
public:

	Application( IrrlichtDevice* device );

	virtual ~Application();

	virtual bool OnEvent (const SEvent &event) _IRR_OVERRIDE_;

	virtual void OnKeyUp_Espace();
	virtual void OnKeyUp_Space();
	virtual void OnKeyUp_Print();
	virtual void OnButton_Wireframe();
	virtual void OnChangedSpinBox_MeshSizeX( f32 value );
	virtual void OnChangedSpinBox_MeshSizeY( f32 value );
	virtual void OnChangedSpinBox_MeshSizeZ( f32 value );
	virtual void OnChangedSpinBox_MatrixCols( f32 value );
	virtual void OnChangedSpinBox_MatrixRows( f32 value );
	virtual void OnChangedSpinBox_DecibelMin( f32 value );
	virtual void OnChangedSpinBox_DecibelMax( f32 value );
	virtual void OnChangedSpinBox_DecibelThreshold( f32 value );
	virtual void OnChangedComboBox_FFTSize( s32 selected );
	virtual void OnChangedComboBox_AxisMode( s32 selected );

	void updateDebugBox();

	bool setup();

	bool setupGUI();

	bool run();

	bool createLinearAxis( core::array<f32>& out, u32 nPoints )
	{
		if (nPoints < 2)
		{
			dbERROR( "createLinearScale() - Invalid param: nPoints %d", nPoints )
			return false;
		}

		// prepare array
		out.reallocate( nPoints );
		out.set_used( 0 );

		// calculate min and max frequency
		const f32 x_step = core::reciprocal( (f32)(nPoints-1) );
		f32 x = 0.0f;

		for (u32 i=0; i<nPoints; i++)
		{
			out.push_back( core::clamp<f32>(x, 0.0f, 1.0f) );
			x += x_step;
		}

		// exit
	}

	bool createLogarithmicAxis( core::array<f32>& out, u32 nPoints, u32 logBase = 10, u32 sampleRate = 44100, u32 fftSize = 1024 )
	{
		if (fftSize < 4)
		{
			dbERROR( "createLogarithmicScale() - Invalid param: fftSize %d", fftSize )
			return false;
		}

		if (nPoints < 2)
		{
			dbERROR( "createLogarithmicScale() - Invalid param: nPoints %d", nPoints )
			return false;
		}

		if (sampleRate < 2*fftSize) // Nyquist Kriterium
		{
			dbERROR( "createLogarithmicScale() - Invalid param: sampleRate %d", sampleRate )
			return false;
		}

		// prepare array
		out.reallocate( nPoints );
		out.set_used( 0 );

		// calculate min and max frequency
		const f32 f_min = 10.0f; // (f32)sampleRate/(f32)fftSize
		const f32 f_max = ((f32)sampleRate*(f32)nPoints)/(f32)fftSize;
		const f32 f_step = (f32)sampleRate/(f32)fftSize;

		// calculate min and max frequency in logarithmic scale
		const f32 l_base = (f32)logBase;
		const f32 l_min = core::Math::log( f_min, l_base );
		const f32 l_max = core::Math::log( f_max, l_base );
		const f32 l_range_inv = core::reciprocal( l_max - l_min );

		f32 freq = f_min;
		f32 v;

		for (u32 i=0; i<nPoints; i++)
		{
			v = core::Math::log( freq, l_base ) - l_min;
			v *= l_range_inv;
			out.push_back( core::clamp<f32>(v, 0.0f, 1.0f) );
			freq += f_step;
		}
	}
};

} // end namespace irr

#endif // AUDIOANIMATOR3D_APP_H
