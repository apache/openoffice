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



#define _INSDLG_CXX

// include ---------------------------------------------------------------

#include <stdio.h>
#include "insdlg.hxx"

#include <tools/urlobj.hxx>
#include <tools/debug.hxx>
#include <bf_svtools/urihelper.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <vcl/group.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/svapp.hxx>

#include <svuidlg.hrc>
#include <bf_so3/svstor.hxx>
#include <bf_so3/ipobj.hxx>
#include "bf_so3/plugin.hxx"
#include "bf_so3/applet.hxx"
#include "bf_so3/outplace.hxx"
#include "bf_so3/staticbaseurl.hxx"

#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <comphelper/processfactory.hxx>

#include <osl/file.hxx>
#include <sot/stg.hxx>

#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <unotools/processfactory.hxx>

#define _SVSTDARR_STRINGSDTOR
#include <bf_svtools/svstdarr.hxx>

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::rtl;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::ui::dialogs;

namespace binfilter
{

/********************** SvObjectServerList ********************************
**************************************************************************/
PRV_SV_IMPL_OWNER_LIST( SvObjectServerList, SvObjectServer )

/*************************************************************************
|*    SvObjectServerList::SvObjectServerList()
|*
|*    Beschreibung
*************************************************************************/
const SvObjectServer * SvObjectServerList::Get( const SvGlobalName & rName ) const
{
	for( ULONG i = 0; i < Count(); i++ )
	{
		if( rName == GetObject( i ).GetClassName() )
			return &GetObject( i );
	}
	return NULL;
}

void SvObjectServerList::Remove( const SvGlobalName & rName )
{
	SvObjectServer * pS = (SvObjectServer *)aTypes.First();
	while( pS )
	{
		if( rName == pS->GetClassName() )
		{
			Remove();
			pS = (SvObjectServer *)aTypes.GetCurObject();
		}
		else
			pS = (SvObjectServer *)aTypes.Next();
	}
}

//---------------------------------------------------------------------
void SvObjectServerList::FillInsertObjects()
/* [Beschreibung]

	Die Liste wird mit allen Typen gef"ullt, die im Insert-Dialog
	ausgew"ahlt werden k"onnen.
*/
{
	try{
	Reference< XMultiServiceFactory > _globalMSFactory= utl::getProcessServiceFactory();
	if( _globalMSFactory.is())
	{
		OUString sProviderService =
		OUString::createFromAscii( "com.sun.star.configuration.ConfigurationProvider" );
		Reference<XMultiServiceFactory > sProviderMSFactory(
			_globalMSFactory->createInstance( sProviderService ),UNO_QUERY );

		if( sProviderMSFactory.is())
		{
			OUString sReaderService =
				OUString::createFromAscii( "com.sun.star.configuration.ConfigurationAccess" );
			Sequence< Any > aArguments( 1 );
			aArguments[0] <<= OUString::createFromAscii( "org.openoffice.Office.Common");

			Reference< XHierarchicalNameAccess > xHierNameAccess(
				sProviderMSFactory->createInstanceWithArguments( sReaderService,aArguments ),
				UNO_QUERY );

			if( xHierNameAccess.is())
			{
				OUString sTagName=  OUString::createFromAscii("OfficeObjects");
				Any _a= xHierNameAccess->getByHierarchicalName(sTagName);
				Reference<XNameAccess> nameAccess;
				_a >>= nameAccess;
				if( nameAccess.is())
				{
					Sequence<OUString> seqNames= nameAccess->getElementNames();
					OUString sSeparator( OUString::createFromAscii("/"));
					for(int i=0; i<seqNames.getLength(); i++)
					{

						OUString sComponentName= sTagName + sSeparator + seqNames[i] + sSeparator +
							OUString::createFromAscii("Name");
						OUString sKey= sTagName + sSeparator + seqNames[i] + sSeparator +
							OUString::createFromAscii("Key");
						Any anyName= xHierNameAccess->getByHierarchicalName(sComponentName);
						Any anyKey= xHierNameAccess->getByHierarchicalName( sKey);

						anyName >>= sComponentName;
						anyKey >>= sKey;

						SvGlobalName aClassName;
						if( aClassName.MakeId( String(sKey.getStr())))
						{
							if( !Get( aClassName ) )
								// noch nicht eingetragen
								Append( SvObjectServer( aClassName, String( sComponentName.getStr() ) ) );

						}
					}
				}
			}
		}

	}


#ifdef WNT
	//SvGlobalName aOleFact = *SvOutPlaceObject::ClassFactory();
	//String aOleObj( SoResid( STR_FURTHER_OBJECT ) );
	//Append( SvObjectServer( aOleFact, aOleObj ) );
    DBG_ERROR( "non-working code!" );
    // TODO: dead corpses
#endif

	}catch( com::sun::star::container::NoSuchElementException)
	{
	}catch( ::com::sun::star::uno::Exception)
	{
	}
	catch(...)
	{
	}
}

} 
