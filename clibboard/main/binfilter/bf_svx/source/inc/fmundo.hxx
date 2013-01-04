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


#ifndef _SVX_FMUNDO_HXX
#define _SVX_FMUNDO_HXX


#ifndef _COM_SUN_STAR_BEANS_XPROPERTYCHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XSCRIPTLISTENER_HPP_
#include <com/sun/star/script/XScriptListener.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINERLISTENER_HPP_
#include <com/sun/star/container/XContainerListener.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif




#ifndef _SVDOUNO_HXX //autogen wg. SdrUnoObj
#include "svdouno.hxx"
#endif

#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
FORWARD_DECLARE_INTERFACE(awt,XControl)
FORWARD_DECLARE_INTERFACE(awt,XControlContainer)
namespace binfilter {

class FmFormModel;
class FmFormObj;
class SdrObject;
class FmXFormView;

//STRIP008 FORWARD_DECLARE_INTERFACE(awt,XControl)
//STRIP008 FORWARD_DECLARE_INTERFACE(awt,XControlContainer)
//FORWARD_DECLARE_INTERFACE(uno,Reference)

//==================================================================
// FmUndoPropertyAction
//==================================================================

//==================================================================
// FmUndoContainerAction
//==================================================================

//==================================================================
// FmUndoModelReplaceAction
//==================================================================

//========================================================================
class FmXUndoEnvironment
	: public ::cppu::WeakImplHelper3<	::com::sun::star::beans::XPropertyChangeListener
									,	::com::sun::star::container::XContainerListener
									,	::com::sun::star::script::XScriptListener
									>
	, public SfxListener
						   //	public ::cppu::OWeakObject
{
	friend class FmFormModel;
	friend class FmXFormView;
	FmFormModel& rModel;

	void*		m_pPropertySetCache;
	sal_uInt32	nLocks;
	sal_Bool	bReadOnly;



public:
	FmXUndoEnvironment(FmFormModel& _rModel);
	~FmXUndoEnvironment();

	// UNO Anbindung
	//	SMART_UNO_DECLARATION(FmXUndoEnvironment, ::cppu::OWeakObject);
	//	virtual sal_Bool queryInterface(UsrUik, ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>&);
	//	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::reflection::XIdlClass>>	getIdlClasses(void);

	void Lock() {nLocks++;}
	void UnLock() {nLocks--;}
	sal_Bool IsLocked() const {return nLocks != 0;}

protected:
// ::com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source) throw( ::com::sun::star::uno::RuntimeException );

// ::com::sun::star::beans::XPropertyChangeListener
	virtual void SAL_CALL propertyChange(const ::com::sun::star::beans::PropertyChangeEvent& evt) throw(::com::sun::star::uno::RuntimeException);

// ::com::sun::star::container::XContainerListener
	virtual void SAL_CALL elementInserted(const ::com::sun::star::container::ContainerEvent& rEvent) throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL elementReplaced(const ::com::sun::star::container::ContainerEvent& rEvent) throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL elementRemoved(const ::com::sun::star::container::ContainerEvent& rEvent) throw(::com::sun::star::uno::RuntimeException);

// XScriptListener
	virtual void SAL_CALL firing(const  ::com::sun::star::script::ScriptEvent& evt) throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Any SAL_CALL approveFiring(const  ::com::sun::star::script::ScriptEvent& evt) throw(::com::sun::star::reflection::InvocationTargetException, ::com::sun::star::uno::RuntimeException);

	virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

// Einfuegen von Objekten
	void AddForms(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer>& rForms);
	void RemoveForms(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer>& rForms);

	void SetReadOnly(sal_Bool bRead) {bReadOnly = bRead;}
	sal_Bool IsReadOnly() const {return bReadOnly;}

	void ModeChanged();

private:
	void AddElement(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& Element);
	void RemoveElement(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& Element);
	void AlterPropertyListening(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& Element);

public:
	// Methoden zur Zuordnung von Controls zu Forms,
	// werden von der Seite und der UndoUmgebung genutzt
	void Inserted(SdrObject* pObj);
	void Removed(SdrObject* pObj);

	void Inserted(FmFormObj* pObj);
	void Removed(FmFormObj* pObj);
};


}//end of namespace binfilter
#endif	//_SVX_FMUNDO_HXX

