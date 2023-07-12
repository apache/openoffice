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
#include "precompiled_shell.hxx"
#include "internal/global.hxx"
#include "classfactory.hxx"
#include "internal/infotips.hxx"
#include "internal/propsheets.hxx"
#include "internal/columninfo.hxx"
#ifdef __MINGW32__
#include <algorithm>
using ::std::max;
using ::std::min;
#endif
#include "internal/thumbviewer.hxx"
#include "internal/shlxthdl.hxx"

//-----------------------------
//
//-----------------------------

long CClassFactory::s_ServerLocks = 0;

//-----------------------------
//
//-----------------------------

CClassFactory::CClassFactory(const CLSID& clsid) :
	m_RefCnt(1),
	m_Clsid(clsid)
{
	InterlockedIncrement(&g_DllRefCnt);
}

//-----------------------------
//
//-----------------------------

CClassFactory::~CClassFactory()
{
	InterlockedDecrement(&g_DllRefCnt);
}

//-----------------------------
// IUnknown methods
//-----------------------------

HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = 0;

	if (IID_IUnknown == riid || IID_IClassFactory == riid)
	{
		IUnknown* pUnk = this;
		pUnk->AddRef();
		*ppvObject = pUnk;
		return S_OK;
	}

	return E_NOINTERFACE;
}

//-----------------------------
//
//-----------------------------

ULONG STDMETHODCALLTYPE CClassFactory::AddRef(void)
{
	return InterlockedIncrement(&m_RefCnt);
}

//-----------------------------
//
//-----------------------------

ULONG STDMETHODCALLTYPE CClassFactory::Release(void)
{
	long refcnt = InterlockedDecrement(&m_RefCnt);

	if (0 == refcnt)
		delete this;

	return refcnt;
}

//-----------------------------
// IClassFactory methods
//-----------------------------

HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(
			IUnknown __RPC_FAR *pUnkOuter,
			REFIID riid,
			void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if ((pUnkOuter != NULL))
		return CLASS_E_NOAGGREGATION;

	IUnknown* pUnk = 0;

	if (CLSID_PROPERTYSHEET_HANDLER == m_Clsid)
		pUnk = static_cast<IShellExtInit*>(new CPropertySheet());

	else if (CLSID_INFOTIP_HANDLER == m_Clsid)
		pUnk = static_cast<IQueryInfo*>(new CInfoTip());

	else if (CLSID_COLUMN_HANDLER == m_Clsid)
		pUnk = static_cast<IColumnProvider*>(new CColumnInfo());

	else if (CLSID_THUMBVIEWER_HANDLER == m_Clsid)
		pUnk = static_cast<IExtractImage*>(new CThumbviewer());

	POST_CONDITION(pUnk != 0, "Could not create COM object");

	if (0 == pUnk)
		return E_OUTOFMEMORY;

	HRESULT hr = pUnk->QueryInterface(riid, ppvObject);

	// if QueryInterface failed the component will destroy itself
	pUnk->Release();

	return hr;
}

//-----------------------------
//
//-----------------------------

HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		InterlockedIncrement(&s_ServerLocks);
	else
		InterlockedDecrement(&s_ServerLocks);

	return S_OK;
}

//-----------------------------
//
//-----------------------------

bool CClassFactory::IsLocked()
{
	return (s_ServerLocks > 0);
}

/* vim: set noet sw=4 ts=4: */
