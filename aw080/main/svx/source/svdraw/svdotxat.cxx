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

#include <svl/style.hxx>
#include <svx/svdotext.hxx>
#include <editeng/editdata.hxx>
#include <svx/svdmodel.hxx>
#include <svx/svditext.hxx>
#include <svx/svdoutl.hxx>
#include <svx/svdorect.hxx> // fuer SetDirty bei AdjustTextFrameWidthAndHeight
#include <svx/svdocapt.hxx> // fuer SetDirty bei AdjustTextFrameWidthAndHeight
#include <svx/svdetc.hxx>
#include <editeng/writingmodeitem.hxx>
#include <editeng/editeng.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/flditem.hxx>
#include <svx/sdtfchim.hxx>
#include <editeng/editview.hxx>
#include <svl/smplhint.hxx>
#include <svl/whiter.hxx>
#include <editeng/outlobj.hxx>
#include <editeng/outliner.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/editobj.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/charscaleitem.hxx>
#include <svl/style.hxx>
#include <svl/itemiter.hxx>
#include <editeng/lrspitem.hxx>
#include <svl/itempool.hxx>
#include <editeng/numitem.hxx>
#include <editeng/editeng.hxx>
#include <editeng/postitem.hxx>
#include <svx/svdlegacy.hxx>
#include <svx/svdtrans.hxx>

////////////////////////////////////////////////////////////////////////////////////////////////////

bool SdrTextObj::AdjustTextFrameWidthAndHeight(Rectangle& rR, bool bHgt, bool bWdt) const
{
	if(bTextFrame)
	{
		if(!rR.IsEmpty())
    	{
	    	SdrFitToSizeType eFit=GetFitToSize();
			bool bFitToSize=(eFit==SDRTEXTFIT_PROPORTIONAL || eFit==SDRTEXTFIT_ALLLINES);
			bool bWdtGrow=bWdt && IsAutoGrowWidth();
			bool bHgtGrow=bHgt && IsAutoGrowHeight();
		    SdrTextAniKind eAniKind=GetTextAniKind();
		    SdrTextAniDirection eAniDir=GetTextAniDirection();
			bool bScroll=eAniKind==SDRTEXTANI_SCROLL || eAniKind==SDRTEXTANI_ALTERNATE || eAniKind==SDRTEXTANI_SLIDE;
			bool bHScroll=bScroll && (eAniDir==SDRTEXTANI_LEFT || eAniDir==SDRTEXTANI_RIGHT);
			bool bVScroll=bScroll && (eAniDir==SDRTEXTANI_UP || eAniDir==SDRTEXTANI_DOWN);
		    
            if (!bFitToSize && (bWdtGrow || bHgtGrow))
		    {
    			Rectangle aR0(rR);
				sal_Int32 nHgt=0,nMinHgt=0,nMaxHgt=0;
				sal_Int32 nWdt=0,nMinWdt=0,nMaxWdt=0;
	    		Size aSiz(rR.GetSize()); aSiz.Width()--; aSiz.Height()--;
		    	Size aMaxSiz(100000,100000);
			
				if(!basegfx::fTools::equalZero(getSdrModelFromSdrObject().GetMaxObjectScale().getX())) 
				{
					aMaxSiz.Width() = basegfx::fround(getSdrModelFromSdrObject().GetMaxObjectScale().getX());
				}

				if(!basegfx::fTools::equalZero(getSdrModelFromSdrObject().GetMaxObjectScale().getY())) 
				{
					aMaxSiz.Height() = basegfx::fround(getSdrModelFromSdrObject().GetMaxObjectScale().getY());
				}
			
			    if (bWdtGrow)
			    {
				    nMinWdt=GetMinTextFrameWidth();
				    nMaxWdt=GetMaxTextFrameWidth();
				    if (nMaxWdt==0 || nMaxWdt>aMaxSiz.Width()) nMaxWdt=aMaxSiz.Width();
				    if (nMinWdt<=0) nMinWdt=1;
				    aSiz.Width()=nMaxWdt;
			    }
			    if (bHgtGrow)
			    {
				    nMinHgt=GetMinTextFrameHeight();
				    nMaxHgt=GetMaxTextFrameHeight();
				    if (nMaxHgt==0 || nMaxHgt>aMaxSiz.Height()) nMaxHgt=aMaxSiz.Height();
				    if (nMinHgt<=0) nMinHgt=1;
				    aSiz.Height()=nMaxHgt;
			    }

                sal_Int32 nHDist=GetTextLeftDistance()+GetTextRightDistance();
				sal_Int32 nVDist=GetTextUpperDistance()+GetTextLowerDistance();
			    aSiz.Width()-=nHDist;
			    aSiz.Height()-=nVDist;
			    if (aSiz.Width()<2) aSiz.Width()=2;   // Mindestgroesse 2
			    if (aSiz.Height()<2) aSiz.Height()=2; // Mindestgroesse 2

			    // #101684#
				bool bInEditMode = IsInEditMode();

			    if(!bInEditMode)
			    {
				    if (bHScroll) aSiz.Width()=0x0FFFFFFF; // Laufschrift nicht umbrechen
				    if (bVScroll) aSiz.Height()=0x0FFFFFFF;
			    }

			    if(pEdtOutl)
			    {
				    pEdtOutl->SetMaxAutoPaperSize(aSiz);
				
                    if (bWdtGrow) 
                    {
					    Size aSiz2(pEdtOutl->CalcTextSize());
					    nWdt=aSiz2.Width()+1; // lieber etwas Tolleranz
					    if (bHgtGrow) nHgt=aSiz2.Height()+1; // lieber etwas Tolleranz
				    } 
                    else 
                    {
					    nHgt=pEdtOutl->GetTextHeight()+1; // lieber etwas Tolleranz
				    }
			    } 
                else 
                {
				    Outliner& rOutliner=ImpGetDrawOutliner();
				    rOutliner.SetPaperSize(aSiz);
					rOutliner.SetUpdateMode(true);
				    // !!! hier sollte ich wohl auch noch mal die Optimierung mit
				    // bPortionInfoChecked usw einbauen
				    OutlinerParaObject* pOutlinerParaObject = GetOutlinerParaObject();
					
                    if ( pOutlinerParaObject != 0 )
				    {
					    rOutliner.SetText(*pOutlinerParaObject);
					    rOutliner.SetFixedCellHeight(((const SdrTextFixedCellHeightItem&)GetMergedItem(SDRATTR_TEXT_USEFIXEDCELLHEIGHT)).GetValue());
				    }
				    if (bWdtGrow)
				    {
					    Size aSiz2(rOutliner.CalcTextSize());
					    nWdt=aSiz2.Width()+1; // lieber etwas Tolleranz
					    if (bHgtGrow) nHgt=aSiz2.Height()+1; // lieber etwas Tolleranz
				    } 
                    else 
                    {
					    nHgt=rOutliner.GetTextHeight()+1; // lieber etwas Tolleranz
				    }
				    
                    rOutliner.Clear();
			    }

                if (nWdt<nMinWdt) 
                    nWdt=nMinWdt;

			    if (nWdt>nMaxWdt) 
                    nWdt=nMaxWdt;

			    nWdt+=nHDist;
			    
                // nHDist kann auch negativ sein
                if (nWdt<1) 
                    nWdt=1;

                if (nHgt<nMinHgt) 
                    nHgt=nMinHgt;

			    if (nHgt>nMaxHgt) 
                    nHgt=nMaxHgt;

			    nHgt+=nVDist;
			    
                // nVDist kann auch negativ sein
                if (nHgt<1) 
                    nHgt=1;

                sal_Int32 nWdtGrow=nWdt-(rR.Right()-rR.Left());
				sal_Int32 nHgtGrow=nHgt-(rR.Bottom()-rR.Top());
				
                if (nWdtGrow==0) 
                    bWdtGrow=false;

				if (nHgtGrow==0) 
                    bHgtGrow=false;
    			
                if (bWdtGrow || bHgtGrow) 
                {
				    if (bWdtGrow) 
                    {
					    SdrTextHorzAdjust eHAdj=GetTextHorizontalAdjust();
					    if (eHAdj==SDRTEXTHORZADJUST_LEFT) 
                            rR.Right()+=nWdtGrow;
					    else if (eHAdj==SDRTEXTHORZADJUST_RIGHT) 
                            rR.Left()-=nWdtGrow;
					    else 
                        {
							    sal_Int32 nWdtGrow2=nWdtGrow/2;
						    rR.Left()-=nWdtGrow2;
						    rR.Right()=rR.Left()+nWdt;
					    }
				    }
    				if (bHgtGrow) 
                    {
					    SdrTextVertAdjust eVAdj=GetTextVerticalAdjust();
					    if (eVAdj==SDRTEXTVERTADJUST_TOP) 
                            rR.Bottom()+=nHgtGrow;
					    else if (eVAdj==SDRTEXTVERTADJUST_BOTTOM) 
                            rR.Top()-=nHgtGrow;
    					else 
                        {
							sal_Int32 nHgtGrow2=nHgtGrow/2;
						    rR.Top()-=nHgtGrow2;
						    rR.Bottom()=rR.Top()+nHgt;
					    }
    				}
	
                    const sal_Int32 aOldRotation(sdr::legacy::GetRotateAngle(*this));
					
                    if (aOldRotation) 
                    {
					    Point aD1(rR.TopLeft());
					    aD1-=aR0.TopLeft();
					    Point aD2(aD1);
						RotatePoint(aD2,Point(),sin(aOldRotation*nPi180), cos(aOldRotation*nPi180));
    					aD2-=aD1;
	    				rR.Move(aD2.X(),aD2.Y());
		    		}
					
                    return true;
			    }
		    }
	    }
    }

	return false;
}

bool SdrTextObj::AdjustTextFrameWidthAndHeight(bool bHgt, bool bWdt)
{
	Rectangle aNeuRect(sdr::legacy::GetLogicRect(*this));
	bool bRet=AdjustTextFrameWidthAndHeight(aNeuRect,bHgt,bWdt);
	
	if (bRet) 
    {
        const SdrObjectChangeBroadcaster aSdrObjectChangeBroadcaster(*this);
		sdr::legacy::SetLogicRect(*this, aNeuRect);
		
		if (dynamic_cast< SdrCaptionObj* >(this)) 
		{ 
			// mal wieder 'nen Hack
			((SdrCaptionObj*)this)->ImpRecalcTail();
		}
		
		SetChanged();
	}

	return bRet;
}

void SdrTextObj::ImpSetTextStyleSheetListeners()
{
	SfxStyleSheetBasePool* pStylePool = getSdrModelFromSdrObject().GetStyleSheetPool();

	if (pStylePool!=0)
	{
		Container aStyles(1024,64,64);
		OutlinerParaObject* pOutlinerParaObject = GetOutlinerParaObject();
	
        if(pOutlinerParaObject)
		{
			// Zunaechst werden alle im ParaObject enthaltenen StyleSheets
			// im Container aStyles gesammelt. Dazu wird die Family jeweils
			// ans Ende des StyleSheet-Namen drangehaengt.
			const EditTextObject& rTextObj=pOutlinerParaObject->GetTextObject();
			XubString aStyleName;
			SfxStyleFamily eStyleFam;
			sal_uInt16 nParaAnz=rTextObj.GetParagraphCount();

			for(sal_uInt16 nParaNum(0); nParaNum < nParaAnz; nParaNum++)
			{
				rTextObj.GetStyleSheet(nParaNum, aStyleName, eStyleFam);

				if(aStyleName.Len())
				{
					XubString aFam = UniString::CreateFromInt32((sal_uInt16)eStyleFam);
					aFam.Expand(5);

					aStyleName += sal_Unicode('|');
					aStyleName += aFam;

					bool bFnd(false);
					sal_uInt32 nNum(aStyles.Count());

					while(!bFnd && nNum > 0)
					{
						// kein StyleSheet doppelt!
						nNum--;
						bFnd = (aStyleName.Equals(*(XubString*)aStyles.GetObject(nNum)));
					}

					if(!bFnd)
					{
						aStyles.Insert(new XubString(aStyleName), CONTAINER_APPEND);
					}
				}
			}
		}

		// nun die Strings im Container durch StyleSheet* ersetzten
		sal_uIntPtr nNum=aStyles.Count();
		
        while (nNum>0) 
        {
			nNum--;
			XubString* pName=(XubString*)aStyles.GetObject(nNum);

			// UNICODE: String aFam(pName->Cut(pName->Len()-6));
			String aFam = pName->Copy(0, pName->Len() - 6);

			aFam.Erase(0,1);
			aFam.EraseTrailingChars();

			// UNICODE: sal_uInt16 nFam=sal_uInt16(aFam);
			sal_uInt16 nFam = (sal_uInt16)aFam.ToInt32();

			SfxStyleFamily eFam=(SfxStyleFamily)nFam;
			SfxStyleSheetBase* pStyleBase=pStylePool->Find(*pName,eFam);
			SfxStyleSheet* pStyle = dynamic_cast< SfxStyleSheet* >( pStyleBase);
			delete pName;
		
            if (pStyle!=0 && pStyle!=GetStyleSheet()) 
            {
				aStyles.Replace(pStyle,nNum);
			} 
            else 
            {
				aStyles.Remove(nNum);
			}
		}

        // jetzt alle ueberfluessigen StyleSheets entfernen
		nNum=GetBroadcasterCount();
		
        while (nNum>0) 
        {
			nNum--;
			SfxBroadcaster* pBroadcast=GetBroadcasterJOE((sal_uInt16)nNum);
			SfxStyleSheet* pStyle = dynamic_cast< SfxStyleSheet* >( pBroadcast);
			if (pStyle!=0 && pStyle!=GetStyleSheet()) 
            {
                // Sonderbehandlung fuer den StyleSheet des Objekts
				if (aStyles.GetPos(pStyle)==CONTAINER_ENTRY_NOTFOUND) 
                {
					EndListening(*pStyle);
				}
			}
		}

        // und schliesslich alle in aStyles enthaltenen StyleSheets mit den vorhandenen Broadcastern mergen
		nNum=aStyles.Count();
		
        while (nNum>0) 
        {
			nNum--;
			SfxStyleSheet* pStyle=(SfxStyleSheet*)aStyles.GetObject(nNum);
			// StartListening soll selbst nachsehen, ob hier nicht evtl. schon gehorcht wird
			StartListening(*pStyle,true);
		}
	}
}

void SdrTextObj::ResizeTextAttributes(const Fraction& xFact, const Fraction& yFact)
{
	OutlinerParaObject* pOutlinerParaObject = GetOutlinerParaObject();

    if (pOutlinerParaObject!=0 && xFact.IsValid() && yFact.IsValid())
	{
		Fraction n100(100,1);
		sal_Int32 nX=sal_Int32(xFact*n100);
		sal_Int32 nY=sal_Int32(yFact*n100);
	
        if (nX<0) 
            nX=-nX;

		if (nX<1) 
            nX=1;

		if (nX>0xFFFF) 
            nX=0xFFFF;

		if (nY<0) 
            nY=-nY;

		if (nY<1) 
            nY=1;

		if (nY>0xFFFF) 
            nY=0xFFFF;

		if (nX!=100 || nY!=100)
		{
			// Rahmenattribute
			const SfxItemSet& rSet = GetObjectItemSet();
			const SvxCharScaleWidthItem& rOldWdt=(SvxCharScaleWidthItem&)rSet.Get(EE_CHAR_FONTWIDTH);
			const SvxFontHeightItem& rOldHgt=(SvxFontHeightItem&)rSet.Get(EE_CHAR_FONTHEIGHT);

			// erstmal die alten Werte holen
			sal_Int32 nRelWdt=rOldWdt.GetValue();
			sal_Int32 nAbsHgt=rOldHgt.GetHeight();
			sal_Int32 nRelHgt=rOldHgt.GetProp();

			// Relative Breite aendern
			nRelWdt*=nX;
			nRelWdt/=nY;
			
            // nicht negativ
            if (nRelWdt<0) 
                nRelWdt=-nRelWdt;

            // und mind. 1%
            if (nRelWdt<=0) 
                nRelWdt=1;       

            if (nRelWdt>0xFFFF) 
                nRelWdt=0xFFFF;

			// Absolute Hoehe aendern
			nAbsHgt*=nY;
			nAbsHgt/=100;

            // nicht negativ
            if (nAbsHgt<0) 
                nAbsHgt=-nAbsHgt; 

            // und mind. 1
            if (nAbsHgt<=0) 
                nAbsHgt=1;

            if (nAbsHgt>0xFFFF) 
                nAbsHgt=0xFFFF;

			// und nun attributieren
            SetObjectItem(SvxCharScaleWidthItem( (sal_uInt16) nRelWdt, EE_CHAR_FONTWIDTH));
            SetObjectItem(SvxFontHeightItem(nAbsHgt,(sal_uInt16)nRelHgt, EE_CHAR_FONTHEIGHT));
			// Zeichen- und Absatzattribute innerhalb des OutlinerParaObjects
			Outliner& rOutliner=ImpGetDrawOutliner();
			rOutliner.SetPaperSize(Size(LONG_MAX,LONG_MAX));
			rOutliner.SetText(*pOutlinerParaObject);
			rOutliner.DoStretchChars((sal_uInt16)nX,(sal_uInt16)nY);
			OutlinerParaObject* pNewPara=rOutliner.CreateParaObject();
			SetOutlinerParaObject(pNewPara);
			rOutliner.Clear();
		}
	}
}

/** #103836# iterates over the paragraphs of a given SdrObject and removes all
			 hard set character attributes with the which ids contained in the
			 given vector
*/
void SdrTextObj::RemoveOutlinerCharacterAttribs( const std::vector<sal_uInt16>& rCharWhichIds )
{
	sal_Int32 nText = getTextCount();

	while( --nText >= 0 )
	{
		SdrText* pText = getText( nText );
		OutlinerParaObject* pOutlinerParaObject = pText ? pText->GetOutlinerParaObject() : 0;

		if(pOutlinerParaObject)
		{
			Outliner* pOutliner = 0;
			
			if( pEdtOutl || (pText == getActiveText()) )
				pOutliner = pEdtOutl;

			if(!pOutliner)
			{
				pOutliner = &ImpGetDrawOutliner();
				pOutliner->SetText(*pOutlinerParaObject);
			}

			ESelection aSelAll( 0, 0, 0xffff, 0xffff );
			std::vector<sal_uInt16>::const_iterator aIter( rCharWhichIds.begin() );
			while( aIter != rCharWhichIds.end() )
			{
				pOutliner->RemoveAttribs( aSelAll, false, (*aIter++) );
			}

			if(!pEdtOutl || (pText != getActiveText()) )
			{
				const sal_uInt32 nParaCount = pOutliner->GetParagraphCount();
				OutlinerParaObject* pTemp = pOutliner->CreateParaObject(0, (sal_uInt16)nParaCount);
				pOutliner->Clear();
				SetOutlinerParaObjectForText(pTemp, pText);
			}
		}
	}
}

bool SdrTextObj::HasText() const
{
	if( pEdtOutl )
		return HasEditText();
	
	OutlinerParaObject* pOPO = GetOutlinerParaObject();

	bool bHasText = false;
	if( pOPO )
	{
		const EditTextObject& rETO = pOPO->GetTextObject();
		sal_uInt16 nParaCount = rETO.GetParagraphCount();

		if( nParaCount > 0 )
			bHasText = (nParaCount > 1) || (rETO.GetText( 0 ).Len() != 0);
	}

	return bHasText;
}

// eof
