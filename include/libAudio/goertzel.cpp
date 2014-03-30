
#include "goertzel.h"

namespace irr
{
namespace core
{
	/// Single frequency detection using Goertzel-Algorithm
	/// superior over FFT for less than 100 detections

	/// source: http://stackoverflow.com/questions/11579367/implementation-of-goertzel-algorithm-in-c

	f32 goertzel_mag2( const core::array<f32>& data, f32 FREQUENCY, u32 SAMPLE_RATE )
	{
		if (data.size() == 0)
			return -1.0f;

		const u32 sampleCount = data.size();

		const f32 fSampleCount = (f32)sampleCount;

		const f32 scaleFactor = 2.0f / fSampleCount;

		const s32 k = (s32) (0.5f + ((fSampleCount * FREQUENCY) / (f32)SAMPLE_RATE));

		const f32 phi = (2.0f * core::PI * (f32)k) / fSampleCount;

		const f32 s = sinf(phi);

		const f32 c = cosf(phi);

		const f32 koeff = 2.0f * c;

		f32 q0 = 0.0f;
		f32 q1 = 0.0f;
		f32 q2 = 0.0f;

		for( u32 i = 0; i < sampleCount; i++ )
		{
			q0 = koeff * q1 - q2 + data[i];
			q2 = q1;
			q1 = q0;
		}

		// calculate the real and imaginary results
		// scaling appropriately
		const f32 re = (q1 - q2 * c) * scaleFactor;

		const f32 im = (q2 * s) * scaleFactor;

		const f32 betrag = sqrtf(re*re + im*im);

		return betrag;
	}

} // end namespace core

} // end namespace irr
