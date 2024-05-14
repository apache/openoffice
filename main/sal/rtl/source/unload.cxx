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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"
#include <rtl/unload.h>
#include <rtl/alloc.h>
#include <rtl/ustring.hxx>
#include <osl/mutex.hxx>
#include <hash_map>

#include <functional>
#include <list>
#include <deque>

using osl::MutexGuard;

//----------------------------------------------------------------------------

static void rtl_notifyUnloadingListeners();

static sal_Bool isEqualTimeValue ( const TimeValue* time1,  const TimeValue* time2)
{
	if( time1->Seconds == time2->Seconds && 
		time1->Nanosec == time2->Nanosec)
		return sal_True;
	else
		return sal_False;
}

static sal_Bool isGreaterTimeValue(  const TimeValue* time1,  const TimeValue* time2)
{
	sal_Bool retval= sal_False;
	if ( time1->Seconds > time2->Seconds)
		retval= sal_True;
	else if ( time1->Seconds == time2->Seconds)
	{
		if( time1->Nanosec > time2->Nanosec)
			retval= sal_True;
	}
	return retval;
}

static sal_Bool isGreaterEqualTimeValue( const TimeValue* time1, const TimeValue* time2)
{
	if( isEqualTimeValue( time1, time2) )
		return sal_True;
	else if( isGreaterTimeValue( time1, time2))
		return sal_True;
	else
		return sal_False;
}

static void addTimeValue( const TimeValue* value1, const TimeValue* value2, TimeValue* result)
{
	sal_uInt64 sum;
	result->Nanosec=0;
	result->Seconds=0;

	sum= value1->Nanosec + value2->Nanosec;
	if( sum >= 1000000000 )
	{
		result->Seconds=1;
		sum -= 1000000000;
	}
	result->Nanosec= (sal_uInt32)sum;
	result->Seconds += value1->Seconds + value2->Seconds;
}


static sal_Bool hasEnoughTimePassed( const TimeValue* unusedSince, const TimeValue* timespan)
{
	sal_Bool retval= sal_False;
	TimeValue currentTime;
	if( osl_getSystemTime( &currentTime))
	{
		TimeValue addedTime;
		addTimeValue( unusedSince, timespan, &addedTime);
		if( isGreaterEqualTimeValue( &currentTime, &addedTime))
			retval= sal_True;
	}
	
	return retval;
}

static osl::Mutex* getUnloadingMutex()
{
	static osl::Mutex * g_pMutex= NULL;
	if (!g_pMutex)
	{
		MutexGuard guard( osl::Mutex::getGlobalMutex() );
		if (!g_pMutex)
		{
			static osl::Mutex g_aMutex;
			g_pMutex= &g_aMutex;
		}
	}
	return g_pMutex;
}

extern "C" void rtl_moduleCount_acquire(rtl_ModuleCount * that )
{
	rtl_StandardModuleCount* pMod= (rtl_StandardModuleCount*)that;
	osl_incrementInterlockedCount( &pMod->counter);
}

extern "C" void rtl_moduleCount_release( rtl_ModuleCount * that )
{
	rtl_StandardModuleCount* pMod= (rtl_StandardModuleCount*)that;
	OSL_ENSURE( pMod->counter >0 , "library counter incorrect" );
	osl_decrementInterlockedCount( &pMod->counter);
	if( pMod->counter == 0)
	{
		MutexGuard guard( getUnloadingMutex());
		
		if( sal_False == osl_getSystemTime( &pMod->unusedSince) )
		{
			// set the time to 0 if we could not get the time
			pMod->unusedSince.Seconds= 0;
			pMod->unusedSince.Nanosec= 0;
		}
	}
}


struct hashModule
{
	size_t operator()( const oslModule& rkey) const
	{
		return (size_t)rkey;
	}
};

typedef std::hash_map<
	const oslModule,
	std::pair<sal_uInt32, component_canUnloadFunc>,
	hashModule,
	std::equal_to<oslModule>
> ModuleMap;

typedef ModuleMap::iterator Mod_IT;

static ModuleMap& getModuleMap()
{
	static ModuleMap * g_pMap= NULL;
	if (!g_pMap)
	{
		MutexGuard guard( getUnloadingMutex() );
		if (!g_pMap)
		{
			static ModuleMap g_aModuleMap;
			g_pMap= &g_aModuleMap;
		}
	}
	return *g_pMap;
}

extern "C" sal_Bool rtl_moduleCount_canUnload( rtl_StandardModuleCount * that, TimeValue * libUnused)
{
	if (that->counter == 0)
	{
		MutexGuard guard( getUnloadingMutex());
		if (libUnused && (that->counter == 0))
		{
			rtl_copyMemory(libUnused, &that->unusedSince, sizeof(TimeValue));
		}
	}	
	return (that->counter == 0);
}


extern "C" sal_Bool SAL_CALL rtl_registerModuleForUnloading( oslModule module)
{
	MutexGuard guard( getUnloadingMutex());
	ModuleMap& moduleMap= getModuleMap();
	sal_Bool ret= sal_True;

	// If the module has been registered before, then find it and increment
	// its reference cout
	Mod_IT it= moduleMap.find( module);
	if( it != moduleMap.end())
	{
		//module already registered, increment ref count
		it->second.first++;
	}
	else
	{
		// Test if the module supports unloading (exports component_canUnload)
		rtl::OUString name(RTL_CONSTASCII_USTRINGPARAM( COMPONENT_CANUNLOAD));
		component_canUnloadFunc pFunc=
			(component_canUnloadFunc)osl_getFunctionSymbol( module, name.pData);
		if (pFunc)
		{
			//register module for the first time, set ref count to 1
			moduleMap[module]= std::make_pair((sal_uInt32)1, pFunc);
		}
		else
			ret= sal_False;
	}
	return ret;
}

extern "C" void SAL_CALL rtl_unregisterModuleForUnloading( oslModule module)
{
	MutexGuard guard( getUnloadingMutex());

	ModuleMap& moduleMap= getModuleMap();
	Mod_IT it= moduleMap.find( module);
	if( it != moduleMap.end() )
	{
		// The module is registered, decrement ref count.
		it->second.first --;

		// If the refcount == 0 then remove the module from the map
		if( it->second.first == 0)
			moduleMap.erase( it);
	}
}

extern "C" void SAL_CALL rtl_unloadUnusedModules( TimeValue* libUnused)
{
	MutexGuard guard( getUnloadingMutex());

	typedef std::list< oslModule > list_type;
	list_type unloadedModulesList;

	ModuleMap& moduleMap= getModuleMap();
	Mod_IT it_e= moduleMap.end();
	
	// notify all listeners
	rtl_notifyUnloadingListeners();

	// prepare default TimeValue if argument is NULL
	TimeValue nullTime={0,0};
	TimeValue* pLibUnused= libUnused? libUnused : &nullTime;

	Mod_IT it= moduleMap.begin();
	for (; it != it_e; ++it)
	{
		//can the module be unloaded?
		component_canUnloadFunc func= it->second.second;
		TimeValue unusedSince= {0, 0};

		if( func( &unusedSince) )
		{
			// module can be unloaded if it has not been used at least for the time
			// specified by the argument libUnused
			if( hasEnoughTimePassed( &unusedSince, pLibUnused))
			{
				// get the reference count and unload the module as many times
				sal_uInt32 refCount= it->second.first;

				for ( sal_uInt32 i=0; i < refCount; i++)
					osl_unloadModule( it->first);

				// mark the module for later removal
				unloadedModulesList.push_front( it->first);
			}
		}	
	}

	// remove all entries containing invalid (unloaded) modules
	list_type::const_iterator un_it= unloadedModulesList.begin();
	for (; un_it != unloadedModulesList.end(); ++un_it)
	{
		moduleMap.erase( *un_it);
	}
}


// ==============================================================================
// Unloading Listener Administration
//===============================================================================
struct hashListener
{
	size_t operator()( const sal_Int32& rkey) const
	{
		return (size_t)rkey;
	}
};

typedef std::hash_map<
	const sal_Int32,
	std::pair<rtl_unloadingListenerFunc, void*>,
	hashListener,
	std::equal_to<sal_Int32>
> ListenerMap;

typedef ListenerMap::iterator Lis_IT;

static ListenerMap& getListenerMap()
{
	static ListenerMap * g_pListeners= NULL;
	if (!g_pListeners)
	{
		MutexGuard guard( getUnloadingMutex() );
		if (!g_pListeners)
		{
			static ListenerMap g_aListenerMap;
			g_pListeners= &g_aListenerMap;
		}
	}
	return *g_pListeners;
}


// This queue contains cookies which have been passed out by rtl_addUnloadingListener and 
// which have been regainded by rtl_removeUnloadingListener. When rtl_addUnloadingListener
// is called then a cookie has to be returned. First we look into the set if there is one 
// available. Otherwise a new cookie will be provided.
// not a new value is returned.

typedef std::deque< sal_Int32 > queue_type;

static queue_type& getCookieQueue()
{
	static queue_type * g_pCookies= NULL;
	if (!g_pCookies)
	{
		MutexGuard guard( getUnloadingMutex() );
		if (!g_pCookies)
		{
			static queue_type g_aCookieQueue;
			g_pCookies= &g_aCookieQueue;
		}
	}
	return *g_pCookies;
}

static sal_Int32 getCookie()
{
	static sal_Int32 cookieValue= 1;

	sal_Int32 retval;
	queue_type& regainedCookies= getCookieQueue();
	if( regainedCookies.empty() )
		retval= cookieValue++;
	else
	{
		retval= regainedCookies.front();
		regainedCookies.pop_front();
	}
	return retval;
}

static inline void recycleCookie( sal_Int32 i)
{
	getCookieQueue().push_back(i);
}


// calling the function twice with the same arguments will return tow different cookies.
// The listener will then notified twice. 

extern "C" 
sal_Int32 SAL_CALL rtl_addUnloadingListener( rtl_unloadingListenerFunc callback, void* _this)
{
	MutexGuard guard( getUnloadingMutex());

	sal_Int32 cookie= getCookie();
	ListenerMap& listenerMap= getListenerMap();
	listenerMap[ cookie]= std::make_pair( callback, _this);
	return cookie;
}


extern "C" 
void SAL_CALL rtl_removeUnloadingListener( sal_Int32 cookie )
{
	MutexGuard guard( getUnloadingMutex());

	ListenerMap& listenerMap= getListenerMap();
	size_t removedElements= listenerMap.erase( cookie);
	if( removedElements )
		recycleCookie( cookie);
}


static void rtl_notifyUnloadingListeners()
{
	ListenerMap& listenerMap= getListenerMap();
	for( Lis_IT it= listenerMap.begin(); it != listenerMap.end(); ++it)
	{	
		rtl_unloadingListenerFunc callbackFunc= it->second.first;
		callbackFunc( it->second.second);
	}
}
