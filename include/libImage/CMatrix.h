#ifndef __IRR_C_DYNAMIC_RECTANGLE_MATRIX_H__
#define __IRR_C_DYNAMIC_RECTANGLE_MATRIX_H__

#include "../CompileConfig.h"

namespace irr
{
namespace core
{

	template <class ElementType>
    class CMatrix : public IReferenceCounted
	{
	private:
		u32 Rows;	/// number of rows ( y-direction )
		u32 Cols;	/// number of columns ( x-direction )
		u32 Size;	/// number of elements in matrix ( == Rows * Cols )
		u32 BitsPerElement; /// size in bits ( 32, 64 )
		ElementType** Data;	/// pointer to 2d field == array of rows ( each row is an arrays with "Cols" elements )
		core::stringc Name;
		bool IsIdentity;
		bool IsDeterminantDirty;
		u32 Rank; // ( Rank always lower or equal then the minimum of (Rows,Cols) == number of linear independant basevectors)
		ElementType Determinant;

	public:

//	const ElementType NullValue = ElementType(0);
//	const ElementType EinsValue = ElementType(1);

//	static ElementType** create2DArray(u32 rows, u32 cols)
//
//	bool empty() const
//
//	void clear()
//
//	void fill( const ElementType& value )
//
//	bool resize(u32 rows, u32 cols, bool keepData = false, bool canShrink = true )
//
//	/// empty default contructor
//	CMatrix()
//
//	/// value contructor
//	CMatrix( u32 rows, u32 cols )
//
//	/// destructor
//	~CMatrix()
//
//	/// copy
//	CMatrix& assign( const CMatrix& other )
//
//	/// copy contructor
//	CMatrix( const CMatrix& other )
//
//	/// clone
//	CMatrix clone() const
//
//	typedef core::vector2d<ElementType> TVector2d;
//
//	TVector2d getMinMax() const
//
//	/// get access ( public ) to raw data
//	ElementType** getData()
//
//	/// get Rows & Cols
//	core::dimension2du getDimension() const
//
//	/// get Number of rows i have (y-dir)
//	u32 getRows() const
//
//	/// get Number of columns i have (x-dir)
//	u32 getCols() const
//
//	// /// get Product of Rows and Colums for linear memory access, its elementcount not the bytesize of allocated mem
//	//u32 getSize() const { return Size; }
//
//	/// get my Name
//	core::stringc getName() const
//
//	/// get my Name
//	void setName( const core::stringc& name = "CMatrix" )
//
//	/// print CMatrix to String
//	core::stringc toString() const
//
//	/// secure access to value ( with out-of-bounds check )
//	ElementType getElement(u32 row, u32 col) const
//
//	/// secure access to value ( with out-of-bounds check )
//	ElementType getElement(u32 index) const
//
//	/// secure access to value ( with out-of-bounds check )
//	bool setElement(u32 row, u32 col, ElementType element)
//
//	/// secure access to value ( with out-of-bounds check )
//	bool setElement(u32 index, ElementType element)
//
//	bool swapRows( u32 row_a, u32 row_b )
//
//	bool shiftRow()
//
//	bool shiftRows( s32 rows )
//
//	bool load( const core::stringc& filename ) const
//
//	bool save( const core::stringc& filename ) const

	static ElementType** create2DArray(u32 rows, u32 cols)
	{
		dbPRINT( "create2DArray(%d,%d)\n", rows, cols);

		// fill with zeros with memset
		const u32 byte_count = (u32)(sizeof(ElementType)*rows*cols);

		// allocate memory for vector that stores vectors of rows
		ElementType** p = new ElementType*[rows];

		// allocate memory for each row vector
		for (u32 y=0; y<rows; y++)
		{
			p[y]=new ElementType[cols];
		}

//		const ElementType NullElement(0);
//
//		for ( u32 y = 0; y < rows; y++ )
//		{
//			for ( u32 x = 0; x < cols; x++ )
//			{
//				p[y][x] = NullElement;
//			}
//		}
		/// beware that the data is still uninitialized ( use fill(0) to get a valid state )
		return p;
	}

	bool empty() const
	{
		if (!Data)
			return true;
		else
			return false;
	}

	void clear()
	{
		dbPRINT( "CMatrix::clear()\n");

		/// delete 2D Array of Floats
		if (!empty())
		{
			/// loop rows
			for (u32 i=0; i<Rows; i++)
			{
				/// delete each row
				ElementType* row = Data[i];
				if (row)
				{
					/// delete array
					delete [] row;

					Data[i] = 0;
				}
			}

			/// delete array of pointer to arrays
			delete [] Data;
			Data = 0;
		}

		Rows = 0;
		Cols = 0;
		Size = 0;
	}

	void fill( const ElementType& value )
	{
		if (empty())
		{
			return;
		}

		for (u32 y=0; y<Rows; y++)
		{
			for (u32 x=0; x<Cols; x++)
			{
				Data[y][x] = value;
			}
		}
	}

	bool resize(u32 rows, u32 cols, bool keepData = false, bool canShrink = true )
	{
		dbPRINT( "CMatrix::resize(%d,%d)\n", rows, cols );

		clear();

		Data = create2DArray( rows, cols );
		Rows = rows;
		Cols = cols;
		Size = rows*cols;

		fill( ElementType(0) );

		return true;
	}

	/// empty default contructor
	CMatrix()
		: Rows(0), Cols(0), Size(0), Data(0), Name("")
	{
		dbPRINT( "CMatrix::CMatrix()\n")
	}

	/// value contructor
	CMatrix( u32 rows, u32 cols )
		: Rows(0), Cols(0), Size(0), Data(0), Name("")
	{
		dbPRINT( "CMatrix::CMatrix(%d,%d)\n", rows, cols )
		Data = create2DArray( rows, cols );
		Rows = rows;
		Cols = cols;
		Size = rows * cols;
	}

	/// destructor
	~CMatrix()
	{
		dbPRINT( "destructor()\n" );
		clear();
	}

	/// copy
	CMatrix& assign( const CMatrix& other )
	{
		dbPRINT( "CMatrix::assign()\n" )

		clear();
		resize( other.getRows(), other.getCols() );

		Rows = other.getRows();
		Cols = other.getCols();
		Size = other.getSize();
		Name = other.getName();

		for (u32 y=0; y<Rows; y++)
		{
			for (u32 x=0; x<Cols; x++)
			{
				Data[y][x] = other.getElement(y,x);
			}
		}
		return *this;
	}

	/// copy contructor
	CMatrix( const CMatrix& other )
		: Rows(0), Cols(0), Size(0), Data(0), Name("")
	{
		dbPRINT( "CMatrix::CMatrix( CMatrix(%d,%d) )\n", other.getRows(), other.getCols() );
		assign( other );
	}

	/// clone
	CMatrix clone() const
	{
		return CMatrix( *this );
	}

	typedef core::vector2d<ElementType> TVector2d;

	TVector2d getMinMax() const
	{
		dbPRINT( "CMatrix::getMinMax()\n" );

		if (!Data)
			return TVector2d( 0.0f, 0.0f );

		TVector2d result( FLT_MAX, FLT_MIN );

		for (u32 y=0; y<Rows; y++)
		{
			for (u32 x=0; x<Cols; x++)
			{
				const ElementType& value = Data[y][x];
				if ( result.X > value ) result.X = value;
				if ( result.Y < value ) result.Y = value;
			}
		}

		dbPRINT( "min = %lf, max = %lf\n", (f64)result.X, (f64)result.Y );

		return result;
	}

	/// get access ( public ) to raw data
	ElementType** getData()
	{
		return Data;
	}

	/// get Rows & Cols
	core::dimension2du getDimension() const
	{
		return core::dimension2du( Rows, Cols );
	}

	/// get Number of rows i have (y-dir)
	u32 getRows() const
	{
		return Rows;
	}

	/// get Number of columns i have (x-dir)
	u32 getCols() const
	{
		return Cols;
	}

	// /// get Product of Rows and Colums for linear memory access, its elementcount not the bytesize of allocated mem
	//u32 getSize() const { return Size; }

	/// get my Name
	core::stringc getName() const
	{
		return Name;
	}

	/// get my Name
	void setName( const core::stringc& name = "CMatrix" )
	{
		Name = name;
	}

	/// print CMatrix to String
	core::stringc toString() const
	{
		core::stringc s("CMatrix<ElementType>("); s+=Rows; s+=","; s+=Cols; s+=",";

		if (Name.size()>0)
		{
			s+=Name;	s+=",";
		}
		s+=") = { \n";

		if (Data)
		{
			for (u32 y=0; y<Rows; y++)
			{
				s+="\t{\t";
				for (u32 x=0; x<Cols; x++)
				{
					s+= core::floor32( (ElementType)Data[y][x] );
					if (x<Cols-1)
					{
						s+=" ";
					}
				}
				s+="\t}";
				if (y<Rows-1)
				{
					s+=",";
				}
				s+="\n";
			}
		}
		s+="};\n";

		return s;
	}

	/// secure access to value ( with out-of-bounds check )
	ElementType getElement(u32 row, u32 col) const
	{
		_IRR_DEBUG_BREAK_IF( row >= Rows );
		_IRR_DEBUG_BREAK_IF( col >= Cols );
		if ((row<Rows) && (col<Cols))
		{
			return Data[row][col];
		}
		else
		{
			return 0.0f;
		}
	}

	/// secure access to value ( with out-of-bounds check )
	ElementType getElement(u32 index) const
	{
		_IRR_DEBUG_BREAK_IF( index >= Size );
		u32 row = index / Cols;
		u32 col = index - row * Cols;
		if ((row>=0) && (col>=0) && (row<Rows) && (col<Cols))
		{
			return Data[row][col];
		}
		else
		{
			return 0.0f;
		}
	}

	/// secure access to value ( with out-of-bounds check )
	bool setElement(u32 row, u32 col, ElementType element)
	{
		_IRR_DEBUG_BREAK_IF( row < 0 );
		_IRR_DEBUG_BREAK_IF( col < 0 );
		_IRR_DEBUG_BREAK_IF( row >= Rows );
		_IRR_DEBUG_BREAK_IF( col >= Cols );
		if ((row<Rows) && (col<Cols))
		{
			Data[row][col] = element;
			return true;
		}

		return false;
	}

	/// secure access to value ( with out-of-bounds check )
	bool setElement(u32 index, ElementType element)
	{
		_IRR_DEBUG_BREAK_IF( index<0 );
		_IRR_DEBUG_BREAK_IF( index>= this->getSize() );
		u32 row = index / Cols;
		u32 col = index - row * Cols;
		if ((row>=0) && (col>=0) && (row<Rows) && (col<Cols))
		{
			Data[row][col] = element;
			return true;
		}

		return false;
	}


	bool swapRows( u32 row_a, u32 row_b )
	{
		dbPRINT( "CMatrix::swapRows()\n");

		if ( row_a == row_b )
			return false;

		if ( row_a >= Rows )
			return false;

		if ( row_b >= Rows )
			return false;

		/// save value at target position
		ElementType* row = Data[row_a];

		/// overwrite target position with new value
		Data[row_a] = Data[row_b];

		/// overwrite source position with save row-data
		Data[row_b] = row;

		return true;
	}

	bool shiftRow()
	{
//	    dbPRINT( "CMatrix::shiftRow()\n" );

		u32 r = 1;
		ElementType** b = new ElementType*[Rows];

		if (!b)
			return false;

		u32 k = 0;
		for (u32 i = r; i<Rows; i++)
		{
			b[i] = Data[k];
			k++;
		}

		k = 0;
		for (u32 i = Rows-r; i<Rows; i++)
		{
			b[k] = Data[i];
			k++;
		}

		for (u32 i = 0; i<Rows; i++)
		{
			Data[i] = b[i];
		}

		delete [] b;

		return true;
	}


	bool shiftRows( s32 rows )
	{
//		dbPRINT( "CMatrix::shiftRows( %d )\n", rows);

		if (rows>0)
		{
			for (u32 y=0; y<Rows; y++)
			{
				s32 i = ( rows+(s32)y );

				if (i<0) i += Rows;
				if (i>=(s32)Rows) i -= (s32)Rows;
				//%((s32)Rows);
				u32 k = (u32)i;
	//            k = Rows-1-k;
	//            k = k % Rows;

				/// save value at target position
				ElementType* row = Data[y];

				/// overwrite target position with new value
				Data[y] = Data[k];

				/// overwrite source position with save row-data
				Data[k] = row;
			}
		}
		else
		{
			rows = core::abs_<s32>(rows);

			for (u32 y=0; y<Rows; y++)
			{
				u32 k = ( (u32)rows+y )%Rows ;

				/// save value at target position
				ElementType* row = Data[y];

				/// overwrite target position with new value
				Data[y] = Data[k];

				/// overwrite source position with save row-data
				Data[k] = row;
			}

		}

		return true;
	}

	bool load( const core::stringc& filename )
	{
		dbPRINT( "CMatrix::load( %s )\n", filename.c_str() );
		return true;
	}

	bool save( const core::stringc& filename ) const
	{
		dbPRINT( "CMatrix::save( %s )\n", filename.c_str() );
		return true;
	}

	///// secure access to value ( with out-of-bounds check )
	//const ElementType& operator() (u32 index) const
	//{
	//    return Data[ (index<getSize())?index:0 ];
	//}
	//
	///// secure access to value ( with out-of-bounds check )
	//ElementType& operator() (u32 index)
	//{
	//    return Data[ (index<getSize())?index:0 ];
	//}
	//
	///// secure access to value ( with out-of-bounds check )
	//const ElementType& operator() (u32 row, u32 col) const
	//{
	//    u32 index = row*Cols+col;
	//    return Data[ (index<getSize())?index:0 ];
	//}
	//
	///// secure access to value ( with out-of-bounds check )
	//ElementType& operator() (u32 row, u32 col)
	//{
	//    u32 index = row*Cols+col;
	//    return Data[ (index<getSize())?index:0 ];
	//}

//
//	/// copy operator overload
//	CMatrix& operator= ( const CMatrix& other )
//	{
//		#ifdef _DEBUG
//		dbPRINT( "operator= ()\n" );
//		#endif // _DEBUG
//
//		return assign(other);
//	}
//
//
//

//
//		/// set row-data ( replace ) with array-values
//		template <class T>
//		bool setRow( u32 row, const T* data, u32 elem_count, ElementType fillSpace = 0.0f )
//		{
//			if (!data)
//			{
//				dbPRINT("fillRow() - ERROR Cant set row of empty CMatrix, return false.\n");
//				return false;
//			}
//
//			if (row >= Rows)
//				return false;
//
//			u32 i_max = core::min_<u32>( elem_count, Cols);
//
//			T* p = const_cast<T*>(data);
//
//			for (u32 i=0; i<i_max; i++)
//			{
//				if (p)
//				{
//					ElementType value = (ElementType)(*p);
//					Data[row][i] = value;
//					p++;
//				}
//				else
//				{
//					break;
//				}
//			}
//
//			if (i_max < Cols)
//			{
//				for (u32 i=i_max; i<Cols; i++)
//				{
//					Data[row][i] = fillSpace;
//				}
//			}
//
//			return true;
//		}
//
//		/// set row-data ( replace ) with array-values
//		template <class T>
//		bool setRow( u32 row, const core::array<T>& data, bool bFillBounds = false, ElementType fillSpace = 0.0f )
//		{
//			if (!Data)
//			{
//				dbPRINT("fillRow() - ERROR Cant set row of empty CMatrix, return false.\n");
//				return false;
//			}
//
//			if (row >= Rows)
//				return false;
//
//			u32 i_max = core::min_<u32>( data.size(), Cols);
//
//			for (u32 i=0; i<i_max; i++)
//			{
//				Data[row][i] = (ElementType)data[i];
//			}
//
//			if (bFillBounds)
//			{
//				if (i_max < Cols)
//				{
//					for (u32 i=i_max; i<Cols; i++)
//					{
//						Data[row][i] = fillSpace;
//					}
//				}
//			}
//			return true;
//		}
//
//
//
//	// Compare me to another CMatrix,
//	// test for equal row- and col-count first,
//	// if true, then check element-wise for equality until false
//	bool operator==(const CMatrix& other)
//	{
//		dbPRINT( "CMatrix::operator== ()\n");
//
//		if (*this == other)
//			return false;   // !dangerous! if somebody turns this equality to true
//
//		const u32 r0 = getRows();
//		const u32 c0 = getCols();
//		const u32 r1 = other.getRows();
//		const u32 c1 = other.getCols();
//
//		// test equality for number of rows and columns first
//		if (r0!=r1) return false;
//		if (c0!=c1)	return false;
//
//		// then test for equality element-wise
//		for (u32 m=0; m<r0; m++)
//		{
//			for (u32 n=0; n<c0; n++)
//			{
//				if ( !core::equals( Data[m][n], other.getElement(m,n) ) )
//					return false;
//			}
//		}
//
//		// if really all same return true,
//		return true;
//	}
//
//	/// inequality operator
//	bool operator!=(const CMatrix& other)
//	{
//		return ( *this == other );
//	}
//
//
//	/// translation operator '+'
//	CMatrix& operator+ ( const ElementType& value )
//	{
//		for (u32 y=0; y<Rows; y++)
//		{
//			for (u32 x=0; x<Cols; x++)
//			{
//				Data[y][x] = Data[y][x] + value;
//			}
//		}
//
//		return *this;
//	}
//
//	/// '+' operator overload
//	CMatrix& operator+= ( const CMatrix& other )
//	{
//		if ( *this == other )
//			return *this;
//
//		for (u32 y=0; y<Rows; y++)
//		{
//			for (u32 x=0; x<Cols; x++)
//			{
//				Data[y][x] = Data[y][x] + other.getElement(y,x);
//			}
//		}
//
//		return *this;
//	}
//
//	/// translation operator '-'
//	CMatrix& operator- ( const ElementType& value )
//	{
//		for (u32 y=0; y<Rows; y++)
//		{
//			for (u32 x=0; x<Cols; x++)
//			{
//				Data[y][x] = Data[y][x] - value;
//			}
//		}
//		return *this;
//	}
//
//	/// '-' operator overload
//	CMatrix& operator-= ( const CMatrix& other )
//	{
//		if ( *this == other )
//			return *this;
//
//		for (u32 y=0; y<Rows; y++)
//		{
//			for (u32 x=0; x<Cols; x++)
//			{
//				Data[y][x] = Data[y][x] - other.getElement(y,x);
//			}
//		}
//
//		return *this;
//	}
//
//	/// scale operator '*'
//	CMatrix& operator* ( const ElementType& value )
//	{
//		for (u32 y=0; y<Rows; y++)
//		{
//			for (u32 x=0; x<Cols; x++)
//			{
//				Data[y][x] = Data[y][x] *value;
//			}
//		}
//		return *this;
//	}
//
//	/// scale operator '/'
//	CMatrix& operator/ ( const ElementType& value )
//	{
//		if (!core::equals( value, NullValue ))
//		{
//			const ElementType value_inv_factor = core::reciprocal( value );
//
//			for (u32 y=0; y<Rows; y++)
//			{
//				for (u32 x=0; x<Cols; x++)
//				{
//					Data[y][x] = Data[y][x] * value_inv_factor;
//				}
//			}
//		}
//
//		return *this;
//	}
//
//	/// added 11.09.2013
//	/// for use with audio-animator
//	/// @remarks should be slow, maybe this solution is insufficient
//	/// @todo compare speed with array of arrays
//	/// since there, only pointers have to be swapped/exchanged, and no data
//
//	video::IImage* createImage( ) const
//	{
//		dbPRINT( "CMatrix::createImage()\n" );
//
//		core::dimension2du img_size( Cols, Rows );
//		video::CImage* img = new video::CImage( video::ECF_A8R8G8B8, img_size);
//		if (!img)
//			return 0;
//
//		img->fill( 0xffffffff );
//
//		const core::vector2df mm = getMinMax();
//		const ElementType height = mm.Y - mm.X;
//
//		for (u32 y = 0; y < core::min_<u32>(Rows, img->getDimension().Height); y++)
//		{
//			for (u32 x = 0; x < core::min_<u32>(Cols, img->getDimension().Width); x++)
//			{
//				ElementType value = Data[y][x];
//				value -= mm.X;
//				value /= height;
//				value = core::clamp<ElementType>( value, 0.0f, 1.0f );
//				video::SColorf color( value, value, value, 1.0f );
//				img->setPixel( x, y, color.toSColor() );
//			}
//		}
//
//		return img;
//	}

	//video::IImage* createHeightMap( ) const
	//{
	//    #ifdef _DEBUG
	//    dbPRINT( "createHeightMap()\n" );
	//    #endif // _DEBUG
	//
	//    core::dimension2du img_size( Cols, Rows );
	//    video::CImage* img = new video::CImage( video::ECF_A8R8G8B8, img_size);
	//    if (!img)
	//        return 0;
	//
	//    img->fill( 0xffffffff );
	//
	//    const core::vector2df mm = getMinMax();
	//    const ElementType height = mm.Y - mm.X;
	//
	//    for (u32 y = 0; y < core::min_<u32>(Rows, img->getDimension().Height); y++)
	//    {
	//        for (u32 x = 0; x < core::min_<u32>(Cols, img->getDimension().Width); x++)
	//        {
	//            ElementType value = Data[y][x];
	//            value -= mm.X;
	//            value /= height;
	//            value = core::clamp<ElementType>( value, 0.0f, 1.0f );
	//            video::SColorf color( value, value, value, 1.0f );
	//            img->setPixel( x, y, color.toSColor() );
	//        }
	//    }
	//
	//    return img;
	//}
//
//	video::ITexture* createTexture( video::IVideoDriver* driver ) const
//	{
//		dbPRINT( "CMatrix::createTexture()\n" );
//
//		if (!driver)
//			return 0;
//
//		video::IImage* img = createImage();
//
//		video::ITexture* tex = driver->addTexture( "createTexture", img, 0 );
//
//		return tex;
//	}

	//
	//		virtual ElementType det() const;
	//		virtual ElementType subMatrix( u32 y, u32 x ) const;
	//		virtual ElementType subDet( u32 y, u32 x ) const;
//
//	void fillRandom(ElementType minRandom, ElementType maxRandom)
//	{
//		dbPRINT( "CMatrix::fillRandom()\n");
//
//		const s32 diff_ = core::round32( core::abs_<s32>( maxRandom - minRandom ) );
//
//		for (u32 y=0; y<Rows; y++)
//		{
//			for (u32 x=0; x<Cols; x++)
//			{
//				s32 random_i_ = rand()%diff_;
//
//				ElementType random_f_ = minRandom + (ElementType)random_i_;
//
//				Data[y][x] = random_f_;
//			}
//		}
//	}
//
//	void fillRandomNormalized()
//	{
//		dbPRINT( "CMatrix::fillRandomNormalized()\n");
//
//		for (u32 y=0; y<Rows; y++)
//		{
//			for (u32 x=0; x<Cols; x++)
//			{
//				ElementType value= core::reciprocal( (ElementType)(1+rand()) );
//				setElement(y,x,value);
//			}
//		}
//	}


};

typedef CMatrix<f32> CMatrixf;

} // end namespace core
} // end namespace irr

#endif // MATRIX_H
