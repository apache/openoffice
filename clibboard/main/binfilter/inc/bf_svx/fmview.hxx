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



#ifndef _SVX_FMVIEW_HXX
#define _SVX_FMVIEW_HXX

#include <bf_svtools/bf_solar.h>

// HACK to avoid too deep includes and to have some
// levels free in svdmark itself (MS compiler include depth limit)
#ifndef _SVDHDL_HXX
#include <bf_svx/svdhdl.hxx>
#endif

#ifndef _E3D_VIEW3D_HXX
#include <bf_svx/view3d.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif

FORWARD_DECLARE_INTERFACE(util,XNumberFormats)
FORWARD_DECLARE_INTERFACE(beans,XPropertySet)
class OutputDevice;
namespace binfilter {
class ExtOutputDevice;
class FmFormModel;
class FmPageViewWinRec;
class FmFormObj;
class FmFormPage;
class FmFormShell;
class FmXFormView;

namespace svx {
	class ODataAccessDescriptor;
}

class FmFormView : public E3dView
{
	friend class FmXFormController;
	friend class FmFormShell;
	friend class FmXFormShell;
	friend class FmXFormView;
	friend class FmExplorer;
	friend class FmFormObj;

	FmXFormView*	pImpl;
	FmFormShell*	pFormShell;

	void Init();
	void SetFormShell( FmFormShell* pShell ) { pFormShell = pShell; }
	FmFormShell* GetFormShell() const { return pFormShell; }

public:
	TYPEINFO();

	FmFormView(FmFormModel* pModel, OutputDevice* pOut);
	virtual ~FmFormView();

	/** create a control pair (label/bound control) for the database field description given.
		@param rFieldDesc
			description of the field. see clipboard format SBA-FIELDFORMAT
		@deprecated
			This method is deprecated. Use the version with a ODataAccessDescriptor instead.
	*/

	/** create a control pair (label/bound control) for the database field description given.
	*/

	virtual void MarkListHasChanged();
	virtual void DelWin(OutputDevice* pWin1);

	virtual SdrPageView* ShowPage(SdrPage* pPage, const Point& rOffs);
	virtual void HidePage(SdrPageView* pPV);

	// for copying complete form structures, not only control models

	/** grab the focus to the first form control on the view
		@param _bForceSync
			<TRUE/> if the handling should be done synchronously.
	*/

	// SdrView
	BOOL KeyInput(const KeyEvent& rKEvt, Window* pWin);

protected:
	void ActivateControls(SdrPageView*);
	void DeactivateControls(SdrPageView*);

	// Hinweis an die UI, da?ein Control erzeugt worden ist
	void ChangeDesignMode(sal_Bool bDesign);

public:
	FmXFormView* GetImpl() const {return pImpl;}

private:
	const OutputDevice* GetActualOutDev() const {return pActualOutDev;}
	void AdjustMarks(const SdrMarkList& rMarkList);
	sal_Bool checkUnMarkAll(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _xSource);
	FmFormObj* getMarkedGrid() const;
};

}//end of namespace binfilter
#endif          // _FML_FMVIEW_HXX

