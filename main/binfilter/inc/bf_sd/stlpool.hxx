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



#ifndef _SD_STLPOOL_HXX
#define _SD_STLPOOL_HXX


#ifndef _SFXSTYLE_HXX //autogen
#include <bf_svtools/style.hxx>
#endif
namespace binfilter {

class SdStyleSheet;
class SdDrawDocument;


class SdStyleSheetPool : public SfxStyleSheetPool
{
private:
	SfxStyleSheetBase* pActualStyleSheet;
	SdDrawDocument*    pDoc;

protected:
	virtual SfxStyleSheetBase* Create(const String& rName,
									  SfxStyleFamily eFamily,
									  USHORT nMask);

public:
						SdStyleSheetPool(SfxItemPool& rPool, SdDrawDocument* pDocument);
	virtual 			~SdStyleSheetPool();

	void				SetActualStyleSheet(SfxStyleSheetBase* pActStyleSheet)
									   { pActualStyleSheet = pActStyleSheet; }
	SfxStyleSheetBase*	GetActualStyleSheet()	 { return pActualStyleSheet; }

	SfxStyleSheetBase*  GetTitleSheet(const String& rLayoutName);

							// Caller muss Liste loeschen
	List*               CreateOutlineSheetList(const String& rLayoutName);

	/** creates all layout style sheets for the givin layout name if they
		don't exist yet.

		@param rLayoutName	Must be the name of a master page
		@param bCheck		If set to true, the debug version will assert if a style
							had to be created. This is used to assert errors in documents
							when styles are missing.
	*/
	void                CreateLayoutStyleSheets(const String& rLayoutName, sal_Bool bCheck = sal_False );

	void                CreatePseudosIfNecessary();
	void				UpdateStdNames();
	void 				AdjustLRSpaceItems();
	static void			PutNumBulletItem( SfxStyleSheetBase* pSheet, Font& rBulletFont );
	Font                GetBulletFont() const;

	SdDrawDocument* 	GetDoc() const { return pDoc; }
};

} //namespace binfilter
#endif	   // _SD_STLPOOL_HXX



