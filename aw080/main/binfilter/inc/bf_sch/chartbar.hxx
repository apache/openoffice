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


#ifndef SCH_CHARTBAR_HXX
#define SCH_CHARTBAR_HXX
class Rectangle;
namespace binfilter {

class ChartModel;
class ChartAxis;
//Achtung! evtl. wird der default-Zuweisungsoperator benutzt!
//bei Erweiterungen bedenken! ganz wichtig!
class ChartBarDescriptor
{
	ChartAxis* mpAxis;
	ChartModel* mpModel;
	long nOverlapPercent;
	long nGapPercent;

	long nOver;      //Position des nächsten Balkens, kann gleich nBarWidth sein
	long nGap;       //Größe der Lücke
	long nPartWidth; //gesamte Breite je Spalte mit Gap
	long nColWidth;  //    "     "     "   "    ohne Gap
	long nBarWidth;  //Breite eines einzelnen Balkens
	long nCnt;       //Anzahl der Elemente, die der Berechnung zugrunde liegen

	long nCurrent;
	long nLeft;

public:

	inline void Assign(ChartModel* pModel,ChartAxis* pAxis)
	{
		mpModel=pModel;
		mpAxis=pAxis;
		SetGap(nGapPercent);//StarOne: Diese Eigenschaften hängen als Attribute an der Achse
		SetOverlap(nOverlapPercent);
	};
    void SetOverlap(long nPercent);
    long GetOverlap() const;
    long GetGap() const;
    void SetGap(long nPercent);

	ChartBarDescriptor(long nOverlap=0,long nGap=100):
		mpAxis(NULL),
		nOverlapPercent(nOverlap),
		nGapPercent(nGap)
	{
	}
	ChartBarDescriptor(const Rectangle& aRect,long nColCnt,long nRowCnt,long nOverlap=0,long nGap=100):
		mpAxis(NULL),
		nOverlapPercent(nOverlap),
		nGapPercent(nGap)
	{
		Create(aRect,nColCnt,nRowCnt);
	}

	~ChartBarDescriptor(){};

    /// copy CTOR using the new model given
    ChartBarDescriptor( const ChartBarDescriptor& rSrc, ChartModel* pNewModel );

	inline long Middle()                      //aktuelle Spaltenmitte
	{
		return (nLeft + (nPartWidth - nGap)/2 );
	};
	inline long NextCol()                     //eine Spalte weiterschalten
	{
		nCurrent = nLeft + nPartWidth;
		nLeft = nCurrent;
		return nCurrent;
	};
	inline long BarLeft()
	{
		return nCurrent;
	};          //linke  BarPosition
	inline long BarRight()
	{
		return nCurrent+nBarWidth;
	};//rechte BarPsoition
	inline long BarWidth()
	{
		return nBarWidth;
	};
	inline long BarTop()
	{
		return nCurrent+nBarWidth;
	};//obere  BarPsoition
	inline long BarBottom()
	{
		return nCurrent;
	};          //untere BarPosition

	inline long NextBar() //innerhalb der Spalte einen Balken weiterschalten
	{
		nCurrent+=nOver;//if(nOver)nCurrent+=nOver;else nCurrent+=nBarWidth;
		return nCurrent;
	};

	void Create(const Rectangle& aRect,long nColCnt,long nRowCnt);
};
} //namespace binfilter
#endif


