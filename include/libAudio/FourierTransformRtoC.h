#ifndef __IRR_EXTENSION_QUICK_FOURIER_TRANSFORM_CLASS_H__
#define __IRR_EXTENSION_QUICK_FOURIER_TRANSFORM_CLASS_H__

#include <CompileConfig.h>

#ifdef _IRR_COMPILE_WITH_FFTW3_

#include <libImage/SRange.h>

namespace irr
{
namespace core
{
	/// @class FOURIER_TRANSFORM real to complex
	/** @brief Umwandlung von Zeitbereich A(t) in Frequenzbereich F(s), s ist komplex
	 *	y = A(x), mit x = 0...N-1 numbers
	 *	in den Frequenzraum F[index] = a + b*i
	 *	z = F(s) = a + b*i, mit s = 0...N-1 numbers
	 *	FourierTransform is orthogonal and symmetric
	 *	Die 'FFT-Size' kann verschieden von der Aufnahmesamplerate sein.
	 *	daher gibt die 'FFT-Size' nur die Anzahl an Eingabesamples an
	 *	die für die Rechnung benutzt werden.
	 *	pro Eingabezahl werden zwei Zahlen erzeugt ( Real- und Imaginärteil )
	 *	sample[index] --> f[index] = { re, im } gespeichert.
	 *	Die tatsächliche Frequenz ist aber abhängig von der Aufnahmesamplerate.
	 *	Betrag |z| = a^2 + b^2 --> Powerspektrum ( real )
	 */
	class FourierTransformRtoC : public IReferenceCounted
	{
	private:
		u32  			Size;
		f64*			InputData;	// real input-vector
		fftw_complex*	OutputData; // complex output-vector
		fftw_plan 		Plan;
		core::stringc   WisdomFile;

	public:
		FourierTransformRtoC( u32 fft_size = 1024, const c8* wisdomFile = "FourierTransformRtoC.wisdom" );

		~FourierTransformRtoC();

		void clear();

		bool resize( u32 fft_size );

		u32 size() const;

		void fft();

		void loadWisdom( const c8* wisdomFile );

		void saveWisdom( const c8* wisdomFile ) const;

		core::stringc getPlanInfo() const;

		// methods for generic types

		template<class T>
		core::SRange<T> getMinMax( const core::array<T>& _in, T min_init, T max_init )
		{
			core::SRange<T> result( min_init, max_init );

			for (u32 i=0; i<_in.size(); i++)
			{
				const T value = _in[i];

				if (result.Min > value)
					result.Min = value;

				if (result.Max < value)
					result.Max = value;
			}

			return result;
		}

		template<class T>
		void setInputData( const core::array<T>& _in )
		{
			u32 i_max = core::min_<u32>( _in.size(), Size );

			for (u32 i=0; i<i_max; i++)
			{
				InputData[i] = (f64)_in[i];
			}

			if (i_max < Size)
			{
				for (u32 i=i_max; i<Size; i++)
				{
					InputData[i] = 0.0;
				}
			}
		}

//		template<class T>
//		void getPowerSpectrum( core::array<T>& _out )
//		{
//			const u32 i_max = core::min_<u32>( _out.size(), Size );
//
//			_out.set_used( 0 );
//
//			for (u32 i=0; i<i_max; i++)
//			{
//				const fftw_complex& z = OutputData[i];		// Lesezugriff auf komplexe Zahl
//
//				f64 z_abs = sqrt( z[0]*z[0] + z[1]*z[1] );	// Betrag der komplexen Zahl z
//
//				_out.push_back( ((T)z_abs) );
//			}
//
//			const core::vector2d<T> z_minmax = getMinMax<T>( _out, (T)FLT_MAX, (T)FLT_MIN );
//			const f64 z_range_inv = core::reciprocal( (f64)z_minmax.Y - (f64)z_minmax.X );
//
//			for (u32 i=0; i<i_max; i++)
//			{
//				T value = (T)(z_range_inv*((f64)_out[i]-(f64)z_minmax.X));
//				_out[i] = value;
//			}
//		}


		template<class T>
		void getPowerSpectrumAsDecibels( core::array<T>& _out )
		{
			const u32 i_max = core::min_<u32>( _out.size(), Size );

			_out.set_used( 0 );

			for (u32 i=0; i<i_max; i++)
			{
				const fftw_complex& z = OutputData[i];	// Lesezugriff auf komplexe Zahl

				f64 z_abs = sqrt( z[0]*z[0] + z[1]*z[1] );	// Betrag der komplexen Zahl z

				z_abs = 20.0*log10( z_abs );				// Umwandlung in Dezibels

				_out.push_back( ((T)z_abs) );
			}
		}

		template<class T>
		void getScaledPowerSpectrumAsDecibelsThresholdFast(
			core::array<T>& _out, T min, T max, T threshold )
		{
			const u32 i_max = core::min_<u32>( _out.size(), Size );

			_out.set_used( 0 );

			const f32 db_range = (f32)(max - min);

			f32 db_range_inv = 0.0f;

			if (db_range>0.0f)
				db_range_inv = core::reciprocal( db_range );

			f32 re, im, zm;

			for (u32 i=0; i<i_max; i++)
			{
				const fftw_complex& z = OutputData[i];		// hole komplexe Zahl z
				re = (f32)z[0];
				im = (f32)z[1];
				zm = sqrtf( re*re + im*im );	// Betrag |z| von z
				zm = 20.0f*log10( zm );				// Umwandlung in Dezibels

				zm -= (f32)min;

				if (zm < (f32)threshold )
					zm = (f32)min;

				zm *= db_range_inv;

				zm = core::clamp<f32>( zm, 0.0f, 1.0f );

				_out.push_back( (T)zm );
			}
		}

		template<class T>
		void getScaledPowerSpectrumAsDecibelsThreshold(
			core::array<T>& _out, T min, T max, T threshold )
		{
			const u32 i_max = core::min_<u32>( _out.size(), Size );

			_out.set_used( 0 );

			const f64 db_range = max - min;
			f64 db_range_inv(0);

			if (db_range>0)
				db_range_inv = core::reciprocal( db_range );

			for (u32 i=0; i<i_max; i++)
			{
				const fftw_complex& z = OutputData[i];		// hole komplexe Zahl z

				f64 z_abs = sqrt( z[0]*z[0] + z[1]*z[1] );	// Betrag |z| von z
				z_abs = 20.0*log10( z_abs );				// Umwandlung in Dezibels

				z_abs -= min;

				if (threshold > z_abs)
					z_abs = min;

				z_abs *= db_range_inv;

				z_abs = core::clamp<f64>( z_abs, 0, 1);

				_out.push_back( (T)z_abs );
			}
		}


		template<class T>
		void getScaledPowerSpectrum(
			core::array<T>& _out,
			f64 dB_min = 0.0,
			f64 dB_max = 150.0 )
		{
			const u32 i_max = core::min_<u32>( _out.size(), Size );

			_out.set_used( 0 );

			for (u32 i=0; i<i_max; i++)
			{
				const fftw_complex& z = OutputData[i];	// read only access to complex number

				f64 z_abs = sqrt( z[0]*z[0] + z[1]*z[1] );	// magnitude ( abs ) of complex number / Betrag der komplexen Zahl z

				z_abs = 20.0*log10( z_abs ) ;				// convert to decibels / Umwandlung in Dezibels

				z_abs -= dB_min;							// value is in range [ 0, dB_range ] now

				if ( z_abs > dB_max )						// clip values to maximum, if greater
					z_abs = dB_max;

				//z_abs *= dB_range_inv;						// value is in range [ 0, 1 ] now, normalized

				_out.push_back( ((T)z_abs) );				// insert at the end of array, no allocation involved
			}
//
//			if (core::equals( y_scale, 0.0f ) ||			// if scale factor is 0 or 1 ( epsilon is ROUNDING_ERROR_F64 )
//				core::equals( y_scale, 1.0f ) )
//				return;									// exit function
//
//			for (u32 i=0; i<_out.size(); i++)				// loop array-elements
//			{
//				_out[i] *= y_scale;							// scale each array element by factor
//			}
		}

		template<class T>
		void getPowerSpectrumAsDecibelsThreshold( core::array<T>& _out, T threshold )
		{
			const u32 i_max = core::min_<u32>( _out.size(), Size );

			_out.set_used( 0 );

			for (u32 i=0; i<i_max; i++)
			{
				const fftw_complex& z = OutputData[i];	// Lesezugriff auf komplexe Zahl

				f64 z_abs = sqrt( z[0]*z[0] + z[1]*z[1] );	// Betrag der komplexen Zahl z

				z_abs = 20.0f*log10( z_abs );				// Umwandlung in Dezibels

				if (threshold > z_abs)
					z_abs = 0.0;

				_out.push_back( ((T)z_abs) );
			}
		}





	};

} // end namespace core
} // end namespace irr

#endif // _IRR_COMPILE_WITH_FFTW3_

#endif // __IRR_EXTENSION_QUICK_FOURIER_TRANSFORM_CLASS_H__
