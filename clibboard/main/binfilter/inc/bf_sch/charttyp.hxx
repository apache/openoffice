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



#ifndef _SFXITEMSET_HXX //autogen
#include <bf_svtools/itemset.hxx>
#endif

#ifndef _SVX_CHRTITEM_HXX //autogen
#include <bf_svx/chrtitem.hxx>
#endif
#ifndef _SVX_TAB_LINE_HXX //autogen
#include <bf_svx/tabline.hxx>
#endif
#include "chtmodel.hxx"
namespace binfilter {

#define SPLINE_NONE  0
#define SPLINE_CUBIC 1
#define SPLINE_B     2

class ChartType
{

	BOOL bHasLines;
	BOOL bIsDonut;
	BOOL bIsPercent;
	BOOL bIs3D;
	BOOL bIsDeep3D;
	BOOL bIsVertical;
	BOOL bIsStacked;

	BOOL bHasVolume;
	BOOL bHasUpDown;
	//Dies ist (noch) ein Booltyp, 0=keins, 1= default.später kann einmal mit Typ=2,3,...
	//ein bestimmtes Symbol an einer Datenreihe gesetzt werden
	INT32 nSymbolType;
	INT32 nShapeType;

	long nSplineType;
	long nSpecialType; //nur fuer Kompatiblität! diese Typen sind derzeit inkonsistent
	long nBaseType;

    void Init();

public:

			 ChartType()                                { Init(); }
			 ChartType( const SvxChartStyle eStyle )    { Init(); SetType( eStyle ); }
			 ChartType( const ChartModel* pModel )      { Init(); SetType( pModel ); }
			 ChartType( SfxItemSet* pAttr )             { Init(); SetType( pAttr );  }

			 //noch etwas gehackt um den Basetype zu setzen, es wird erstmal ein default
			 //SetType(SvxChartType) aufgerufen, da Bars,Pie...Typ im attrset nicht
			 //vorhanden ist
	virtual ~ChartType(){};


	SvxChartStyle GetChartStyle() const;

	void SetType(const SvxChartStyle eChartStyle);
	void SetType(const ChartModel* pModel);
	void SetType(const SfxItemSet* pAttr);

	void GetAttrSet(SfxItemSet *pAttr);

	void SetBaseType(const long nBase) {nBaseType=nBase;};
	long GetBaseType() const           {return nBaseType;};
	long GetBaseType(const SvxChartStyle eChartStyle) const;

	BOOL HasLines() const {return bHasLines;};
	BOOL HasLines(const SvxChartStyle eChartStyle) const;

	BOOL IsDonut() const {return bIsDonut;};
	BOOL IsDonut(const SvxChartStyle eChartStyle) const;

	BOOL IsPercent(const SvxChartStyle eChartStyle) const;
	BOOL IsPercent()    const {return bIsPercent;};

	BOOL Is3D(const SvxChartStyle eChartStyle) const;
	BOOL Is3D()         const {return bIs3D;};

	BOOL HasSplines() const {return nSplineType!=SPLINE_NONE;};
	BOOL HasSplines(const SvxChartStyle eChartStyle) const;

	long GetSplineType() const {return nSplineType;};
	long GetSplineType(const SvxChartStyle eChartStyle) const;

	BOOL IsVertical() const {return bIsVertical;};
	BOOL IsVertical(const SvxChartStyle eChartStyle) const;

	BOOL IsStacked(const SvxChartStyle eChartStyle) const;
	BOOL IsStacked()    const {return bIsStacked;};

	BOOL HasSymbols(const SvxChartStyle eChartStyle) const;
	BOOL HasSymbols()   const {return nSymbolType!=SVX_SYMBOLTYPE_NONE;};

	BOOL IsDeep3D(const SvxChartStyle eChartStyle) const;
	BOOL IsDeep3D()     const {return bIsDeep3D;};
};

} //namespace binfilter
