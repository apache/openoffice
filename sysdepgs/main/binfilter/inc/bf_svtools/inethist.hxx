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


#ifndef _INETHIST_HXX
#define _INETHIST_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef _SFXBRDCST_HXX
#include <bf_svtools/brdcst.hxx>
#endif

#ifndef _SFXHINT_HXX
#include <bf_svtools/hint.hxx>
#endif

namespace binfilter
{

/*========================================================================
 *
 * INetURLHistory interface.
 *
 *=======================================================================*/
class INetURLHistory_Impl;
class INetURLHistory : public SfxBroadcaster
{
	struct StaticInstance
	{
		INetURLHistory * operator()();
	};
	friend INetURLHistory * StaticInstance::operator()();

	/** Representation.
	*/
	INetURLHistory_Impl *m_pImpl;

	/** Construction/Destruction.
	*/
	INetURLHistory (void);
	virtual ~INetURLHistory (void);

	/** Implementation.
	*/
	static void NormalizeUrl_Impl (INetURLObject &rUrl);

	 void PutUrl_Impl   (const INetURLObject &rUrl);
	 BOOL QueryUrl_Impl (const INetURLObject &rUrl);

	/** Not implemented.
	*/
	INetURLHistory (const INetURLHistory&);
	INetURLHistory& operator= (const INetURLHistory&);

public:
	/** GetOrCreate.
	*/
	 static INetURLHistory* GetOrCreate (void);

	/** QueryProtocol.
	*/
	BOOL QueryProtocol (INetProtocol eProto) const
	{
		return ((eProto == INET_PROT_FILE ) ||
				(eProto == INET_PROT_FTP  ) ||
				(eProto == INET_PROT_HTTP ) ||
				(eProto == INET_PROT_HTTPS)    );
	}

	/** QueryUrl.
	*/
	BOOL QueryUrl (const INetURLObject &rUrl)
	{
		if (QueryProtocol (rUrl.GetProtocol()))
			return QueryUrl_Impl (rUrl);
		else
			return FALSE;
	}

	BOOL QueryUrl (const String &rUrl)
	{
		INetProtocol eProto =
			INetURLObject::CompareProtocolScheme (rUrl);
		if (QueryProtocol (eProto))
			return QueryUrl_Impl (INetURLObject (rUrl));
		else
			return FALSE;
	}

	/** PutUrl.
	*/
	void PutUrl (const INetURLObject &rUrl)
	{
		if (QueryProtocol (rUrl.GetProtocol()))
			PutUrl_Impl (rUrl);
	}

	void PutUrl (const String &rUrl)
	{
		INetProtocol eProto =
			INetURLObject::CompareProtocolScheme (rUrl);
		if (QueryProtocol (eProto))
			PutUrl_Impl (INetURLObject (rUrl));
	}
};

/*========================================================================
 *
 * INetURLHistoryHint (broadcasted from PutUrl()).
 *
 *=======================================================================*/
DECL_PTRHINT (, INetURLHistoryHint, const INetURLObject);

}

#endif /* _INETHIST_HXX */

