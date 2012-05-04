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
#include <svx/sdr/properties/attributeproperties.hxx>
#include <svx/sdr/properties/itemsettools.hxx>
#include <tools/debug.hxx>
#include <svl/itemset.hxx>
#include <svl/style.hxx>
#include <svl/whiter.hxx>
#include <svl/poolitem.hxx>
#include <svx/svdobj.hxx>
#include <svx/svddef.hxx>
#include <svx/xit.hxx>
#include <svx/xbtmpit.hxx>
#include <svx/xlndsit.hxx>
#include <svx/xlnstit.hxx>
#include <svx/xlnedit.hxx>
#include <svx/xflgrit.hxx>
#include <svx/xflftrit.hxx>
#include <svx/xflhtit.hxx>
#include <svx/xlnasit.hxx>
#include <svx/xflasit.hxx>
#include <svx/svdmodel.hxx>
#include <svx/svdtrans.hxx>
#include <svx/svdpage.hxx>

// #114265#
#include <svl/smplhint.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
	namespace properties
	{
		void AttributeProperties::ImpAddStyleSheet(SfxStyleSheet* pNewStyleSheet, sal_Bool bDontRemoveHardAttr)
		{
			// test if old StyleSheet is cleared, else it would be lost
			// after this method -> memory leak (!)
			DBG_ASSERT(!mpStyleSheet, "Old style sheet not deleted before setting new one (!)");

			if(pNewStyleSheet)
			{
				mpStyleSheet = pNewStyleSheet;

				// local ItemSet is needed here, force it
				GetObjectItemSet();

				// register as listener
				StartListening(pNewStyleSheet->GetPool());
				StartListening(*pNewStyleSheet);

				// Delete hard attributes where items are set in the style sheet
				if(!bDontRemoveHardAttr)
				{
					const SfxItemSet& rStyle = pNewStyleSheet->GetItemSet();
					SfxWhichIter aIter(rStyle);
					sal_uInt16 nWhich = aIter.FirstWhich();

					while(nWhich)
					{
						if(SFX_ITEM_SET == rStyle.GetItemState(nWhich))
						{
							mpItemSet->ClearItem(nWhich);
						}

						nWhich = aIter.NextWhich();
					}
				}

				// set new stylesheet as parent
				mpItemSet->SetParent(&pNewStyleSheet->GetItemSet());
			}
		}

		void AttributeProperties::ImpRemoveStyleSheet()
		{
			// Check type since it is destroyed when the type is deleted
			if(GetStyleSheet() && dynamic_cast< SfxStyleSheet* >(mpStyleSheet))
			{
				EndListening(*mpStyleSheet);
				EndListening(mpStyleSheet->GetPool());

				// reset parent of ItemSet
				if(mpItemSet)
				{
					mpItemSet->SetParent(0L);
				}

				SdrObject& rObj = GetSdrObject();
				rObj.ActionChanged();
			}

			mpStyleSheet = 0L;
		}

		// create a new itemset
		SfxItemSet& AttributeProperties::CreateObjectSpecificItemSet(SfxItemPool& rPool)
		{
			return *(new SfxItemSet(rPool,

				// ranges from SdrAttrObj
				SDRATTR_START, SDRATTR_SHADOW_LAST,
				SDRATTR_MISC_FIRST, SDRATTR_MISC_LAST,
				SDRATTR_TEXTDIRECTION, SDRATTR_TEXTDIRECTION,

				// end
				0, 0));
		}

		AttributeProperties::AttributeProperties(SdrObject& rObj)
		:	DefaultProperties(rObj),
			mpStyleSheet(0L)
		{
		}

		AttributeProperties::AttributeProperties(const AttributeProperties& rProps, SdrObject& rObj)
		:	DefaultProperties(rProps, rObj),
			mpStyleSheet(0L)
		{
			if(rProps.GetStyleSheet())
			{
				ImpAddStyleSheet(rProps.GetStyleSheet(), sal_True);
			}
		}

		AttributeProperties::~AttributeProperties()
		{
			ImpRemoveStyleSheet();
		}
		
		BaseProperties& AttributeProperties::Clone(SdrObject& rObj) const
		{
			return *(new AttributeProperties(*this, rObj));
		}

		void AttributeProperties::ItemSetChanged(const SfxItemSet& /*rSet*/)
		{
			// own modifications
			SdrObject& rObj = GetSdrObject();

			rObj.SetChanged();
		}

		void AttributeProperties::ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem)
		{
			if(pNewItem)
			{
				const SfxPoolItem* pItem = pNewItem;

				switch( nWhich )
				{
					case XATTR_FILLBITMAP:
					{
						pItem = ((XFillBitmapItem*)pItem)->checkForUniqueItem( &GetSdrObject().getSdrModelFromSdrObject() );
						break;
					}
					case XATTR_LINEDASH:
					{
						pItem = ((XLineDashItem*)pItem)->checkForUniqueItem( &GetSdrObject().getSdrModelFromSdrObject() );
						break;
					}
					case XATTR_LINESTART:
					{
						pItem = ((XLineStartItem*)pItem)->checkForUniqueItem( &GetSdrObject().getSdrModelFromSdrObject() );
						break;
					}
					case XATTR_LINEEND:
					{
						pItem = ((XLineEndItem*)pItem)->checkForUniqueItem( &GetSdrObject().getSdrModelFromSdrObject() );
						break;
					}
					case XATTR_FILLGRADIENT:
					{
						pItem = ((XFillGradientItem*)pItem)->checkForUniqueItem( &GetSdrObject().getSdrModelFromSdrObject() );
						break;
					}
					case XATTR_FILLFLOATTRANSPARENCE:
					{
						// #85953# allow all kinds of XFillFloatTransparenceItem to be set
						pItem = ((XFillFloatTransparenceItem*)pItem)->checkForUniqueItem( &GetSdrObject().getSdrModelFromSdrObject() );
						break;
					}
					case XATTR_FILLHATCH:
					{
						pItem = ((XFillHatchItem*)pItem)->checkForUniqueItem( &GetSdrObject().getSdrModelFromSdrObject() );
						break;
					}
				}

				// set item
				if(pItem)
				{
					// force ItemSet
					GetObjectItemSet();
					mpItemSet->Put(*pItem);

					// delete item if it was a generated one
					if(pItem != pNewItem)
					{
						delete (SfxPoolItem*)pItem;
					}
				}
			}
			else
			{
				// clear item if ItemSet exists
				if(mpItemSet)
				{
					mpItemSet->ClearItem(nWhich);
				}
			}
		}

		void AttributeProperties::SetStyleSheet(SfxStyleSheet* pNewStyleSheet, sal_Bool bDontRemoveHardAttr)
		{
			ImpRemoveStyleSheet();
			ImpAddStyleSheet(pNewStyleSheet, bDontRemoveHardAttr);

			SdrObject& rObj = GetSdrObject();
			rObj.ActionChanged();
		}

		SfxStyleSheet* AttributeProperties::GetStyleSheet() const
		{
			return mpStyleSheet;
		}

		void AttributeProperties::ForceStyleToHardAttributes()
		{
			if(GetStyleSheet() && dynamic_cast< SfxStyleSheet* >(mpStyleSheet))
			{
				// prepare copied, new itemset, but WITHOUT parent
				GetObjectItemSet();
				SfxItemSet* pDestItemSet = new SfxItemSet(*mpItemSet);
				pDestItemSet->SetParent(0L);

				// pepare forgetting the current stylesheet like in RemoveStyleSheet()
				EndListening(*mpStyleSheet);
				EndListening(mpStyleSheet->GetPool());

				// prepare the iter; use the mpObjectItemSet which may have less
				// WhichIDs than the style.
				SfxWhichIter aIter(*pDestItemSet);
				sal_uInt16 nWhich(aIter.FirstWhich());
				const SfxPoolItem *pItem = NULL;

				// now set all hard attributes of the current at the new itemset
				while(nWhich)
				{
					// #i61284# use mpItemSet with parents, makes things easier and reduces to
					// one loop
					if(SFX_ITEM_SET == mpItemSet->GetItemState(nWhich, true, &pItem))
					{
						pDestItemSet->Put(*pItem);
					}

					nWhich = aIter.NextWhich();
				}

				// replace itemsets
				delete mpItemSet;
				mpItemSet = pDestItemSet;

				// set necessary changes like in RemoveStyleSheet()
				GetSdrObject().ActionChanged();

				mpStyleSheet = NULL;
			}
		}

		void AttributeProperties::Notify(SfxBroadcaster& rBC, const SfxHint& rHint)
		{
			sal_Bool bHintUsed(sal_False);

			const SfxStyleSheetHint *pStyleHint = dynamic_cast< const SfxStyleSheetHint* >( &rHint);

			if(pStyleHint && pStyleHint->GetStyleSheet() == GetStyleSheet())
			{
				SdrObject& rObj = GetSdrObject();
				const SdrObjectChangeBroadcaster aSdrObjectChangeBroadcaster(rObj, HINT_OBJCHG_ATTR);

				switch(pStyleHint->GetHint())
				{
					case SFX_STYLESHEET_CREATED			:
					{
						// cannot happen, nothing to do
						break;
					}
					case SFX_STYLESHEET_MODIFIED		:
					case SFX_STYLESHEET_CHANGED			:
					{
						// notify change
						break;
					}
					case SFX_STYLESHEET_ERASED			:
					case SFX_STYLESHEET_INDESTRUCTION	:
					{
						// Style needs to be exchanged
						SfxStyleSheet* pNewStSh = 0L;

						// #111111#
						// Do nothing if object is in destruction, else a StyleSheet may be found from
						// a StyleSheetPool which is just being deleted itself. and thus it would be fatal
						// to register as listener to that new StyleSheet.
						if(!rObj.getSdrModelFromSdrObject().IsInDestruction())
						{
							if(dynamic_cast< SfxStyleSheet* >(GetStyleSheet()))
							{
								pNewStSh = (SfxStyleSheet*)rObj.getSdrModelFromSdrObject().GetStyleSheetPool()->Find(
									GetStyleSheet()->GetParent(), GetStyleSheet()->GetFamily());
							}

							if(!pNewStSh)
							{
								pNewStSh = rObj.getSdrModelFromSdrObject().GetDefaultStyleSheet();
							}
						}

						// remove used style, it's erased or in destruction
						ImpRemoveStyleSheet();

						if(pNewStSh)
						{
							ImpAddStyleSheet(pNewStSh, sal_True);
						}

						break;
					}
				}

				// Get old BoundRect. Do this after the style change is handled
				// in the ItemSet parts because GetBoundRect() may calculate a new
				rObj.SetChanged();

				bHintUsed = sal_True;
			}

			if(!bHintUsed)
			{
				// forward to SdrObject ATM. Not sure if this will be necessary
				// in the future.
				GetSdrObject().Notify(rBC, rHint);
			}
		}
	} // end of namespace properties
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
