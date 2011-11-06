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



#ifndef _SVX_FMPAGE_HXX
#define _SVX_FMPAGE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVDPAGE_HXX //autogen
#include <bf_svx/svdpage.hxx>
#endif

#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
FORWARD_DECLARE_INTERFACE(container,XNameContainer)
namespace binfilter {

class StarBASIC;
class FmFormModel;
class FmFormPageImpl;	// haelt die Liste aller Forms

//FORWARD_DECLARE_INTERFACE(uno,Reference)
//STRIP008 FORWARD_DECLARE_INTERFACE(container,XNameContainer)

class SfxJSArray;
class HelpEvent;

class FmFormPage : public SdrPage
{
	friend class FmFormObj;
	FmFormPageImpl* pImpl;

public:
	TYPEINFO();

	FmFormPage(FmFormModel& rModel,StarBASIC*, FASTBOOL bMasterPage=sal_False);
	~FmFormPage();

	using SdrPage::NbcInsertObject;
	using SdrPage::ReplaceObject;

	virtual void  	WriteData(SvStream& rOut) const;
	virtual void  	ReadData(const SdrIOHeader& rHead, SvStream& rIn);
	virtual void  	SetModel(SdrModel* pNewModel);

	virtual SdrPage* Clone() const;

	virtual void	InsertObject(SdrObject* pObj, ULONG nPos = CONTAINER_APPEND,
									const SdrInsertReason* pReason=NULL);

	virtual SdrObject* RemoveObject(ULONG nObjNum);

#ifndef SVX_LIGHT
	/**	Insert _pClone into the page.
		If _pClone is no form object, InsertObject will be called.
		If _pClone is a form object, the page will create forms as needed to properly place the control model of the new object
		(so it is located within the forms hierarchy in an position analogous to the source objects model with it's hierarchy).
		If _pClone is NULL, it will be created via _pSourceObject->Clone(SdrPage*, SdrModel*).
		If bTryPreserveName is sal_True, the property value for "Name" will be restored after calling the InsertObject (which otherwise
		may alter the name to ensure uniqueness).
	*/

	/** Create an environment (within the forms hierarchy of the page) for inserting a form object which is a clone
		of the given form object (which has to belong to a foreign page).
		"environment" means that the UnoControlModel of the given object may be inserted into our forms hierarchy
		without (to much) structure loss.
		If _pObj isn't a form object, nothing happens.
	*/
#endif

	// Zugriff auf alle Formulare
	const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer>& GetForms() const;

	FmFormPageImpl*  GetImpl() const {return pImpl;}

public:
	String				aPageName;
	StarBASIC* 			pBasic;

	const String& 		GetName() const { return aPageName; }
	void 				SetName( const String& rName ) { aPageName = rName; }
	StarBASIC*      	GetBasic() const { return pBasic; }
};

}//end of namespace binfilter
#endif          // _SVX_FMPAGE_HXX

