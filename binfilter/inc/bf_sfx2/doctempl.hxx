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


#ifndef _SFXDOCTEMPL_HXX
#define _SFXDOCTEMPL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _REF_HXX //autogen
#include <tools/ref.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
namespace binfilter {

// CLASS -----------------------------------------------------------------

class SfxObjectShell;

#ifndef SFX_DECL_OBJECTSHELL_DEFINED
#define SFX_DECL_OBJECTSHELL_DEFINED
SV_DECL_REF(SfxObjectShell)
SV_DECL_LOCK(SfxObjectShell)
#endif

class SfxDocTemplate_Impl;

#ifndef SFX_DECL_DOCTEMPLATES_DEFINED
#define SFX_DECL_DOCTEMPLATES_DEFINED
SV_DECL_REF(SfxDocTemplate_Impl)
#endif

// class SfxDocumentTemplates --------------------------------------------

class SfxDocumentTemplates
{
private:
	SfxDocTemplate_ImplRef	pImp;

public:
						SfxDocumentTemplates();
						SfxDocumentTemplates(const SfxDocumentTemplates &);
						~SfxDocumentTemplates();

	BOOL				IsConstructed() { return pImp != NULL; }
	void				Construct();

	static BOOL			SaveDir( /*SfxTemplateDir &rEntry */ ) ;
	const SfxDocumentTemplates &operator=(const SfxDocumentTemplates &);


	BOOL                IsRegionLoaded( USHORT nIdx ) const;
	USHORT				GetRegionCount() const;
	const String&		GetRegionName(USHORT nIdx) const;					//dv!
	String 				GetFullRegionName(USHORT nIdx) const;
	USHORT				GetRegionNo( const String &rRegionName ) const;

	USHORT				GetCount(USHORT nRegion) const;
	USHORT				GetCount( const String &rName) const;
	const String&		GetName(USHORT nRegion, USHORT nIdx) const;			//dv!
	String				GetFileName(USHORT nRegion, USHORT nIdx) const;
	String				GetPath(USHORT nRegion, USHORT nIdx) const;

	String				GetDefaultTemplatePath(const String &rLongName);

	// Pfad zur Vorlage geben lassen; logischer Name muss angegeben
	// werden, damit beim Ueberschreiben einer Vorlage der
	// richtige Dateiname gefunden werden kann
	String				GetTemplatePath(USHORT nRegion, const String &rLongName) const;

	// Speichern als Vorlage hat geklappt -> Aktualisieren
	void			NewTemplate(USHORT nRegion,
								const String &rLongName,
								const String &rFileName);




	BOOL 			GetLogicNames( const String& rPath, String& rRegion, String& rName ) const;

	/** updates the configuration where the document templates structure is stored.

		<p>The info about the document templates (which files, which groups etc.) is stored in the
		configuration. This means that just by copying files into OOo's template directories, this
		change is not reflected in the SfxDocumentTemplates - 'cause the configuration is not synchronous with
		the file system. This can be enforced with this method.</p>

	@param _bSmart
		The update of the configuration is rather expensive - nothing you want to do regulary if you don't really
		need it. So you have the possibility to do a smart update - it first checks if the update if necessary.
		In case the update is needed, the additional check made it somewhat more expensive. In case it's not
		necessary (which should be the usual case), the check alone is (much) less expensive than the real update.
		<br/>
		So set <arg>_bSmart</arg> to <TRUE/> to do a check for necessity first.
	*/
};

}//end of namespace binfilter
#endif // #ifndef _SFXDOCTEMPL_HXX

