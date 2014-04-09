/**
 * Copyright (C) 2002-2014 by Benjamin Hampe <benjaminhampe@gmx.de>
 *
 * @page AudioAnimator3d
 *
 * @brief This program creates an animated 3d power-spectrum from audio-files
 *
 * For conditions of distribution and use, see copyright notice in irrlicht.h
 * idea found at http://www.youtube.com/watch?v=gbuZVcw3ZiM
 * no copyright infringement intended i just like working with audio-data
 *
 * to compile this program u need fftw3, SFML2.1, freetype and Irrlicht
 *
 * tested on Windows7 32-bit Professional
 * tested on Windows7 64-bit Home Premium
 *
 * test system 1: Core2Duo E6600 2x 2,4 GHz, 2GiB RAM, Ati HD5450 512MiByte VRAM
 * test system 2: Core i5 2x 2,53 GHz, 4GiB RAM, Geforce 310M 1024MiByte VRAM + Intel Graphics Hybrid
 */

#include <CompileConfig.h>

// extensions
#include <debugPrint.h>
#include <core.h>

#include <libImage/SRange.h>
#include <libImage/UString.h>
#include <libImage/IGUITTFont.h>
#include <libImage/CGUITTFont.h>
#include <libImage/CMath.h>
#include <libImage/CMatrix.h>
#include <libImage/CMatrixSceneNode.h>
#include <libImage/ColorConstants.h>
#include <libImage/IColorGradient.h>
#include <libImage/CLinearColorGradient.h>
#include <libImage/CLinearColorGradientTable.h>
#include <libImage/renderByPrimitiveType.h>
#include <libImage/sinCosTable.h>
#include <libImage/geometry.h>
#include <libImage/geometry_meshbuffer.h>

#include <libAudio/FourierTransformRtoC.h>
#include <libAudio/irrAudio.h>
#include <libAudio/IAudioPlayer.h>
#include <libAudio/CAudioPlayerSFML.h>
#include <libAudio/CGUIAudioPlayer.h>

#include <../source/Irrlicht/os.cpp>
#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CMeshSceneNode.cpp>
#include <../source/Irrlicht/CShadowVolumeSceneNode.cpp>

#include "AudioAnimator3d_App.h"

using namespace irr;

/// This window callback allows the user to save & exit, don't save, or cancel.
//static void window_cb (Fl_Widget *widget, void *)
//{
//    Fl_Window *window = (Fl_Window *)widget;
//
//    // fl_choice presents a modal dialog window with up to three choices.
//    int result = fl_choice("Do you want to save before quitting?",
//                           "Don't Save",  // 0
//                           "Save",        // 1
//                           "Cancel"       // 2
//                           );
//    if (result == 0) {  // Close without saving
//        window->hide();
//    } else if (result == 1) {  // Save and close
//        save();
//        window->hide();
//    } else if (result == 2) {  // Cancel / don't close
//        // don't do anything
//    }
//}
//
///Set your window's callback wherever you set up your Fl_Window, e.g. in your main function:
//window->callback( win_cb );

s32 main( s32 argc, c8** argv)
{
#ifdef _IRR_COMPILE_WITH_FLTK_
	Fl::scheme("gtk+");

#endif // _IRR_COMPILE_WITH_FLTK_

	IrrlichtDevice* device = createOpenGlDevice( 0, -200, 32, false, false );
	if (!device)
	{
		dbPRINT("Could not create Irrlicht-Device\n")
		exit(-2);
	}

	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	gui::IGUISkin* skin = env->createSkin( gui::EGST_WINDOWS_CLASSIC );
	//gui::IGUISkin* skin = env->createSkin( gui::EGST_WINDOWS_METALLIC );
	//gui::IGUISkin* skin = env->createSkin( gui::EGST_BURNING_SKIN );
	env->setSkin( skin );

	dbPRINT( "chosen skin-type = %d\n", env->getSkin()->getType() )

	Application app( device );

#ifdef _IRR_COMPILE_WITH_FLTK_

	int width, a;
	Fl::screen_xywh(a,a,width,a);
	// Yes, we could get screen width via Irrlicht too.

//	Fl_Window *window = new Fl_Window(width-300,0,300,180,"Edit properties");
//	Fl_Button *btn = new Fl_Button(20,40,260,100,"Go higher!");
//	btn->user_data(0);
//	btn->callback(btn_pressed);
//	window->end();
//	window->show();

#endif // _IRR_COMPILE_WITH_FLTK_

	app.run();
	return 0;
}

//
//class Application : public Fl_Window
//{
//	Fl_Native_File_Chooser *fc;
//	// Does file exist?
//	int exist(const char *filename)
//	{
//		FILE *fp = fopen(filename, "r");
//		if (fp) { fclose(fp); return(1); }
//		else    { return(0); }
//	}
//	// 'Open' the file
//	void open(const char *filename) {
//	printf("Open '%s'\n", filename);
//	}
//	// 'Save' the file
//	//    Create the file if it doesn't exist
//	//    and save something in it.
//	//
//	void save(const char *filename) {
//	printf("Saving '%s'\n", filename);
//	if ( !exist(filename) ) {
//	  FILE *fp = fopen(filename, "w");				// create file if it doesn't exist
//	  if ( fp ) {
//		// A real app would do something useful here.
//		fprintf(fp, "Hello world.\n");
//		fclose(fp);
//	  } else {
//		fl_message("Error: %s: %s", filename, strerror(errno));
//	  }
//	} else {
//	  // A real app would do something useful here.
//	}
//	}
//	// Handle an 'Open' request from the menu
//	static void open_cb(Fl_Widget *w, void *v) {
//	Application *app = (Application*)v;
//	app->fc->title("Open");
//	app->fc->type(Fl_Native_File_Chooser::BROWSE_FILE);		// only picks files that exist
//	switch ( app->fc->show() ) {
//	  case -1: break;	// Error
//	  case  1: break; 	// Cancel
//	  default:		// Choice
//		app->fc->preset_file(app->fc->filename());
//		app->open(app->fc->filename());
//	break;
//	}
//	}
//	// Handle a 'Save as' request from the menu
//	static void saveas_cb(Fl_Widget *w, void *v) {
//	Application *app = (Application*)v;
//	app->fc->title("Save As");
//	app->fc->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);	// need this if file doesn't exist yet
//	switch ( app->fc->show() ) {
//	  case -1: break;	// Error
//	  case  1: break; 	// Cancel
//	  default:		// Choice
//		app->fc->preset_file(app->fc->filename());
//		app->save(app->fc->filename());
//	break;
//	}
//	}
//	// Handle a 'Save' request from the menu
//	static void save_cb(Fl_Widget *w, void *v) {
//	Application *app = (Application*)v;
//	if ( strlen(app->fc->filename()) == 0 ) {
//	  saveas_cb(w,v);
//	} else {
//	  app->save(app->fc->filename());
//	}
//	}
//	static void quit_cb(Fl_Widget *w, void *v) {
//	exit(0);
//	}
//	// Return an 'untitled' default pathname
//	const char* untitled_default() {
//	static char *filename = 0;
//	if ( !filename ) {
//	  const char *home =
//		getenv("HOME") ? getenv("HOME") :		 // unix
//	getenv("HOME_PATH") ? getenv("HOME_PATH") :	 // windows
//	".";						 // other
//	  filename = (char*)malloc(strlen(home)+20);
//	  sprintf(filename, "%s/untitled.txt", home);
//	}
//	return(filename);
//	}
//	public:
//	// CTOR
//	Application() : Fl_Window(400,200,"Native File Chooser Example") {
//	Fl_Menu_Bar *menu = new Fl_Menu_Bar(0,0,400,25);
//	menu->add("&File/&Open",  FL_COMMAND+'o', open_cb, (void*)this);
//	menu->add("&File/&Save",  FL_COMMAND+'s', save_cb, (void*)this);
//	menu->add("&File/&Save As", 0,  saveas_cb, (void*)this);
//	menu->add("&File/&Quit",  FL_COMMAND+'q', quit_cb);
//	// Describe the demo..
//	Fl_Box *box = new Fl_Box(20,25+20,w()-40,h()-40-25);
//	box->color(45);
//	box->box(FL_FLAT_BOX);
//	box->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE|FL_ALIGN_WRAP);
//	box->label("This demo shows an example of implementing "
//			   "common 'File' menu operations like:\n"
//			   "    File/Open, File/Save, File/Save As\n"
//		   "..using the Fl_Native_File_Chooser widget.\n\n"
//		   "Note 'Save' and 'Save As' really *does* create files! "
//		   "This is to show how behavior differs when "
//		   "files exist vs. do not.");
//	box->labelsize(12);
//	// Initialize the file chooser
//	fc = new Fl_Native_File_Chooser();
//	fc->filter("Text\t*.txt\n");
//	fc->preset_file(untitled_default());
//	end();
//	}
//};
//
//s32 main( s32 argc, c8** argv)
//{
//	Fl::scheme("gtk+");
//	Application *app = new Application();
//	app->show(argc,argv);
//	return(Fl::run());
//}
