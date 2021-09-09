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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"

#include "osl/file.hxx"

#include "osl/diagnose.h"
#include "rtl/alloc.h"

#include "system.h"
#include "file_error_transl.h"
#include "file_url.h"

#include <algorithm>
#include <limits>

#include <string.h>
#include <pthread.h>
#include <sys/mman.h>

#if defined(MACOSX)

#include <sys/param.h>
#include <sys/mount.h>
#define HAVE_O_EXLOCK

// add MACOSX Time Value
#define TimeValue CFTimeValue
#include <CoreFoundation/CoreFoundation.h>
#undef TimeValue

#endif /* MACOSX */

#ifdef DEBUG_OSL_FILE
#	define OSL_FILE_TRACE 0 ? (void)(0) : osl_trace
#	define PERROR( a, b ) perror( a ); fprintf( stderr, b )
#else
#	define OSL_FILE_TRACE 1 ? (void)(0) : osl_trace
#	define PERROR( a, b )
#endif

/*******************************************************************
 *
 * FileHandle_Impl interface
 *
 ******************************************************************/
struct FileHandle_Impl
{
	pthread_mutex_t m_mutex;
	rtl_String *    m_strFilePath; /* holds native file path */
	int             m_fd;

	/** State
	*/
	enum StateBits
	{
		STATE_SEEKABLE  = 1, /* default */
		STATE_READABLE  = 2, /* default */
		STATE_WRITEABLE = 4, /* open() sets, write() requires, else osl_File_E_BADF */
		STATE_MODIFIED  = 8  /* write() sets, flush() resets */
	};
	int          m_state;

	sal_uInt64   m_size;    /* file size */
	off_t        m_offset;  /* physical offset from begin of file */
	off_t        m_fileptr; /* logical offset from begin of file */

	off_t        m_bufptr;  /* buffer offset from begin of file */
	size_t       m_buflen;  /* buffer filled [0, m_bufsiz - 1] */

	size_t       m_bufsiz;
	sal_uInt8 *  m_buffer;

	explicit FileHandle_Impl (int fd, char const * path = "<anon>");
	~FileHandle_Impl();

	static void* operator new (size_t n);
	static void  operator delete (void * p, size_t);

	static size_t getpagesize();

	sal_uInt64   getPos() const;
	oslFileError setPos (sal_uInt64 uPos);

	sal_uInt64   getSize() const;
	oslFileError setSize (sal_uInt64 uSize);

	oslFileError readAt (
		off_t        nOffset,
		void *       pBuffer,
		size_t       nBytesRequested,
		sal_uInt64 * pBytesRead);

	oslFileError writeAt (
		off_t        nOffset,
		void const * pBuffer,
		size_t       nBytesToWrite,
		sal_uInt64 * pBytesWritten);

	oslFileError readFileAt (
		off_t        nOffset,
		void *       pBuffer,
		size_t       nBytesRequested,
		sal_uInt64 * pBytesRead);

	oslFileError writeFileAt (
		off_t        nOffset,
		void const * pBuffer,
		size_t       nBytesToWrite,
		sal_uInt64 * pBytesWritten);

	oslFileError readLineAt (
		off_t           nOffset,
		sal_Sequence ** ppSequence,
		sal_uInt64 *    pBytesRead);

	oslFileError writeSequence_Impl (
		sal_Sequence ** ppSequence,
		size_t *        pnOffset,
		const void *    pBuffer,
		size_t          nBytes);

	oslFileError syncFile();

	/** Buffer cache / allocator.
	*/
	class Allocator
	{
		rtl_cache_type * m_cache;
		size_t           m_bufsiz;

		Allocator (Allocator const &);
		Allocator & operator= (Allocator const &);

	public:
		static Allocator & get();

		void allocate (sal_uInt8 ** ppBuffer, size_t * pnSize);
		void deallocate (sal_uInt8 * pBuffer);

	protected:
		Allocator();
		~Allocator();
	};

	/** Guard.
	*/
	class Guard
	{
		pthread_mutex_t * m_mutex;

	public:
		explicit Guard(pthread_mutex_t * pMutex);
		~Guard();
	};
};

/*******************************************************************
 * FileHandle_Impl implementation
 ******************************************************************/

FileHandle_Impl::Allocator &
FileHandle_Impl::Allocator::get()
{
	static Allocator g_aBufferAllocator;
	return g_aBufferAllocator;
}

FileHandle_Impl::Allocator::Allocator()
	: m_cache  (0),
	  m_bufsiz (0)
{
	size_t const pagesize = FileHandle_Impl::getpagesize();
	if (size_t(-1) != pagesize)
	{
		m_cache = rtl_cache_create (
			"osl_file_buffer_cache", pagesize, 0, 0, 0, 0, 0, 0, 0);
		if (0 != m_cache)
			m_bufsiz = pagesize;
	}
}
FileHandle_Impl::Allocator::~Allocator()
{
	rtl_cache_destroy (m_cache), m_cache = 0;
}

void FileHandle_Impl::Allocator::allocate (sal_uInt8 ** ppBuffer, size_t * pnSize)
{
	OSL_PRECOND((0 != ppBuffer) && (0 != pnSize), "FileHandle_Impl::Allocator::allocate(): contract violation");
	if ((0 != ppBuffer) && (0 != pnSize))
		 *ppBuffer = static_cast< sal_uInt8* >(rtl_cache_alloc(m_cache)), *pnSize = m_bufsiz;
}
void FileHandle_Impl::Allocator::deallocate (sal_uInt8 * pBuffer)
{
	if (0 != pBuffer)
		rtl_cache_free (m_cache, pBuffer);
}

FileHandle_Impl::Guard::Guard(pthread_mutex_t * pMutex)
	: m_mutex (pMutex)
{
	OSL_PRECOND (m_mutex != 0, "FileHandle_Impl::Guard::Guard(): null pointer.");
	(void) pthread_mutex_lock (m_mutex); // ignoring EINVAL ...
}
FileHandle_Impl::Guard::~Guard()
{
	OSL_PRECOND (m_mutex != 0, "FileHandle_Impl::Guard::~Guard(): null pointer.");
	(void) pthread_mutex_unlock (m_mutex);
}

FileHandle_Impl::FileHandle_Impl (int fd, char const * path)
	: m_strFilePath (0),
	  m_fd      (fd),
	  m_state   (STATE_SEEKABLE | STATE_READABLE),
	  m_size    (0),
	  m_offset  (0),
	  m_fileptr (0),
	  m_bufptr  (-1),
	  m_buflen  (0),
	  m_bufsiz  (0),
	  m_buffer  (0)
{
	(void) pthread_mutex_init(&m_mutex, 0);
	rtl_string_newFromStr (&m_strFilePath, path);
	Allocator::get().allocate (&m_buffer, &m_bufsiz);
	if (0 != m_buffer)
		memset (m_buffer, 0, m_bufsiz);
}
FileHandle_Impl::~FileHandle_Impl()
{
	Allocator::get().deallocate (m_buffer), m_buffer = 0;
	rtl_string_release (m_strFilePath), m_strFilePath = 0;
	(void) pthread_mutex_destroy(&m_mutex); // ignoring EBUSY ...
}

void* FileHandle_Impl::operator new (size_t n)
{
	return rtl_allocateMemory(n);
}
void FileHandle_Impl::operator delete (void * p, size_t)
{
	rtl_freeMemory(p);
}

size_t FileHandle_Impl::getpagesize()
{
#if defined(FREEBSD) || defined(NETBSD) || defined(MACOSX)
	return sal::static_int_cast< size_t >(::getpagesize());
#else /* POSIX */
	return sal::static_int_cast< size_t >(::sysconf(_SC_PAGESIZE));
#endif /* xBSD || POSIX */
}

sal_uInt64 FileHandle_Impl::getPos() const
{
	return sal::static_int_cast< sal_uInt64 >(m_fileptr);
}

oslFileError FileHandle_Impl::setPos (sal_uInt64 uPos)
{
	OSL_FILE_TRACE("FileHandle_Impl::setPos(%d, %lld) => %lld", m_fd, getPos(), uPos);
	m_fileptr = sal::static_int_cast< off_t >(uPos);
	return osl_File_E_None;
}

sal_uInt64 FileHandle_Impl::getSize() const
{
	off_t const bufend = std::max((off_t)(0), m_bufptr) + m_buflen;
	return std::max(m_size, sal::static_int_cast< sal_uInt64 >(bufend));
}

oslFileError FileHandle_Impl::setSize (sal_uInt64 uSize)
{
	off_t const nSize = sal::static_int_cast< off_t >(uSize);
	if (-1 == ftruncate (m_fd, nSize))
	{
		/* Failure. Save original result. Try fallback algorithm */
		oslFileError result = oslTranslateFileError (OSL_FET_ERROR, errno);

		/* Check against current size. Fail upon 'shrink' */
		if (uSize <= getSize())
		{
			/* Failure upon 'shrink'. Return original result */
			return (result);
		}

		/* Save current position */
		off_t const nCurPos = (off_t)lseek (m_fd, (off_t)0, SEEK_CUR);
		if (nCurPos == (off_t)(-1))
			return (result);

		/* Try 'expand' via 'lseek()' and 'write()' */
		if (-1 == lseek (m_fd, (off_t)(nSize - 1), SEEK_SET))
			return (result);

		if (-1 == write (m_fd, (char*)"", (size_t)1))
		{
			/* Failure. Restore saved position */
			(void) lseek (m_fd, (off_t)(nCurPos), SEEK_SET);
			return (result);
		}

		/* Success. Restore saved position */
		if (-1 == lseek (m_fd, (off_t)nCurPos, SEEK_SET))
			return (result);
	}

	OSL_FILE_TRACE("osl_setFileSize(%d, %lld) => %ld", m_fd, getSize(), nSize);
	m_size = sal::static_int_cast< sal_uInt64 >(nSize);
	return osl_File_E_None;
}

oslFileError FileHandle_Impl::readAt (
	off_t        nOffset,
	void *       pBuffer,
	size_t       nBytesRequested,
	sal_uInt64 * pBytesRead)
{
	OSL_PRECOND((m_state & STATE_SEEKABLE), "FileHandle_Impl::readAt(): not seekable");
	if (!(m_state & STATE_SEEKABLE))
		return osl_File_E_SPIPE;

	OSL_PRECOND((m_state & STATE_READABLE), "FileHandle_Impl::readAt(): not readable");
	if (!(m_state & STATE_READABLE))
		return osl_File_E_BADF;

#if defined(LINUX) || defined(SOLARIS) || defined(FREEBSD) || defined(MACOSX)

	ssize_t nBytes = ::pread (m_fd, pBuffer, nBytesRequested, nOffset);
	if ((-1 == nBytes) && (EOVERFLOW == errno))
	{
		/* Some 'pread()'s fail with EOVERFLOW when reading at (or past)
		 * end-of-file, different from 'lseek() + read()' behavior.
		 * Returning '0 bytes read' and 'osl_File_E_None' instead.
		 */
		nBytes = 0;
	}
	if (-1 == nBytes)
		return oslTranslateFileError (OSL_FET_ERROR, errno);

#else /* no pread(2) ! */

	if (nOffset != m_offset)
	{
		if (-1 == ::lseek (m_fd, nOffset, SEEK_SET))
			return oslTranslateFileError (OSL_FET_ERROR, errno);
		m_offset = nOffset;
	}

	ssize_t nBytes = ::read (m_fd, pBuffer, nBytesRequested);
	if (-1 == nBytes)
		return oslTranslateFileError (OSL_FET_ERROR, errno);
	m_offset += nBytes;

#endif /* no pread(2) ! */

	OSL_FILE_TRACE("FileHandle_Impl::readAt(%d, %lld, %ld)", m_fd, nOffset, nBytes);
	*pBytesRead = nBytes;
	return osl_File_E_None;
}

oslFileError FileHandle_Impl::writeAt (
	off_t        nOffset,
	void const * pBuffer,
	size_t       nBytesToWrite,
	sal_uInt64 * pBytesWritten)
{
	OSL_PRECOND((m_state & STATE_SEEKABLE), "FileHandle_Impl::writeAt(): not seekable");
	if (!(m_state & STATE_SEEKABLE))
		return osl_File_E_SPIPE;

	OSL_PRECOND((m_state & STATE_WRITEABLE), "FileHandle_Impl::writeAt(): not writeable");
	if (!(m_state & STATE_WRITEABLE))
		return osl_File_E_BADF;

#if defined(LINUX) || defined(SOLARIS) || defined(FREEBSD) || defined(MACOSX)

	ssize_t nBytes = ::pwrite (m_fd, pBuffer, nBytesToWrite, nOffset);
	if (-1 == nBytes)
		return oslTranslateFileError (OSL_FET_ERROR, errno);

#else /* no pwrite(2) ! */

	if (nOffset != m_offset)
	{
		if (-1 == ::lseek (m_fd, nOffset, SEEK_SET))
			return oslTranslateFileError (OSL_FET_ERROR, errno);
		m_offset = nOffset;
	}

	ssize_t nBytes = ::write (m_fd, pBuffer, nBytesToWrite);
	if (-1 == nBytes)
		return oslTranslateFileError (OSL_FET_ERROR, errno);
	m_offset += nBytes;

#endif /* no pwrite(2) ! */

	OSL_FILE_TRACE("FileHandle_Impl::writeAt(%d, %lld, %ld)", m_fd, nOffset, nBytes);
	m_size = std::max (m_size, sal::static_int_cast< sal_uInt64 >(nOffset + nBytes));

	*pBytesWritten = nBytes;
	return osl_File_E_None;
}

oslFileError FileHandle_Impl::readFileAt (
	off_t        nOffset,
	void *       pBuffer,
	size_t       nBytesRequested,
	sal_uInt64 * pBytesRead)
{
	if (0 == (m_state & STATE_SEEKABLE))
	{
		// not seekable (pipe)
		ssize_t nBytes = ::read (m_fd, pBuffer, nBytesRequested);
		if (-1 == nBytes)
			return oslTranslateFileError (OSL_FET_ERROR, errno);
		*pBytesRead = nBytes;
		return osl_File_E_None;
	}
	else if (0 == m_buffer)
	{
		// not buffered
		return readAt (nOffset, pBuffer, nBytesRequested, pBytesRead);
	}
	else
	{
		sal_uInt8 * buffer = static_cast<sal_uInt8*>(pBuffer);
		for (*pBytesRead = 0; nBytesRequested > 0; )
		{
			off_t  const bufptr = (nOffset / m_bufsiz) * m_bufsiz;
			size_t const bufpos = (nOffset % m_bufsiz);

			if (bufptr != m_bufptr)
			{
				// flush current buffer
				oslFileError result = syncFile();
				if (result != osl_File_E_None)
					return (result);
				m_bufptr = -1, m_buflen = 0;

				if (nBytesRequested >= m_bufsiz)
				{
					// buffer too small, read through from file
					sal_uInt64 uDone = 0;
					result = readAt (nOffset, &(buffer[*pBytesRead]), nBytesRequested, &uDone);
					if (result != osl_File_E_None)
						return (result);

					nBytesRequested -= uDone, *pBytesRead += uDone;
					return osl_File_E_None;
				}

				// update buffer (pointer)
				sal_uInt64 uDone = 0;
				result = readAt (bufptr, m_buffer, m_bufsiz, &uDone);
				if (result != osl_File_E_None)
					return (result);
				m_bufptr = bufptr, m_buflen = uDone;
			}
			if (bufpos >= m_buflen)
			{
				// end of file
				return osl_File_E_None;
			}

			size_t const bytes = std::min (m_buflen - bufpos, nBytesRequested);
			OSL_FILE_TRACE("FileHandle_Impl::readFileAt(%d, %lld, %ld)", m_fd, nOffset, bytes);

			memcpy (&(buffer[*pBytesRead]), &(m_buffer[bufpos]), bytes);
			nBytesRequested -= bytes, *pBytesRead += bytes, nOffset += bytes;
		}
		return osl_File_E_None;
	}
}

oslFileError FileHandle_Impl::writeFileAt (
	off_t        nOffset,
	void const * pBuffer,
	size_t       nBytesToWrite,
	sal_uInt64 * pBytesWritten)
{
	if (0 == (m_state & STATE_SEEKABLE))
	{
		// not seekable (pipe)
		ssize_t nBytes = ::write (m_fd, pBuffer, nBytesToWrite);
		if (-1 == nBytes)
			return oslTranslateFileError (OSL_FET_ERROR, errno);
		*pBytesWritten = nBytes;
		return osl_File_E_None;
	}
	else if (0 == m_buffer)
	{
		// not buffered
		return writeAt (nOffset, pBuffer, nBytesToWrite, pBytesWritten);
	}
	else
	{
		sal_uInt8 const * buffer = static_cast<sal_uInt8 const *>(pBuffer);
		for (*pBytesWritten = 0; nBytesToWrite > 0; )
		{
			off_t  const bufptr = (nOffset / m_bufsiz) * m_bufsiz;
			size_t const bufpos = (nOffset % m_bufsiz);
			if (bufptr != m_bufptr)
			{
				// flush current buffer
				oslFileError result = syncFile();
				if (result != osl_File_E_None)
					return (result);
				m_bufptr = -1, m_buflen = 0;

				if (nBytesToWrite >= m_bufsiz)
				{
					// buffer to small, write through to file
					sal_uInt64 uDone = 0;
					result = writeAt (nOffset, &(buffer[*pBytesWritten]), nBytesToWrite, &uDone);
					if (result != osl_File_E_None)
						return (result);
					if (uDone != nBytesToWrite)
						return osl_File_E_IO;

					nBytesToWrite -= uDone, *pBytesWritten += uDone;
					return osl_File_E_None;
				}

				// update buffer (pointer)
				sal_uInt64 uDone = 0;
				result = readAt (bufptr, m_buffer, m_bufsiz, &uDone);
				if (result != osl_File_E_None)
					return (result);
				m_bufptr = bufptr, m_buflen = uDone;
			}

			size_t const bytes = std::min (m_bufsiz - bufpos, nBytesToWrite);
			OSL_FILE_TRACE("FileHandle_Impl::writeFileAt(%d, %lld, %ld)", m_fd, nOffset, bytes);

			memcpy (&(m_buffer[bufpos]), &(buffer[*pBytesWritten]), bytes);
			nBytesToWrite -= bytes, *pBytesWritten += bytes, nOffset += bytes;

			m_buflen = std::max(m_buflen, bufpos + bytes);
			m_state |= STATE_MODIFIED;
		}
		return osl_File_E_None;
	}
}

oslFileError FileHandle_Impl::readLineAt (
	off_t           nOffset,
	sal_Sequence ** ppSequence,
	sal_uInt64 *    pBytesRead)
{
	oslFileError result = osl_File_E_None;

	off_t bufptr = nOffset / m_bufsiz * m_bufsiz;
	if (bufptr != m_bufptr)
	{
		/* flush current buffer */
		result = syncFile();
		if (result != osl_File_E_None)
			return (result);

		/* update buffer (pointer) */
		sal_uInt64 uDone = 0;
		result = readAt (bufptr, m_buffer, m_bufsiz, &uDone);
		if (result != osl_File_E_None)
			return (result);

		m_bufptr = bufptr, m_buflen = uDone;
	}

	static int const LINE_STATE_BEGIN = 0;
	static int const LINE_STATE_CR    = 1;
	static int const LINE_STATE_LF    = 2;

	size_t bufpos = nOffset - m_bufptr, curpos = bufpos, dstpos = 0;
	int    state  = (bufpos >= m_buflen) ? LINE_STATE_LF : LINE_STATE_BEGIN;

	for ( ; state != LINE_STATE_LF; )
	{
		if (curpos >= m_buflen)
		{
			/* buffer examined */
			if (0 < (curpos - bufpos))
			{
				/* flush buffer to sequence */
				result = writeSequence_Impl (
					ppSequence, &dstpos, &(m_buffer[bufpos]), curpos - bufpos);
				if (result != osl_File_E_None)
					return (result);
				*pBytesRead += curpos - bufpos, nOffset += curpos - bufpos;
			}

			bufptr = nOffset / m_bufsiz * m_bufsiz;
			if (bufptr != m_bufptr)
			{
				/* update buffer (pointer) */
				sal_uInt64 uDone = 0;
				result = readAt (bufptr, m_buffer, m_bufsiz, &uDone);
				if (result != osl_File_E_None)
					return (result);
				m_bufptr = bufptr, m_buflen = uDone;
			}

			bufpos = nOffset - m_bufptr, curpos = bufpos;
			if (bufpos >= m_buflen)
				break;
		}
		switch (state)
		{
		case LINE_STATE_CR:
			state = LINE_STATE_LF;
			switch (m_buffer[curpos])
			{
			case 0x0A: /* CRLF */
				/* eat current char */
				curpos++;
				break;
			default: /* single CR */
				/* keep current char */
				break;
			}
			break;
		default:
			/* determine next state */
			switch (m_buffer[curpos])
			{
			case 0x0A: /* single LF */
				state = LINE_STATE_LF;
				break;
			case 0x0D: /* CR */
				state = LINE_STATE_CR;
				break;
			default: /* advance to next char */
				curpos++;
				break;
			}
			if (state != LINE_STATE_BEGIN)
			{
				/* store (and eat) the newline char */
				m_buffer[curpos] = 0x0A, curpos++;

				/* flush buffer to sequence */
				result = writeSequence_Impl (
					ppSequence, &dstpos, &(m_buffer[bufpos]), curpos - bufpos - 1);
				if (result != osl_File_E_None)
					return (result);
				*pBytesRead += curpos - bufpos, nOffset += curpos - bufpos;
			}
			break;
		}
	}

	result = writeSequence_Impl (ppSequence, &dstpos, 0, 0);
	if (result != osl_File_E_None)
		return (result);
	if (0 < dstpos)
		return osl_File_E_None;
	if (bufpos >= m_buflen)
		return osl_File_E_AGAIN;
	return osl_File_E_None;
}

oslFileError FileHandle_Impl::writeSequence_Impl (
	sal_Sequence ** ppSequence,
	size_t *        pnOffset,
	const void *    pBuffer,
	size_t          nBytes)
{
	sal_Int32 nElements = *pnOffset + nBytes;
	if (!*ppSequence)
	{
		/* construct sequence */
		rtl_byte_sequence_constructNoDefault(ppSequence, nElements);
	}
	else if (nElements != (*ppSequence)->nElements)
	{
		/* resize sequence */
		rtl_byte_sequence_realloc(ppSequence, nElements);
	}
	if (*ppSequence != 0)
	{
		/* fill sequence */
		memcpy(&((*ppSequence)->elements[*pnOffset]), pBuffer, nBytes), *pnOffset += nBytes;
	}
	return (*ppSequence != 0) ? osl_File_E_None : osl_File_E_NOMEM;
}

oslFileError FileHandle_Impl::syncFile()
{
	oslFileError result = osl_File_E_None;
	if (m_state & STATE_MODIFIED)
	{
		sal_uInt64 uDone = 0;
		result = writeAt (m_bufptr, m_buffer, m_buflen, &uDone);
		if (result != osl_File_E_None)
			return (result);
		if (uDone != m_buflen)
			return osl_File_E_IO;
		m_state &= ~STATE_MODIFIED;
	}
	return (result);
}

/****************************************************************************
 * osl_createFileHandleFromFD
 ***************************************************************************/
extern "C" oslFileHandle osl_createFileHandleFromFD( int fd )
{
	if (-1 == fd)
		return 0; // EINVAL

	struct stat aFileStat;
	if (-1 == fstat (fd, &aFileStat))
		return 0; // EBADF

	FileHandle_Impl * pImpl = new FileHandle_Impl (fd);
	if (0 == pImpl)
		return 0; // ENOMEM

	// assume writeable
	pImpl->m_state |= FileHandle_Impl::STATE_WRITEABLE;
	if (!S_ISREG(aFileStat.st_mode))
	{
		/* not a regular file, mark not seekable */
		pImpl->m_state &= ~FileHandle_Impl::STATE_SEEKABLE;
	}
	else
	{
		/* regular file, init current size */
		pImpl->m_size = sal::static_int_cast< sal_uInt64 >(aFileStat.st_size);
	}

	OSL_FILE_TRACE("osl_createFileHandleFromFD(%d, writeable) => %s",
					pImpl->m_fd, rtl_string_getStr(pImpl->m_strFilePath));
	return (oslFileHandle)(pImpl);
}

/*******************************************************************
 * osl_file_adjustLockFlags
 ******************************************************************/
static int osl_file_adjustLockFlags (const char * path, int flags)
{
#ifdef MACOSX
	/*
	 * The AFP implementation of MacOS X 10.4 treats O_EXLOCK in a way
	 * that makes it impossible for AOO to create a backup copy of the
	 * file it keeps opened. OTOH O_SHLOCK for AFP behaves as desired by
	 * the AOO file handling, so we need to check the path of the file
	 * for the filesystem name.
	 */
	struct statfs s;
	if( 0 <= statfs( path, &s ) )
	{
		if( 0 == strncmp("afpfs", s.f_fstypename, 5) )
		{
			flags &= ~O_EXLOCK;
			flags |=  O_SHLOCK;
		}
		else
		{
			/* Needed flags to allow opening a webdav file */
			flags &= ~(O_EXLOCK | O_SHLOCK | O_NONBLOCK);
		}
	}
#endif /* MACOSX */

	(void) path;
	return flags;
}

/****************************************************************************
 * osl_file_queryLocking
 ***************************************************************************/
struct Locking_Impl
{
	int m_enabled;
	Locking_Impl() : m_enabled(0)
	{
#ifndef HAVE_O_EXLOCK
		m_enabled = ((getenv("SAL_ENABLE_FILE_LOCKING") != 0) || (getenv("STAR_ENABLE_FILE_LOCKING") != 0));
#endif /* HAVE_O_EXLOCK */
	}
};
static int osl_file_queryLocking (sal_uInt32 uFlags)
{
	if (!(uFlags & osl_File_OpenFlag_NoLock))
	{
		if ((uFlags & osl_File_OpenFlag_Write) || (uFlags & osl_File_OpenFlag_Create))
		{
			static Locking_Impl g_locking;
			return (g_locking.m_enabled != 0);
		}
	}
	return 0;
}

/****************************************************************************
 * osl_openFile
 ***************************************************************************/
#ifdef HAVE_O_EXLOCK
#define OPEN_WRITE_FLAGS ( O_RDWR | O_EXLOCK | O_NONBLOCK )
#define OPEN_CREATE_FLAGS ( O_CREAT | O_EXCL | O_RDWR | O_EXLOCK | O_NONBLOCK )
#else
#define OPEN_WRITE_FLAGS ( O_RDWR )
#define OPEN_CREATE_FLAGS ( O_CREAT | O_EXCL | O_RDWR )
#endif

oslFileError
SAL_CALL osl_openFile( rtl_uString* ustrFileURL, oslFileHandle* pHandle, sal_uInt32 uFlags )
{
	oslFileError eRet;

	if ((ustrFileURL == 0) || (ustrFileURL->length == 0) || (pHandle == 0))
		return osl_File_E_INVAL;

	/* convert file URL to system path */
	char buffer[PATH_MAX];
	eRet = FileURLToPath (buffer, sizeof(buffer), ustrFileURL);
	if (eRet != osl_File_E_None)
		return eRet;
#ifdef MACOSX
	if (macxp_resolveAlias (buffer, sizeof(buffer)) != 0)
		return oslTranslateFileError (OSL_FET_ERROR, errno);
#endif /* MACOSX */

	/* set mode and flags */
	int mode  = S_IRUSR | S_IRGRP | S_IROTH;
	int flags = O_RDONLY;
	if (uFlags & osl_File_OpenFlag_Write)
	{
		mode |= S_IWUSR | S_IWGRP | S_IWOTH;
		flags = OPEN_WRITE_FLAGS;
	}
	if (uFlags & osl_File_OpenFlag_Create)
	{
		mode |= S_IWUSR | S_IWGRP | S_IWOTH;
		flags = OPEN_CREATE_FLAGS;
	}
	if (uFlags & osl_File_OpenFlag_NoLock)
	{
#ifdef HAVE_O_EXLOCK
		flags &= ~(O_EXLOCK | O_SHLOCK | O_NONBLOCK);
#endif /* HAVE_O_EXLOCK */
	}
	else
	{
		flags = osl_file_adjustLockFlags (buffer, flags);
	}

	/* open the file */
	int fd = open( buffer, flags, mode );
	if (-1 == fd)
		return oslTranslateFileError (OSL_FET_ERROR, errno);

	/* reset O_NONBLOCK flag */
	if (flags & O_NONBLOCK)
	{
		int f = fcntl (fd, F_GETFL, 0);
		if (-1 == f)
		{
			eRet = oslTranslateFileError (OSL_FET_ERROR, errno);
			(void) close(fd);
			return eRet;
		}
		if (-1 == fcntl (fd, F_SETFL, (f & ~O_NONBLOCK)))
		{
			eRet = oslTranslateFileError (OSL_FET_ERROR, errno);
			(void) close(fd);
			return eRet;
		}
	}

	/* get file status (mode, size) */
	struct stat aFileStat;
	if (-1 == fstat (fd, &aFileStat))
	{
		eRet = oslTranslateFileError (OSL_FET_ERROR, errno);
		(void) close(fd);
		return eRet;
	}
	if (!S_ISREG(aFileStat.st_mode))
	{
		/* we only open regular files here */
		(void) close(fd);
		return osl_File_E_INVAL;
	}

	if (osl_file_queryLocking (uFlags))
	{
#ifdef MACOSX
		if (-1 == flock (fd, LOCK_EX | LOCK_NB))
		{
			/* Mac OSX returns ENOTSUP for webdav drives. We should try read lock */
			if ((errno != ENOTSUP) || ((-1 == flock (fd, LOCK_SH | LOCK_NB)) && (errno != ENOTSUP)))
			{
				eRet = oslTranslateFileError (OSL_FET_ERROR, errno);
				(void) close(fd);
				return eRet;
			}
		}
#else /* F_SETLK */
		{
			struct flock aflock;

			aflock.l_type = F_WRLCK;
			aflock.l_whence = SEEK_SET;
			aflock.l_start = 0;
			aflock.l_len = 0;

			if (-1 == fcntl (fd, F_SETLK, &aflock))
			{
				eRet = oslTranslateFileError (OSL_FET_ERROR, errno);
				(void) close(fd);
				return eRet;
			}
		}
#endif /* F_SETLK */
	}

	/* allocate memory for impl structure */
	FileHandle_Impl * pImpl = new FileHandle_Impl (fd, buffer);
	if (!pImpl)
	{
		eRet = oslTranslateFileError (OSL_FET_ERROR, ENOMEM);
		(void) close(fd);
		return eRet;
	}
	if (flags & O_RDWR)
		pImpl->m_state |= FileHandle_Impl::STATE_WRITEABLE;
	pImpl->m_size = sal::static_int_cast< sal_uInt64 >(aFileStat.st_size);

	OSL_TRACE("osl_openFile(%d, %s) => %s", pImpl->m_fd,
			flags & O_RDWR ? "writeable":"readonly",
			rtl_string_getStr(pImpl->m_strFilePath));

	*pHandle = (oslFileHandle)(pImpl);
	return osl_File_E_None;
}

/****************************************************************************/
/* osl_closeFile */
/****************************************************************************/
oslFileError
SAL_CALL osl_closeFile( oslFileHandle Handle )
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((pImpl == 0) || (pImpl->m_fd < 0))
		return osl_File_E_INVAL;

	(void) pthread_mutex_lock (&(pImpl->m_mutex));

	/* close(2) implicitly (and unconditionally) unlocks */
	OSL_TRACE("osl_closeFile(%d) => %s", pImpl->m_fd, rtl_string_getStr(pImpl->m_strFilePath));
	oslFileError result = pImpl->syncFile();
	if (result != osl_File_E_None)
	{
		/* close, ignoring double failure */
		(void) close (pImpl->m_fd);
	}
	else if (-1 == close (pImpl->m_fd))
	{
		/* translate error code */
		result = oslTranslateFileError (OSL_FET_ERROR, errno);
	}

	(void) pthread_mutex_unlock (&(pImpl->m_mutex));
	delete pImpl;
	return (result);
}

/************************************************
 * osl_syncFile
 ***********************************************/
oslFileError
SAL_CALL osl_syncFile(oslFileHandle Handle)
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd))
		return osl_File_E_INVAL;

	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));

	OSL_TRACE("osl_syncFile(%d)", pImpl->m_fd);
	oslFileError result = pImpl->syncFile();
	if (result != osl_File_E_None)
		return (result);
	if (-1 == fsync (pImpl->m_fd))
		return oslTranslateFileError (OSL_FET_ERROR, errno);

	return osl_File_E_None;
}

/*******************************************
 osl_mapFile
********************************************/
oslFileError
SAL_CALL osl_mapFile (
	oslFileHandle Handle,
	void**        ppAddr,
	sal_uInt64    uLength,
	sal_uInt64    uOffset,
	sal_uInt32    uFlags
)
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == ppAddr))
		return osl_File_E_INVAL;
	*ppAddr = 0;

	static sal_uInt64 const g_limit_size_t = std::numeric_limits< size_t >::max();
	if (g_limit_size_t < uLength)
		return osl_File_E_OVERFLOW;
	size_t const nLength = sal::static_int_cast< size_t >(uLength);

	static sal_uInt64 const g_limit_off_t = std::numeric_limits< off_t >::max();
	if (g_limit_off_t < uOffset)
		return osl_File_E_OVERFLOW;
	off_t const nOffset = sal::static_int_cast< off_t >(uOffset);

	void* p = mmap(NULL, nLength, PROT_READ, MAP_SHARED, pImpl->m_fd, nOffset);
	if (MAP_FAILED == p)
		return oslTranslateFileError(OSL_FET_ERROR, errno);
	*ppAddr = p;

	if (uFlags & osl_File_MapFlag_RandomAccess)
	{
		// Determine memory pagesize.
		size_t const nPageSize = FileHandle_Impl::getpagesize();
		if (size_t(-1) != nPageSize)
		{
			/*
			 * Pagein, touching first byte of every memory page.
			 * Note: volatile disables optimizing the loop away.
			 */
			sal_uInt8 * pData (reinterpret_cast<sal_uInt8*>(*ppAddr));
			size_t      nSize (nLength);

			volatile sal_uInt8 c = 0;
			while (nSize > nPageSize)
			{
				c ^= pData[0];
				pData += nPageSize;
				nSize -= nPageSize;
			}
			if (nSize > 0)
			{
				c^= pData[0];
				pData += nSize;
				nSize -= nSize;
			}
		}
	}
	if (uFlags & osl_File_MapFlag_WillNeed)
	{
		// On Linux, madvise(..., MADV_WILLNEED) appears to have the undesirable
		// effect of not returning until the data has actually been paged in, so
		// that its net effect would typically be to slow down the process
		// (which could start processing at the beginning of the data while the
		// OS simultaneously pages in the rest); on other platforms, it remains
		// to be evaluated whether madvise or equivalent is available and
		// actually useful:
#if defined (FREEBSD) || defined (MACOSX)
		int e = posix_madvise(p, nLength, POSIX_MADV_WILLNEED);
		if (e != 0)
		{
			OSL_TRACE(
				"posix_madvise(..., POSIX_MADV_WILLNEED) failed with %d", e);
		}
#elif defined SOLARIS
		if (madvise(static_cast< caddr_t >(p), nLength, MADV_WILLNEED) != 0)
		{
			OSL_TRACE("madvise(..., MADV_WILLNEED) failed with %d", errno);
		}
#endif
	}
	return osl_File_E_None;
}

/*******************************************
 osl_unmapFile
********************************************/
oslFileError
SAL_CALL osl_unmapFile (void* pAddr, sal_uInt64 uLength)
{
	if (0 == pAddr)
		return osl_File_E_INVAL;

	static sal_uInt64 const g_limit_size_t = std::numeric_limits< size_t >::max();
	if (g_limit_size_t < uLength)
		return osl_File_E_OVERFLOW;
	size_t const nLength = sal::static_int_cast< size_t >(uLength);

	if (-1 == munmap(static_cast<char*>(pAddr), nLength))
		return oslTranslateFileError(OSL_FET_ERROR, errno);

	return osl_File_E_None;
}

/*******************************************
 osl_readLine
********************************************/
oslFileError
SAL_CALL osl_readLine (
	oslFileHandle   Handle,
	sal_Sequence ** ppSequence)
{
	FileHandle_Impl * pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == ppSequence))
		return osl_File_E_INVAL;
	sal_uInt64 uBytesRead = 0;

	// read at current fileptr; fileptr += uBytesRead;
	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	oslFileError result = pImpl->readLineAt (
		pImpl->m_fileptr, ppSequence, &uBytesRead);
	if (result == osl_File_E_None)
		pImpl->m_fileptr += uBytesRead;
	return (result);
}

/*******************************************
 osl_readFile
********************************************/
oslFileError
SAL_CALL osl_readFile (
	oslFileHandle Handle,
	void *        pBuffer,
	sal_uInt64    uBytesRequested,
	sal_uInt64 *  pBytesRead)
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == pBuffer) || (0 == pBytesRead))
		return osl_File_E_INVAL;

	static sal_uInt64 const g_limit_ssize_t = std::numeric_limits< ssize_t >::max();
	if (g_limit_ssize_t < uBytesRequested)
		return osl_File_E_OVERFLOW;
	size_t const nBytesRequested = sal::static_int_cast< size_t >(uBytesRequested);

	// read at current fileptr; fileptr += *pBytesRead;
	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	oslFileError result = pImpl->readFileAt (
		pImpl->m_fileptr, pBuffer, nBytesRequested, pBytesRead);
	if (result == osl_File_E_None)
		pImpl->m_fileptr += *pBytesRead;
	return (result);
}

/*******************************************
 osl_writeFile
********************************************/
oslFileError
SAL_CALL osl_writeFile (
	oslFileHandle Handle,
	const void *  pBuffer,
	sal_uInt64    uBytesToWrite,
	sal_uInt64 *  pBytesWritten)
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == pBuffer) || (0 == pBytesWritten))
		return osl_File_E_INVAL;
	if (0 == (pImpl->m_state & FileHandle_Impl::STATE_WRITEABLE))
		return osl_File_E_BADF;

	static sal_uInt64 const g_limit_ssize_t = std::numeric_limits< ssize_t >::max();
	if (g_limit_ssize_t < uBytesToWrite)
		return osl_File_E_OVERFLOW;
	size_t const nBytesToWrite = sal::static_int_cast< size_t >(uBytesToWrite);

	// write at current fileptr; fileptr += *pBytesWritten;
	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	oslFileError result = pImpl->writeFileAt (
		pImpl->m_fileptr, pBuffer, nBytesToWrite, pBytesWritten);
	if (result == osl_File_E_None)
		pImpl->m_fileptr += *pBytesWritten;
	return (result);
}

/*******************************************
 osl_readFileAt
********************************************/
oslFileError
SAL_CALL osl_readFileAt (
	oslFileHandle Handle,
	sal_uInt64    uOffset,
	void*         pBuffer,
	sal_uInt64    uBytesRequested,
	sal_uInt64*   pBytesRead)
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == pBuffer) || (0 == pBytesRead))
		return osl_File_E_INVAL;
	if (0 == (pImpl->m_state & FileHandle_Impl::STATE_SEEKABLE))
		return osl_File_E_SPIPE;

	static sal_uInt64 const g_limit_off_t = std::numeric_limits< off_t >::max();
	if (g_limit_off_t < uOffset)
		return osl_File_E_OVERFLOW;
	off_t const nOffset = sal::static_int_cast< off_t >(uOffset);

	static sal_uInt64 const g_limit_ssize_t = std::numeric_limits< ssize_t >::max();
	if (g_limit_ssize_t < uBytesRequested)
		return osl_File_E_OVERFLOW;
	size_t const nBytesRequested = sal::static_int_cast< size_t >(uBytesRequested);

	// read at specified fileptr
	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	return pImpl->readFileAt (nOffset, pBuffer, nBytesRequested, pBytesRead);
}

/*******************************************
 osl_writeFileAt
********************************************/
oslFileError
SAL_CALL osl_writeFileAt (
	oslFileHandle Handle,
	sal_uInt64    uOffset,
	const void*   pBuffer,
	sal_uInt64    uBytesToWrite,
	sal_uInt64*   pBytesWritten)
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == pBuffer) || (0 == pBytesWritten))
		return osl_File_E_INVAL;
	if (0 == (pImpl->m_state & FileHandle_Impl::STATE_SEEKABLE))
		return osl_File_E_SPIPE;
	if (0 == (pImpl->m_state & FileHandle_Impl::STATE_WRITEABLE))
		return osl_File_E_BADF;

	static sal_uInt64 const g_limit_off_t = std::numeric_limits< off_t >::max();
	if (g_limit_off_t < uOffset)
		return osl_File_E_OVERFLOW;
	off_t const nOffset = sal::static_int_cast< off_t >(uOffset);

	static sal_uInt64 const g_limit_ssize_t = std::numeric_limits< ssize_t >::max();
	if (g_limit_ssize_t < uBytesToWrite)
		return osl_File_E_OVERFLOW;
	size_t const nBytesToWrite = sal::static_int_cast< size_t >(uBytesToWrite);

	// write at specified fileptr
	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	return pImpl->writeFileAt (nOffset, pBuffer, nBytesToWrite, pBytesWritten);
}

/****************************************************************************/
/* osl_isEndOfFile */
/****************************************************************************/
oslFileError
SAL_CALL osl_isEndOfFile( oslFileHandle Handle, sal_Bool *pIsEOF )
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == pIsEOF))
		return osl_File_E_INVAL;

	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	*pIsEOF = (pImpl->getPos() == pImpl->getSize());
	return osl_File_E_None;
}

/************************************************
 * osl_getFilePos
 ***********************************************/
oslFileError
SAL_CALL osl_getFilePos( oslFileHandle Handle, sal_uInt64* pPos )
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == pPos))
		return osl_File_E_INVAL;

	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	*pPos = pImpl->getPos();
	return osl_File_E_None;
}

/*******************************************
 osl_setFilePos
********************************************/
oslFileError
SAL_CALL osl_setFilePos (oslFileHandle Handle, sal_uInt32 uHow, sal_Int64 uOffset)
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd))
		return osl_File_E_INVAL;

	static sal_Int64 const g_limit_off_t = std::numeric_limits< off_t >::max();
	if (g_limit_off_t < uOffset)
		return osl_File_E_OVERFLOW;
	off_t nPos = 0, nOffset = sal::static_int_cast< off_t >(uOffset);

	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	switch(uHow)
	{
		case osl_Pos_Absolut:
			if (0 > nOffset)
				return osl_File_E_INVAL;
			break;

		case osl_Pos_Current:
			nPos = sal::static_int_cast< off_t >(pImpl->getPos());
			if ((0 > nOffset) && (-1*nOffset > nPos))
				return osl_File_E_INVAL;
			if (g_limit_off_t < nPos + nOffset)
				return osl_File_E_OVERFLOW;
			break;

		case osl_Pos_End:
			nPos = sal::static_int_cast< off_t >(pImpl->getSize());
			if ((0 > nOffset) && (-1*nOffset > nPos))
				return osl_File_E_INVAL;
			if (g_limit_off_t < nPos + nOffset)
				return osl_File_E_OVERFLOW;
			break;

		default:
			return osl_File_E_INVAL;
	}

	return pImpl->setPos (nPos + nOffset);
}

/****************************************************************************
 * osl_getFileSize
 ****************************************************************************/
oslFileError
SAL_CALL osl_getFileSize( oslFileHandle Handle, sal_uInt64* pSize )
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd) || (0 == pSize))
		return osl_File_E_INVAL;

	FileHandle_Impl::Guard lock (&(pImpl->m_mutex));
	*pSize = pImpl->getSize();
	return osl_File_E_None;
}

/************************************************
 * osl_setFileSize
 ***********************************************/
oslFileError
SAL_CALL osl_setFileSize( oslFileHandle Handle, sal_uInt64 uSize )
{
	FileHandle_Impl* pImpl = static_cast<FileHandle_Impl*>(Handle);

	if ((0 == pImpl) || (-1 == pImpl->m_fd))
		return osl_File_E_INVAL;
	if (0 == (pImpl->m_state & FileHandle_Impl::STATE_WRITEABLE))
		return osl_File_E_BADF;

	static sal_uInt64 const g_limit_off_t = std::numeric_limits< off_t >::max();
	if (g_limit_off_t < uSize)
		return osl_File_E_OVERFLOW;

	oslFileError result = pImpl->syncFile();
	if (result != osl_File_E_None)
		return (result);
	pImpl->m_bufptr = -1, pImpl->m_buflen = 0;

	return pImpl->setSize (uSize);
}
