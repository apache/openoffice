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







#ifndef _SV_METAACT_HXX
#include <vcl/metaact.hxx>
#endif



#include "svdobj.hxx"

#ifndef _XDEF_HXX
#include "xdef.hxx"
#endif

#include "xattr.hxx"
#include "xoutx.hxx"

#include "svdoimp.hxx"
#include "svdattr.hxx"
namespace binfilter {


///////////////////////////////////////////////////////////////////////////////

#define ITEMVALUE(ItemSet,Id,Cast)	((const Cast&)(ItemSet).Get(Id)).GetValue()

////////////////////////////////////////////////////////////////////////////////////////////////////

// #100127# Bracket filled output with a comment, if recording a Mtf
/*N*/ ImpGraphicFill::ImpGraphicFill( const SdrObject& 		rObj, 
/*N*/                                 const ExtOutputDevice& 	rXOut, 
/*N*/                                 const SfxItemSet& 		rFillItemSet, 
/*N*/                                 bool 					bIsShadow		) :
/*N*/     mrObj( rObj ),
/*N*/     mrXOut( rXOut ),
/*N*/     mbCommentWritten( false )
/*N*/ {
/*N*/ 	const SfxItemSet& rSet = rObj.GetItemSet();
/*N*/     XFillStyle eFillStyle( ITEMVALUE( rFillItemSet, XATTR_FILLSTYLE, XFillStyleItem ) );
/*N*/     XGradient aGradient( ITEMVALUE( rFillItemSet, XATTR_FILLGRADIENT, XFillGradientItem ) );
/*N*/     XHatch aHatch( ITEMVALUE( rFillItemSet, XATTR_FILLHATCH, XFillHatchItem ) );
/*N*/ 
/*N*/ 	sal_Int32 nDX( ((SdrShadowXDistItem&)(rSet.Get(SDRATTR_SHADOWXDIST))).GetValue() );
/*N*/ 	sal_Int32 nDY( ((SdrShadowYDistItem&)(rSet.Get(SDRATTR_SHADOWYDIST))).GetValue() );
/*N*/ 
/*N*/     GDIMetaFile* pMtf=NULL;
/*N*/     if( eFillStyle != XFILL_NONE &&
/*N*/         (pMtf=mrXOut.GetOutDev()->GetConnectMetaFile()) )
/*?*/     {DBG_BF_ASSERT(0, "STRIP");//STRIP001 
/*?*/     }
/*N*/ }

// #100127# Bracket filled output with a comment, if recording a Mtf
/*N*/ ImpGraphicFill::~ImpGraphicFill()
/*N*/ {
/*N*/     GDIMetaFile* pMtf=NULL;
/*N*/     if( mbCommentWritten &&
/*N*/         (pMtf=mrXOut.GetOutDev()->GetConnectMetaFile()) )
/*N*/     {
/*?*/         pMtf->AddAction( new MetaCommentAction( "XPATHFILL_SEQ_END" ) );
/*N*/     }
/*N*/ }

///////////////////////////////////////////////////////////////////////////////

// #104609# Extracted from XOutputDevice::ImpCalcBmpFillStartValues
}
