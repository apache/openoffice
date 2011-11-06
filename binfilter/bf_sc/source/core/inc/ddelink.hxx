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



#ifndef SC_DDELINK_HXX
#define SC_DDELINK_HXX

#ifndef _LNKBASE_HXX //autogen
#include <bf_so3/lnkbase.hxx>
#endif
#ifndef _SFXBRDCST_HXX //autogen
#include <bf_svtools/brdcst.hxx>
#endif
namespace binfilter {

class ScDocument;
class ScMatrix;
class ScMultipleReadHeader;
class ScMultipleWriteHeader;

class ScDdeLink : public ::binfilter::SvBaseLink, public SfxBroadcaster
{
private:
static BOOL	bIsInUpdate;

	ScDocument*		pDoc;

	String			aAppl;			// Verbindungsdaten
	String			aTopic;
	String			aItem;
	BYTE			nMode;			// Zahlformat-Modus

	BOOL			bNeedUpdate;	// wird gesetzt, wenn Update nicht moeglich war

	ScMatrix*		pResult;		// Ergebnis

public:
	TYPEINFO();

			ScDdeLink( ScDocument* pD,
 						const String& rA, const String& rT, const String& rI,
						BYTE nM );
			ScDdeLink( ScDocument* pD, SvStream& rStream, ScMultipleReadHeader& rHdr );
	virtual ~ScDdeLink();

	void			Store( SvStream& rStream, ScMultipleWriteHeader& rHdr ) const;

											// von SvBaseLink ueberladen:
	virtual void	DataChanged( const String& rMimeType,
								const ::com::sun::star::uno::Any & rValue );

											// von SfxBroadcaster ueberladen:

											// fuer Interpreter:

	const ScMatrix*	GetResult() const	{ return pResult; }
	ScMatrix* GetResult()	{return pResult; }

	const String&	GetAppl() const		{ return aAppl; }
	const String&	GetTopic() const	{ return aTopic; }
	const String&	GetItem() const		{ return aItem; }
	BYTE			GetMode() const		{ return nMode; }

	void			TryUpdate();
};


} //namespace binfilter
#endif

