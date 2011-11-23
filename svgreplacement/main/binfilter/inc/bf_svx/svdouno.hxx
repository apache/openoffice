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



#ifndef _SVDOUNO_HXX
#define _SVDOUNO_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_AWT_XCONTROLMODEL_HPP_
#include <com/sun/star/awt/XControlModel.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROL_HPP_
#include <com/sun/star/awt/XControl.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _SVDORECT_HXX
#include <bf_svx/svdorect.hxx>
#endif
namespace binfilter {

//************************************************************
//   Vorausdeklarationen
//************************************************************

class SdrView;
class SdrControlEventListenerImpl;

//************************************************************
//   SdrUnoObj
//************************************************************

class SdrUnoObj : public SdrRectObj
{
	friend class				SdrPageView;
	friend class				SdrControlEventListenerImpl;

	SdrControlEventListenerImpl*	pEventListener;

	String						aUnoControlModelTypeName;
	String						aUnoControlTypeName;
	BOOL						bOwnUnoControlModel;

protected:
	::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > xUnoControlModel; // kann auch von aussen gesetzt werden

private:

public:
	TYPEINFO();

	SdrUnoObj(const String& rModelName, BOOL bOwnsModel = TRUE);
	virtual ~SdrUnoObj();

	virtual void SetPage(SdrPage* pNewPage);
	virtual void SetModel(SdrModel* pModel);



	virtual void NbcMove(const Size& rSize);
	virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
	virtual void NbcSetSnapRect(const Rectangle& rRect);
	virtual void NbcSetLogicRect(const Rectangle& rRect);
    virtual void NbcSetLayer(SdrLayerID nLayer);

	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);


	::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > GetUnoControlModel() const {return xUnoControlModel;}

	const String& GetUnoControlModelTypeName() const { return aUnoControlTypeName; }
	const String& GetUnoControlTypeName() const { return aUnoControlTypeName; }

	void SetUnoControlModel(::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > xModel);
	void VisAreaChanged(const OutputDevice* pOut=NULL);
};

}//end of namespace binfilter
#endif          // _SVDOUNO_HXX

