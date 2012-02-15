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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _UNOEVTLSTNR_HXX
#include <unoevtlstnr.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_EVENTOBJECT_HPP_
#include <com/sun/star/lang/EventObject.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XEVENTLISTENER_HPP_
#include <com/sun/star/lang/XEventListener.hpp>
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;

/* -----------------22.04.99 11:24-------------------
 *
 * --------------------------------------------------*/
SV_IMPL_PTRARR(SwEvtLstnrArray, XEventListenerPtr);

/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwEventListenerContainer::SwEventListenerContainer( uno::XInterface* pxParent) :
	pListenerArr(0),
	pxParent(pxParent)
{
}
/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwEventListenerContainer::~SwEventListenerContainer()
{
	if(pListenerArr && pListenerArr->Count())
	{
		pListenerArr->DeleteAndDestroy(0, pListenerArr->Count());
	}
	delete pListenerArr;
}
/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwEventListenerContainer::AddListener(const uno::Reference< lang::XEventListener > & rxListener)
{
	if(!pListenerArr)
		pListenerArr = new SwEvtLstnrArray;
	uno::Reference< lang::XEventListener > * pInsert = new uno::Reference< lang::XEventListener > ;
	*pInsert = rxListener;
	pListenerArr->Insert(pInsert, pListenerArr->Count());
}
/*-- 22.04.99 11:25:00---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool	SwEventListenerContainer::RemoveListener(const uno::Reference< lang::XEventListener > & rxListener)
{
	if(!pListenerArr)
		return sal_False;
	else
	{
	 	lang::XEventListener* pLeft = rxListener.get();
		for(sal_uInt16 i = 0; i < pListenerArr->Count(); i++)
		{
			XEventListenerPtr pElem = pListenerArr->GetObject(i);
		 	lang::XEventListener* pRight = pElem->get();
			if(pLeft == pRight)
			{
				pListenerArr->Remove(i);
				delete pElem;
				return sal_True;
			}
		}
	}
	return sal_False;
}
/*-- 22.04.99 11:25:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void	SwEventListenerContainer::Disposing()
{
	if(!pListenerArr)
		return;

	lang::EventObject aObj(pxParent);
	for(sal_uInt16 i = 0; i < pListenerArr->Count(); i++)
	{
		XEventListenerPtr pElem = pListenerArr->GetObject(i);
		(*pElem)->disposing(aObj);
	}
	pListenerArr->DeleteAndDestroy(0, pListenerArr->Count());
}


}
