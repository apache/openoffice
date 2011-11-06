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



#ifndef INCLUDED_SVTOOLS_PRINTOPTIONS_HXX
#define INCLUDED_SVTOOLS_PRINTOPTIONS_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_OPTIONS_HXX
#include <bf_svtools/options.hxx>
#endif

class PrinterOptions;

namespace binfilter
{

class SvtPrintOptions_Impl;

// -----------------------
// - SvtBasePrintOptions -
// -----------------------

class  SvtBasePrintOptions: public Options
{
protected:

    SvtPrintOptions_Impl* m_pDataContainer;

    void        SetDataContainer( SvtPrintOptions_Impl* pDataContainer ) { m_pDataContainer = pDataContainer; }

public:

    static ::osl::Mutex& GetOwnStaticMutex();

public:

                SvtBasePrintOptions();
                virtual ~SvtBasePrintOptions();
};

// ---------------------
// - SvtPrinterOptions -
// ---------------------

class  SvtPrinterOptions : public SvtBasePrintOptions
{
private:

    static SvtPrintOptions_Impl*    m_pStaticDataContainer;	/// impl. data container as dynamic pointer for smaller memory requirements!
	static sal_Int32			    m_nRefCount;	        /// internal ref count mechanism

public:

    SvtPrinterOptions();
    virtual ~SvtPrinterOptions();
};

// -----------------------
// - SvtPrintFileOptions -
// -----------------------

class  SvtPrintFileOptions : public SvtBasePrintOptions
{
private:

    static SvtPrintOptions_Impl*    m_pStaticDataContainer;	/// impl. data container as dynamic pointer for smaller memory requirements!
	static sal_Int32			    m_nRefCount;	        /// internal ref count mechanism

public:

    SvtPrintFileOptions();
    virtual ~SvtPrintFileOptions();
};

}

#endif // INCLUDED_SVTOOLS_PRINTOPTIONS_HXX
