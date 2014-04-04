// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef AUDIOANIMATOR3D_APP_H
#define AUDIOANIMATOR3D_APP_H

#include <hampe.h>

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
	scene::SMeshBuffer FFT_FrontMesh;

	/// +++ create UI controls +++
	gui::IGUIButton* WireframeButton;

	gui::IGUISpinBox* ui_MeshSizeX;
	gui::IGUISpinBox* ui_MeshSizeY;
	gui::IGUISpinBox* ui_MeshSizeZ;
	gui::IGUISpinBox* ui_MatrixCols;
	gui::IGUISpinBox* ui_MatrixRows;
	gui::IGUISpinBox* ui_DecibelMin;
	gui::IGUISpinBox* ui_DecibelMax;
	gui::IGUISpinBox* ui_DecibelThreshold;
	gui::IGUIComboBox* ui_FFT_Size;

public:

	Application( IrrlichtDevice* device );

	virtual ~Application();

	virtual bool OnEvent (const SEvent &event) _IRR_OVERRIDE_;

	bool setup();

	bool setupGUI();

	bool run();
};

} // end namespace irr

#endif // AUDIOANIMATOR3D_APP_H
