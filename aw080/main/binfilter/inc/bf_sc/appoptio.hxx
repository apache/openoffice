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



#ifndef SC_APPOPTIO_HXX
#define SC_APPOPTIO_HXX

#include <bf_svtools/bf_solar.h>



#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

#ifndef _SVX_ZOOMITEM_HXX //autogen
#include <bf_svx/zoomitem.hxx>
#endif

#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef SC_OPTUTIL_HXX
#include "optutil.hxx"
#endif
namespace binfilter {


class ScAppOptions
{
public:
				ScAppOptions();
				ScAppOptions( const ScAppOptions& rCpy );
				~ScAppOptions();

	void		SetDefaults();

	void		SetAppMetric( FieldUnit eUnit )	{ eMetric = eUnit;		}
	FieldUnit	GetAppMetric() const			{ return eMetric;		}
	void		SetZoom( USHORT nNew )			{ nZoom = nNew;			}
	USHORT		GetZoom() const					{ return nZoom;			}
	void		SetZoomType( SvxZoomType eNew ) { eZoomType = eNew;		}
	SvxZoomType	GetZoomType() const				{ return eZoomType;		}
	USHORT		GetLRUFuncListCount() const		{ return nLRUFuncCount;	}
	USHORT*		GetLRUFuncList() const			{ return pLRUList;		}
	void		SetLRUFuncList( const USHORT* pList,
								const USHORT  nCount );
	void		SetStatusFunc( USHORT nNew )	{ nStatusFunc = nNew;	}
	USHORT		GetStatusFunc() const			{ return nStatusFunc;	}
	void		SetAutoComplete( BOOL bNew )	{ bAutoComplete = bNew; }
	BOOL		GetAutoComplete() const			{ return bAutoComplete;	}
	void		SetDetectiveAuto( BOOL bNew )	{ bDetectiveAuto = bNew; }
	BOOL		GetDetectiveAuto() const		{ return bDetectiveAuto; }

	void		SetTrackContentColor(sal_uInt32 nNew) { nTrackContentColor = nNew; }
	sal_uInt32	GetTrackContentColor() const	 { return nTrackContentColor; }
	void		SetTrackInsertColor(sal_uInt32 nNew)	 { nTrackInsertColor = nNew;  }
	sal_uInt32	GetTrackInsertColor() const		 { return nTrackInsertColor;  }
	void		SetTrackDeleteColor(sal_uInt32 nNew)	 { nTrackDeleteColor = nNew;  }
	sal_uInt32	GetTrackDeleteColor() const		 { return nTrackDeleteColor;  }
	void		SetTrackMoveColor(sal_uInt32 nNew)	 { nTrackMoveColor = nNew;    }
	sal_uInt32	GetTrackMoveColor() const		 { return nTrackMoveColor;    }

	ScLkUpdMode GetLinkMode() const				{ return eLinkMode ;}
	void		SetLinkMode( ScLkUpdMode nSet )	{ 	eLinkMode  = nSet;}

	void		SetDefaultObjectSizeWidth(INT32 nNew)	{ nDefaultObjectSizeWidth = nNew; }
	INT32		GetDefaultObjectSizeWidth() const		{ return nDefaultObjectSizeWidth; }
	void		SetDefaultObjectSizeHeight(INT32 nNew)	{ nDefaultObjectSizeHeight = nNew; }
	INT32		GetDefaultObjectSizeHeight() const		{ return nDefaultObjectSizeHeight; }


	const ScAppOptions&	operator=	( const ScAppOptions& rOpt );
/*N*/ 	friend SvStream& 	operator>>	( SvStream& rStream, ScAppOptions& rOpt );
/*N*/ 	friend SvStream&	operator<<	( SvStream& rStream, const ScAppOptions& rOpt );

private:
	FieldUnit	eMetric;
	USHORT		nLRUFuncCount;
	USHORT*		pLRUList;
	SvxZoomType eZoomType;
	USHORT		nZoom;
	USHORT		nStatusFunc;
	BOOL		bAutoComplete;
	BOOL		bDetectiveAuto;
	sal_uInt32	nTrackContentColor;
	sal_uInt32	nTrackInsertColor;
	sal_uInt32	nTrackDeleteColor;
	sal_uInt32	nTrackMoveColor;
	ScLkUpdMode eLinkMode;
	INT32		nDefaultObjectSizeWidth;
	INT32		nDefaultObjectSizeHeight;
};


//==================================================================
//	Config Item containing app options
//==================================================================

class ScAppCfg : public ScAppOptions
{
	//	spread about 5 config paths
	//!	split ScAppOptions into different classes

	ScLinkConfigItem	aLayoutItem;
	ScLinkConfigItem	aInputItem;
	ScLinkConfigItem	aRevisionItem;
	ScLinkConfigItem	aContentItem;
	ScLinkConfigItem	aSortListItem;
	ScLinkConfigItem	aMiscItem;

	DECL_LINK( LayoutCommitHdl, void* );
	DECL_LINK( InputCommitHdl, void* );
	DECL_LINK( RevisionCommitHdl, void* );
	DECL_LINK( ContentCommitHdl, void* );
	DECL_LINK( SortListCommitHdl, void* );
	DECL_LINK( MiscCommitHdl, void* );

	::com::sun::star::uno::Sequence<rtl::OUString> GetLayoutPropertyNames();
	::com::sun::star::uno::Sequence<rtl::OUString> GetInputPropertyNames();
	::com::sun::star::uno::Sequence<rtl::OUString> GetRevisionPropertyNames();
	::com::sun::star::uno::Sequence<rtl::OUString> GetContentPropertyNames();
	::com::sun::star::uno::Sequence<rtl::OUString> GetSortListPropertyNames();
	::com::sun::star::uno::Sequence<rtl::OUString> GetMiscPropertyNames();

public:
			ScAppCfg();

	void	SetOptions( const ScAppOptions& rNew ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 	void	SetOptions( const ScAppOptions& rNew );
};


} //namespace binfilter
#endif

