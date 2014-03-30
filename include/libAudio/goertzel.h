#ifndef __IRR_EXT_GOERTZEL_ALGORITHM_H__
#define __IRR_EXT_GOERTZEL_ALGORITHM_H__

#include "../CompileConfig.h"

namespace irr
{
namespace core
{
	/// Single frequency detection using Goertzel-Algorithm
	/// superior over FFT for less than 100 detections

	/// source: http://stackoverflow.com/questions/11579367/implementation-of-goertzel-algorithm-in-c

	f32 goertzel_mag2( const core::array<f32>& data, f32 FREQUENCY, u32 SAMPLE_RATE );

} // end namespace core
} // end namespace irr

#endif // __IRR_EXT_GOERTZEL_ALGORITHM_H__
