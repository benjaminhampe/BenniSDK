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

#include <cstdlib>
#include <iostream>
#include <cmath>

namespace std {

	double round(double v, double digit)
	{
		double p = pow(10.0, digit);
		double t = v * p;
		double r = floor(t + 0.5);
		return r / p;
	}

	float round(float v, float digit)
	{
		float p = powf(10.0f, digit);
		float t = v * p;
		float r = floorf(t + 0.5f);
		return r / p;
	}
}

using namespace irr;

void testMath( const core::stringc& filename = "testMath.out" )
{
	dbPRINT( "testMath()\n" );

	std::fstream fout( filename.c_str(), std::ios::out );
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
			fout << "[" << i << "] " << *f << " :: ceil = " << v0 << "ceil32 = " << v1 << std::endl;
		}

		if (i%tries_until_next_info == 0)
		{
			std::cout<< "Tested " << i << " Numbers, found " << found_errors << " errors." << std::endl;
		}
	}

	std::cout << "found_errors = " << found_errors << std::endl;
	fout << "found_errors = " << found_errors << std::endl;
	fout.close();
}

s32 main( s32 argc, c8** argv )
{
	PRINT_INFO();

	std::cout << "STL Math double" << std::endl;
	std::cout << std::round(4.45, 1) << std::endl;
	std::cout << std::round(4.55, 1) << std::endl;

	std::cout << "STL Math float" << std::endl;
	std::cout << std::round(4.45f, 1) << std::endl;
	std::cout << std::round(4.55f, 1) << std::endl;

	std::cout << "My Math" << std::endl;
	std::cout << core::Math::round(4.45, 1) << std::endl;
	std::cout << core::Math::round(4.55, 1) << std::endl;

	std::cout << "My Math" << std::endl;
	std::cout << core::Math::round(4.45f, 1) << std::endl;
	std::cout << core::Math::round(4.55f, 1) << std::endl;
	//testMath( "testMath.out" );

	std::cout << "STL Math double" << std::endl;
	std::cout << std::round(-4.45, 1) << std::endl;
	std::cout << std::round(-4.55, 1) << std::endl;

	std::cout << "STL Math float" << std::endl;
	std::cout << std::round(-4.45f, 1) << std::endl;
	std::cout << std::round(-4.55f, 1) << std::endl;

	std::cout << "My Math" << std::endl;
	std::cout << core::Math::round(-4.45, 1) << std::endl;
	std::cout << core::Math::round(-4.55, 1) << std::endl;

	std::cout << "My Math" << std::endl;
	std::cout << core::Math::round(-4.45f, 1) << std::endl;
	std::cout << core::Math::round(-4.55f, 1) << std::endl;
	//testMath( "testMath.out" );
	//app destroys device in destructor
	return 0;
}
