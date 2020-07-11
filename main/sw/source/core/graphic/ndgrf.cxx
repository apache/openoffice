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
#include "precompiled_sw.hxx"
#include <hintids.hxx>
#include <vcl/salbtype.hxx>             // FRound
#include <tools/urlobj.hxx>
#include <svl/undo.hxx>
#ifndef SVTOOLS_FSTATHELPER_HXX
#include <svl/fstathelper.hxx>
#endif
#include <svtools/imap.hxx>
#include <svtools/filter.hxx>
#include <sot/storage.hxx>
#include <sfx2/linkmgr.hxx>
#include <editeng/boxitem.hxx>
#include <sot/formats.hxx>
#include <fmtfsize.hxx>
#include <fmturl.hxx>
#include <frmfmt.hxx>
#include <doc.hxx>
#include <frmatr.hxx>
#include <grfatr.hxx>
#include <swtypes.hxx>
#include <ndgrf.hxx>
#include <fmtcol.hxx>
#include <hints.hxx>
#include <swbaslnk.hxx>
#include <pagefrm.hxx>
#include <editsh.hxx>
#include <pam.hxx>

#include <unotools/ucbstreamhelper.hxx>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XTransactedObject.hpp>
#include <tools/link.hxx>
#include <vcl/svapp.hxx>
#include <com/sun/star/io/XSeekable.hpp>
#include <retrieveinputstreamconsumer.hxx>
#include <drawinglayer/processor2d/objectinfoextractor2d.hxx>
#include <drawinglayer/primitive2d/objectinfoprimitive2d.hxx>
#include <unotools/cacheoptions.hxx>

using namespace com::sun::star;

#define SWAPGRAPHIC_TIMEOUT     5000

// For comments see same method used in svx
sal_uInt32 getCacheTimeInMs()
{
    static bool bSetAtAll(true);

    if(bSetAtAll)
    {
        static bool bSetToPreferenceTime(true);

        if(bSetToPreferenceTime)
        {
            const SvtCacheOptions aCacheOptions;
            const sal_Int32 nSeconds(aCacheOptions.GetGraphicManagerObjectReleaseTime());

            return nSeconds * 1000 / 12;
        }
        else
        {
            return SWAPGRAPHIC_TIMEOUT;
        }
    }

    return 0;
}

// --------------------
// SwGrfNode
// --------------------
SwGrfNode::SwGrfNode(
    const SwNodeIndex & rWhere,
    const String& rGrfName,
    const String& rFltName,
    const Graphic* pGraphic,
    SwGrfFmtColl *pGrfColl,
    SwAttrSet* pAutoAttr )
        : SwNoTxtNode( rWhere, ND_GRFNODE, pGrfColl, pAutoAttr ),
          maGrfObj(),
          mpReplacementGraphic( 0 ),
          mbLinkedInputStreamReady( false ),
          mbIsStreamReadOnly( sal_False )
{
    maGrfObj.SetSwapStreamHdl( LINK( this, SwGrfNode, SwapGraphic ), getCacheTimeInMs() );
    bInSwapIn = bChgTwipSize = bChgTwipSizeFromPixel = bLoadLowResGrf = bFrameInPaint = bScaleImageMap = sal_False;

    bGrafikArrived = sal_True;
    ReRead( rGrfName, rFltName, pGraphic, 0, sal_False );
}

SwGrfNode::SwGrfNode(
    const SwNodeIndex & rWhere,
    const GraphicObject& rGrfObj,
    SwGrfFmtColl *pGrfColl,
    SwAttrSet* pAutoAttr )
        : SwNoTxtNode( rWhere, ND_GRFNODE, pGrfColl, pAutoAttr ),
          maGrfObj( rGrfObj ),
          mpReplacementGraphic( 0 ),
          mbLinkedInputStreamReady( false ),
          mbIsStreamReadOnly( sal_False )
{
    maGrfObj = rGrfObj;
    maGrfObj.SetSwapStreamHdl( LINK( this, SwGrfNode, SwapGraphic ), getCacheTimeInMs() );
    if ( rGrfObj.HasUserData() && rGrfObj.IsSwappedOut() )
        maGrfObj.SetSwapState();
    bInSwapIn = bChgTwipSize = bChgTwipSizeFromPixel = bLoadLowResGrf = bFrameInPaint = bScaleImageMap = sal_False;
    bGrafikArrived = sal_True;
}

// Konstruktor fuer den SW/G-Reader. Dieser ctor wird verwendet,
// wenn eine gelinkte Grafik gelesen wird. Sie liest diese NICHT ein.

SwGrfNode::SwGrfNode(
    const SwNodeIndex & rWhere,
    const String& rGrfName,
    const String& rFltName,
    SwGrfFmtColl *pGrfColl,
    SwAttrSet* pAutoAttr )
        :
          SwNoTxtNode( rWhere, ND_GRFNODE, pGrfColl, pAutoAttr ),
          maGrfObj(),
          mpReplacementGraphic( 0 ),
          mbLinkedInputStreamReady( false ),
          mbIsStreamReadOnly( sal_False )
{
    maGrfObj.SetSwapStreamHdl( LINK( this, SwGrfNode, SwapGraphic ), getCacheTimeInMs() );

    Graphic aGrf;
    aGrf.SetDefaultType();
    maGrfObj.SetGraphic( aGrf, rGrfName );

    bInSwapIn = bChgTwipSize = bChgTwipSizeFromPixel = bLoadLowResGrf =
            bFrameInPaint = bScaleImageMap = sal_False;
    bGrafikArrived = sal_True;

    InsertLink( rGrfName, rFltName );
    if ( IsLinkedFile() )
    {
        INetURLObject aUrl( rGrfName );
        if ( INET_PROT_FILE == aUrl.GetProtocol() &&
             FStatHelper::IsDocument( aUrl.GetMainURL( INetURLObject::NO_DECODE ) ) )
        {
            // File vorhanden, Verbindung herstellen ohne ein Update
            ( (SwBaseLink*) &refLink )->Connect();
        }
    }
}

sal_Bool SwGrfNode::ReRead(
    const String& rGrfName,
    const String& rFltName,
    const Graphic* pGraphic,
    const GraphicObject* pGrfObj,
    sal_Bool bNewGrf )
{
    sal_Bool bReadGrf = sal_False, bSetTwipSize = sal_True;
    delete mpReplacementGraphic;
    mpReplacementGraphic = 0;

    ASSERT( pGraphic || pGrfObj || rGrfName.Len(),
        "GraphicNode without a name, Graphic or GraphicObject" );

    // ReadRead mit Namen
    if ( refLink.Is() )
    {
        ASSERT( !bInSwapIn, "ReRead: stehe noch im SwapIn" );

        if ( rGrfName.Len() )
        {
            // Besonderheit: steht im FltNamen DDE, handelt es sich um eine
            //					DDE-gelinkte Grafik
            String sCmd( rGrfName );
            if ( rFltName.Len() )
            {
                sal_uInt16 nNewType;
                if ( rFltName.EqualsAscii( "DDE" ) )
                    nNewType = OBJECT_CLIENT_DDE;
                else
                {
                    sfx2::MakeLnkName( sCmd, 0, rGrfName, aEmptyStr, &rFltName );
                    nNewType = OBJECT_CLIENT_GRF;
                }

                if ( nNewType != refLink->GetObjType() )
                {
                    refLink->Disconnect();
                    ( (SwBaseLink*) &refLink )->SetObjType( nNewType );
                }
            }

            refLink->SetLinkSourceName( sCmd );
        }
        else		// kein Name mehr, Link aufheben
        {
            GetDoc()->GetLinkManager().Remove( refLink );
            refLink.Clear();
        }

        if ( pGraphic )
        {
            maGrfObj.SetGraphic( *pGraphic, rGrfName );
            onGraphicChanged();
            bReadGrf = sal_True;
        }
        else if ( pGrfObj )
        {
            maGrfObj = *pGrfObj;
            if ( pGrfObj->HasUserData() && pGrfObj->IsSwappedOut() )
                maGrfObj.SetSwapState();
            maGrfObj.SetLink( rGrfName );
            onGraphicChanged();
            bReadGrf = sal_True;
        }
        else
        {
            // MIB 25.02.97: Daten der alten Grafik zuruecksetzen, damit
            // die korrekte Ersatz-Darstellung erscheint, wenn die
            // der neue Link nicht geladen werden konnte.
            Graphic aGrf;
            aGrf.SetDefaultType();
            maGrfObj.SetGraphic( aGrf, rGrfName );

            if ( refLink.Is() )
            {
                if ( getLayoutFrm( GetDoc()->GetCurrentLayout() ) )
                {
                    SwMsgPoolItem aMsgHint( RES_GRF_REREAD_AND_INCACHE );
                    ModifyNotification( &aMsgHint, &aMsgHint );
                }
                else if ( bNewGrf )
                {
                    //TODO refLink->setInputStream(getInputStream());
                    ( (SwBaseLink*) &refLink )->SwapIn();
                }
            }
            onGraphicChanged();
            bSetTwipSize = sal_False;
        }
    }
    else if ( pGraphic && !rGrfName.Len() )
    {
        // MIB 27.02.2001: Old stream must be deleted before the new one is set.
        if ( HasEmbeddedStreamName() )
            DelStreamName();

        maGrfObj.SetGraphic( *pGraphic );
        onGraphicChanged();
        bReadGrf = sal_True;
    }
    else if ( pGrfObj && !rGrfName.Len() )
    {
        // MIB 27.02.2001: Old stream must be deleted before the new one is set.
        if ( HasEmbeddedStreamName() )
            DelStreamName();

        maGrfObj = *pGrfObj;
        onGraphicChanged();
        if ( pGrfObj->HasUserData() && pGrfObj->IsSwappedOut() )
            maGrfObj.SetSwapState();
        bReadGrf = sal_True;
    }
    else if ( !bNewGrf && GRAPHIC_NONE != maGrfObj.GetType() )
        return sal_True;

    else
    {
        if ( HasEmbeddedStreamName() )
            DelStreamName();

        // einen neuen Grafik-Link anlegen
        InsertLink( rGrfName, rFltName );

        if ( GetNodes().IsDocNodes() )
        {
            if ( pGraphic )
            {
                maGrfObj.SetGraphic( *pGraphic, rGrfName );
                onGraphicChanged();
                bReadGrf = sal_True;
                // Verbindung herstellen ohne ein Update; Grafik haben wir!
                ( (SwBaseLink*) &refLink )->Connect();
            }
            else if ( pGrfObj )
            {
                maGrfObj = *pGrfObj;
                maGrfObj.SetLink( rGrfName );
                onGraphicChanged();
                bReadGrf = sal_True;
                // Verbindung herstellen ohne ein Update; Grafik haben wir!
                ( (SwBaseLink*) &refLink )->Connect();
            }
            else
            {
                Graphic aGrf;
                aGrf.SetDefaultType();
                maGrfObj.SetGraphic( aGrf, rGrfName );
                onGraphicChanged();
                if ( bNewGrf )
                {
                    ( (SwBaseLink*) &refLink )->SwapIn();
                }
            }
        }
    }

    // Bug 39281: Size nicht sofort loeschen - Events auf ImageMaps
    //			  sollten nicht beim Austauschen nicht ins "leere greifen"
    if ( bSetTwipSize )
        SetTwipSize( ::GetGraphicSizeTwip( maGrfObj.GetGraphic(), 0 ) );

    // erzeuge noch einen Update auf die Frames
    if ( bReadGrf && bNewGrf )
    {
        SwMsgPoolItem aMsgHint( RES_UPDATE_ATTR );
        ModifyNotification( &aMsgHint, &aMsgHint );
    }

    return bReadGrf;
}

SwGrfNode::~SwGrfNode()
{
    delete mpReplacementGraphic;
    mpReplacementGraphic = 0;

    mpThreadConsumer.reset();

    SwDoc* pDoc = GetDoc();
    if ( refLink.Is() )
    {
        ASSERT( !bInSwapIn, "DTOR: stehe noch im SwapIn" );
        pDoc->GetLinkManager().Remove( refLink );
        refLink->Disconnect();
    }
    else
    {
        // #i40014# - A graphic node, which are in linked
        // section, whose link is another section is the document, doesn't
        // have to remove the stream from the storage.
        // Because it's hard to detect this case here and it would only fix
        // one problem with shared graphic files - there are also problems,
        // a certain graphic file is referenced by two independent graphic nodes,
        // brush item or drawing objects, the stream isn't no longer removed here.
        // To do this stuff correct, a reference counting on shared streams
        // inside one document have to be implemented.
//        if( !pDoc->IsInDtor() && HasStreamName() )
//          DelStreamName();
    }
    //#39289# Die Frames muessen hier bereits geloescht weil der DTor der
    //Frms die Grafik noch fuer StopAnimation braucht.
    if ( GetDepends() )
        DelFrms();
}

/// allow reaction on change of content of GraphicObject
void SwGrfNode::onGraphicChanged()
{
    // try to access SwFlyFrmFmt; since title/desc/name are set there, there is no
    // use to continue if it is not yet set. If not yet set, call onGraphicChanged()
    // when it is set.
    SwFlyFrmFmt* pFlyFmt = dynamic_cast< SwFlyFrmFmt* >( GetFlyFmt() );

    if ( pFlyFmt != NULL )
    {
        const bool bWasSwappedOut = ( GetGrfObj().IsSwappedOut() == sal_True );

        String aName;
        String aTitle;
        String aDesc;
        const SvgDataPtr& rSvgDataPtr = GetGrf().getSvgData();

        if ( rSvgDataPtr.get() )
        {
            const drawinglayer::primitive2d::Primitive2DSequence aSequence( rSvgDataPtr->getPrimitive2DSequence() );

            if ( aSequence.hasElements() )
            {
                drawinglayer::geometry::ViewInformation2D aViewInformation2D;
                drawinglayer::processor2d::ObjectInfoPrimitiveExtractor2D aProcessor( aViewInformation2D );

                aProcessor.process( aSequence );

                const drawinglayer::primitive2d::ObjectInfoPrimitive2D* pResult = aProcessor.getResult();

                if ( pResult )
                {
                    aName = pResult->getName();
                    aTitle = pResult->getTitle();
                    aDesc = pResult->getDesc();
                }
            }
        }

        if ( aTitle.Len() > 0 )
        {
            SetTitle( aTitle );
        }
        else if ( aName.Len() > 0 )
        {
            SetTitle( aName );
        }

        if ( aDesc.Len() > 0 )
        {
            SetDescription( aDesc );
        }

        if ( bWasSwappedOut )
        {
            SwapOut();
        }
    }
}

void SwGrfNode::SetGraphic(
    const Graphic& rGraphic,
    const String& rLink )
{
    maGrfObj.SetGraphic( rGraphic, rLink );
    onGraphicChanged();
}

const GraphicObject* SwGrfNode::GetReplacementGrfObj() const
{
    if ( !mpReplacementGraphic )
    {
        const SvgDataPtr& rSvgDataPtr = GetGrfObj().GetGraphic().getSvgData();

        if ( rSvgDataPtr.get() )
        {
            const_cast< SwGrfNode* >( this )->mpReplacementGraphic = new GraphicObject( rSvgDataPtr->getReplacement() );
        }
    }

    return mpReplacementGraphic;
}

SwCntntNode *SwGrfNode::SplitCntntNode(
    const SwPosition & )
{
    return this;
}

SwGrfNode * SwNodes::MakeGrfNode(
    const SwNodeIndex & rWhere,
    const String& rGrfName,
    const String& rFltName,
    const Graphic* pGraphic,
    SwGrfFmtColl* pGrfColl,
    SwAttrSet* pAutoAttr,
    sal_Bool bDelayed )
{
    ASSERT( pGrfColl, "MakeGrfNode: Formatpointer ist 0." );
    SwGrfNode *pNode;
    // Delayed erzeugen nur aus dem SW/G-Reader
    if ( bDelayed )
        pNode = new SwGrfNode( rWhere, rGrfName,
            rFltName, pGrfColl, pAutoAttr );
    else
        pNode = new SwGrfNode( rWhere, rGrfName,
            rFltName, pGraphic, pGrfColl, pAutoAttr );
    return pNode;
}

SwGrfNode * SwNodes::MakeGrfNode(
    const SwNodeIndex & rWhere,
    const GraphicObject& rGrfObj,
    SwGrfFmtColl* pGrfColl,
    SwAttrSet* pAutoAttr )
{
    ASSERT( pGrfColl, "MakeGrfNode: Formatpointer ist 0." );
    return new SwGrfNode( rWhere, rGrfObj, pGrfColl, pAutoAttr );
}

Size SwGrfNode::GetTwipSize() const
{
    return nGrfSize;
}

sal_Bool SwGrfNode::ImportGraphic(
    SvStream& rStrm )
{
    Graphic aGraphic;
    const String aURL( maGrfObj.GetUserData() );

    if ( !GraphicFilter::GetGraphicFilter()->ImportGraphic( aGraphic, aURL, rStrm ) )
    {
        delete mpReplacementGraphic;
        mpReplacementGraphic = 0;

        maGrfObj.SetGraphic( aGraphic );
        maGrfObj.SetUserData( aURL );
        onGraphicChanged();
        return sal_True;
    }

    return sal_False;
}

// Returnwert:
// -1 : ReRead erfolgreich
//  0 : nicht geladen
//  1 : Einlesen erfolgreich

short SwGrfNode::SwapIn(
    sal_Bool bWaitForData )
{
    if ( bInSwapIn )					// nicht rekuriv!!
        return !maGrfObj.IsSwappedOut();

    short nRet = 0;
    bInSwapIn = sal_True;
    SwBaseLink* pLink = (SwBaseLink*) (::sfx2::SvBaseLink*) refLink;

    if ( pLink )
    {
        if ( GRAPHIC_NONE == maGrfObj.GetType() ||
             GRAPHIC_DEFAULT == maGrfObj.GetType() )
        {
            // noch nicht geladener Link
            //TODO pLink->setInputStream(getInputStream());
            if ( pLink->SwapIn( bWaitForData ) )
                nRet = -1;
            else if ( GRAPHIC_DEFAULT == maGrfObj.GetType() )
            {
                // keine default Bitmap mehr, also neu Painten!
                delete mpReplacementGraphic;
                mpReplacementGraphic = 0;

                maGrfObj.SetGraphic( Graphic() );
                onGraphicChanged();
                SwMsgPoolItem aMsgHint( RES_GRAPHIC_PIECE_ARRIVED );
                ModifyNotification( &aMsgHint, &aMsgHint );
            }
        }
        else if ( maGrfObj.IsSwappedOut() )
        {
            // nachzuladender Link
            //TODO pLink->setInputStream(getInputStream());
            nRet = pLink->SwapIn( bWaitForData ) ? 1 : 0;
        }
        else
            nRet = 1;
    }
    else if ( maGrfObj.IsSwappedOut() )
    {
        // Die Grafik ist im Storage oder im TempFile drin
        if ( !HasEmbeddedStreamName() )
            nRet = (short) maGrfObj.SwapIn();
        else
        {

            try
            {
                String aStrmName, aPicStgName;
                _GetStreamStorageNames( aStrmName, aPicStgName );
                uno::Reference< embed::XStorage > refPics = _GetDocSubstorageOrRoot( aPicStgName );
                SvStream* pStrm = _GetStreamForEmbedGrf( refPics, aStrmName );
                if ( pStrm )
                {
                    if ( ImportGraphic( *pStrm ) )
                        nRet = 1;
                    delete pStrm;
                }
            }
            catch ( uno::Exception& )
            {
                ASSERT( false, "<SwGrfNode::SwapIn(..)> - unhandled exception!" );
            }
        }

        if ( 1 == nRet )
        {
            SwMsgPoolItem aMsg( RES_GRAPHIC_SWAPIN );
            ModifyNotification( &aMsg, &aMsg );
        }
    }
    else
        nRet = 1;
    DBG_ASSERTWARNING( nRet, "Grafik kann nicht eingeswapt werden" );

    if ( nRet )
    {
        if ( !nGrfSize.Width() && !nGrfSize.Height() )
            SetTwipSize( ::GetGraphicSizeTwip( maGrfObj.GetGraphic(), 0 ) );
    }
    bInSwapIn = sal_False;
    return nRet;
}

short SwGrfNode::SwapOut()
{
    if ( maGrfObj.GetType() != GRAPHIC_DEFAULT
         && maGrfObj.GetType() != GRAPHIC_NONE
         && !maGrfObj.IsSwappedOut()
         && !bInSwapIn )
    {
        if ( !refLink.Is() )
        {
            // Das Swapping brauchen wir nur fuer Embedded Pictures
            // Die Grafik wird in eine TempFile geschrieben, wenn
            // sie frisch eingefuegt war, d.h. wenn es noch keinen
            // Streamnamen im Storage gibt.
            if ( !HasEmbeddedStreamName() )
                if ( !maGrfObj.SwapOut() )
                    return 0;
        }
        // Geschriebene Grafiken oder Links werden jetzt weggeschmissen
        return (short) maGrfObj.SwapOut( NULL );
    }
    return 1;
}

sal_Bool SwGrfNode::GetFileFilterNms(
    String* pFileNm,
    String* pFilterNm ) const
    {
    sal_Bool bRet = sal_False;
    if ( refLink.Is() && refLink->GetLinkManager() )
    {
        sal_uInt16 nType = refLink->GetObjType();
        if ( OBJECT_CLIENT_GRF == nType )
            bRet = refLink->GetLinkManager()->GetDisplayNames(
                refLink, 0, pFileNm, 0, pFilterNm );
        else if ( OBJECT_CLIENT_DDE == nType && pFileNm && pFilterNm )
        {
            String sApp, sTopic, sItem;
            if ( refLink->GetLinkManager()->GetDisplayNames(
                refLink, &sApp, &sTopic, &sItem ) )
            {
                ( *pFileNm = sApp ) += sfx2::cTokenSeperator;
                ( *pFileNm += sTopic ) += sfx2::cTokenSeperator;
                *pFileNm += sItem;
                pFilterNm->AssignAscii( RTL_CONSTASCII_STRINGPARAM( "DDE" ) );
                bRet = sal_True;
            }
        }
    }
    return bRet;
}

// Eine Grafik Undo-faehig machen. Falls sie sich bereits in
// einem Storage befindet, muss sie geladen werden.

sal_Bool SwGrfNode::SavePersistentData()
{
    if ( refLink.Is() )
    {
        ASSERT( !bInSwapIn, "SavePersistentData: stehe noch im SwapIn" );
        GetDoc()->GetLinkManager().Remove( refLink );
        return sal_True;
    }

    // Erst mal reinswappen, falls sie im Storage ist
    if ( HasEmbeddedStreamName() && !SwapIn() )
        return sal_False;

    // #i44367#
    // Do not delete graphic file in storage, because the graphic file could
    // be referenced by other graphic nodes.
    // Because it's hard to detect this case here and it would only fix
    // one problem with shared graphic files - there are also problems,
    // a certain graphic file is referenced by two independent graphic nodes,
    // brush item or drawing objects, the stream isn't no longer removed here.
    // To do this stuff correct, a reference counting on shared streams
    // inside one document have to be implemented.
    // Important note: see also fix for #i40014#
//    if( HasStreamName() )
//        DelStreamName();

    // Und in TempFile rausswappen
    return (sal_Bool) SwapOut();
}

sal_Bool SwGrfNode::RestorePersistentData()
{
    if ( refLink.Is() )
    {
        IDocumentLinksAdministration* pIDLA = getIDocumentLinksAdministration();
        refLink->SetVisible( pIDLA->IsVisibleLinks() );
        pIDLA->GetLinkManager().InsertDDELink( refLink );
        if ( getIDocumentLayoutAccess()->GetCurrentLayout() )
            refLink->Update();
    }
    return sal_True;
}

void SwGrfNode::InsertLink(
    const String& rGrfName,
    const String& rFltName )
{
    refLink = new SwBaseLink( sfx2::LINKUPDATE_ONCALL, FORMAT_GDIMETAFILE, this );

    IDocumentLinksAdministration* pIDLA = getIDocumentLinksAdministration();
    if ( GetNodes().IsDocNodes() )
    {
        refLink->SetVisible( pIDLA->IsVisibleLinks() );
        if ( rFltName.EqualsAscii( "DDE" ) )
        {
            sal_uInt16 nTmp = 0;
            String sApp, sTopic, sItem;
            sApp = rGrfName.GetToken( 0, sfx2::cTokenSeperator, nTmp );
            sTopic = rGrfName.GetToken( 0, sfx2::cTokenSeperator, nTmp );
            sItem = rGrfName.Copy( nTmp );
            pIDLA->GetLinkManager().InsertDDELink( refLink,
                sApp, sTopic, sItem );
        }
        else
        {
            sal_Bool bSync = rFltName.EqualsAscii( "SYNCHRON" );
            refLink->SetSynchron( bSync );
            refLink->SetContentType( SOT_FORMATSTR_ID_SVXB );

            pIDLA->GetLinkManager().InsertFileLink( *refLink,
            OBJECT_CLIENT_GRF, rGrfName,
                ( !bSync && rFltName.Len() ? &rFltName : 0 ) );
        }
    }
    maGrfObj.SetLink( rGrfName );
}

void SwGrfNode::ReleaseLink()
{
    if ( refLink.Is() )
    {
        const String aFileName( maGrfObj.GetLink() );
        const Graphic aLocalGraphic( maGrfObj.GetGraphic() );
        const bool bHasOriginalData( aLocalGraphic.IsLink() );

        {
            bInSwapIn = sal_True;
            SwBaseLink* pLink = (SwBaseLink*) (::sfx2::SvBaseLink*) refLink;
            //TODO pLink->setInputStream(getInputStream());
            pLink->SwapIn( sal_True, sal_True );
            bInSwapIn = sal_False;
        }

        getIDocumentLinksAdministration()->GetLinkManager().Remove( refLink );
        refLink.Clear();
        maGrfObj.SetLink();

        // #15508# added extra processing after getting rid of the link. Use whatever is
        // known from the formally linked graphic to get to a state as close to a directly
        // unlinked insterted graphic as possible. Goal is to have a valid GfxLink at the
        // ImplGraphic (see there) that holds temporary data to the original data and type
        // information about the original data. Only when this is given will
        // SvXMLGraphicHelper::ImplInsertGraphicURL which is used at export use that type
        // and use the original graphic at export for the ODF, without evtl. recoding
        // of trhe bitmap graphic data to something without loss (e.g. PNG) but bigger
        if ( bHasOriginalData )
        {
            // #15508# if we have the original data at the Graphic, let it survive
            // by using that Graphic again, this time at a GraphicObject without link.
            // This happens e.g. when inserting a linked graphic and breaking the link
            maGrfObj.SetGraphic( aLocalGraphic );
        }
        else if ( aFileName.Len() )
        {
            // #15508# we have no original data, but a file name. This happens e.g.
            // when inserting a linked graphic and save, reload document. Try to access
            // that data from the original file; if this works, use it. Else use the
            // data we have (but without knowing the original format)
            int nRes = GRFILTER_OK;
            GraphicFilter* pFlt = GraphicFilter::GetGraphicFilter();
            Graphic aNew;
            nRes = GraphicFilter::LoadGraphic( aFileName, String(), aNew, pFlt );

            if ( GRFILTER_OK == nRes )
            {
                maGrfObj.SetGraphic( aNew );
            }
        }
    }
}

void SwGrfNode::SetTwipSize(
    const Size& rSz )
{
    nGrfSize = rSz;
    if ( IsScaleImageMap() && nGrfSize.Width() && nGrfSize.Height() )
    {
        // Image-Map an Grafik-Groesse anpassen
        ScaleImageMap();

        // Image-Map nicht noch einmal skalieren
        SetScaleImageMap( sal_False );
    }
}

void SwGrfNode::ScaleImageMap()
{
    if ( !nGrfSize.Width() || !nGrfSize.Height() )
        return;

    // dann die Image-Map skalieren
    SwFrmFmt* pFmt = GetFlyFmt();

    if ( !pFmt )
        return;

    SwFmtURL aURL( pFmt->GetURL() );
    if ( !aURL.GetMap() )
        return;

    sal_Bool bScale = sal_False;
    Fraction aScaleX( 1, 1 );
    Fraction aScaleY( 1, 1 );

    const SwFmtFrmSize& rFrmSize = pFmt->GetFrmSize();
    const SvxBoxItem& rBox = pFmt->GetBox();

    if ( !rFrmSize.GetWidthPercent() )
    {
        SwTwips nWidth = rFrmSize.GetWidth();

        nWidth -= rBox.CalcLineSpace( BOX_LINE_LEFT ) +
                  rBox.CalcLineSpace( BOX_LINE_RIGHT );

        ASSERT( nWidth>0, "Gibt es 0 twip breite Grafiken!?" );

        if ( nGrfSize.Width() != nWidth )
        {
            aScaleX = Fraction( nGrfSize.Width(), nWidth );
            bScale = sal_True;
        }
    }
    if ( !rFrmSize.GetHeightPercent() )
    {
        SwTwips nHeight = rFrmSize.GetHeight();

        nHeight -= rBox.CalcLineSpace( BOX_LINE_TOP ) +
                   rBox.CalcLineSpace( BOX_LINE_BOTTOM );

        ASSERT( nHeight>0, "Gibt es 0 twip hohe Grafiken!?" );

        if ( nGrfSize.Height() != nHeight )
        {
            aScaleY = Fraction( nGrfSize.Height(), nHeight );
            bScale = sal_True;
        }
    }

    if ( bScale )
    {
        aURL.GetMap()->Scale( aScaleX, aScaleY );
        pFmt->SetFmtAttr( aURL );
    }
}

void SwGrfNode::DelStreamName()
{
    if ( HasEmbeddedStreamName() )
    {
        // Dann die Grafik im Storage loeschen
        uno::Reference< embed::XStorage > xDocStg = GetDoc()->GetDocStorage();
        if ( xDocStg.is() )
        {
            try
            {
                String aPicStgName, aStrmName;
                _GetStreamStorageNames( aStrmName, aPicStgName );
                uno::Reference< embed::XStorage > refPics = xDocStg;
                if ( aPicStgName.Len() )
                    refPics = xDocStg->openStorageElement( aPicStgName, embed::ElementModes::READWRITE );
                refPics->removeElement( aStrmName );
                uno::Reference< embed::XTransactedObject > xTrans( refPics, uno::UNO_QUERY );
                if ( xTrans.is() )
                    xTrans->commit();
            }
            catch ( uno::Exception& )
            {
                // --> OD 2005-04-25 #i48434#
                ASSERT( false, "<SwGrfNode::DelStreamName()> - unhandled exception!" );
                // <--
            }
        }

        maGrfObj.SetUserData();
    }
}

/** helper method to get a substorage of the document storage for readonly access.

 #i53025#
 A substorage with the specified name will be opened readonly. If the provided
 name is empty the root storage will be returned.
 */
uno::Reference< embed::XStorage > SwGrfNode::_GetDocSubstorageOrRoot(
    const String& aStgName ) const
    {
    uno::Reference< embed::XStorage > refStor =
            const_cast< SwGrfNode* >( this )->GetDoc()->GetDocStorage();
    ASSERT( refStor.is(), "Kein Storage am Doc" );

    if ( aStgName.Len() )
    {
        if ( refStor.is() )
            return refStor->openStorageElement( aStgName, embed::ElementModes::READ );
    }

    return refStor;
}

/** helper method to determine stream for the embedded graphic.

 #i48434#
 Important note: caller of this method has to handle the thrown exceptions
 #i53025#
 Storage, which should contain the stream of the embedded graphic, is
 provided via parameter. Otherwise the returned stream will be closed
 after the method returns, because its parent stream is closed and deleted.
 Proposed name of embedded graphic stream is also provided by parameter.

 @author OD
 */
SvStream* SwGrfNode::_GetStreamForEmbedGrf(
    const uno::Reference< embed::XStorage >& _refPics,
    String& _aStrmName ) const
    {
    SvStream* pStrm( 0L );

    if ( _refPics.is() && _aStrmName.Len() )
    {
        // If stream doesn't exist in the storage, try access the graphic file by
        // re-generating its name.
        // A save action can have changed the filename of the embedded graphic,
        // because a changed unique ID of the graphic is calculated.
        // #b6364738#
        // recursive calls of <GetUniqueID()> have to be avoided.
        // Thus, use local static boolean to assure this.
        static bool bInRegenerateStrmName( false );
        if ( !bInRegenerateStrmName &&
             ( !_refPics->hasByName( _aStrmName ) ||
               !_refPics->isStreamElement( _aStrmName ) ) )
        {
            bInRegenerateStrmName = true;
            xub_StrLen nExtPos = _aStrmName.Search( '.' );
            String aExtStr = _aStrmName.Copy( nExtPos );
            Graphic aGraphic( GetGrfObj().GetGraphic() );
            if ( aGraphic.GetType() != GRAPHIC_NONE )
            {
                _aStrmName = String( GetGrfObj().GetUniqueID(), RTL_TEXTENCODING_ASCII_US );
                _aStrmName += aExtStr;
            }
            bInRegenerateStrmName = false;
        }

        // assure that graphic file exist in the storage.
        if ( _refPics->hasByName( _aStrmName ) &&
             _refPics->isStreamElement( _aStrmName ) )
        {
            uno::Reference< io::XStream > refStrm = _refPics->openStreamElement( _aStrmName, embed::ElementModes::READ );
            pStrm = utl::UcbStreamHelper::CreateStream( refStrm );
        }
        else
        {
            ASSERT( false, "<SwGrfNode::_GetStreamForEmbedGrf(..)> - embedded graphic file not found!" );
        }
    }

    return pStrm;
}


void SwGrfNode::_GetStreamStorageNames(
    String& rStrmName,
    String& rStorName ) const
    {
    rStorName.Erase();
    rStrmName.Erase();

    String aUserData( maGrfObj.GetUserData() );
    if ( !aUserData.Len() )
        return;

    String aProt( RTL_CONSTASCII_STRINGPARAM( "vnd.sun.star.Package:" ) );
    if ( 0 == aUserData.CompareTo( aProt, aProt.Len() ) )
    {
        // 6.0 (XML) Package
        xub_StrLen nPos = aUserData.Search( '/' );
        if ( STRING_NOTFOUND == nPos )
        {
            rStrmName = aUserData.Copy( aProt.Len() );
        }
        else
        {
            xub_StrLen nPathStart = aProt.Len();
            if ( 0 == aUserData.CompareToAscii( "./", 2 ) )
                nPathStart += 2;
            rStorName = aUserData.Copy( nPathStart, nPos - nPathStart );
            rStrmName = aUserData.Copy( nPos + 1 );
        }
    }
    else
    {
        ASSERT( false,
            "<SwGrfNode::_GetStreamStorageNames(..)> - unknown graphic URL type. Code for handling 3.1 - 5.2 storages has been deleted by issue i53025." );
    }
    ASSERT( STRING_NOTFOUND == rStrmName.Search( '/' ),
        "invalid graphic stream name" );
}


SwCntntNode* SwGrfNode::MakeCopy(
    SwDoc* pDoc,
    const SwNodeIndex& rIdx ) const
    {
    // kopiere die Formate in das andere Dokument:
    SwGrfFmtColl* pColl = pDoc->CopyGrfColl( *GetGrfColl() );

    Graphic aTmpGrf;
    SwBaseLink* pLink = (SwBaseLink*) (::sfx2::SvBaseLink*) refLink;
    if ( !pLink && HasEmbeddedStreamName() )
    {
        try
        {
            String aStrmName, aPicStgName;
            _GetStreamStorageNames( aStrmName, aPicStgName );
            uno::Reference< embed::XStorage > refPics = _GetDocSubstorageOrRoot( aPicStgName );
            SvStream* pStrm = _GetStreamForEmbedGrf( refPics, aStrmName );
            if ( pStrm )
            {
                const String aURL( maGrfObj.GetUserData() );
                GraphicFilter::GetGraphicFilter()->ImportGraphic( aTmpGrf, aURL, *pStrm );
                delete pStrm;
            }
        }
        catch ( uno::Exception& )
        {
            ASSERT( false, "<SwGrfNode::MakeCopy(..)> - unhandled exception!" );
        }
    }
    else
    {
        if ( maGrfObj.IsSwappedOut() )
            const_cast< SwGrfNode* >( this )->SwapIn();
        aTmpGrf = maGrfObj.GetGraphic();
    }

    const sfx2::LinkManager& rMgr = getIDocumentLinksAdministration()->GetLinkManager();
    String sFile, sFilter;
    if ( IsLinkedFile() )
        rMgr.GetDisplayNames( refLink, 0, &sFile, 0, &sFilter );
    else if ( IsLinkedDDE() )
    {
        String sTmp1, sTmp2;
        rMgr.GetDisplayNames( refLink, &sTmp1, &sTmp2, &sFilter );
        sfx2::MakeLnkName( sFile, &sTmp1, sTmp2, sFilter );
        sFilter.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "DDE" ) );
    }

    SwGrfNode* pGrfNd = pDoc->GetNodes().MakeGrfNode( rIdx, sFile, sFilter,
        &aTmpGrf, pColl,
        (SwAttrSet*) GetpSwAttrSet() );
    pGrfNd->SetTitle( GetTitle() );
    pGrfNd->SetDescription( GetDescription() );
    pGrfNd->SetContour( HasContour(), HasAutomaticContour() );
    return pGrfNd;
}

IMPL_LINK( SwGrfNode, SwapGraphic, GraphicObject*, pGrfObj )
{
    SvStream* pRet;

    // #101174#: Keep graphic while in swap in. That's at least important
    // when breaking links, because in this situation a reschedule call and
    // a DataChanged call lead to a paint of the graphic.
    if ( pGrfObj->IsInSwapOut() && ( IsSelected() || bInSwapIn ) )
        pRet = GRFMGR_AUTOSWAPSTREAM_NONE;
    else if ( refLink.Is() )
    {
        if ( pGrfObj->IsInSwapIn() )
        {
            // then make it by your self
            if ( !bInSwapIn )
            {
                sal_Bool bIsModifyLocked = IsModifyLocked();
                LockModify();
                SwapIn( sal_False );
                if ( !bIsModifyLocked )
                    UnlockModify();
            }
            pRet = GRFMGR_AUTOSWAPSTREAM_NONE;
        }
        else
            pRet = GRFMGR_AUTOSWAPSTREAM_LINK;
    }
    else
    {
        pRet = GRFMGR_AUTOSWAPSTREAM_TEMP;

        if ( HasEmbeddedStreamName() )
        {
            try
            {
                String aStrmName, aPicStgName;
                _GetStreamStorageNames( aStrmName, aPicStgName );
                uno::Reference< embed::XStorage > refPics = _GetDocSubstorageOrRoot( aPicStgName );
                SvStream* pStrm = _GetStreamForEmbedGrf( refPics, aStrmName );
                if ( pStrm )
                {
                    if ( pGrfObj->IsInSwapOut() )
                    {
                        pRet = GRFMGR_AUTOSWAPSTREAM_LINK;
                    }
                    else
                    {
                        ImportGraphic( *pStrm );
                        pRet = GRFMGR_AUTOSWAPSTREAM_LOADED;
                    }
                    delete pStrm;
                }
            }
            catch ( uno::Exception& )
            {
                ASSERT( false, "<SwapGraphic> - unhandled exception!" );
            }
        }
    }

    return (long) pRet;
}

// alle QuickDraw-Bitmaps eines speziellen Docs loeschen
void DelAllGrfCacheEntries(
    SwDoc* pDoc )
{
    if ( pDoc )
    {
        // alle Graphic-Links mit dem Namen aus dem Cache loeschen
        const sfx2::LinkManager& rLnkMgr = pDoc->GetLinkManager();
        const ::sfx2::SvBaseLinks& rLnks = rLnkMgr.GetLinks();
        SwGrfNode* pGrfNd;
        String sFileNm;
        for ( sal_uInt16 n = rLnks.Count(); n; )
        {
            ::sfx2::SvBaseLink* pLnk = &( *rLnks[--n] );
            if ( pLnk && OBJECT_CLIENT_GRF == pLnk->GetObjType()
                 && rLnkMgr.GetDisplayNames( pLnk, 0, &sFileNm )
                 && pLnk->ISA( SwBaseLink )
                 && 0 != ( pGrfNd = ( (SwBaseLink*) pLnk )->GetCntntNode()->GetGrfNode() ) )
            {
                pGrfNd->ReleaseGraphicFromCache();
            }
        }
    }
}

// returns the with our graphic attributes filled Graphic-Attr-Structure
GraphicAttr& SwGrfNode::GetGraphicAttr(
    GraphicAttr& rGA,
    const SwFrm* pFrm ) const
    {
    const SwAttrSet& rSet = GetSwAttrSet();

    rGA.SetDrawMode( (GraphicDrawMode) rSet.GetDrawModeGrf().GetValue() );

    const SwMirrorGrf & rMirror = rSet.GetMirrorGrf();
    sal_uLong nMirror = BMP_MIRROR_NONE;
    if ( rMirror.IsGrfToggle() && pFrm && !pFrm->FindPageFrm()->OnRightPage() )
    {
        switch (rMirror.GetValue())
        {
        case RES_MIRROR_GRAPH_DONT:
            nMirror = BMP_MIRROR_HORZ;
            break;
        case RES_MIRROR_GRAPH_VERT:
            nMirror = BMP_MIRROR_NONE;
            break;
        case RES_MIRROR_GRAPH_HOR:
            nMirror = BMP_MIRROR_HORZ | BMP_MIRROR_VERT;
            break;
        default:
            nMirror = BMP_MIRROR_VERT;
            break;
        }
    }
    else
        switch (rMirror.GetValue())
        {
        case RES_MIRROR_GRAPH_BOTH:
            nMirror = BMP_MIRROR_HORZ | BMP_MIRROR_VERT;
            break;
        case RES_MIRROR_GRAPH_VERT:
            nMirror = BMP_MIRROR_HORZ;
            break;
        case RES_MIRROR_GRAPH_HOR:
            nMirror = BMP_MIRROR_VERT;
            break;
        }

    rGA.SetMirrorFlags( nMirror );

    const SwCropGrf& rCrop = rSet.GetCropGrf();
    rGA.SetCrop( TWIP_TO_MM100( rCrop.GetLeft() ),
        TWIP_TO_MM100( rCrop.GetTop() ),
        TWIP_TO_MM100( rCrop.GetRight() ),
        TWIP_TO_MM100( rCrop.GetBottom() ) );

    const SwRotationGrf& rRotation = rSet.GetRotationGrf();
    rGA.SetRotation( rRotation.GetValue() );

    rGA.SetLuminance( rSet.GetLuminanceGrf().GetValue() );
    rGA.SetContrast( rSet.GetContrastGrf().GetValue() );
    rGA.SetChannelR( rSet.GetChannelRGrf().GetValue() );
    rGA.SetChannelG( rSet.GetChannelGGrf().GetValue() );
    rGA.SetChannelB( rSet.GetChannelBGrf().GetValue() );
    rGA.SetGamma( rSet.GetGammaGrf().GetValue() );
    rGA.SetInvert( rSet.GetInvertGrf().GetValue() );

    const sal_uInt16 nTrans = rSet.GetTransparencyGrf().GetValue();
    rGA.SetTransparency( (sal_uInt8) FRound(
        Min( nTrans, (sal_uInt16) 100 ) * 2.55 ) );

    return rGA;
}

sal_Bool SwGrfNode::IsTransparent() const
{
    sal_Bool bRet = maGrfObj.IsTransparent();
    if ( !bRet )	// ask the attribut
        bRet = 0 != GetSwAttrSet().GetTransparencyGrf().GetValue();

    return bRet;
}

sal_Bool SwGrfNode::IsSelected() const
{
    sal_Bool bRet = sal_False;
    const SwEditShell* pESh = GetDoc()->GetEditShell();
    if ( pESh )
    {
        const SwNode* pN = this;
        const ViewShell* pV = pESh;
        do
        {
            if ( pV->ISA( SwEditShell ) && pN == &( (SwCrsrShell*) pV )
                                                   ->GetCrsr()->GetPoint()->nNode.GetNode() )
            {
                bRet = sal_True;
                break;
            }
        }
        while (pESh != ( pV = (ViewShell*) pV->GetNext() ));
    }
    return bRet;
}

boost::weak_ptr< SwAsyncRetrieveInputStreamThreadConsumer > SwGrfNode::GetThreadConsumer()
{
    return mpThreadConsumer;
}

void SwGrfNode::TriggerAsyncRetrieveInputStream()
{
    if ( !IsLinkedFile() )
    {
        ASSERT( false,
            "<SwGrfNode::TriggerAsyncLoad()> - Method is misused. Method call is only valid for graphic nodes, which refer a linked graphic file" );
        return;
    }

    if ( mpThreadConsumer.get() == 0 )
    {
        mpThreadConsumer.reset( new SwAsyncRetrieveInputStreamThreadConsumer( *this ) );

        String sGrfNm;
        refLink->GetLinkManager()->GetDisplayNames( refLink, 0, &sGrfNm, 0, 0 );

        mpThreadConsumer->CreateThread( sGrfNm );
    }
}

bool SwGrfNode::IsLinkedInputStreamReady() const
{
    return mbLinkedInputStreamReady;
}

void SwGrfNode::ApplyInputStream(
    com::sun::star::uno::Reference< com::sun::star::io::XInputStream > xInputStream,
    const sal_Bool bIsStreamReadOnly )
{
    if ( IsLinkedFile() )
    {
        if ( xInputStream.is() )
        {
            mxInputStream = xInputStream;
            mbIsStreamReadOnly = bIsStreamReadOnly;
            mbLinkedInputStreamReady = true;
            SwMsgPoolItem aMsgHint( RES_LINKED_GRAPHIC_STREAM_ARRIVED );
            ModifyNotification( &aMsgHint, &aMsgHint );
        }
    }
}

void SwGrfNode::UpdateLinkWithInputStream()
{
    // do not work on link, if a <SwapIn> has been triggered.
    if ( !bInSwapIn && IsLinkedFile() )
    {
        GetLink()->setStreamToLoadFrom( mxInputStream, mbIsStreamReadOnly );
        GetLink()->Update();
        SwMsgPoolItem aMsgHint( RES_GRAPHIC_ARRIVED );
        ModifyNotification( &aMsgHint, &aMsgHint );

        mxInputStream.clear();
        GetLink()->clearStreamToLoadFrom();
        mbLinkedInputStreamReady = false;
        mpThreadConsumer.reset();
    }
}


bool SwGrfNode::IsAsyncRetrieveInputStreamPossible() const
{
    bool bRet = false;

    if ( IsLinkedFile() )
    {
        String sGrfNm;
        refLink->GetLinkManager()->GetDisplayNames( refLink, 0, &sGrfNm, 0, 0 );
        String sProtocol( RTL_CONSTASCII_STRINGPARAM( "vnd.sun.star.pkg:" ) );
        if ( sGrfNm.CompareTo( sProtocol, sProtocol.Len() ) != 0 )
        {
            bRet = true;
        }
    }

    return bRet;
}
