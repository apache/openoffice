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



#ifndef _ATTRIBUTEPROPERTIES_HXX
#define _ATTRIBUTEPROPERTIES_HXX

#ifndef _DEFAULTPROPERTIES_HXX
#include <sdr/properties/defaultproperties.hxx>
#endif
namespace binfilter {

//////////////////////////////////////////////////////////////////////////////

namespace Sdr
{
	namespace Properties
	{
		class AttributeProperties : public DefaultProperties
		{
			// add style sheet, do all the necessary handling
			void ImpAddStyleSheet(SfxStyleSheet* pNewStyleSheet, sal_Bool bDontRemoveHardAttr);

			// remove StyleSheet, do all the necessary handling
			void ImpRemoveStyleSheet();

		protected:
			// the SytleSheet of this object
			SfxStyleSheet*									mpStyleSheet;

			// create a new itemset
			virtual SfxItemSet& CreateObjectSpecificItemSet(SfxItemPool& pPool);

		public:
			// basic constructor
			AttributeProperties(SdrObject& rObj);

			// constructor for copying, but using new object
			AttributeProperties(const AttributeProperties& rProps, SdrObject& rObj);

			// Clone() operator, normally just calls the local copy constructor
			virtual Properties& Clone(SdrObject& rObj) const;

			// destructor
			virtual ~AttributeProperties();
			
			// react on ItemSet changes
			virtual void ItemSetChanged(const SfxItemSet& rSet);

			// Do the ItemChange, may do special handling
			virtual void ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem = 0);

			// set a new StyleSheet and broadcast
			virtual void SetStyleSheet(SfxStyleSheet* pNewStyleSheet, sal_Bool bDontRemoveHardAttr);

			// get the installed StyleSheet
			virtual SfxStyleSheet* GetStyleSheet() const;

			// pre/post-process saving
			virtual void PreProcessSave();
			virtual void PostProcessSave();

			// Move properties to a new ItemPool.
			virtual void MoveToItemPool(SfxItemPool* pSrcPool, SfxItemPool* pDestPool, SdrModel* pNewModel = 0L);

			// Set new model.
			virtual void SetModel(SdrModel* pOldModel, SdrModel* pNewModel);

			// force all attributes which come from styles to hard attributes
			// to be able to live without the style.
			virtual void ForceStyleToHardAttributes(sal_Bool bPseudoSheetsOnly = sal_False);

			// This is the notifyer from SfxListener
			virtual void Notify(SfxBroadcaster& rBC, const SfxHint& rHint);
		};
	} // end of namespace Properties
} // end of namespace Sdr

//////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_ATTRIBUTEPROPERTIES_HXX

// eof
