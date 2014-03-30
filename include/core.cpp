#include "core.h"

namespace irr
{

bool isMouseOver( const core::position2di& pos, const core::recti& target_rect )
{
	bool result(false);
	if (pos.X >= target_rect.UpperLeftCorner.X)
		if (pos.X <= target_rect.LowerRightCorner.X)
			if (pos.Y >= target_rect.UpperLeftCorner.Y)
				if (pos.Y <= target_rect.LowerRightCorner.Y)
					result = true;
	return result;
}

//! set Parent Element Text ( i.e. the caption of a parent's window title )
bool setText( gui::IGUIElement* element, const core::stringw& text )
{
	if (!element)
		return false;

	element->setText( text.c_str() );

	return true;
}

/// setVisible(true) to all SceneNodes
bool showAll( scene::ISceneManager* smgr )
{
	_IRR_DEBUG_BREAK_IF( !smgr )

	if (!smgr)
		return false;

	scene::ISceneNode* root = smgr->getRootSceneNode();

	_IRR_DEBUG_BREAK_IF( !root )

	if (!root)
		return false;

	const core::list<scene::ISceneNode*>& Nodes = root->getChildren();

	core::list<scene::ISceneNode*>::ConstIterator it = Nodes.begin();

	while( it != Nodes.end() )
	{
		if (*it)
		{
			(*it)->setVisible( true );
		}

		it++;
	}

	return true;
}


/// setVisible(false) to all SceneNodes
bool hideAll( scene::ISceneManager* smgr )
{
	_IRR_DEBUG_BREAK_IF( !smgr )
	if (!smgr)
		return false;

	scene::ISceneNode* root = smgr->getRootSceneNode();

	_IRR_DEBUG_BREAK_IF( !root )
	if (!root)
		return false;

	const core::list<scene::ISceneNode*>& Nodes = root->getChildren();

	core::list<scene::ISceneNode*>::ConstIterator it = Nodes.begin();

	while( it != Nodes.end() )
	{
		if ((*it))
		{
			(*it)->setVisible( false );
		}

		it++;
	}

	return true;
}

///@brief create a silent ( not logging ) IrrlichtDevice
IrrlichtDevice* createSilentNullDevice()
{
	SIrrlichtCreationParameters params;
	params.LoggingLevel = ELL_NONE;
	params.DriverType = video::EDT_NULL;
	return createDeviceEx( params );
}

///@brief create a hardware accelerated IrrlichtDevice, when possible
IrrlichtDevice* createIrrlichtDevice( s32 w, s32 h, s32 bpp, bool fullscreen, bool aa )
{
	/// user choice
	//if (bUserChoice
	//video::E_DRIVER_TYPE myDriverType = driverChoiceConsole(true);

	/// set default CreationParameters
	SIrrlichtCreationParameters params;
	params.LoggingLevel = ELL_INFORMATION;
	params.DriverType = video::EDT_OPENGL;
	params.AntiAlias = (aa)?(video::EAAM_ALPHA_TO_COVERAGE):(video::EAAM_OFF);
	params.EventReceiver = 0;
	params.HighPrecisionFPU = true;
	params.Doublebuffer = true;
	params.Vsync = false;
	params.Fullscreen = fullscreen;
	//params.ZBufferBits = 32;
	//params.Stencilbuffer = true;
	//params.WithAlphaChannel = false;

	/// create silentNullDevice
	IrrlichtDevice* nulldev = createSilentNullDevice();
    if (nulldev)
    {
		video::IVideoModeList* videoModes = nulldev->getVideoModeList();

        /// choose WindowResolution
		if (w>0)
			params.WindowSize.Width = w;
		else
			params.WindowSize.Width = videoModes->getDesktopResolution().Width + w;

		if (h>0)
			params.WindowSize.Height = h;
		else
			params.WindowSize.Height = videoModes->getDesktopResolution().Height + h;

        /// choose BitsPerPixel
        if (bpp > 0)
			params.Bits = bpp;
        else
			params.Bits = videoModes->getDesktopDepth();

        /// choose DriverType
        if (!nulldev->isDriverSupported( video::EDT_OPENGL ))
		{
			dbERROR( "EDT_OPENGL not supported\n" )
			if (!nulldev->isDriverSupported( video::EDT_DIRECT3D9 ))
			{
				dbERROR( "EDT_DIRECT3D9 not supported\n" )
				if (!nulldev->isDriverSupported( video::EDT_BURNINGSVIDEO ))
				{
					dbERROR( "EDT_BURNINGSVIDEO not supported\n" )
					if (!nulldev->isDriverSupported( video::EDT_SOFTWARE ))
					{
						dbERROR( "EDT_SOFTWARE not supported\n" )
						params.DriverType = video::EDT_NULL;
					}
					else
					{
						params.DriverType = video::EDT_SOFTWARE;
					}
				}
				else
				{
					params.DriverType = video::EDT_BURNINGSVIDEO;
				}
			}
			else
			{
				params.DriverType = video::EDT_DIRECT3D9;
			}
		}

		nulldev->drop();
    }
    else
	{
		dbERROR("Could not create Null device\n")
		exit(-911);
	}

	/// create Device
	IrrlichtDevice* device = createDeviceEx( params );
	if (!device)
	{
		dbERROR("Could not create OpenGL device\n")
		exit(-912);
	}

	/// Setup
    device->setResizable( true );

	dbPRINT( "Setup OK\n" );

	return device;
}

///@brief collect some device infos and return as string
core::stringw getInfoString( IrrlichtDevice* device, const core::stringw& user_text )
{
	core::stringw txt = user_text;

	if (!device)
	{
		return txt;
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::ICursorControl* cursor = device->getCursorControl();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	if (txt.size() > 0 )
		txt += L" | ";

	/// IVideoDriver
	txt += L"fps( ";
	txt += driver->getFPS();
	txt += L" ), poly( ";
	txt += driver->getPrimitiveCountDrawn(); txt += L" / ";
	txt += driver->getMaximalPrimitiveCount(); txt += L" ), ";

	/// ICursorControl
	const core::position2di& mousePos = cursor->getPosition();
	txt += L"mouse( ";
	txt += mousePos.X;
	txt += L", ";
	txt += mousePos.Y;
	txt += L" ) ";

	/// ICameraSceneNode
	scene::ICameraSceneNode* cam = smgr->getActiveCamera();
	if (cam)
	{
		const core::vector3df& pos = cam->getAbsolutePosition();
		txt += L"cam( pos(";
		txt += core::round32(pos.X); txt += L",";
		txt += core::round32(pos.Y); txt += L",";
		txt += core::round32(pos.Z); txt += L"), ";

		const core::vector3df& eye = cam->getTarget();
		txt += L"eye(";
		txt += core::round32(eye.X); txt += L",";
		txt += core::round32(eye.Y); txt += L",";
		txt += core::round32(eye.Z); txt += L"), ";
		txt += L"near(";
		txt += cam->getNearValue();
		txt += L"), far(";
		txt += core::round32(cam->getFarValue() ); txt += L")";
		txt += L" )";
	}

	///
	return txt;
}

///@brief run a given IrrlichtDevice with limited framerate
/// if frameLimit < 0 then limit to monitor-refresh-rate
/// if frameLimit = 0 then no limit
/// if frameLimit > 0 then limit to frameLimit
void runDeviceWithFrameLimit( IrrlichtDevice* device, s32 frameLimit, video::ITexture* backdrop )
{
	dbPRINT( "runDeviceWithFrameLimit()\n" )

	if (!device)
	{
		dbERROR( "Invalid pointer to IrrlichtDevice\n")
		return;
	}

	/// some pointers and variables
	video::IVideoDriver* driver = device->getVideoDriver();
	core::dimension2du screen = driver->getScreenSize();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	ITimer* timer = device->getTimer();

	u32 timeLastWindowTitleUpate(0);
	u32 timeWaitWindowTitleUpate(500);

	u32 timeSyncWait = 0;
	u32 timeLastSync = 0;

	if (frameLimit < 0)
	{
		frameLimit = getRefreshRate( device );
	}

	if (frameLimit != 0)
	{
		timeSyncWait = core::round32( 1000.0f / (f32)frameLimit);
	}

	dbPRINT( "frameLimit = %d Hz\n", frameLimit );
	dbPRINT( "monitor has %d Hz\n", getRefreshRate( device ) );
	dbPRINT( "timeSyncWait = %d ms\n", timeSyncWait );

	/// main loop
    while (device && device->run())
    {
		/// Resize-Event
      	if (screen != driver->getScreenSize() )
		{
			screen = driver->getScreenSize();
			scene::ICameraSceneNode* camera = smgr->getActiveCamera();
			if (camera)
			{
				f32 aspect = (f32)screen.Width / (f32)screen.Height;
				camera->setAspectRatio( aspect );
			}
		}

		/// if window is active ( can be minimized but still active )
        if (device->isWindowActive())
        {
			/// render all
			if ( timer->getRealTime() - timeLastSync > timeSyncWait )
			{
				timeLastSync = timer->getRealTime();

				driver->beginScene( true, true, video::SColor(255,0,0,0) );

				if (backdrop)
					drawBackdrop( driver, backdrop );

				smgr->drawAll();
				env->drawAll();
				driver->endScene();


			}

			/// window-title update
			if ( timer->getRealTime() - timeLastWindowTitleUpate > timeWaitWindowTitleUpate )
			{
				const core::stringw& caption = getInfoString( device, L"(c) 2008 - 2014 by BenjaminHampe@gmx.de" );

				device->setWindowCaption( caption.c_str() );

				timeLastWindowTitleUpate = timer->getRealTime();
			}
		}
        else
        {
            device->yield();
        }
    }

	if (device)
		device->drop();
}

///@brief run a given IrrlichtDevice
void runDevice(IrrlichtDevice* device)
{
	dbPRINT( "runDevice()\n" )

	if (!device)
	{
		dbERROR( "Invalid pointer to IrrlichtDevice\n")
		return;
	}

	/// some pointers and variables
	video::IVideoDriver* driver = device->getVideoDriver();
	core::dimension2du screen = driver->getScreenSize();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	ITimer* timer = device->getTimer();

	u32 timeLastWindowTitleUpate(0);
	u32 timeWaitWindowTitleUpate(500);

	/// main loop
    while (device && device->run())
    {
		/// Resize-Event
      	if (screen != driver->getScreenSize() )
		{
			screen = driver->getScreenSize();
			scene::ICameraSceneNode* camera = smgr->getActiveCamera();
			if (camera)
			{
				f32 aspect = (f32)screen.Width / (f32)screen.Height;
				camera->setAspectRatio( aspect );
			}
		}

		/// if window is active ( can be minimized but still active )
        if (device->isWindowActive())
        {
			/// if window is active ( can be minimized but still active )
			if (device->isWindowFocused())
			{
				/// render all
				driver->beginScene( true, true, video::SColor(255,0,0,0) );
				smgr->drawAll();
				env->drawAll();
				driver->endScene();

				/// window-title update
				if ( timer->getRealTime() - timeLastWindowTitleUpate > timeWaitWindowTitleUpate )
				{
					const core::stringw& caption = getInfoString( device, L"(c) 2008 - 2014 by BenjaminHampe@gmx.de" );

					device->setWindowCaption( caption.c_str() );

					timeLastWindowTitleUpate = timer->getRealTime();
				}
			}
			else
			{
				device->yield();
			}
		}
        else
        {
            device->yield();
        }
    }

	if (device)
		device->drop();
}

//! @brief get text-width
u32 getTextWidth( gui::IGUIFont* font, const core::stringw& txt )
{
	if (!font)
		return 0;

	return font->getDimension( txt.c_str() ).Width;
}

//! @brief get text-height
u32 getTextHeight( gui::IGUIFont* font, const core::stringw& txt )
{
	if (!font)
		return 0;

	return font->getDimension( txt.c_str() ).Height;
}

//! @brief save & quick & aligned text-drawing
void drawText( gui::IGUIFont* font,	const core::stringw& txt,
	const core::position2di& pos, const video::SColor& color,
	s32 hAlign,	s32 vAlign,	core::recti* clipRect)
{
	//dbPRINT( "drawText()\n" );

	/// exit-condition
	if (!font)
	{
		dbERROR( "Invalid pointer to IGUIFont\n" );
		return;
	}

	/// get text-dimension
	const core::dimension2du& txt_size = font->getDimension( txt.c_str() );

	/// local vars for text-position
	s32 x = pos.X;
	s32 y = pos.Y;

	/// horizontal alignment
	if (hAlign == 0) { x -= (s32)(txt_size.Width>>1); }
	else if (hAlign > 0 ) {	x -= (s32)(txt_size.Width); }

	/// vertical alignment
	if (vAlign == 0) { y -= (s32)(txt_size.Height>>1); }
	else if (vAlign > 0 ) {	y -= (s32)(txt_size.Height); }

	/// draw text to current render-target ( screen or RTT )
	font->draw( txt, core::recti( core::position2di(x,y), txt_size ),
		color, false, false, clipRect);
}

///@brief draw backdrop-texture to current rendertarget ( screen or RTT )
void drawBackdrop( video::IVideoDriver* driver, video::ITexture* tex )
{
	if (!driver)
		return;

	if (!tex)
		return;

	const core::dimension2du& scr_size = driver->getScreenSize();
	const core::dimension2du& tex_size = tex->getSize();

	const f32 aspect = (f32)tex_size.Height / (f32)tex_size.Width;
	const s32 height = aspect * scr_size.Width;
	const s32 y = ( (s32)scr_size.Height - height ) / 2;

	const core::recti r_dst( 0, y, scr_size.Width - 1, y + height );
	const core::recti r_src( core::position2di(0,0), tex_size );

	driver->draw2DImage( tex, r_dst, r_src );
}

//! @brief create a IGUIWindow
gui::IGUIWindow* createWindow( gui::IGUIEnvironment* env,
	const wchar_t* txt, s32 x, s32 y, u32 w, u32 h,
	bool bModal, gui::IGUIElement* parent, s32 id)
{
	gui::IGUIWindow* win = 0;

	if (!env)
		return win;

	if (!parent)
		parent = env->getRootGUIElement();

	win = env->addWindow(
		core::recti( core::position2di(x,y), core::dimension2du(w,h)),
		bModal, txt, parent, id );

	return win;
}

///@brief create a recti from position2di and dimension2du
core::recti makeRect( s32 x, s32 y, u32 w, u32 h)
{
	return core::recti( core::position2di(x,y), core::dimension2du(w,h) );
}

///@brief buildProjectionMatrixOrthoLH with 6 params, not 4 as irrlicht only does
core::matrix4 buildProjectionMatrixOrthoLH( f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar )
{
	core::matrix4 M;
	_IRR_DEBUG_BREAK_IF( core::equals(left,right) ); //divide by zero
	_IRR_DEBUG_BREAK_IF( core::equals(top,bottom) ); //divide by zero
	_IRR_DEBUG_BREAK_IF( core::equals(zNear,zFar) ); //divide by zero

	//	2/(r-l)      0            0           0
	//	0            2/(t-b)      0           0
	//	0            0            1/(zf-zn)   0
	//	(l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

	M[0] = 2.f/(right-left);
	M[1] = 0.f;
	M[2] = 0.f;
	M[3] = 0.f;

	M[4] = 0.f;
	M[5] = 2.f/(top-bottom);
	M[6] = 0.f;
	M[7] = 0.f;

	M[8] = 0.f;
	M[9] = 0.f;
	M[10] = 1.f/(zFar-zNear);
	M[11] = 0.f;

	M[12] = (left+right)/(left-right);
	M[13] = (top+bottom)/(bottom-top);
	M[14] = zNear/(zNear-zFar);
	M[15] = 1.f;

	return M;
}

///@brief getWindowPosition
core::recti getWindowPosition( IrrlichtDevice* device )
{
	core::recti pos(-1,-1,0,0);

	if (!device)
	{
		dbERROR("getWindowPosition() - Invalid pointer to IrrlichtDevice\n");
		return pos;
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	if (!driver)
	{
		dbERROR("getWindowPosition() - Invalid pointer to IVideoDriver\n");
		return pos;
	}

	#ifdef _IRR_WINDOWS_API_
	HWND hWnd = reinterpret_cast<HWND>( driver->getExposedVideoData().OpenGLWin32.HWnd );
	RECT hRect;
	BOOL result = GetWindowRect( hWnd, &hRect );

	pos.UpperLeftCorner.X = (s32)hRect.left;
	pos.UpperLeftCorner.Y = (s32)hRect.top;
	pos.LowerRightCorner.X = (s32)hRect.right;
	pos.LowerRightCorner.Y = (s32)hRect.bottom;

	if (result==FALSE)
	{
		dbERROR("getWindowPosition() - Error in WinApi\n");
	}
	#else
		#warning getWindowPosition() not implemented for this platform yet.
	#endif
	return pos;
}

///@brief setWindowPosition
bool setWindowPosition( IrrlichtDevice* device, s32 x, s32 y )
{
	if (!device)
	{
		dbERROR("setWindowPosition() - Invalid pointer to IrrlichtDevice\n");
		return false;
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	if (!driver)
	{
		dbERROR("setWindowPosition() - Invalid pointer to IVideoDriver\n");
		return false;
	}

	#ifdef _IRR_WINDOWS_API_
	const core::recti winRect = getWindowPosition( device );
	const HWND hWnd = reinterpret_cast<HWND>( driver->getExposedVideoData().OpenGLWin32.HWnd );
	const u32 hStyle = GetWindowLong( hWnd, GWL_EXSTYLE );

	const BOOL result = SetWindowPos( hWnd, HWND_TOP, x, y, winRect.getWidth(), winRect.getHeight(), hStyle );
	if (result == FALSE)
	{
		dbERROR("setWindowPosition() - Error in WinApi\n");
	}
	#else
		#warning setWindowPosition() not implemented for this platform yet.
	#endif
	return true;
}

///@brief center Window
bool centerWindow( IrrlichtDevice* device )
{
	if (!device)
	{
		dbERROR("centerWindow() - Invalid pointer to IrrlichtDevice\n");
		return false;
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	if (!driver)
	{
		dbERROR("centerWindow() - Invalid pointer to IVideoDriver\n");
		return false;
	}

	const core::dimension2du desktopSize = device->getVideoModeList()->getDesktopResolution();
	const core::dimension2du screenSize = driver->getScreenSize();
	const s32 winX = (s32)(desktopSize.Width - screenSize.Width)  / 2 ;
	const s32 winY = (s32)(desktopSize.Height - screenSize.Height)  / 2 ;

	return setWindowPosition( device, winX, winY );
}

///@brief Get refresh rate of ( primary? ) monitor
u32 getRefreshRate( IrrlichtDevice* device )
{
	u32 refreshRate = 0;

	if (!device)
	{
		dbERROR("getRefreshRate() - Invalid pointer to IrrlichtDevice\n");
		return refreshRate;
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	if (!driver)
	{
		dbERROR("getRefreshRate() - Invalid pointer to IVideoDriver\n");
		return refreshRate;
	}

	#ifdef _IRR_WINDOWS_API_
//	const core::recti winRect = getWindowPosition( device );
//	const HWND hWnd = reinterpret_cast<HWND>( driver->getExposedVideoData().OpenGLWin32.HWnd );
//	const u32 hStyle = GetWindowLong( hWnd, GWL_EXSTYLE );

	DEVMODE lpDevMode;
	memset(&lpDevMode, 0, sizeof(DEVMODE));
	lpDevMode.dmSize = sizeof(DEVMODE);
	lpDevMode.dmDriverExtra = 0;

	if(EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode) == 0)
	{
//		refreshRate = 60; // default value if cannot retrieve from user settings.
	}
	else
	{
		refreshRate = lpDevMode.dmDisplayFrequency;
	}
	#else
		#warning getRefreshRate() not implemented for this platform yet.
	#endif

	return refreshRate;
}

} // end namespace irr

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
