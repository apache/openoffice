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


#ifndef _SW_PRINTDATA_HXX
#define _SW_PRINTDATA_HXX


#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _RTL_USTRING_HXX_
#include "rtl/ustring.hxx"
#endif
namespace binfilter {

struct SwPrintData
{
	sal_Bool bPrintGraphic, bPrintTable, bPrintDraw, bPrintControl, bPrintPageBackground, 
			 bPrintBlackFont, bPrintLeftPage, bPrintRightPage, bPrintReverse, bPrintProspect, 
			 bPrintSingleJobs, bPaperFromSetup, bModified;

    sal_Int16           nPrintPostIts;
    ::rtl::OUString       sFaxName;

    SwPrintData()
    {
		bPrintGraphic           =
		bPrintTable             =
		bPrintDraw              =
		bPrintControl           =
		bPrintLeftPage          =
		bPrintRightPage         =
		bPrintPageBackground    = sal_True;

		bPaperFromSetup         =
		bPrintReverse           =
		bPrintProspect          =
		bPrintSingleJobs        =
		bModified        		=
		bPrintBlackFont         = sal_False;

		nPrintPostIts           = 0;
    }

    sal_Bool operator==(const SwPrintData& rData)const
    {
        return
        bPrintGraphic       ==   rData.bPrintGraphic       &&
        bPrintTable         ==   rData.bPrintTable         &&
        bPrintDraw          ==   rData.bPrintDraw          &&
        bPrintControl       ==   rData.bPrintControl       &&
        bPrintPageBackground==   rData.bPrintPageBackground&&
        bPrintBlackFont     ==   rData.bPrintBlackFont     &&
        bPrintLeftPage      ==   rData.bPrintLeftPage      &&
        bPrintRightPage     ==   rData.bPrintRightPage     &&
        bPrintReverse       ==   rData.bPrintReverse       &&
        bPrintProspect      ==   rData.bPrintProspect      &&
        bPrintSingleJobs    ==   rData.bPrintSingleJobs    &&
        bPaperFromSetup     ==   rData.bPaperFromSetup     &&
        nPrintPostIts       ==   rData.nPrintPostIts       &&
        sFaxName            ==   rData.sFaxName;
    }
	sal_Bool IsPrintGraphic()	const { return bPrintGraphic; }
	sal_Bool IsPrintTable()		const { return bPrintTable; }
	sal_Bool IsPrintDraw()		const { return bPrintDraw; }
	sal_Bool IsPrintControl()	const { return bPrintControl; }
	sal_Bool IsPrintLeftPage()	const { return bPrintLeftPage; }
	sal_Bool IsPrintRightPage()	const { return bPrintRightPage; }
    sal_Bool IsPrintReverse()   const { return bPrintReverse; }
	sal_Bool IsPaperFromSetup()	const { return bPaperFromSetup; }
	sal_Bool IsPrintProspect()	const { return bPrintProspect; }
	sal_Bool IsPrintPageBackground() const { return bPrintPageBackground; }
	sal_Bool IsPrintBlackFont() const { return bPrintBlackFont;}
	sal_Bool IsPrintSingleJobs() const { return bPrintSingleJobs;}
    sal_Int16 GetPrintPostIts() const { return nPrintPostIts; }
	const ::rtl::OUString		GetFaxName() const{return sFaxName;}

	void SetPrintGraphic  ( sal_Bool b ) { doSetModified(); bPrintGraphic = b;}
	void SetPrintTable	  ( sal_Bool b ) { doSetModified(); bPrintTable = b;}
	void SetPrintDraw	  ( sal_Bool b ) { doSetModified(); bPrintDraw = b;}
	void SetPrintControl  ( sal_Bool b ) { doSetModified(); bPrintControl = b; }
	void SetPrintLeftPage ( sal_Bool b ) { doSetModified(); bPrintLeftPage = b;}
	void SetPrintRightPage( sal_Bool b ) { doSetModified(); bPrintRightPage = b;}
    void SetPrintReverse  ( sal_Bool b ) { doSetModified(); bPrintReverse = b;}
	void SetPaperFromSetup( sal_Bool b ) { doSetModified(); bPaperFromSetup = b;}
    void SetPrintPostIts    ( sal_Int16 n){ doSetModified(); nPrintPostIts = n; }
	void SetPrintProspect	( sal_Bool b ) { doSetModified(); bPrintProspect = b; }
	void SetPrintPageBackground(sal_Bool b){ doSetModified(); bPrintPageBackground = b;}
	void SetPrintBlackFont(sal_Bool b){ doSetModified(); bPrintBlackFont = b;}
	void SetPrintSingleJobs(sal_Bool b){ doSetModified(); bPrintSingleJobs = b;}
	void SetFaxName(const ::rtl::OUString& rSet){sFaxName = rSet;}
	virtual void doSetModified () { bModified = sal_True;}
	sal_Bool WasModified () { return bModified; }
};

} //namespace binfilter
#endif  //_SW_PRINTDATA_HXX

