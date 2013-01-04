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



#ifndef SVTOOLS_INSTRM_HXX
#define SVTOOLS_INSTRM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

namespace com { namespace sun { namespace star { namespace io {
	class XInputStream;
	class XSeekable;
} } } }

namespace binfilter
{

class SvDataPipe_Impl;

//============================================================================
class  SvInputStream: public SvStream
{
	com::sun::star::uno::Reference< com::sun::star::io::XInputStream >
	    m_xStream;
	com::sun::star::uno::Reference< com::sun::star::io::XSeekable >
	    m_xSeekable;
	SvDataPipe_Impl * m_pPipe;
	ULONG m_nSeekedFrom;

	 bool open();

	 virtual ULONG GetData(void * pData, ULONG nSize);

	 virtual ULONG PutData(void const *, ULONG);

	 virtual ULONG SeekPos(ULONG nPos);

	 virtual void FlushData();

	 virtual void SetSize(ULONG);

public:
	SvInputStream(
		com::sun::star::uno::Reference< com::sun::star::io::XInputStream >
		        const &
		    rTheStream);

	virtual ~SvInputStream();

	virtual USHORT IsA() const;

	virtual void AddMark(ULONG nPos);

	virtual void RemoveMark(ULONG nPos);
};

}

#endif // SVTOOLS_INSTRM_HXX

