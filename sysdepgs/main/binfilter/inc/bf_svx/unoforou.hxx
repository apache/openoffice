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



#ifndef _SVX_UNOFOROU_HXX
#define _SVX_UNOFOROU_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVX_UNOEDSRC_HXX
#include <bf_svx/unoedsrc.hxx>
#endif

#include <bf_svx/editdata.hxx>
namespace binfilter {

class Outliner;
class SdrObject;

//	SvxOutlinerForwarder - SvxTextForwarder fuer Outliner

class SvxOutlinerForwarder : public SvxTextForwarder
{
private:
	Outliner&			rOutliner;
    SdrObject*			pSdrObject;

	/** this pointer may be null or point to an item set for the attribs of
		the selection maAttribsSelection */
	mutable SfxItemSet* mpAttribsCache;

	/** if we have a chached attribute item set, this is the selection of it */
	mutable ESelection	maAttribCacheSelection;

	/** this pointer may be null or point to an item set for the paragraph
		mnParaAttribsCache */
	mutable SfxItemSet*	mpParaAttribsCache;

	/** if we have a cached para attribute item set, this is the paragraph of it */
	mutable USHORT		mnParaAttribsCache;

public:
						SvxOutlinerForwarder( Outliner& rOutl, SdrObject* pSdrObj );
	virtual				~SvxOutlinerForwarder();

	virtual USHORT		GetParagraphCount() const;
	virtual USHORT		GetTextLen( USHORT nParagraph ) const;
	virtual String		GetText( const ESelection& rSel ) const;
	virtual SfxItemSet	GetAttribs( const ESelection& rSel, BOOL bOnlyHardAttrib = 0 ) const;
	virtual	SfxItemSet	GetParaAttribs( USHORT nPara ) const;
	virtual void		SetParaAttribs( USHORT nPara, const SfxItemSet& rSet );
	virtual void		GetPortions( USHORT nPara, SvUShorts& rList ) const;

	virtual USHORT		GetItemState( const ESelection& rSel, USHORT nWhich ) const;
	virtual USHORT		GetItemState( USHORT nPara, USHORT nWhich ) const;

	virtual void		QuickInsertText( const String& rText, const ESelection& rSel );
	virtual void		QuickInsertField( const SvxFieldItem& rFld, const ESelection& rSel );
	virtual void		QuickSetAttribs( const SfxItemSet& rSet, const ESelection& rSel );
	virtual void		QuickInsertLineBreak( const ESelection& rSel );

	virtual SfxItemPool* GetPool() const;

	virtual XubString    CalcFieldValue( const SvxFieldItem& rField, USHORT nPara, USHORT nPos, Color*& rpTxtColor, Color*& rpFldColor );
	virtual BOOL		IsValid() const;

	Outliner&			GetOutliner() const { return rOutliner; }

    virtual LanguageType 	GetLanguage( USHORT, USHORT ) const;
    virtual USHORT			GetFieldCount( USHORT nPara ) const;
    virtual EFieldInfo		GetFieldInfo( USHORT nPara, USHORT nField ) const;
    virtual EBulletInfo     GetBulletInfo( USHORT nPara ) const;
    virtual Rectangle		GetCharBounds( USHORT nPara, USHORT nIndex ) const;
    virtual Rectangle		GetParaBounds( USHORT nPara ) const;
    virtual MapMode			GetMapMode() const;
	virtual OutputDevice*	GetRefDevice() const;
    virtual sal_Bool		GetIndexAtPoint( const Point&, USHORT& nPara, USHORT& nIndex ) const;
    virtual sal_Bool		GetWordIndices( USHORT nPara, USHORT nIndex, USHORT& nStart, USHORT& nEnd ) const;
    virtual sal_Bool 		GetAttributeRun( USHORT& nStartIndex, USHORT& nEndIndex, USHORT nPara, USHORT nIndex ) const;
    virtual USHORT			GetLineCount( USHORT nPara ) const;
    virtual USHORT			GetLineLen( USHORT nPara, USHORT nLine ) const;
    virtual sal_Bool		Delete( const ESelection& );
    virtual sal_Bool		InsertText( const String&, const ESelection& );
    virtual sal_Bool		QuickFormatDoc( BOOL bFull=FALSE );
    virtual USHORT			GetDepth( USHORT nPara ) const;
    virtual sal_Bool		SetDepth( USHORT nPara, USHORT nNewDepth );

	/* this method flushes internal caches for this forwarder */
	void				flushCache();
};

}//end of namespace binfilter
#endif

