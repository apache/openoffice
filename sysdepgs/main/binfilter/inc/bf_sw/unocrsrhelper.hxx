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


#ifndef _UNOCRSRHELPER_HXX
#define _UNOCRSRHELPER_HXX


#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif
#ifndef _UNOCRSR_HXX
#include <unocrsr.hxx>
#endif
namespace binfilter {

class SfxItemSet; 
struct SfxItemPropertyMap; 

class SwPaM;


namespace com{ namespace sun{ namespace star{
	namespace uno{
		class Any;
	}
}}}
/* -----------------------------14.12.00 15:06--------------------------------

 ---------------------------------------------------------------------------*/
namespace SwUnoCursorHelper
{
	sal_Bool 					getCrsrPropertyValue(const SfxItemPropertyMap* pMap
										, SwPaM& rPam
										, ::com::sun::star::uno::Any *pAny
                                        , ::com::sun::star::beans::PropertyState& eState
                                        , const SwTxtNode* pNode = 0 );

	void 						GetCurPageStyle(SwPaM& rPaM, String &rString);

	inline sal_Bool 			IsStartOfPara(SwPaM& rUnoCrsr)
										{ return rUnoCrsr.GetPoint()->nContent == 0;}
	inline sal_Bool 			IsEndOfPara(SwPaM& rUnoCrsr)
										{ return rUnoCrsr.GetCntntNode() &&
											rUnoCrsr.GetPoint()->nContent == rUnoCrsr.GetCntntNode()->Len();}

	void 						resetCrsrPropertyValue(const SfxItemPropertyMap* pMap, SwPaM& rPam);
	void 						InsertFile(SwUnoCrsr* pUnoCrsr,
									const String& rFileName,
									const String& rFilterName,
									const String& rFilterOption,
									const String& rPassword);

	void 						getNumberingProperty(
									SwPaM& rPam,
									::com::sun::star::beans::PropertyState& eState,
									::com::sun::star::uno::Any *pAny );

	void 						setNumberingProperty(
									const ::com::sun::star::uno::Any& rValue,
									SwPaM& rPam);

	sal_Int16 					IsNodeNumStart(
									SwPaM& rPam,
									::com::sun::star::beans::PropertyState& eState);

} //STRIP008 ;

} //namespace binfilter
#endif
