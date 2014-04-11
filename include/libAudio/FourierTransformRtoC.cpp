// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "FourierTransformRtoC.h"

#ifdef _IRR_COMPILE_WITH_FFTW3_

namespace irr
{
namespace core
{

FourierTransformRtoC::FourierTransformRtoC( u32 fft_size, const c8* wisdomFile )
: Size(fft_size), InputData(0), OutputData(0), WisdomFile(wisdomFile)
{
	loadWisdom( WisdomFile.c_str() );

	InputData = new f64[Size];
	if (!InputData)
	{
		dbERROR("Could not create InputVectorArray.\n")
	}
	else
	{
		for (u32 i=0; i<Size; i++)
		{
			InputData[i] = 0.0;
		}
	}

	OutputData = (fftw_complex*) fftw_malloc( Size*sizeof(fftw_complex) );
	if (!OutputData)
	{
		dbERROR("Could not create OutputVectorArray.\n")
	}

	Plan = fftw_plan_dft_r2c_1d( Size, InputData, OutputData, FFTW_MEASURE );

	dbPRINT( "PlanInfo = %s\n", getPlanInfo().c_str())
}

FourierTransformRtoC::~FourierTransformRtoC()
{
	saveWisdom( WisdomFile.c_str() );
	clear();
}

u32 FourierTransformRtoC::size() const
{
	return Size;
}

bool FourierTransformRtoC::resize( u32 fft_size )
{
	fftw_destroy_plan(Plan);
	fftw_free(OutputData);
	OutputData = 0;
	fftw_cleanup();

	Size = fft_size;

	InputData = new f64[Size];
	if (!InputData)
	{
		dbERROR("Could not create InputVectorArray.\n")
	}
	else
	{
		for (u32 i=0; i<Size; i++)
		{
			InputData[i] = 0.0;
		}
	}

	OutputData = (fftw_complex*) fftw_malloc( Size*sizeof(fftw_complex) );
	if (!OutputData)
	{
		dbERROR("Could not create OutputVectorArray.\n")
	}

	Plan = fftw_plan_dft_r2c_1d( Size, InputData, OutputData, FFTW_MEASURE );

	dbPRINT( "PlanInfo = %s\n", getPlanInfo().c_str())
}

void FourierTransformRtoC::clear()
{
	fftw_destroy_plan(Plan);
	fftw_free(OutputData);
	OutputData = 0;
	fftw_cleanup();

	if (InputData)
	{
		delete [] InputData;
		InputData = 0;
	}
}

void FourierTransformRtoC::loadWisdom( const c8* wisdomFile )
{
		// Check for wisdom
	if( fftw_import_wisdom_from_filename( wisdomFile ) == 0 )
	{
		dbERROR("wisdom not loaded.\n")
	}
	else
	{
		dbPRINT("wisdom loaded.\n")
	}
}

void FourierTransformRtoC::saveWisdom( const c8* wisdomFile ) const
{
	// Store Wisdom
	fftw_export_wisdom_to_filename( wisdomFile );
}

void FourierTransformRtoC::fft()
{
	fftw_execute( Plan );
}

core::stringc FourierTransformRtoC::getPlanInfo() const
{
	f64 plan_add, plan_mul, plan_fma, fft_cost = fftw_cost( Plan );
	fftw_flops( Plan, &plan_add, &plan_mul, &plan_fma );

	core::stringc txt = "RtoC with N="; txt += Size; txt += "\n";
	txt += "PlanCost = "; txt += fft_cost; txt += "\n";
	txt += "Used Additions = "; txt += plan_add; txt += "\n";
	txt += "Used Multiplications = "; txt += plan_mul; txt += "\n";
	txt += "Used Fused MulAdd = "; txt += plan_fma;
	return txt;
}

} // end namespace core
} // end namespace irr

#endif // _IRR_COMPILE_WITH_FFTW3_
