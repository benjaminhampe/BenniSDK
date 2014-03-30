/** Example 000 Xml CbProj SuperBuilder

creates a new CodeBlocks Project file from an old one for super fast compiling
by creating a "super" *.cpp file that includes all other *.cpp files
and deactivates all included source files from getting compiled each alone

@author Benjamin Hampe	\< benjaminhampe@googlemail.com \>

1. Load CbProj file and save all found sources to a table ( one for gcc and one for g++ )
2. deactivate all activated source-files from getting compiled and linked
3. create new source-files ( one for gcc and one for g++ ) that only includes the deactivated source-files
as include directives
4. save new project-file next to old one

(5.) user needs to open new project-file and compile as always --> big speed improvement for compile-time

*/

#include <irrlicht.h>
#include <irrXML.h>

#include <libExt.h>

#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CMeshSceneNode.cpp>
#include <../source/Irrlicht/CShadowVolumeSceneNode.cpp>

#ifdef _DEBUG
	#include <../source/Irrlicht/os.cpp>
#endif // _DEBUG

using namespace irr;
//using namespace core;
//using namespace scene;
//using namespace video;
//using namespace io;
//using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define PRINT(args...) { printf(args); }
#else
#define PRINT(args...)
#endif

void PRINT_INFO()
{
#if defined(__x86_64__) || defined(_M_X64)
	PRINT("Architecture = x86 64-Bit\n");
#elif defined(__i386) || defined(_M_IX86)
	PRINT("Architecture = x86 32-Bit\n");
#else
	PRINT("Architecture = unknown\n");
#endif
}


/**
	A typical event receiver.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(IrrlichtDevice* device) : Device(device)
	{
		PRINT("MyEventReceiver()\n")

        if (Device)
        {
            Device->setEventReceiver(this);
            PRINT("MyEventReceiver attached to IrrlichtDevice\n");
        }
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT )
		{
			switch ( event.GUIEvent.EventType )
			{
				//handle button click events
				case gui::EGET_BUTTON_CLICKED:
				{

				}
				break;
			}
		}

		return false;
	}

private:
	IrrlichtDevice* Device;
};


/**
	A typical irrlicht application.
*/
class MyApp
{
public:
	MyApp( s32 argc, c8** argv )
		: Argc(argc), Argv(argv), Device(0), ClearColor(255,255,255,255)
	{
		// print CommandLine arguments
		PRINT("MyApp::MyApp()\n");
		PRINT("ArgumentCount = %d\n", Argc);
		for ( s32 i=0; i<Argc; i++)
			PRINT("Argument[%d] = %s\n", i, Argv[i]);

        //create IrrlichtDevice
        SIrrlichtCreationParameters param;
        param.DriverType = video::EDT_SOFTWARE;
        #if defined(_DEBUG) || defined(DEBUG)
        param.LoggingLevel = ELL_DEBUG;
        #else
        param.LoggingLevel = ELL_NONE;
        #endif
        param.WindowSize.set(640,480);

        //create the irrlicht device using the settings
        Device = createDeviceEx(param);
        if (!Device)
        {
            // You can add your own exception handling on driver failure
            exit(0);
        }

        Device->setWindowCaption(L"MyApp");
 	}

	~MyApp()
	{
		PRINT("MyApp::~MyApp()\n");

        if (Device)
        {
            Device->closeDevice();
            Device->drop();
        }
	}

	IrrlichtDevice* getDevice()
	{
		if (!Device)
		{
			PRINT("getDevice() - No valid pointer to IrrlichtDevice\n");
			//fflush();
			exit(-15);
		}
		return Device;
	}

	ILogger* getLogger() { return getDevice()->getLogger(); }

	video::IVideoDriver* getVideoDriver() { return getDevice()->getVideoDriver(); }

	gui::IGUIEnvironment* getGUIEnvironment() { return getDevice()->getGUIEnvironment(); }

	scene::ISceneManager* getSceneManager() { return getDevice()->getSceneManager(); }

	bool run()
	{
		// PRINT("MyApp::run()\n");

        if (!Device)
        	return false;

		if (!Device->run())
			return false;

		return true;
	}

	bool drawAll()
	{
		// PRINT("MyApp::drawAll()\n");

        if (!Device)
        	return false;

		if (Device->isWindowActive())
		{
			video::IVideoDriver* driver	= Device->getVideoDriver();
			gui::IGUIEnvironment* env = Device->getGUIEnvironment();
			scene::ISceneManager* smgr = Device->getSceneManager();

			driver->beginScene(true, true, ClearColor);
			smgr->drawAll();
			env->drawAll();
			driver->endScene();
		}
		else
		{
			Device->yield();
		}
		Device->sleep(10);

        return true;
	}

private:
	s32 Argc;
	c8** Argv;
	IrrlichtDevice* Device;
	video::SColor ClearColor;
};

bool createSuperBuildFromCbProject( IrrlichtDevice* device, const core::stringc& xmlFilename )
{
	PRINT("createSuperBuildFromProject()\n")

    if (!device)
    {
		PRINT("Invalid pointer to IrrlichtDevice\n")
		return false;
    } // END-IF

    io::IXMLReader* xml = device->getFileSystem()->createXMLReader( io::path( xmlFilename ) );
    if (!xml)
    {
		PRINT("Could not create IXMLReader from file\n")
		return false;
    } // END-IF

//	const wchar_t* src_ext_gcc[] = { L"c", L"cc", L"c99" };
//	const wchar_t* src_ext_gpp[] = { L"cpp", L"cxx", L"c++" };

	core::array<core::stringw> src_ext_gcc;
	src_ext_gcc.push_back( L"c" );
	src_ext_gcc.push_back( L"cc" );
	src_ext_gcc.push_back( L"c99" );

	core::array<core::stringw> src_ext_gpp;
	src_ext_gpp.push_back( L"cpp" );
	src_ext_gpp.push_back( L"cxx" );
	src_ext_gpp.push_back( L"c++" );

    const core::stringw unitTag(L"Unit"); //we'll be looking for this tag in the xml
    const core::stringw filenameAttrib(L"filename"); //constant for videotag
    core::stringw currentTag; //keep track of our current section
	core::stringw attribName;
	core::stringw attribValue;

	core::array<core::stringw> unit_list_gcc;
	core::array<core::stringw> unit_list_gpp;


	/// <Unit filename="CbProjSuperBuilder_main.cpp" />

    //while there is more to read
    while (xml->read())
    {
		currentTag = xml->getNodeName();

		if ( currentTag.equals_ignore_case( unitTag ) && xml->isEmptyElement() )
		{
//			if ( xml->getNodeType() == io::EXN_ELEMENT )
//			{
//				if ( xml->isEmptyElement() )
//				{
			const u32 attribCount = xml->getAttributeCount();

			for ( u32 e = 0; e < attribCount; e++ )
			{
				attribName = xml->getAttributeName( e );

				if ( attribName.equals_ignore_case( filenameAttrib ) )
				{
					attribValue = xml->getAttributeValue( e );

					s32 pos = attribValue.findLast( L'.' );

					if ( pos != -1 )
					{
						core::stringw fileExt = attribValue.subString( pos + 1, attribValue.size() - pos - 1 );

						/// push to table of source units for G++ Compiler

						bool foundExt = false;

						for ( u32 i = 0; i < src_ext_gpp.size(); i++ )
						{
							if ( fileExt.equals_ignore_case( src_ext_gpp[i] ) )
							{
								unit_list_gpp.push_back( attribValue );
								foundExt = true;
								break;
							} // END-IF
						} // END-FOR

						/// push to table of source units for GCC Compiler

						if (!foundExt)
						{
							for ( u32 i = 0; i < src_ext_gcc.size(); i++ )
							{
								if ( fileExt.equals_ignore_case( src_ext_gcc[i] ) )
								{
									unit_list_gcc.push_back( attribValue );
									break;
								} // END-IF
							} // END-FOR
						} // END-IF
					} // END-IF
				} // END-IF
			} // END-FOR
		} // END-IF
	} // END-WHILE

    // don't forget to delete the xml reader
    xml->drop();

	PRINT( "g++ unit count = %d\n", unit_list_gpp.size() );
	for ( u32 i = 0; i < unit_list_gpp.size(); i++ )
	{
		PRINT( "g++ unit [%d] = %ls\n", i, unit_list_gpp[i].c_str() );
	} // END-FOR

	PRINT( "gcc unit count = %d\n", unit_list_gcc.size() );
	for ( u32 i = 0; i < unit_list_gcc.size(); i++ )
	{
		PRINT( "gcc unit [%d] = %ls\n", i, unit_list_gcc[i].c_str() );
	} // END-FOR
    return true;
}

/*
The main function. Creates all objects and does the XML handling.
*/
s32 main( s32 argc, c8** argv )
{
	PRINT_INFO();

    MyApp app( argc, argv );

	MyEventReceiver receiver( app.getDevice() );

	// createSuperBuildFromCbProject( app.getDevice(), "CbProjSuperBuilder.cbp" );

	createSuperBuildFromCbProject( app.getDevice(), "CbProjSuperBuilder.cbp" );

	while (app.run())
	{
		app.drawAll();
	}

	//app destroys device in destructor
	return 0;
}
