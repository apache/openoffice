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



#ifndef SC_CHARTLIS_HXX
#define SC_CHARTLIS_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif

#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif
#ifndef SC_COLLECT_HXX
#include "collect.hxx"
#endif
#ifndef SC_RANGELST_HXX
#include "rangelst.hxx"
#endif

//STRIP008 class ScDocument;
//STRIP008 class ScChartUnoData;

#ifndef _COM_SUN_STAR_CHART_XCHARTDATA_HPP_
#include <com/sun/star/chart/XChartData.hpp>
#endif
namespace binfilter {
class ScDocument;
class ScChartUnoData;

class ScChartListener : public StrData, public SfxListener
{
private:
	ScRangeListRef	aRangeListRef;
	ScChartUnoData*	pUnoData;
	ScDocument* 	pDoc;
	BOOL			bUsed;	// fuer ScChartListenerCollection::FreeUnused
	BOOL			bDirty;
	BOOL			bSeriesRangesScheduled;

					// not implemented
	ScChartListener& operator=( const ScChartListener& );

public:
					ScChartListener( const String& rName, ScDocument* pDoc,
									const ScRange& rRange );
					ScChartListener( const String& rName, ScDocument* pDoc,
									const ScRangeListRef& rRangeListRef );
					ScChartListener( const ScChartListener& );
	virtual			~ScChartListener();
	virtual DataObject*	Clone() const;

	void			SetUno( const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >& rListener,
							const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartData >& rSource );
	::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >	GetUnoListener() const;
	::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartData >						GetUnoSource() const;

	BOOL			IsUno() const	{ return (pUnoData != NULL); }

	virtual void 	Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
	void			StartListeningTo();
	void			EndListeningTo();
	void			Update();
	const ScRangeListRef&	GetRangeList() const { return aRangeListRef; }
	void			SetRangeList( const ScRangeListRef& rNew ) { aRangeListRef = rNew; }
	void			SetRangeList( const ScRange& rNew );
	BOOL			IsUsed() const { return bUsed; }
	void			SetUsed( BOOL bFlg ) { bUsed = bFlg; }
	BOOL			IsDirty() const { return bDirty; }
	void			SetDirty( BOOL bFlg ) { bDirty = bFlg; }

	// if chart series ranges are to be updated later on (e.g. DeleteTab, InsertTab)
	void			ScheduleSeriesRanges()		{ bSeriesRangesScheduled = TRUE; }

};

class ScChartListenerCollection : public StrCollection
{
private:
	Timer			aTimer;
	ScDocument*		pDoc;

					DECL_LINK( TimerHdl, Timer* );

					// not implemented
	ScChartListenerCollection& operator=( const ScChartListenerCollection& );

public:
					ScChartListenerCollection( ScDocument* pDoc );
					ScChartListenerCollection( const ScChartListenerCollection& );
	virtual	DataObject*	Clone() const;

	virtual			~ScChartListenerCollection();

					// nur nach copy-ctor noetig, wenn neu ins Dok gehaengt

	void			ChangeListening( const String& rName,
									const ScRangeListRef& rRangeListRef,
									BOOL bDirty = FALSE );
	// FreeUnused nur wie in ScDocument::UpdateChartListenerCollection verwenden!
	void			FreeUnused();
	void			FreeUno( const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >& rListener,
							 const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartData >& rSource );
	void			StartTimer();
	void			UpdateDirtyCharts();
	void			SetDirty();


	void			UpdateScheduledSeriesRanges();
	void			UpdateSeriesRangesContainingTab( USHORT nTab );

};


} //namespace binfilter
#endif

