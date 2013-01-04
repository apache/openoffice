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



#ifndef _SD_SDFILTER_HXX
#define _SD_SDFILTER_HXX

#include "rtl/ustring.hxx"

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORSUPPLIER_HPP_
#include <com/sun/star/task/XStatusIndicatorSupplier.hpp>
#endif
namespace vos { class OModule; }
namespace binfilter {

// ------------
// - SdFilter -
// ------------

class SfxMedium;
class SdDrawDocShell;
class SdDrawDocument;
class SfxProgress;

class SdFilter
{
protected:

	::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >				mxModel;
	::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator >	mxStatusIndicator;

protected:

	SfxMedium&					mrMedium;
	SdDrawDocShell&				mrDocShell;
	SdDrawDocument&				mrDocument;
	SfxProgress*				mpProgress;
	sal_Bool					mbIsDraw : 1;
	sal_Bool					mbShowProgress : 1;

	void						CreateStatusIndicator();
	void						CreateProgress();

public:
	
								SdFilter( SfxMedium& rMedium, SdDrawDocShell& rDocShell, sal_Bool bShowProgress );
								~SdFilter();

	virtual sal_Bool			Import();
	virtual sal_Bool			Export();

	sal_Bool					IsProgress() const { return mbShowProgress; }
	sal_Bool					IsDraw() const { return mbIsDraw; }
	sal_Bool					IsImpress() const { return !mbIsDraw; }
};

} //namespace binfilter
#endif // _SD_SDFILTER_HXX
