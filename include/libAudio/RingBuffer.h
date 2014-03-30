// Copyright (C) 2002-2014 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXT_RING_BUFFER_H__
#define __IRR_EXT_RING_BUFFER_H__

#include <irrlicht.h>

namespace irr {
namespace core {

	template <class T>
	class RingBuffer : public IReferenceCounted
	{
	private:
		core::array<T> Data;
		u32 Counter;

	public:
		///@brief Create RingBuffer object
		RingBuffer( u32 elementCount = 1024 ) : Counter(0)
		{
			// allocate memory for data
			Data.reallocate( elementCount );
			Data.set_used( elementCount );

			// initialize data with zeros
			const T zeroElement(0);
			for (u32 i=0; i<Data.size(); i++ )
			{
				Data[i] = zeroElement;
			}
		}

		///@brief Destroy RingBuffer object
		~RingBuffer()
		{
			Data.clear();
		}

		///@brief Return number of elements the array can hold
		virtual u32 size() const
		{
			return Data.size();
		}

		///@brief Add element at end of data-array
		virtual void push_back( const T& element )
		{
			Data[ Counter ] = element;
			Counter++;
			Counter = Counter % Data.size();
		}

		///@brief Save, read-only access to data-element by index
		virtual const T& operator[] ( u32 index ) const
		{
			return Data[ index % Data.size() ];
		}

		///@brief Save, read-only access to complete array-data
		virtual const core::array<T>& getArray () const
		{
			return Data;
		}

		///@brief Resize data-array
		virtual void resize( u32 elementCount, bool canShrink = true )
		{
			// allocate memory for data
			Data.reallocate( elementCount, canShrink );
			Data.set_used( elementCount );

			// initialize data with zeros
			const T zeroElement(0);
			for (u32 i=0; i<Data.size(); i++ )
			{
				Data[i] = zeroElement;
			}

			Counter = 0;
		}
	};

} // end namespace core
} // end namespace irr

#endif // __IRR_EXT_RING_BUFFER_H__
