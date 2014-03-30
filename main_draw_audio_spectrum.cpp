// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#define MY_APP_TITLE "AudioFile to Spectrum"

using namespace irr;

s32 main( s32 argc, c8** argv)
{
	/// SMFL Timer ( using static class instance )
	sf::Clock timer;
	timer.restart();
	u32 time_start = timer.getElapsedTime().asMilliseconds();

// Flag set by ‘--verbose’.
//static int verbose_flag;
//
//	int c;
//
//	while (1)
//	{
//		static struct option long_options[] =
//		{
//			/* These options set a flag. */
//			{"verbose", no_argument,       &verbose_flag, 1},
//			{"brief",   no_argument,       &verbose_flag, 0},
//			/* These options don't set a flag.
//			We distinguish them by their indices. */
//			{"add",     no_argument,       0, 'a'},
//			{"append",  no_argument,       0, 'b'},
//			{"delete",  required_argument, 0, 'd'},
//			{"create",  required_argument, 0, 'c'},
//			{"file",    required_argument, 0, 'f'},
//			{0, 0, 0, 0}
//		};
//		/* getopt_long stores the option index here. */
//		int option_index = 0;
//
//		c = getopt_long (argc, argv, "abc:d:f:",
//				long_options, &option_index);
//
//		/* Detect the end of the options. */
//		if (c == -1)
//			break;
//
//		switch (c)
//		{
//			case 0:
//			/* If this option set a flag, do nothing else now. */
//			if (long_options[option_index].flag != 0)
//			break;
//			printf ("option %s", long_options[option_index].name);
//			if (optarg)
//			printf (" with arg %s", optarg);
//			printf ("\n");
//			break;
//
//			case 'a':
//			puts ("option -a\n");
//			break;
//
//			case 'b':
//			puts ("option -b\n");
//			break;
//
//			case 'c':
//			printf ("option -c with value `%s'\n", optarg);
//			break;
//
//			case 'd':
//			printf ("option -d with value `%s'\n", optarg);
//			break;
//
//			case 'f':
//			printf ("option -f with value `%s'\n", optarg);
//			break;
//
//			case '?':
//			/* getopt_long already printed an error message. */
//			break;
//
//			default:
//			abort ();
//		}
//	}
//	/* Instead of reporting ‘--verbose’
//	and ‘--brief’ as they are encountered,
//	we report the final status resulting from them. */
//	if (verbose_flag)
//		puts ("verbose flag is set");
//
//	/* Print any remaining command line arguments (not options). */
//	if (optind < argc)
//	{
//		printf ("non-option ARGV-elements: ");
//		while (optind < argc)
//		printf ("%s ", argv[optind++]);
//		putchar ('\n');
//	}
//
//	exit (0);

	printf("Time to parse commandline = %d ms\n", timer.getElapsedTime().asMilliseconds() - time_start);
	timer.restart();

	/// Create NullDevice ( silent = no logging )

	SIrrlichtCreationParameters params;
	params.LoggingLevel = ELL_NONE;
	params.DriverType = video::EDT_NULL;
    IrrlichtDevice* device = createDeviceEx( params );
	if (!device)
	{
		exit(-2);
	}

	device->setWindowCaption( core::stringw( MY_APP_TITLE ).c_str() );

    device->setResizable( true );

	video::IVideoDriver* driver = device->getVideoDriver();

	video::IVideoModeList* monitors = device->getVideoModeList();

	const core::dimension2du desktop_size = monitors->getDesktopResolution();

	core::dimension2du screen_size = driver->getScreenSize();

	/// init random number generator with current system-time ( most probably random )

	ITimer* irr_timer = device->getTimer();

	srand( irr_timer->getRealTime() );

	// print information and reset timer
	printf("Time to create Irrlicht-Engine silent NullDevice = %d ms\n", timer.getElapsedTime().asMilliseconds() - time_start);

	timer.restart();

    /// create image ( software ) in RAM memory
	core::dimension2du img_size( desktop_size.Width, 128 );

	video::ECOLOR_FORMAT color_format = video::ECF_A8R8G8B8;

	video::CImage* img = new video::CImage( color_format, img_size );
	if (!img)
	{
		printf("ERROR while creating image %d x %d x %d\n",
			img_size.Width, img_size.Height, video::IImage::getBitsPerPixelFromFormat( color_format ) );
	}

	// print information and reset timer
	printf("Time to create image %d x %d x %d = %d Bytes = %d ms\n", img_size.Width, img_size.Height,
			video::IImage::getBitsPerPixelFromFormat( color_format ) ,
			(video::IImage::getBitsPerPixelFromFormat( color_format )>>3) * img_size.Width * img_size.Height,
			timer.getElapsedTime().asMilliseconds() - time_start);

	timer.restart();


	/// Create PlayList
	core::stringc FileName = MY_AUDIO_FILE;
	core::array<core::stringc> PlayList;
	PlayList.push_back( FileName );

	/*
		loop PlayList here
	*/

	sf::SoundBuffer my_soundbuffer;

	io::path savename = _IRR_TEXT("");

	ITimer::RealTimeDate date;

	/// loop filenames ( playlist )

	for ( u32 file_index=0; file_index<PlayList.size(); file_index++)
	{
		/// set current filename

		FileName = PlayList[file_index];

		/// load file with SFML ( ogg, flac, wav, mp3? )

		// skip, if file could not be loaded
		if (!my_soundbuffer.loadFromFile( FileName.c_str() ))
		{
			printf("ERROR while loading audio-file %s\n", FileName.c_str() );
		}
		// work with loaded file
		else
		{
			u32 my_samplerate = my_soundbuffer.getSampleRate();
			u32 my_samplecount = my_soundbuffer.getSampleCount();
			u32 my_channelcount = my_soundbuffer.getChannelCount();
			u32 my_duration = getDurationFromSoundBufferInMillis( &my_soundbuffer );

			// print information and reset timer
			printf("Time to load sound-buffer = %d ms\n", timer.getElapsedTime().asMilliseconds() - time_start);
			printf("\tFileName = %s\n", FileName.c_str() );
			printf("\tDuration = %f sec\n", 0.001f*my_duration );
			printf("\tChannelCount = %d\n", my_channelcount );
			printf("\tSampleRate = %d\n", my_samplerate);
			printf("\tSampleCount = %d\n", my_samplecount);

			timer.restart();

			// high is yellow, low is red color, we should also try to use full 24Bit RGB values for storing
			for (u32 i=0; i<my_channelcount; i++)
			{
				/// draw waveform

				img->fill(0);

				if (!drawAudioWaveformFast( img, &my_soundbuffer, i ) )
				{
					printf("ERROR: while drawing AudioWaveform ch%d to image\n", i);
				}

				printf("Time to drawAudioWaveform ch%d = %d ms\n", i, timer.getElapsedTime().asMilliseconds() - time_start);
				timer.restart();

				// save image to file

				date = device->getTimer()->getRealTimeAndDate();

				savename = _IRR_TEXT("img_");
				savename += currentDateTimeString( device->getTimer() ); // add current datetime so filename is unique
				savename += _IRR_TEXT("_ch"); savename += i;
				savename += _IRR_TEXT("_wav.png");

				if ( !device->getVideoDriver()->writeImageToFile( img, savename ) )
				{
					printf("ERROR: while saving image ch%d, %s\n", i, core::stringc(savename).c_str());
				}

				printf("Time to save image to file = %d ms\n", timer.getElapsedTime().asMilliseconds() - time_start);
				timer.restart();

//				/// draw waveform 2
//
//				img->fill(0);
//
//				if (!drawAudioWaveform( img, &my_soundbuffer, i ) )
//				{
//					printf("ERROR: while drawing AudioWaveform ch%d to image\n", i);
//				}
//
//				printf("Time to drawAudioWaveform ch%d = %d ms\n", i, timer.getElapsedTime().asMilliseconds() - time_start);
//				timer.restart();
//
//				// save image to file
//
//				date = device->getTimer()->getRealTimeAndDate();
//
//				savename = _IRR_TEXT("img_");
//				savename += currentDateTimeString( device->getTimer() ); // add current datetime so filename is unique
//				savename += _IRR_TEXT("_ch"); savename += i;
//				savename += _IRR_TEXT("_wav.png");
//
//				if ( !device->getVideoDriver()->writeImageToFile( img, savename ) )
//				{
//					printf("ERROR: while saving image ch%d, %s\n", i, core::stringc(savename).c_str());
//				}
//
//				printf("Time to save image to file = %d ms\n", timer.getElapsedTime().asMilliseconds() - time_start);
//				timer.restart();


				/// draw powerspectrum for each channel( abs( FourierTransform A(t) -> P(omega=2pi*f) )

				img->fill(0);

				if (!drawAudioSpectrum( img, &my_soundbuffer, i ) )
				{
					printf("ERROR: while drawing AudioSpectrum ch%d to image\n", i);
				}

				printf("Time to drawAudioSpectrum ch%d = %d ms\n", i, timer.getElapsedTime().asMilliseconds() - time_start);
				timer.restart();

				// save image to file

				date = device->getTimer()->getRealTimeAndDate();

				savename = _IRR_TEXT("img_");
				savename += currentDateTimeString( device->getTimer() ); // add current datetime so filename is unique
				savename += _IRR_TEXT("_ch"); savename += i;
				savename += _IRR_TEXT("_powerspectrum.png");

				if ( !device->getVideoDriver()->writeImageToFile( img, savename ) )
				{
					printf("ERROR: while saving image ch%d, %s\n", i, core::stringc(savename).c_str());
				}

				printf("Time to save image to file = %d ms\n", timer.getElapsedTime().asMilliseconds() - time_start);
				timer.restart();


			} /// end loop channel_index
		} /// end loop loaded files
	} /// end loop files in playlist


	img->drop();

	device->drop();

	return 0;
}
