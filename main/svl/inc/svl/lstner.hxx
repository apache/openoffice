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


#ifndef _SFXLSTNER_HXX
#define _SFXLSTNER_HXX

#include "svl/svldllapi.h"
#include <tools/rtti.hxx>
#include <svl/svarray.hxx>

/*	Only the NAME is needed: the destructors below name RuntimeException in an
	exception specification, which neither compiler requires to be a complete
	type, so this header stays free of any UNO include. On GCC and Sun CC
	SAL_THROW expands to nothing at all (sal/types.h), so the declaration is
	simply unused there. */
namespace com { namespace sun { namespace star { namespace uno {
	class RuntimeException;
} } } }


class SfxBroadcaster;
class SfxHint;

#ifndef _SFX_LSTNER_CXX
typedef SvPtrarr SfxBroadcasterArr_Impl;
#endif

#define SFX_NOTIFY( rBC, rBCT, rHint, rHintT ) \
		Notify( rBC, rHint )

//-------------------------------------------------------------------------

class SVL_DLLPUBLIC SfxListener
{
	SfxBroadcasterArr_Impl aBCs;

private:
	const SfxListener&	operator=(const SfxListener &); // n.i., ist verboten

public:
	TYPEINFO();

						SfxListener();
						SfxListener( const SfxListener &rCopy );
	/*	The specification is paperwork, not behaviour: this destructor throws
		nothing and MSVC does not enforce a dynamic specification at run time.
		It is required because classes derived from this one ALSO derive from a
		UNO base whose destructor is declared SAL_THROW( (RuntimeException) ).
		From C++11 on the compiler gives a destructor with no written
		specification an implicit "never throws", and the derived class's own
		implicit specification -- deduced from its bases -- is then weaker than
		that, which an override may not be (C2694). Under C++03 no implicit
		specification exists and this is inert. */
	virtual 			~SfxListener() SAL_THROW( (::com::sun::star::uno::RuntimeException) );

	sal_Bool				StartListening( SfxBroadcaster& rBroadcaster, sal_Bool bPreventDups = sal_False );
	sal_Bool				EndListening( SfxBroadcaster& rBroadcaster, sal_Bool bAllDups = sal_False );
	void				EndListening( sal_uInt16 nNo );
	void				EndListeningAll();
	sal_Bool				IsListening( SfxBroadcaster& rBroadcaster ) const;

	sal_uInt16				GetBroadcasterCount() const
						{ return aBCs.Count(); }
	SfxBroadcaster* 	GetBroadcasterJOE( sal_uInt16 nNo ) const
						{ return (SfxBroadcaster*) aBCs.GetObject(nNo); }

	virtual void		Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

#ifndef _NOTIFY_HXX
	void RemoveBroadcaster_Impl( SfxBroadcaster& rBC );
#endif
};

#endif
