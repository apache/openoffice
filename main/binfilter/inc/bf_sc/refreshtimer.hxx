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



#ifndef SC_REFRESHTIMER_HXX
#define SC_REFRESHTIMER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SV_TIMER_HXX
#include <vcl/timer.hxx>
#endif
#ifndef _VOS_MUTEX_HXX
#include <vos/mutex.hxx>
#endif
namespace binfilter {

#define SC_REFRESHTIMER_CONTROL_LIST 0
#if SC_REFRESHTIMER_CONTROL_LIST
class ScRefreshTimer;
DECLARE_LIST( ScRefreshTimerList, ScRefreshTimer* )
#endif

class ScRefreshTimerControl
{
private:
			::vos::OMutex		aMutex;
			USHORT				nBlockRefresh;

public:
#if SC_REFRESHTIMER_CONTROL_LIST
			ScRefreshTimerList	aList;
#endif

								ScRefreshTimerControl() : nBlockRefresh(0) {}

			void				SetAllowRefresh( BOOL b )
									{
										if ( b && nBlockRefresh )
											--nBlockRefresh;
										else if ( !b && nBlockRefresh < (USHORT)(~0) )
											++nBlockRefresh;
									}
			BOOL				IsRefreshAllowed() const	{ return !nBlockRefresh; }
			::vos::OMutex&		GetMutex()					{ return aMutex; }
};


class ScRefreshTimerProtector
{
private:
	ScRefreshTimerControl * const *	ppControl;
public:
								ScRefreshTimerProtector( ScRefreshTimerControl * const *	 pp );
								~ScRefreshTimerProtector()
									{
										if ( ppControl && *ppControl )
											(*ppControl)->SetAllowRefresh( TRUE );
									}
};


class ScRefreshTimer : public AutoTimer
{
private:
	ScRefreshTimerControl * const *	ppControl;

			void				AppendToControl()
									{
#if SC_REFRESHTIMER_CONTROL_LIST
										if ( ppControl && *ppControl )
											(*ppControl)->aList.Insert( this, LIST_APPEND );
#endif
									}
			void				RemoveFromControl()
									{
#if SC_REFRESHTIMER_CONTROL_LIST
										if ( ppControl && *ppControl )
											(*ppControl)->aList.Remove( this );
#endif
									}

			void				Start()
									{
										if ( GetTimeout() )
											AutoTimer::Start();
									}

public:
								ScRefreshTimer() : ppControl(0)
									{ SetTimeout( 0 ); }
								ScRefreshTimer( ULONG nSeconds ) : ppControl(0)
									{
										SetTimeout( nSeconds * 1000 );
										Start();
									}
								ScRefreshTimer( const ScRefreshTimer& r )
									: AutoTimer( r ), ppControl(0)
									{}
	virtual						~ScRefreshTimer();

			ScRefreshTimer&		operator=( const ScRefreshTimer& r )
                                    {
                                        SetRefreshControl(0);
                                        AutoTimer::operator=( r );
                                        return *this;
                                    }

			BOOL				operator==( const ScRefreshTimer& r ) const
									{ return GetTimeout() == r.GetTimeout(); }

			BOOL				operator!=( const ScRefreshTimer& r ) const
									{ return !ScRefreshTimer::operator==( r ); }

			void				StartRefreshTimer()
									{ Start(); }

			void				SetRefreshControl( ScRefreshTimerControl * const * pp )
									{
										RemoveFromControl();
										ppControl = pp;
										AppendToControl();
									}

			void				SetRefreshHandler( const Link& rLink )
									{ SetTimeoutHdl( rLink ); }

			ULONG				GetRefreshDelay() const
									{ return GetTimeout() / 1000; }

			void				StopRefreshTimer()
									{ Stop(); }

	virtual	void				SetRefreshDelay( ULONG nSeconds );
};


} //namespace binfilter
#endif // SC_REFRESHTIMER_HXX

