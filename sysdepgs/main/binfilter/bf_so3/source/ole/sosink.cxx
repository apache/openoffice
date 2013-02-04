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




#include "socont.h"


namespace binfilter {

/*
 * CImpIAdviseSink::CImpIAdviseSink
 * CImpIAdviseSink::~CImpIAdviseSink
 *
 * Parameters (Constructor):
 *  pSO_Cont         PCSO_Cont of the tenant we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

CImpIAdviseSink::CImpIAdviseSink(PCSO_Cont pSO_Cont
	, LPUNKNOWN pUnkOuter)
	{
	m_cRef=0;
	m_pTen=pSO_Cont;
	m_pUnkOuter=pUnkOuter;
	return;
	}

CImpIAdviseSink::~CImpIAdviseSink(void)
	{
	return;
	}




/*
 * CImpIAdviseSink::QueryInterface
 * CImpIAdviseSink::AddRef
 * CImpIAdviseSink::Release
 *
 * Purpose:
 *  IUnknown members for CImpIAdviseSink object.
 */

STDMETHODIMP CImpIAdviseSink::QueryInterface(REFIID riid, PPVOID ppv)
	{
	return m_pUnkOuter->QueryInterface(riid, ppv);
	}


STDMETHODIMP_(ULONG) CImpIAdviseSink::AddRef(void)
	{
	++m_cRef;
	return m_pUnkOuter->AddRef();
	}

STDMETHODIMP_(ULONG) CImpIAdviseSink::Release(void)
	{
	--m_cRef;
	return m_pUnkOuter->Release();
	}




/*
 * CImpIAdviseSink::OnDataChange
 *
 * Unused since we don't IDataObject::Advise.
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnDataChange(LPFORMATETC
	, LPSTGMEDIUM)
	{
	return;
	}







/*
 * CImpIAdviseSink::OnViewChange
 *
 * Purpose:
 *  Notifes the advise sink that presentation data changed in the
 *  data object to which we're connected providing the right time
 *  to update displays using such presentations.
 *
 * Parameters:
 *  dwAspect        DWORD indicating which aspect has changed.
 *  lindex          LONG indicating the piece that changed.
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnViewChange(DWORD dwAspect
	, LONG)
	{
	//Repaint only if this is the right aspect
	//if (dwAspect==m_pTen->m_fe.dwAspect)
	if (dwAspect==m_pTen->dwAspect)
		{
		m_pTen->m_pPG->SetModified();
		// VCL and OLE aspects have the same values
		//m_pTen->m_pPG->ViewChanged( dwAspect );
		//m_pTen->m_pPG->m_fDirty=TRUE;
		//m_pTen->Repaint();
		}

	return;
	}





/*
 * CImpIAdviseSink::OnRename
 *
 * Purpose:
 *  Informs the advise sink that a linked object has been renamed.
 *  Generally only the OLE default handler cares about this.
 *
 * Parameters:
 *  pmk             LPMONIKER providing the new name of the object
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnRename(LPMONIKER)
	{
	/*
	 * As a container this is unimportant to us since it really
	 * tells the handler's implementation of IOleLink that the
	 * object's moniker has changed.  Since we get this call
	 * from the handler, we don't have to do anything ourselves.
	 */
	return;
	}






/*
 * CImpIAdviseSink::OnSave
 *
 * Purpose:
 *  Informs the advise sink that the OLE object has been saved
 *  persistently.  The primary purpose of this is for containers
 *  that want to make optimizations for objects that are not in a
 *  saved state, so on this you have to disable such optimizations.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnSave(void)
	{
	/*
	 * A Container has nothing to do here as this notification is
	 * only useful when we have an ADVFCACHE_ONSAVE advise set up,
	 * which we don't.  So we ignore it.
	 */
	return;
	}





/*
 * CImpIAdviseSink::OnClose
 *
 * Purpose:
 *  Informs the advise sink that the OLE object has closed and is
 *  no longer bound in any way.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnClose(void)
	{
	/*
	 * This doesn't have much to do with us again as it's only
	 * used to notify the handler's IOleLink implementation of the
	 * change in the object.  We don't have to do anything since
	 * we'll also get an IOleClientSite::OnShowWindow(FALSE) to
	 * tell us to repaint.
	 */

	/*
	 * If we are dealing with an OLE 1.0 server it may not call
	 * IOleClientSite::OnShowWindow(FALSE) properly, so to protect
	 * ourselves we make sure the object is drawn as closed on
	 * this notification.
	 */
	m_pTen->ShowAsOpen(FALSE);
	return;
	}
}
