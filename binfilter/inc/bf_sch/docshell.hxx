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



#ifndef _SCH_DOCSHELL_HXX
#define _SCH_DOCSHELL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SCHDLL0_HXX //autogen
#include "schdll0.hxx"
#endif

#ifndef _SFX_OBJFAC_HXX //autogen
#include <bf_sfx2/docfac.hxx>
#endif

#ifndef _SFX_SHELL_HXX //autogen
#include <bf_sfx2/shell.hxx>
#endif

#ifndef _SVX_FLSTITEM_HXX //autogen
#include <bf_svx/flstitem.hxx>
#endif

#ifndef _SFX_OBJSH_HXX //autogen
#include <bf_sfx2/objsh.hxx>
#endif
#ifndef _SFX_INTERNO_HXX //autogen
#include <bf_sfx2/interno.hxx>
#endif
#ifndef _SCHDLL0_HXX //autogen
#include <schdll0.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <bf_sfx2/request.hxx>
#endif

#ifndef _SVSTOR_HXX //autogen
#include <bf_so3/svstor.hxx>
#endif

#ifndef _CHTMODEL_HXX
#include "chtmodel.hxx"
#endif

#ifndef _SFXDOCFILE_HXX //autogen
#include <bf_sfx2/docfile.hxx>
#endif

#include "glob.hxx"

#ifndef _COM_SUN_STAR_CHART_CHARTDATACHANGEEVENT_HPP_
#include <com/sun/star/chart/ChartDataChangeEvent.hpp>
#endif
namespace binfilter {

#ifndef SO2_DECL_SVSTORAGESTREAM_DEFINED
#define SO2_DECL_SVSTORAGESTREAM_DEFINED
SO2_DECL_REF(SvStorageStream)
#endif

class FontList;
class SfxUndoManager;
class SfxStyleSheetBasePool;
class SdStyleSheetPool;
class SfxProgress;
class SfxPrinter;
class SfxMenuBarManager;

/*************************************************************************
|*
|* Document-Shell fuer Draw-Dokumente
|*
\************************************************************************/

class SchChartDocShell: public SfxObjectShell, public SfxInPlaceObject
{
 private:
	ChartModel*			pChDoc;
	SfxUndoManager* 	pUndoManager;
	SfxPrinter* 		pPrinter;
	FontList*			pFontList;
	SfxProgress*		pProgress;
	BOOL 				bInitNewNoNewDoc;
	BOOL 				bOwnPrinter;
	BOOL				mbClipboardExport;

	void Construct() throw();

	 // #61907# bei deaktivierung des InPlaceEdit evtl. gebufferte neue Daten
	// übernehmen
 protected:

 public:
	TYPEINFO();

	SFX_DECL_OBJECTFACTORY_DLL(SchChartDocShell,SCH_MOD());

	SchChartDocShell(SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED) throw();
	~SchChartDocShell() throw();


	virtual BOOL InitNew(SvStorage*) throw();
	virtual BOOL Load(SvStorage*) throw();
	virtual BOOL Save() throw();
	virtual BOOL SaveAs(SvStorage* pNewStor) throw();
	virtual BOOL SaveCompleted(SvStorage* pNewStor) throw();

	virtual Rectangle GetVisArea(USHORT nAspect) const throw();
	virtual void SetVisArea(const Rectangle& rRect) throw();


	virtual SfxUndoManager* GetUndoManager() throw();

	SfxPrinter* GetPrinter() throw();
	void SetPrinter( SfxPrinter *pNewPrinter, BOOL bIsDeletedHere = FALSE ) throw();

	virtual void     OnDocumentPrinterChanged(Printer* pNewPrinter) throw();

    /** @return if the chart is embedded in a document, this may be a virtual
                device for layout, or a printer, according to what is set in the
                container.  Otherwise GetPrinter() is called.
     */
    OutputDevice * GetRefDevice();

	ChartModel& GetDoc() { return *pChDoc; }
	ChartModel* GetModelPtr() { return pChDoc; }
	void SetModelPtr( ChartModel* pModel ) { pChDoc = pModel; }

	virtual SfxStyleSheetBasePool* GetStyleSheetPool() throw();

	void UpdateTablePointers() throw();

	virtual void FillClass(SvGlobalName* pClassName,
						   ULONG*  pFormat,
						   String* pAppName,
						   String* pFullTypeName,
						   String* pShortTypeName,
						   long    nFileFormat = SOFFICE_FILEFORMAT_CURRENT ) const throw();

	virtual ULONG GetMiscStatus() const throw();

	virtual void    SetModified( BOOL = TRUE ) throw();

	void DataModified( ::com::sun::star::chart::ChartDataChangeEvent& ) throw();

	/// if this flag is set the XML export always exports the table
	void SetClipboardExport( BOOL bSet = sal_False )
		{ mbClipboardExport = bSet; }
	BOOL GetClipboardExport() const
		{ return mbClipboardExport; }
	
};

SO2_DECL_IMPL_REF(SchChartDocShell)

} //namespace binfilter
#endif		// _SCH_DOCSHELL_HXX
