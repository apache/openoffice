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


#ifndef _SFX_MISCCFG_HXX
#define _SFX_MISCCFG_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
namespace binfilter {

/*--------------------------------------------------------------------
	 Beschreibung:
 --------------------------------------------------------------------*/

class SfxMiscCfg : public ::utl::ConfigItem
{
	BOOL			bPaperSize;     // printer warnings
	BOOL			bPaperOrientation;
	BOOL			bNotFound;
	sal_Int32		nYear2000;		// two digit year representation

	const ::com::sun::star::uno::Sequence<rtl::OUString>& GetPropertyNames();
	void					Load();

public:
	SfxMiscCfg( );
	~SfxMiscCfg( );

	virtual void 			Notify( const ::com::sun::star::uno::Sequence<rtl::OUString>& aPropertyNames);
	virtual void			Commit();

	BOOL		IsNotFoundWarning() 	const {return bNotFound;}

	BOOL		IsPaperSizeWarning() 	const {return bPaperSize;}

	BOOL		IsPaperOrientationWarning() 	const {return bPaperOrientation;}
				// 0 ... 99
	sal_Int32	GetYear2000()			const { return nYear2000; }
};

}//end of namespace binfilter
#endif // _MISCCFG_HXX

