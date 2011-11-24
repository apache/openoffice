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
#include "precompiled_svx.hxx"

#define ITEMID_FONTHEIGHT   EE_CHAR_FONTHEIGHT

#include "svx/svdotext.hxx"
#include "svx/svdetc.hxx"
#include "editeng/outlobj.hxx"
#include "svx/svdoutl.hxx"
#include "svx/svdmodel.hxx"
#include "editeng/fhgtitem.hxx"
#include <editeng/eeitem.hxx>
#include <svl/itemset.hxx>
#include <svx/svdtrans.hxx>

SdrText::SdrText( SdrTextObj& rObject, OutlinerParaObject* pOutlinerParaObject /* = 0 */ )
: mpOutlinerParaObject( pOutlinerParaObject )
, mrObject( rObject )
, mbPortionInfoChecked( false )
{
	OSL_ENSURE(&mrObject, "SdrText created without SdrTextObj (!)");
}

SdrText::~SdrText()
{
    clearWeak();
    delete mpOutlinerParaObject;
}

void SdrText::CheckPortionInfo( SdrOutliner& rOutliner )
{
	if(!mbPortionInfoChecked)
	{
		// Optimierung: ggf. BigTextObject erzeugen
		mbPortionInfoChecked=true;
	
		if(mpOutlinerParaObject && rOutliner.ShouldCreateBigTextObject())
        {
            // #i102062# MemoryLeak closed
            delete mpOutlinerParaObject;
			mpOutlinerParaObject = rOutliner.CreateParaObject();
        }
	}
}

void SdrText::ReformatText()
{
	mbPortionInfoChecked = false;
	mpOutlinerParaObject->ClearPortionInfo();
}

const SfxItemSet& SdrText::GetItemSet() const
{
	return const_cast< SdrText* >(this)->GetObjectItemSet();
}

void SdrText::SetOutlinerParaObject( OutlinerParaObject* pTextObject )
{
	if( mpOutlinerParaObject != pTextObject )
	{
		if( mpOutlinerParaObject )
		{
			delete mpOutlinerParaObject;
		}

		mpOutlinerParaObject = pTextObject;

		mbPortionInfoChecked = false;
	}
}

OutlinerParaObject* SdrText::GetOutlinerParaObject() const
{
	return mpOutlinerParaObject;
}

/** returns the current OutlinerParaObject and removes it from this instance */
OutlinerParaObject* SdrText::RemoveOutlinerParaObject()
{
	OutlinerParaObject* pOPO = mpOutlinerParaObject;

	mpOutlinerParaObject = 0;
	mbPortionInfoChecked = false;

	return pOPO;
}

void SdrText::ForceOutlinerParaObject( sal_uInt16 nOutlMode )
{
	if( !mpOutlinerParaObject )
	{
		Outliner* pOutliner = SdrMakeOutliner( nOutlMode, &mrObject.getSdrModelFromSdrObject() );
		if( pOutliner )
		{
			Outliner& aDrawOutliner = mrObject.getSdrModelFromSdrObject().GetDrawOutliner();
			pOutliner->SetCalcFieldValueHdl( aDrawOutliner.GetCalcFieldValueHdl() );

			pOutliner->SetStyleSheet( 0, GetStyleSheet());
			OutlinerParaObject* pOutlinerParaObject = pOutliner->CreateParaObject();
			SetOutlinerParaObject( pOutlinerParaObject );

			delete pOutliner;
		}
	}
}

const SfxItemSet& SdrText::GetObjectItemSet()
{
	return mrObject.GetObjectItemSet();
}

void SdrText::SetObjectItem(const SfxPoolItem& rItem)
{
	mrObject.SetObjectItem( rItem );
}

SfxStyleSheet* SdrText::GetStyleSheet() const
{
	return mrObject.GetStyleSheet();
}
