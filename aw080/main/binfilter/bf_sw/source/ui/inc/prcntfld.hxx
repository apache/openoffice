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


#ifndef _PRCNTFLD_HXX
#define _PRCNTFLD_HXX

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
namespace binfilter {

class PercentField : public MetricField
{
	long		nRefValue;		// 100%-Wert fuer Umrechnung (in Twips)
	long 		nOldMax, nOldMin, nOldSpinSize, nOldBaseValue;
	long		nLastPercent, nLastValue;
	USHORT		nOldDigits;
	FieldUnit	eOldUnit;
    sal_Bool    bLockAutoCalculation; //prevent recalcution of percent values when the 
                                        //reference value is changed


public:
 	PercentField( Window* pWin, const ResId& rResId );








    void        LockAutoCalculation(sal_Bool bLock) {bLockAutoCalculation = bLock;}
    sal_Bool    IsAutoCalculationLocked()const {return bLockAutoCalculation;}
};

} //namespace binfilter
#endif // _PRCNTFLD_HXX

