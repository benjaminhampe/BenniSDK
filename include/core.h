// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_CORE_H__
#define __IRR_EXT_CORE_H__

#include <CompileConfig.h>

#ifdef _IRR_WINDOWS_
#include <windows.h>
#endif // _IRR_WINDOWS_

namespace irr
{
	///@brief checks if the given coords are over ( on border or inside ) given rectangle
	bool isMouseOver( const core::position2di& pos, const core::recti& target_rect );

	///@brief set Parent Element Text ( i.e. the caption of a parent's window title )
	bool setText( gui::IGUIElement* element, const core::stringw& text );

	///@brief setVisible(false) to all SceneNodes
	bool hideAll( scene::ISceneManager* smgr );

	///@brief setVisible(true) to all SceneNodes
	bool showAll( scene::ISceneManager* smgr );

	///@brief create a silent ( not logging ) IrrlichtDevice
	IrrlichtDevice* createSilentNullDevice();

	///@brief create a hardware accelerated IrrlichtDevice, when possible
	IrrlichtDevice* createOpenGlDevice( s32 w = -200, s32 h = -100, s32 bpp = 0, bool fullscreen = false, bool aa = false );

	///@brief collect some device infos and return as string
	core::stringw getInfoString( IrrlichtDevice* device, const core::stringw& user_text );

	///@brief run a given IrrlichtDevice
	void runDevice( IrrlichtDevice* device );

	///@brief run a given IrrlichtDevice with limited framerate
	/// if frameLimit < 0 then limit to monitor-refresh-rate
	/// if frameLimit = 0 then no limit
	/// if frameLimit > 0 then limit to frameLimit
	void runDeviceWithFrameLimit( IrrlichtDevice* device, s32 frameLimit = -1, video::ITexture* backdrop = 0 );

	//! @brief get text-width
	u32 getTextWidth( gui::IGUIFont* font, const core::stringw& txt );

	//! @brief get text-height
	u32 getTextHeight( gui::IGUIFont* font, const core::stringw& txt );

	//! @brief save & quick & aligned text-drawing
	void drawText( gui::IGUIFont* font, const core::stringw& txt,
		const core::position2di& pos, const video::SColor& color = 0xffffffff,
		s32 hAlign = -1, s32 vAlign = -1, core::recti* clipRect = 0);

	//! @brief create a aligned static UI text-element
	gui::IGUIStaticText* createLabel( gui::IGUIEnvironment* env, gui::IGUIElement* parent,
		const core::stringw& txt, s32 x, s32 y, s32 hAlign = -1, s32 vAlign = -1,
		s32 padding = 5, bool withBorder = false, bool wordWrap = false, bool fillBackground = true,
		gui::IGUIFont* font = 0, s32 id = -1);

	//! @brief create a FPS-Camera
    scene::ICameraSceneNode* createFPS( scene::ISceneManager* smgr, f32 move_speed, f32 turn_speed);

	///@brief draw backdrop-texture to current rendertarget ( screen or RTT )
	void drawBackdrop( video::IVideoDriver* driver, video::ITexture* tex );

	//! @brief drawLoadingScreen()
	bool drawLoadingScreen( video::IVideoDriver* driver, video::ITexture* background_tex,
		const core::stringw& text, gui::IGUIFont* text_font, const video::SColor& text_color = 0xffffffff);

	//! @brief create a IGUIWindow
	gui::IGUIWindow* createWindow( gui::IGUIEnvironment* env,
		const wchar_t* txt, s32 x, s32 y, u32 w, u32 h,
		bool bModal = false, gui::IGUIElement* parent = 0, s32 id = -1);

	///@brief create a recti from position2di and dimension2du
	core::recti makeRect( s32 x, s32 y, u32 w, u32 h);

	///@brief buildProjectionMatrixOrthoLH with 6 params, not 4 as irrlicht only does
	core::matrix4 buildProjectionMatrixOrthoLH( f32 left, f32 right, f32 bottom, f32 top, f32 zNear = 0.1f, f32 zFar = 1000.0f );

	/// SetWindowLong( hWnd, GWL_STYLE, LONG dwNewLong );
	//GWL_EXSTYLE -20 Sets a new extended window style.
	//GWL_HINSTANCE -6 Sets a new application instance handle.
	//GWL_ID -12 Sets a new identifier of the child window. The window cannot be a top-level window.
	//GWL_STYLE -16 Sets a new window style.
	//GWL_USERDATA -21 Sets the user data associated with the window. This data is intended for use by the application that created the window. Its value is initially zero.
	//GWL_WNDPROC -4 Sets a new address for the window procedure. You cannot change this attribute if the window does not belong to the same process as the calling thread.
	//WS_BORDER 0x00800000L	The window has a thin-line border.
	//WS_CAPTION 0x00C00000L The window has a title bar (includes the WS_BORDER style).
	//WS_CHILD 0x40000000L The window is a child window. A window with this style cannot have a menu bar. This style cannot be used with the WS_POPUP style.
	//WS_CHILDWINDOW 0x40000000L Same as the WS_CHILD style.
	//WS_CLIPCHILDREN 0x02000000L	Excludes the area occupied by child windows when drawing occurs within the parent window. This style is used when creating the parent window.
	//WS_CLIPSIBLINGS 0x04000000L	Clips child windows relative to each other; that is, when a particular child window receives a WM_PAINT message, the WS_CLIPSIBLINGS style clips all other overlapping child windows out of the region of the child window to be updated. If WS_CLIPSIBLINGS is not specified and child windows overlap, it is possible, when drawing within the client area of a child window, to draw within the client area of a neighboring child window.
	//WS_DISABLED 0x08000000L The window is initially disabled. A disabled window cannot receive input from the user. To change this after a window has been created, use the EnableWindow function.
	//WS_DLGFRAME 0x00400000L The window has a border of a style typically used with dialog boxes. A window with this style cannot have a title bar.
	//WS_GROUP 0x00020000L The window is the first control of a group of controls. The group consists of this first control and all controls defined after it, up to the next control with the WS_GROUP style. The first control in each group usually has the WS_TABSTOP style so that the user can move from group to group. The user can subsequently change the keyboard focus from one control in the group to the next control in the group by using the direction keys.You can turn this style on and off to change dialog box navigation. To change this style after a window has been created, use the SetWindowLong function.
	//WS_HSCROLL 0x00100000L The window has a horizontal scroll bar.
	//WS_ICONIC 0x20000000L The window is initially minimized. Same as the WS_MINIMIZE style.
	//WS_MAXIMIZE 0x01000000L The window is initially maximized.
	//WS_MAXIMIZEBOX 0x00010000L The window has a maximize button. Cannot be combined with the WS_EX_CONTEXTHELP style. The WS_SYSMENU style must also be specified.
	//WS_MINIMIZE 0x20000000L The window is initially minimized. Same as the WS_ICONIC style.
	//WS_MINIMIZEBOX 0x00020000L The window has a minimize button. Cannot be combined with the WS_EX_CONTEXTHELP style. The WS_SYSMENU style must also be specified.
	//WS_OVERLAPPED 0x00000000L The window is an overlapped window. An overlapped window has a title bar and a border. Same as the WS_TILED style.
	//WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)	 The window is an overlapped window. Same as the WS_TILEDWINDOW style.
	//WS_POPUP 0x80000000L The windows is a pop-up window. This style cannot be used with the WS_CHILD style.
	//WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)	The window is a pop-up window. The WS_CAPTION and WS_POPUPWINDOW styles must be combined to make the window menu visible.
	//WS_SIZEBOX 0x00040000L The window has a sizing border. Same as the WS_THICKFRAME style.
	//WS_SYSMENU 0x00080000L The window has a window menu on its title bar. The WS_CAPTION style must also be specified.
	//WS_TABSTOP 0x00010000L The window is a control that can receive the keyboard focus when the user presses the TAB key. Pressing the TAB key changes the keyboard focus to the next control with the WS_TABSTOP style.You can turn this style on and off to change dialog box navigation. To change this style after a window has been created, use the SetWindowLong function. For user-created windows and modeless dialogs to work with tab stops, alter the message loop to call the IsDialogMessage function.
	//WS_THICKFRAME 0x00040000L The window has a sizing border. Same as the WS_SIZEBOX style.
	//WS_TILED 0x00000000L The window is an overlapped window. An overlapped window has a title bar and a border. Same as the WS_OVERLAPPED style.
	//WS_TILEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)	The window is an overlapped window. Same as the WS_OVERLAPPEDWINDOW style.
	//WS_VISIBLE 0x10000000L	The window is initially visible. This style can be turned on and off by using the ShowWindow or SetWindowPos function.
	//WS_VSCROLL 0x00200000L The window has a vertical scroll bar.

	///@brief getWindowPosition on Desktop
	core::recti getWindowPosition( IrrlichtDevice* device );

	///@brief setWindowPosition on Desktop
	bool setWindowPosition( IrrlichtDevice* device, s32 x, s32 y );

	///@brief centerWindow on Desktop
	bool centerWindow( IrrlichtDevice* device );

	///@brief Get refresh rate of ( primary? ) monitor
	u32 getRefreshRate( IrrlichtDevice* device );

	///@brief Set skin transparency 0..255
	void setSkinTransparency(u32 alpha, gui::IGUISkin * skin);

} // end namespace irr

#endif // __IRR_EXT_CORE_H__
