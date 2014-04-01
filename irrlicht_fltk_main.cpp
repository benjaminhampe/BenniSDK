/// Copyright (C) 2002-2013 Benjamin Hampe
/// This file is part of the "irrlicht-engine"
/// For conditions of distribution and use, see copyright notice in irrlicht.h

#include <cstdint>	// intptr_t, uintptr_t
#include <cstdio>	// printf
#include <cstdlib>	// exit,malloc
#include <cstring>	// strerror
#include <cerrno>	// errno

#include <irrlicht.h>

#include <../source/Irrlicht/CColorConverter.cpp>
#include <../source/Irrlicht/CImage.cpp>
#include <../source/Irrlicht/os.cpp>
#include <../source/Irrlicht/CMeshSceneNode.cpp>
#include <../source/Irrlicht/CShadowVolumeSceneNode.cpp>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Box.H>
#include <FL/fl_ask.H>

using namespace irr;
//using namespace core;
//using namespace scene;
//using namespace video;





void btn_pressed(Fl_Widget*, void *data) {

   scene::ICameraSceneNode* ptr = (scene::ICameraSceneNode *) data;

   core::vector3df pos = ptr->getPosition();
   pos.Y++;
   ptr->setPosition(pos);
}

s32 main( s32 argc, c8** argv)
{
//	Fl::scheme("gtk+");

	SIrrlichtCreationParameters params;
	params.AntiAlias = video::EAAM_QUALITY;
	params.DriverType = video::EDT_OPENGL;
	params.HighPrecisionFPU = true;
	params.WindowSize = core::dimension2du(800,600);
	params.UsePerformanceTimer = true;
	params.Doublebuffer = true;
	params.WindowPosition = core::position2di(100,100);
	params.Fullscreen = false;
	params.Bits = 32;
	params.Vsync = false;
	params.WithAlphaChannel = true;

	IrrlichtDevice* device = createDeviceEx( params );

	if (!device)
	  return 1;

	video::IVideoDriver *driver = device->getVideoDriver();
	scene::ISceneManager *smgr = device->getSceneManager();
	ITimer *timer = device->getTimer();

	scene::IAnimatedMesh* mesh = smgr->getMesh("../../media/sydney.md2");
	if (!mesh)
	{
		device->drop();
		return 1;
	}

	scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	if (node) {
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture("../../media/sydney.png") );
	}

	scene::ISceneNode *cam = smgr->addCameraSceneNode( 0,
		core::vector3df(0,30,-40), core::vector3df(0,5,0));

	int width, a;
	Fl::screen_xywh(a,a,width,a);
	// Yes, we could get screen width via Irrlicht too.

	Fl_Window *window = new Fl_Window(width-300,0,300,180,"Edit properties");
	Fl_Button *btn = new Fl_Button(20,40,260,100,"Go higher!");
	btn->user_data(cam);
	btn->callback(btn_pressed);
	window->end();
	window->show();

	int fps=-1, lastfps= -1;
	long long time, lasttime = timer->getTime(), timediff;
	wchar_t cfps[7];

	while(device->run())
	{
		driver->beginScene(true, true, video::SColor(255,100,101,140));
		smgr->drawAll();
		driver->endScene();

		fps = driver->getFPS();
		if (fps != lastfps)
		{
			swprintf(cfps,7,L"%d",fps);
			cfps[6] = L'\0';
			// Why this and not stringw + whatever?
			// Think of it the next time you meditate.
			device->setWindowCaption(cfps);
			lastfps = fps;
		}

		Fl::check(); // This keeps our FLTK windows interactive.

		time = timer->getTime();
		timediff = 16-(time-lasttime);
		if (timediff > 0)
			device->sleep(timediff);
		// Yes, this is unrelated to FLTK.
		// I just prefer all my projects to be efficient.

		lasttime = timer->getTime();
	}

	device->drop();

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
