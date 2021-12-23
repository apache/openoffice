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
#include "precompiled_sd.hxx"
#include <sfx2/docfile.hxx>
#include <vcl/svapp.hxx>
#include <editeng/outliner.hxx>
#ifndef _SVXLINK_HXX
#include <sfx2/linkmgr.hxx>
#endif
#include <svx/svdotext.hxx>
#include <tools/urlobj.hxx>
#include <editeng/outlobj.hxx>
#include <svl/urihelper.hxx>
#include <editeng/xmlcnitm.hxx>
#include <svx/svditer.hxx>
#include <tools/list.hxx>

#include "sdresid.hxx"
#include "sdpage.hxx"
#include "glob.hxx"
#include "glob.hrc"
#include "drawdoc.hxx"
#include "stlpool.hxx"
//#include "sdiocmpt.hxx"
#include "pglink.hxx"
//#include "strmname.h"
#include "anminfo.hxx"

#include "../ui/inc/strings.hrc"
#include "../ui/inc/DrawDocShell.hxx"

// #90477#
#include <tools/tenccvt.hxx>
#include <svl/itemset.hxx>

using namespace ::sd;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::office;

extern void NotifyDocumentEvent( SdDrawDocument* pDocument, const rtl::OUString& rEventName, const Reference< XInterface >& xSource );

/*************************************************************************
|*
|* SetPresentationLayout, setzt: Layoutnamen, Masterpage-Verkn�pfung und
|* Vorlagen fuer Praesentationsobjekte
|*
|* Vorraussetzungen: - Die Seite muss bereits das richtige Model kennen!
|*					 - Die entsprechende Masterpage muss bereits im Model sein.
|*					 - Die entsprechenden StyleSheets muessen bereits im
|*					   im StyleSheetPool sein.
|*
|*  bReplaceStyleSheets = sal_True : Benannte StyleSheets werden ausgetauscht
|*                        sal_False: Alle StyleSheets werden neu zugewiesen
|*
|*  bSetMasterPage      = sal_True : MasterPage suchen und zuweisen
|*
|*  bReverseOrder       = sal_False: MasterPages von vorn nach hinten suchen
|*                        sal_True : MasterPages von hinten nach vorn suchen (fuer Undo-Action)
|*
\************************************************************************/

void SdPage::SetPresentationLayout(const String& rLayoutName,
								   sal_Bool bReplaceStyleSheets,
								   sal_Bool bSetMasterPage,
								   sal_Bool bReverseOrder)
{
	/*********************************************************************
	|* Layoutname der Seite
	\********************************************************************/
	String aOldLayoutName(maLayoutName); // merken
	maLayoutName = rLayoutName;
	maLayoutName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( SD_LT_SEPARATOR ));
	maLayoutName += String(SdResId(STR_LAYOUT_OUTLINE));

	/*********************************************************************
	|* ggf. Masterpage suchen und setzen
	\********************************************************************/
	if (bSetMasterPage && !IsMasterPage())
	{
		SdPage* pMaster;
		SdPage* pFoundMaster = 0;
		sal_uInt16 nMaster = 0;
		sal_uInt16 nMasterCount = pModel->GetMasterPageCount();

		if( !bReverseOrder )
		{
			for ( nMaster = 0; nMaster < nMasterCount; nMaster++ )
			{
				pMaster = static_cast<SdPage*>(pModel->GetMasterPage(nMaster));
				if (pMaster->GetPageKind() == mePageKind && pMaster->GetLayoutName() == maLayoutName)
				{
					pFoundMaster = pMaster;
					break;
				}
			}
		}
		else
		{
			for ( nMaster = nMasterCount; nMaster > 0; nMaster-- )
			{
				pMaster = static_cast<SdPage*>(pModel->GetMasterPage(nMaster - 1));
				if (pMaster->GetPageKind() == mePageKind && pMaster->GetLayoutName() == maLayoutName)
				{
					pFoundMaster = pMaster;
					break;
				}
			}
		}

		DBG_ASSERT(pFoundMaster, "Masterpage for presentation layout not found!");

		// this should never happen, but we play failsafe here
		if( pFoundMaster == 0 )
			pFoundMaster = static_cast< SdDrawDocument *>(pModel)->GetSdPage( 0, mePageKind );

		if( pFoundMaster )
			TRG_SetMasterPage(*pFoundMaster);
	}

	/*********************************************************************
	|* Vorlagen fuer Praesentationsobjekte
	\********************************************************************/
	// Listen mit:
	// - Vorlagenzeigern fuer Gliederungstextobjekt (alte und neue Vorlagen)
	// - Replacedaten fuer OutlinerParaObject
	List aOutlineStyles;
	List aOldOutlineStyles;
	List aReplList;
	sal_Bool bListsFilled = sal_False;

	sal_uLong nObjCount = GetObjCount();

	for (sal_uLong nObj = 0; nObj < nObjCount; nObj++)
	{
		SdrTextObj* pObj = (SdrTextObj*) GetObj(nObj);

		if (pObj->GetObjInventor() == SdrInventor &&
			pObj->GetObjIdentifier() == OBJ_OUTLINETEXT)
		{
			if (!bListsFilled || !bReplaceStyleSheets)
			{
				String aFullName;
				String aOldFullName;
				SfxStyleSheetBase* pSheet = NULL;
				SfxStyleSheetBasePool* pStShPool = pModel->GetStyleSheetPool();

				for (sal_Int16 i = -1; i < 9; i++)
				{
					aFullName = maLayoutName;
					aOldFullName = aOldLayoutName;
					aFullName += sal_Unicode( ' ' );
					aFullName += String::CreateFromInt32( (sal_Int32) (i <= 0 ) ? 1 : i + 1);
					aOldFullName += sal_Unicode( ' ' );
					aOldFullName += String::CreateFromInt32( (sal_Int32) (i <= 0 ) ? 1 : i + 1 );

					pSheet = pStShPool->Find(aOldFullName, SD_STYLE_FAMILY_MASTERPAGE);
					DBG_ASSERT(pSheet, "alte Gliederungsvorlage nicht gefunden");
					aOldOutlineStyles.Insert(pSheet, LIST_APPEND);

					pSheet = pStShPool->Find(aFullName, SD_STYLE_FAMILY_MASTERPAGE);
					DBG_ASSERT(pSheet, "neue Gliederungsvorlage nicht gefunden");
					aOutlineStyles.Insert(pSheet, LIST_APPEND);

					if (bReplaceStyleSheets && pSheet)
					{
						// Replace anstatt Set
						StyleReplaceData* pReplData = new StyleReplaceData;
						pReplData->nNewFamily = pSheet->GetFamily();
						pReplData->nFamily    = pSheet->GetFamily();
						pReplData->aNewName   = aFullName;
						pReplData->aName      = aOldFullName;
						aReplList.Insert(pReplData, LIST_APPEND);
					}
					else
					{
						OutlinerParaObject* pOPO = ((SdrTextObj*)pObj)->GetOutlinerParaObject();

						if( pOPO )
							pOPO->SetStyleSheets( i, aFullName, SD_STYLE_FAMILY_MASTERPAGE );
					}
				}

				bListsFilled = sal_True;
			}

			SfxStyleSheet* pSheet = (SfxStyleSheet*)aOutlineStyles.First();
			SfxStyleSheet* pOldSheet = (SfxStyleSheet*)aOldOutlineStyles.First();
			while (pSheet)
			{
				if (pSheet != pOldSheet)
				{
					pObj->EndListening(*pOldSheet);

					if (!pObj->IsListening(*pSheet))
						pObj->StartListening(*pSheet);
				}

				pSheet = (SfxStyleSheet*)aOutlineStyles.Next();
				pOldSheet = (SfxStyleSheet*)aOldOutlineStyles.Next();
			}

			OutlinerParaObject* pOPO = ((SdrTextObj*)pObj)->GetOutlinerParaObject();
			if ( bReplaceStyleSheets && pOPO )
			{
				StyleReplaceData* pReplData = (StyleReplaceData*) aReplList.First();

				while( pReplData )
				{
					pOPO->ChangeStyleSheets( pReplData->aName, pReplData->nFamily, pReplData->aNewName, pReplData->nNewFamily );
					pReplData = (StyleReplaceData*) aReplList.Next();
				}
			}
		}
		else if (pObj->GetObjInventor() == SdrInventor &&
				 pObj->GetObjIdentifier() == OBJ_TITLETEXT)
		{
			// PresObjKind nicht ueber GetPresObjKind() holen, da dort nur
			// die PresObjListe beruecksichtigt wird. Es sollen aber alle
			// "Titelobjekte" hier beruecksichtigt werden (Paste aus Clipboard usw.)
			SfxStyleSheet* pSheet = GetStyleSheetForPresObj(PRESOBJ_TITLE);

			if (pSheet)
				pObj->SetStyleSheet(pSheet, sal_True);
		}
		else
		{
			SfxStyleSheet* pSheet = GetStyleSheetForPresObj(GetPresObjKind(pObj));

			if (pSheet)
				pObj->SetStyleSheet(pSheet, sal_True);
		}
	}

	for (sal_uLong i = 0; i < aReplList.Count(); i++)
	{
		delete (StyleReplaceData*) aReplList.GetObject(i);
	}
}


/*************************************************************************
|* das Gliederungstextobjekt bei den Vorlagen fuer die Gliederungsebenen
|* abmelden
\************************************************************************/

void SdPage::EndListenOutlineText()
{
	SdrObject* pOutlineTextObj = GetPresObj(PRESOBJ_OUTLINE);

	if (pOutlineTextObj)
	{
		SdStyleSheetPool* pSPool = (SdStyleSheetPool*)pModel->GetStyleSheetPool();
		DBG_ASSERT(pSPool, "StyleSheetPool nicht gefunden");
		String aTrueLayoutName(maLayoutName);
		aTrueLayoutName.Erase( aTrueLayoutName.SearchAscii( SD_LT_SEPARATOR ));
		List* pOutlineStyles = pSPool->CreateOutlineSheetList(aTrueLayoutName);
		for (SfxStyleSheet* pSheet = (SfxStyleSheet*)pOutlineStyles->First();
			 pSheet;
			 pSheet = (SfxStyleSheet*)pOutlineStyles->Next())
			{
				pOutlineTextObj->EndListening(*pSheet);
			}

		delete pOutlineStyles;
	}
}

/*************************************************************************
|* Neues Modell setzen
\************************************************************************/

void SdPage::SetModel(SdrModel* pNewModel)
{
	DisconnectLink();

	// Model umsetzen
	FmFormPage::SetModel(pNewModel);

	ConnectLink();
}

/*************************************************************************
|* Ist die Seite read-only?
\************************************************************************/

FASTBOOL SdPage::IsReadOnly() const
{
	return sal_False;
}

/*************************************************************************
|* Beim sfx2::LinkManager anmelden
\************************************************************************/

void SdPage::ConnectLink()
{
	sfx2::LinkManager* pLinkManager = pModel!=NULL ? pModel->GetLinkManager() : NULL;

	if (pLinkManager && !mpPageLink && maFileName.Len() && maBookmarkName.Len() &&
		mePageKind==PK_STANDARD && !IsMasterPage() &&
		( (SdDrawDocument*) pModel)->IsNewOrLoadCompleted())
	{
		/**********************************************************************
		* Anmelden
		* Nur Standardseiten duerfen gelinkt sein
		**********************************************************************/
		::sd::DrawDocShell* pDocSh = ((SdDrawDocument*) pModel)->GetDocSh();

		if (!pDocSh || pDocSh->GetMedium()->GetOrigURL() != maFileName)
		{
			// Keine Links auf Dokument-eigene Seiten!
			mpPageLink = new SdPageLink(this, maFileName, maBookmarkName);
			String aFilterName(SdResId(STR_IMPRESS));
			pLinkManager->InsertFileLink(*mpPageLink, OBJECT_CLIENT_FILE,
										 maFileName, &aFilterName, &maBookmarkName);
			mpPageLink->Connect();
		}
	}
}


/*************************************************************************
|* Beim sfx2::LinkManager abmelden
\************************************************************************/

void SdPage::DisconnectLink()
{
	sfx2::LinkManager* pLinkManager = pModel!=NULL ? pModel->GetLinkManager() : NULL;

	if (pLinkManager && mpPageLink)
	{
		/**********************************************************************
		* Abmelden
		* (Bei Remove wird *pGraphicLink implizit deleted)
		**********************************************************************/
		pLinkManager->Remove(mpPageLink);
		mpPageLink=NULL;
	}
}

/*************************************************************************
|* Copy-Ctor
\************************************************************************/

SdPage::SdPage(const SdPage& rSrcPage)
:	FmFormPage(rSrcPage)
,	SdrObjUserCall()
,	mpItems(NULL)
{
	mePageKind				= rSrcPage.mePageKind;
	meAutoLayout			= rSrcPage.meAutoLayout;

	SdrObject* pObj = 0;
	while((pObj = rSrcPage.maPresentationShapeList.getNextShape(pObj)) != 0)
		InsertPresObj(GetObj(pObj->GetOrdNum()), rSrcPage.GetPresObjKind(pObj));

	mbSelected				= sal_False;
	mnTransitionType		= rSrcPage.mnTransitionType;
	mnTransitionSubtype		= rSrcPage.mnTransitionSubtype;
	mbTransitionDirection	= rSrcPage.mbTransitionDirection;
	mnTransitionFadeColor	= rSrcPage.mnTransitionFadeColor;
	mfTransitionDuration	= rSrcPage.mfTransitionDuration;
	mePresChange			= rSrcPage.mePresChange;
	mnTime					= rSrcPage.mnTime;
	mbSoundOn				= rSrcPage.mbSoundOn;
	mbExcluded				= rSrcPage.mbExcluded;

	maLayoutName			= rSrcPage.maLayoutName;
	maSoundFile				= rSrcPage.maSoundFile;
	mbLoopSound				= rSrcPage.mbLoopSound;
	mbStopSound				= rSrcPage.mbStopSound;
	maCreatedPageName		= String();
	maFileName				= rSrcPage.maFileName;
	maBookmarkName			= rSrcPage.maBookmarkName;
	mbScaleObjects			= rSrcPage.mbScaleObjects;
	mbBackgroundFullSize	= rSrcPage.mbBackgroundFullSize;
	meCharSet				= rSrcPage.meCharSet;
	mnPaperBin				= rSrcPage.mnPaperBin;
	meOrientation			= rSrcPage.meOrientation;

	// header footer
	setHeaderFooterSettings( rSrcPage.getHeaderFooterSettings() );

	mpPageLink				= NULL; // Wird beim Einfuegen ueber ConnectLink() gesetzt
}



/*************************************************************************
|* Clone
\************************************************************************/

SdrPage* SdPage::Clone() const
{
	return Clone(NULL);
}

SdrPage* SdPage::Clone(SdrModel* pNewModel) const
{
	DBG_ASSERT( pNewModel == 0, "sd::SdPage::Clone(), new page ignored, please check code! CL" );
	(void)pNewModel;

	SdPage* pNewPage = new SdPage(*this);

	cloneAnimations( *pNewPage );

	// fix user calls for duplicated slide
	SdrObjListIter aSourceIter( *this, IM_DEEPWITHGROUPS );
	SdrObjListIter aTargetIter( *pNewPage, IM_DEEPWITHGROUPS );

	while( aSourceIter.IsMore() && aTargetIter.IsMore() )
	{
		SdrObject* pSource = aSourceIter.Next();
		SdrObject* pTarget = aTargetIter.Next();

		if( pSource->GetUserCall() )
			pTarget->SetUserCall( pNewPage );
	}

	return pNewPage;
}

/*************************************************************************
|* GetTextStyleSheetForObject
\************************************************************************/


SfxStyleSheet* SdPage::GetTextStyleSheetForObject( SdrObject* pObj ) const
{
	const PresObjKind eKind = ((SdPage*)this)->GetPresObjKind(pObj);
	if( eKind != PRESOBJ_NONE )
	{
		return ((SdPage*)this)->GetStyleSheetForPresObj(eKind);
	}

	return FmFormPage::GetTextStyleSheetForObject( pObj );
}

SfxItemSet* SdPage::getOrCreateItems()
{
	if( mpItems == NULL )
		mpItems = new SfxItemSet( pModel->GetItemPool(), SDRATTR_XMLATTRIBUTES, SDRATTR_XMLATTRIBUTES );

	return mpItems;
}


sal_Bool SdPage::setAlienAttributes( const com::sun::star::uno::Any& rAttributes )
{
	SfxItemSet* pSet = getOrCreateItems();

	SvXMLAttrContainerItem aAlienAttributes( SDRATTR_XMLATTRIBUTES );
	if( aAlienAttributes.PutValue( rAttributes, 0 ) )
	{
		pSet->Put( aAlienAttributes );
		return sal_True;
	}

	return sal_False;
}

void SdPage::getAlienAttributes( com::sun::star::uno::Any& rAttributes )
{
	const SfxPoolItem* pItem;

	if( (mpItems == NULL) || ( SFX_ITEM_SET != mpItems->GetItemState( SDRATTR_XMLATTRIBUTES, sal_False, &pItem ) ) )
	{
		SvXMLAttrContainerItem aAlienAttributes;
		aAlienAttributes.QueryValue( rAttributes, 0 );
	}
	else
	{
		((SvXMLAttrContainerItem*)pItem)->QueryValue( rAttributes, 0 );
	}
}

void SdPage::RemoveEmptyPresentationObjects()
{
	SdrObjListIter	aShapeIter( *this, IM_DEEPWITHGROUPS );

	SdrObject* pShape;
	for( pShape = aShapeIter.Next(); pShape; pShape = aShapeIter.Next() )
	{
		if( pShape && pShape->IsEmptyPresObj() )
		{
			RemoveObject( pShape->GetOrdNum() );
			SdrObject::Free( pShape );
		}

	}
}

sal_Int16 SdPage::getTransitionType (void) const
{
	return mnTransitionType;
}

void SdPage::setTransitionType( sal_Int16 nTransitionType )
{
	mnTransitionType = nTransitionType;
	ActionChanged();
}

sal_Int16 SdPage::getTransitionSubtype (void) const
{
	return mnTransitionSubtype;
}

void SdPage::setTransitionSubtype ( sal_Int16 nTransitionSubtype )
{
	mnTransitionSubtype = nTransitionSubtype;
	ActionChanged();
}

sal_Bool SdPage::getTransitionDirection (void) const
{
	return mbTransitionDirection;
}

void SdPage::setTransitionDirection ( sal_Bool bTransitionbDirection )
{
	mbTransitionDirection = bTransitionbDirection;
	ActionChanged();
}

sal_Int32 SdPage::getTransitionFadeColor (void) const
{
	return mnTransitionFadeColor;
}

void SdPage::setTransitionFadeColor ( sal_Int32 nTransitionFadeColor )
{
	mnTransitionFadeColor = nTransitionFadeColor;
	ActionChanged();
}

double SdPage::getTransitionDuration (void) const
{
	return mfTransitionDuration;
}

void SdPage::setTransitionDuration ( double fTranstionDuration )
{
	mfTransitionDuration = fTranstionDuration;
	ActionChanged();
}

namespace sd {
extern void createAnnotation( Reference< XAnnotation >& xAnnotation, SdPage* pPage );
extern SdrUndoAction* CreateUndoInsertOrRemoveAnnotation( const Reference< XAnnotation >& xAnnotation, bool bInsert );
}

void SdPage::createAnnotation( ::com::sun::star::uno::Reference< ::com::sun::star::office::XAnnotation >& xAnnotation )
{
	sd::createAnnotation( xAnnotation, this );
}

void SdPage::addAnnotation( const Reference< XAnnotation >& xAnnotation, int nIndex )
{
	if( (nIndex == -1) || (nIndex > (int)maAnnotations.size()) )
	{
		maAnnotations.push_back( xAnnotation );
	}
	else
	{
		maAnnotations.insert( maAnnotations.begin() + nIndex, xAnnotation );
	}

	if( pModel && pModel->IsUndoEnabled() )
	{
		SdrUndoAction* pAction = CreateUndoInsertOrRemoveAnnotation( xAnnotation, true );
		if( pAction )
			pModel->AddUndo( pAction );
	}

	SetChanged();

	if( pModel )
	{
		pModel->SetChanged();
		Reference< XInterface > xSource( xAnnotation, UNO_QUERY );
		NotifyDocumentEvent( static_cast< SdDrawDocument* >( pModel ), rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "OnAnnotationInserted" ) ), xSource );
	}
}

void SdPage::removeAnnotation( const Reference< XAnnotation >& xAnnotation )
{
	if( pModel && pModel->IsUndoEnabled() )
	{
		SdrUndoAction* pAction = CreateUndoInsertOrRemoveAnnotation( xAnnotation, false );
		if( pAction )
			pModel->AddUndo( pAction );
	}

	AnnotationVector::iterator iter = std::find( maAnnotations.begin(), maAnnotations.end(), xAnnotation );
	if( iter != maAnnotations.end() )
		maAnnotations.erase( iter );

	if( pModel )
	{
		pModel->SetChanged();
		Reference< XInterface > xSource( xAnnotation, UNO_QUERY );
		NotifyDocumentEvent( static_cast< SdDrawDocument* >( pModel ), rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "OnAnnotationRemoved" ) ), xSource );
	}
}

