//// Copyright (C) 2002-2014 Benjamin Hampe
//// This file is part of the "irrlicht-engine"
//// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_S_RANGE_H__
#define __IRR_EXT_S_RANGE_H__

namespace irr {
namespace core {

	template <class T>
	struct SRange
	{
		T Min;
		T Max;

		SRange() : Min(T(0)), Max(T(1))
		{}

		SRange( const T& min, const T& max ) : Min(min), Max(max)
		{}

		T d() const
		{
			return Max - Min;
		}
	};

	typedef SRange<f32> SRangef;

} // end namespace core
} // end namespace irr

#endif // __IRR_EXT_S_RANGE_H__
