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



#ifndef _SVDFIELD_HXX
#define _SVDFIELD_HXX

#ifndef _FLDITEM_HXX
#ifndef ITEMID_FIELD
#ifndef _EDITDATA_HXX
#include <bf_svx/editdata.hxx>  /* das include wird wg. EE_FEATURE_FIELD benoetigt */
#endif
#define ITEMID_FIELD EE_FEATURE_FIELD  /* wird fuer #include <bf_svx/flditem.hxx> benoetigt */
#endif
#include <bf_svx/flditem.hxx>
#endif
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////
// Do not remove this, it is still used in src536a!
void SdrRegisterFieldClasses();
////////////////////////////////////////////////////////////////////////////////////////////////////

#define SDR_MEASUREFIELD 50

class SdrMeasureObj;

enum SdrMeasureFieldKind {SDRMEASUREFIELD_VALUE,SDRMEASUREFIELD_UNIT,SDRMEASUREFIELD_ROTA90BLANCS};

class SdrMeasureField: public SvxFieldData {
	SdrMeasureFieldKind eMeasureFieldKind;
public:
	SV_DECL_PERSIST1(SdrMeasureField,SvxFieldData,SDR_MEASUREFIELD)
	SdrMeasureField(): eMeasureFieldKind(SDRMEASUREFIELD_VALUE) {}
	SdrMeasureField(SdrMeasureFieldKind eNewKind): eMeasureFieldKind(eNewKind) {}
	virtual ~SdrMeasureField();
	virtual SvxFieldData* Clone() const;
	virtual int operator==(const SvxFieldData&) const;
	SdrMeasureFieldKind GetMeasureFieldKind() const { return eMeasureFieldKind; }
	void SetMeasureFieldKind(SdrMeasureFieldKind eNewKind) { eMeasureFieldKind=eNewKind; }
	void TakeRepresentation(const SdrMeasureObj& rObj, XubString& rStr) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDFIELD_HXX

