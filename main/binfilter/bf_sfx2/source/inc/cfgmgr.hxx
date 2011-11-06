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


#ifndef	_SFXCFGMGR_HXX
#define	_SFXCFGMGR_HXX

#include <bf_so3/svstor.hxx>
class SvStream;
class String;
namespace binfilter {

class SfxObjectShell;
class SfxConfigItem;
class SfxConfigItemArr_Impl;
struct SfxConfigItem_Impl;
class SfxConfigManager
{
private:
	SotStorageRef	m_xStorage;
	SfxObjectShell*	pObjShell;
	SfxConfigItemArr_Impl* pItemArr;
	USHORT			nErrno;
	BOOL			bModified;

					// to prevent from using
					SfxConfigManager( const SfxConfigManager& );
					const SfxConfigManager& operator=( const SfxConfigManager& );

    BOOL            LoadConfiguration( SotStorage& rStorage );
    BOOL            StoreConfiguration_Impl( SotStorage* pStorage=NULL );

public:
					enum Error {
						ERR_NO,
						ERR_READ,
						ERR_WRITE,
						ERR_OPEN,
						ERR_FILETYPE,
						ERR_VERSION,
						ERR_IMPORT,
						ERR_EXPORT
					};

    static String   GetStorageName();
    static BOOL     HasConfiguration( SotStorage& rStorage );

					// construct a configmanager from a storage
					// pStor == NULL means default config manager ( soffice.cfg )
					SfxConfigManager( SotStorage* pStor = NULL );

					// construct a configmanager from a documents' storage
                    SfxConfigManager( SfxObjectShell& rDoc );
					~SfxConfigManager();

	SotStorage*		GetConfigurationStorage( SotStorage* );

    void            AddConfigItem( SfxConfigItem& rCItem );
    void            RemoveConfigItem( SfxConfigItem& rCItem );
    SfxConfigItem*  GetNextItem( SfxConfigItem& rCItem );
    USHORT          GetErrorCode()
                    { return nErrno; }

					// store the whole configuration into a storage
					// pStore == NULL means store into own storage
	BOOL    		StoreConfiguration( SotStorage* pStorage=NULL );

    BOOL            LoadConfigItem( SfxConfigItem& );
    BOOL            StoreConfigItem( SfxConfigItem& );
    BOOL			StoreAlwaysConfigItem( SfxConfigItem& );

					// Reload all items using a special stream
	void			ReInitialize( const String& rStreamName );

	void 			SetModified(BOOL);
	BOOL			IsModified()
					{ return bModified; }
	SfxObjectShell*	GetObjectShell() const
					{ return pObjShell; }
};

}//end of namespace binfilter
#endif // #ifndef _SFXCFGMGR_HXX

