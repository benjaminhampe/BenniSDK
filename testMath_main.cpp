/**
	testMath.cpp
*/

#include <iostream>
#include <fstream>
#include <cmath>

#include <irrlicht.h>
#include <libImage/CMath.h>

#if defined(_DEBUG) || defined(DEBUG)
#define dbPRINT(args...) { fprintf( stdout, args); fflush(stdout); }
#else
#define dbPRINT(args...)
#endif

void PRINT_INFO()
{
#if defined(__x86_64__) || defined(_M_X64)
	dbPRINT("Architecture = x86 64-Bit\n");
#elif defined(__i386) || defined(_M_IX86)
	dbPRINT("Architecture = x86 32-Bit\n");
#else
	PRINT("Architecture = unknown\n");
#endif
}

using namespace irr;
using namespace std;

void testMath( const core::stringc& filename = "testMath.out" )
{
	dbPRINT( "testMath()\n" );

	fstream fout( filename.c_str(), ios::out );

	if (!fout.is_open())
	{
		dbPRINT( "Could not open file %s\n", filename.c_str() );
		return;
	}

	u32 found_errors = 0;

	const u32 tries_until_next_info = 100000000;

	for ( u32 i=0; i<0xffffffff; i++ )
	{
		f32* f = (f32*)&i;

		s32 v0 = (s32)ceil( *f );
		s32 v1 = core::ceil32( *f );

		bool b01 = core::equals( v0, v1 );

		if (!b01)
		{
			found_errors++;
			fout << "[" << i << "] " << *f << " :: ceil = " << v0 << "ceil32 = " << v1 << endl;
		}

		if (i%tries_until_next_info == 0)
		{
			cout<< "Tested " << i << " Numbers, found " << found_errors << " errors." << endl;
		}
	}

	cout << "found_errors = " << found_errors << endl;
	fout << "found_errors = " << found_errors << endl;
	fout.close();
}

/*
The main function. Creates all objects and does the XML handling.
*/
s32 main( s32 argc, c8** argv )
{
	PRINT_INFO();

	testMath( "testMath.out" );

	//app destroys device in destructor
	return 0;
}
