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



#ifndef _TRANSPRT_HXX
#define _TRANSPRT_HXX "$Revision: 1.5 $"

#include <bf_svtools/bf_solar.h>

#ifndef _ERRCODE_HXX
#include <tools/errcode.hxx>
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _BINDING_HXX
#include <bf_so3/binding.hxx>
#endif

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif

class DateTime;

namespace binfilter {
class SvBindingTransport;
class SvBindingTransportFactory;
class SvBindingTransportCallback;
class SvBindingTransportContext;

#ifndef COPYCTOR_API
#define COPYCTOR_API(C) C (const C&); C& operator= (const C&)
#endif

/*========================================================================
 *
 * SvBindingTransport interface.
 *
 *======================================================================*/
class SO3_DLLPUBLIC SvBindingTransport
{
	/** Not implemented.
	*/
	SO3_DLLPRIVATE COPYCTOR_API(SvBindingTransport);

public:
	SvBindingTransport (void);
	virtual ~SvBindingTransport (void);

	virtual void Start (void) = 0;
	virtual void Abort (void) = 0;
};

/*========================================================================
 *
 * SvBindingTransportFactory interface.
 *
 *======================================================================*/
class SO3_DLLPUBLIC SvBindingTransportFactory
{
	/** Not implemented.
	*/
	SO3_DLLPRIVATE COPYCTOR_API(SvBindingTransportFactory);

public:
	SvBindingTransportFactory (void);
	virtual ~SvBindingTransportFactory (void);

	virtual BOOL HasTransport (
		const String &rUrl) = 0;

	virtual SvBindingTransport* CreateTransport (
		const String               &rUrl,
		SvBindingTransportContext  &rCtx,
		SvBindingTransportCallback *pCallback) = 0;
};

/*========================================================================
 *
 * SvBindingTransportCallback interface.
 *
 *======================================================================*/
class SO3_DLLPUBLIC SvBindingTransportCallback
{
public:
	virtual void OnStart (void) = 0;

	virtual void OnError (ErrCode eErrCode) = 0;

	virtual void OnMimeAvailable (const String &rMime) = 0;

	virtual void OnExpiresAvailable (const DateTime &rExpires) = 0;

	virtual void OnHeaderAvailable (
		const String &rName, const String &rValue) = 0;

	virtual void OnDataAvailable (
		SvStatusCallbackType eType,
		ULONG nSize, SvLockBytes *pLockBytes) = 0;

	virtual void OnProgress (
		ULONG nNow, ULONG nEnd, SvBindStatus eStatus) = 0;

	virtual void OnRedirect (const String &rUrl) = 0;
};

/*========================================================================
 *
 * SvBindingTransportContext.
 *
 *======================================================================*/
class SO3_DLLPUBLIC SvBindingTransportContext
{
	/** Representation.
	*/
	SvBindAction   m_eBindAction;
	SvBindMode     m_eBindMode;
	StreamMode     m_eStrmMode;
	USHORT         m_nPriority;
	String         m_aReferer;
	String         m_aSendMimeType;
	SvLockBytesRef m_xPostLockBytes;

public:
	virtual ~SvBindingTransportContext (void);

	/** BindAction.
	*/
	SvBindAction GetBindAction (void) const { return m_eBindAction; }
	void SetBindAction (SvBindAction eAction) { m_eBindAction = eAction; }

	/** BindMode.
	*/
	SvBindMode GetBindMode (void) const { return m_eBindMode; }
	void SetBindMode (SvBindMode eMode) { m_eBindMode = eMode; }

	/** StreamMode.
	*/
	StreamMode GetStreamMode (void) const { return m_eStrmMode; }
	void SetStreamMode (StreamMode eMode) { m_eStrmMode = eMode; }

	/** Priority.
	*/
	USHORT GetPriority (void) const { return m_nPriority; }
	void SetPriority (USHORT nPriority) { m_nPriority = nPriority; }

	/** Referer.
	*/
	const String& GetReferer (void) const { return m_aReferer; }
	void SetReferer (const String &rReferer) { m_aReferer = rReferer; }

	/** SendMimeType (actions PUT and POST).
	*/
	const String& GetSendMimeType (void) const
	{
		return m_aSendMimeType;
	}
	void SetSendMimeType (const String &rSendMimeType)
	{
		m_aSendMimeType = rSendMimeType;
	}

	/** PostLockBytes (actions PUT and POST).
	*/
	SvLockBytes* GetPostLockBytes (void) const
	{
		return m_xPostLockBytes;
	}
	void SetPostLockBytes (SvLockBytesRef &rxPostLockBytes)
	{
		m_xPostLockBytes = rxPostLockBytes;
	}
};

/*========================================================================
 *
 * SvLockBytesFactory interface.
 *
 *======================================================================*/
class SO3_DLLPUBLIC SvLockBytesFactory
{
	/** Representation.
	*/
	String m_aWildcard;

	/** Not implemented.
	*/
	SO3_DLLPRIVATE COPYCTOR_API(SvLockBytesFactory);

public:
	virtual ~SvLockBytesFactory (void);

	virtual SvLockBytesRef CreateLockBytes (
		const String &rUrl, String &rMime) = 0;

	static SvLockBytesFactory* GetFactory (const String &rUrl);

	const String& GetWildcard (void) const { return m_aWildcard; }
};

/*========================================================================
 *
 * SfxSimpleLockBytesFactory interface.
 *
 *======================================================================*/
class SO3_DLLPUBLIC SfxSimpleLockBytesFactory :
	public SvLockBytesFactory, public SvRefBase
{
	/** Representation.
	*/
	SvLockBytesRef m_xLockBytes;
	String         m_aMime;

	static USHORT  m_nCounter;

	/** Not implemented.
	*/
	SO3_DLLPRIVATE COPYCTOR_API(SfxSimpleLockBytesFactory);

public:
	/** Destruction (protected) (SvRefBase).
	*/
	virtual ~SfxSimpleLockBytesFactory (void);

public:
	virtual SvLockBytesRef CreateLockBytes (
		const String &rUrl, String &rMime);
};

SV_DECL_IMPL_REF(SfxSimpleLockBytesFactory);

/*========================================================================
 *
 * SvLockBytesTransportFactory interface.
 *
 *======================================================================*/
class SO3_DLLPUBLIC SvLockBytesTransportFactory : public SvBindingTransportFactory
{
	/** Not implemented.
	*/
	SO3_DLLPRIVATE COPYCTOR_API(SvLockBytesTransportFactory);

public:
	SvLockBytesTransportFactory (void);
	virtual ~SvLockBytesTransportFactory (void);

	virtual BOOL HasTransport (
		const String &rUrl);

	virtual SvBindingTransport* CreateTransport (
		const String               &rUrl,
		SvBindingTransportContext  &rCtx,
		SvBindingTransportCallback *pCB);
};

/*========================================================================
 *
 * CntTransportFactory interface.
 *
 *======================================================================*/
class SO3_DLLPUBLIC CntTransportFactory : public SvBindingTransportFactory
{
	/** Not implemented.
	*/
	SO3_DLLPRIVATE COPYCTOR_API(CntTransportFactory);

public:
	CntTransportFactory (void);
	virtual ~CntTransportFactory (void);

	virtual BOOL HasTransport (
		const String &rUrl);

	virtual SvBindingTransport* CreateTransport (
		const String               &rUrl,
		SvBindingTransportContext  &rCtx,
		SvBindingTransportCallback *pCB);
};

/*========================================================================
 *
 * The End.
 *
 *======================================================================*/

}

#endif /* _TRANSPRT_HXX */

