/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/



// Save NDEBUG state
#ifdef NDEBUG
#define STREAMHELPER_HXX_HAD_NDEBUG
#undef NDEBUG
#endif

#if OSL_DEBUG_LEVEL == 0
#define NDEBUG
#endif
#include <assert.h>

#define Max( a, b )     (((a)>(b)) ? (a) : (b) )
#define Min( a, b )		(((a)<(b)) ? (a) : (b) )

namespace io_stm {

class IFIFO_OutOfBoundsException :
				public Exception
{};

class IFIFO_OutOfMemoryException :
				public Exception
{};

class IFIFO
{
public:


	virtual void 	write( const Sequence<sal_Int8> &)=0;

	virtual void 	read( Sequence<sal_Int8> & , sal_Int32 nBytesToRead )=0;
	virtual void 	skip( sal_Int32 nBytesToSkip )=0;
	virtual sal_Int32 	getSize() const throw(  )  =0;
	virtual void 	shrink() throw() = 0;

	virtual ~IFIFO() {};
};


class IRingBuffer_OutOfBoundsException :
				public Exception
{};

class IRingBuffer_OutOfMemoryException :
				public Exception
{};

class IRingBuffer
{
public:
	/***
	* overwrites data at given position. Size is automatically extended, when
	* data is written beyond end.
	*
	***/

	virtual void 	writeAt( sal_Int32 nPos, const Sequence<sal_Int8> &)=0;
	virtual void 	readAt( sal_Int32 nPos, Sequence<sal_Int8> & , sal_Int32 nBytesToRead ) const=0;
	virtual sal_Int32 	getSize() const throw(  )  =0;
	virtual void 	forgetFromStart( sal_Int32 nBytesToForget )=0;
	virtual void	forgetFromEnd( sal_Int32 nBytesToForget )=0;
	virtual void 	shrink() throw() = 0;
	virtual ~IRingBuffer() {};
};


class MemRingBuffer :
		public IRingBuffer
{
public:
	MemRingBuffer();
	virtual ~MemRingBuffer();

	virtual void 	writeAt( sal_Int32 nPos, const Sequence<sal_Int8> &);
	virtual void 	readAt( sal_Int32 nPos, Sequence<sal_Int8> & , sal_Int32 nBytesToRead ) const;
	virtual sal_Int32 	getSize() const throw(  );
	virtual void 	forgetFromStart( sal_Int32 nBytesToForget );
	virtual void	forgetFromEnd( sal_Int32 nBytesToForget );

	virtual void shrink() throw();

private:

	void resizeBuffer( sal_Int32 nMinSize );
	inline void checkInvariants()
	{
		assert( m_nBufferLen >= 0 );
		assert( m_nOccupiedBuffer >= 0 );
		assert( m_nOccupiedBuffer <= m_nBufferLen );
		assert( m_nStart >= 0 );
		assert( 0 == m_nStart || m_nStart < m_nBufferLen );
	}

	sal_Int8 	*m_p;
	sal_Int32 	m_nBufferLen;
	sal_Int32 	m_nStart;
	sal_Int32 	m_nOccupiedBuffer;
};


class MemFIFO :
		public  IFIFO,
		private MemRingBuffer
{
public:
	virtual void 	write( const Sequence<sal_Int8> &);
	virtual void 	read( Sequence<sal_Int8> & , sal_Int32 nBytesToRead );
	virtual void 	skip( sal_Int32 nBytesToSkip );
	virtual sal_Int32 	getSize()  const throw(  )
						{ return MemRingBuffer::getSize(); }
	virtual void 	shrink() throw()
						{ MemRingBuffer::shrink(); }

};

// Restore NDEBUG state
#ifdef STREAMHELPER_HXX_HAD_NDEBUG
#define NDEBUG
#else
#undef NDEBUG
#endif

}
