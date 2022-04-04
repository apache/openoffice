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


#ifndef INCLUDED_CURLLOCKSTORE_HXX
#define INCLUDED_CURLLOCKSTORE_HXX

#include <map>
#include <osl/mutex.hxx>
#include <rtl/ref.hxx>
#include "CurlTypes.hxx"
#include "CurlSession.hxx"

namespace http_dav_ucp
{

class TickerThread;
class CurlSession;

struct ltptr
{
    bool operator()( const CurlLock * p1, const CurlLock * p2 ) const
    {
        return p1 < p2;
    }
};

typedef struct _LockInfo
{
    rtl::Reference< CurlSession > xSession;
    sal_Int32 nLastChanceToSendRefreshRequest;

    _LockInfo()
        : nLastChanceToSendRefreshRequest( -1 ) {}

    _LockInfo( rtl::Reference< CurlSession > const & _xSession,
              sal_Int32 _nLastChanceToSendRefreshRequest )
        : xSession( _xSession )
        , nLastChanceToSendRefreshRequest( _nLastChanceToSendRefreshRequest ) {}

} LockInfo;

typedef std::map< CurlLock *, LockInfo, ltptr > LockInfoMap;

class CurlLockStore
{
    osl::Mutex         m_aMutex;
    TickerThread     * m_pTickerThread;
    LockInfoMap        m_aLockInfoMap;

public:
    CurlLockStore();
    ~CurlLockStore();

    void registerSession( CurlSession aSession );

    CurlLock * findByUri( rtl::OUString const & rUri );

    void addLock( CurlLock * pLock,
                  rtl::Reference< CurlSession > const & xSession,
                  // time in seconds since Jan 1 1970
                  // -1: infinite lock, no refresh
                  sal_Int32 nLastChanceToSendRefreshRequest );

    void updateLock( CurlLock * pLock,
                     sal_Int32 nLastChanceToSendRefreshRequest );

    void removeLock( CurlLock * pLock );

    void refreshLocks();

private:
    void startTicker();
    void stopTicker();
};

} // namespace http_dav_ucp

#endif // INCLUDED_CURLLOCKSTORE_HXX
