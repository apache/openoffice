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


#ifndef _SVX_FMOBJ_HXX
#define _SVX_FMOBJ_HXX

#ifndef _SVDOUNO_HXX
#include "svdouno.hxx"
#endif

#ifndef _COM_SUN_STAR_SCRIPT_SCRIPTEVENTDESCRIPTOR_HPP_
#include <com/sun/star/script/ScriptEventDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
namespace binfilter {

class FmFormView;

//==================================================================
// FmFormObj
//==================================================================
class FmXForms;
class FmFormObj: public SdrUnoObj
{
	friend class FmForm;
	friend class FmFormPage;
	friend class FmFormPageImpl;
	friend class FmFormObjFactory;
	friend class FmXUndoEnvironment;
	friend class SvxFmDrawPage;
	friend class SvxFmMSFactory;

	::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor >	aEvts;  // events des Objects
	::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor>	m_aEventsHistory;
				// valid if and only if m_pEnvironmentHistory != NULL, this are the events which we're set when
				// m_pEnvironmentHistory was created

	FmFormView* pTempView;
	sal_uInt32		nEvent;

	// Informationen fuer die Controlumgebung
	// werden nur vorgehalten, wenn ein Object sich nicht in einer Objectliste befindet
	::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer>		xParent;
	::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >	m_xEnvironmentHistory;
	sal_Int32			nPos;
	sal_Int32			m_nType;

public:
	TYPEINFO();

protected:
	FmFormObj(sal_Int32 _nType);

	const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer>& GetParent() const {return xParent;}
	const ::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor >& GetEvents() const {return aEvts;}
	sal_Int32 GetPos() const {return nPos;}

public:
	virtual ~FmFormObj();
	virtual void SetPage(SdrPage* pNewPage);

	virtual sal_uInt32 GetObjInventor() const;
	virtual sal_uInt16 GetObjIdentifier() const;




	/** returns the type of this form object. See fmglob.hxx
	*/
	sal_Int32	getType() const;

protected:
	virtual void     WriteData(SvStream& rOut) const;
	virtual void     ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

	DECL_LINK(OnCreate, void* );
};


}//end of namespace binfilter
#endif // _FM_FMOBJ_HXX

