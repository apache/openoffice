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


#ifndef _SVX_FMGROUP_HXX
#define _SVX_FMGROUP_HXX

#ifndef _SVX_FMCPONT_HXX
#include "fmcpont.hxx"
#endif

#ifndef __DBFORM_HXX
#include <dbform.hxx>
#endif
namespace binfilter {

DECLARE_LIST( FmCtrlModelList, XInterface* )//STRIP008 DECLARE_LIST( FmCtrlModelList, XInterface* );

struct FmXGroupModel_Impl;

//==================================================================
// FmXGroupModel
//==================================================================
class FmXGroupModel : public XBoundControl,
					  public XLoadListener,
					  public XPropertyChangeListener,
					  public XGroup,
					  public FmXControlModel
{
	FmFormControlMap		aControlMap;

	friend Reflection*		FmXGroupModel_getReflection();
	FmXGroupModel_Impl*		pGroupModelData;			// Properties

public:
	FmXGroupModel();
	virtual ~FmXGroupModel();

	// UNO Anbindung
	SMART_UNO_DECLARATION( FmXGroupModel, FmXControlModel );
	virtual XInterface*		queryInterface(UsrUik);
	virtual XIdlClassRef	getIdlClass();

	// XGroup
	virtual INT32 getFormControlCount();
	virtual XFormControlRef getFormControlByName( const XubString& sName ) const;
	virtual XFormControlRef getFormControlByIndex( INT32 Index ) const;
	virtual void appendFormControl( const XFormControlRef& FormControl );
	virtual XFormControlRef removeFormControl( const XFormControlRef& FormControl );

	// XFormControl
	virtual void setParent( const XFormRef& Parent );

	// XEventListener
	virtual void disposing( const EventObject& Source );

	// XPropertiesChangeListener
	virtual void propertyChange( const PropertyChangeEvent& evt );

	// XLoadListener
	virtual void loaded( const EventObject& rEvent );
	virtual void unloaded( const EventObject& rEvent );

	// XBoundControl
	virtual void addBoundControlListener( const XBoundControlListenerRef& l );
	virtual void removeBoundControlListener( const XBoundControlListenerRef& l );

	// XPersistObject
	virtual XubString getServiceName( void ) const;
	virtual void write( const XObjectOutputStreamRef& OutStream );
	virtual void read( const XObjectInputStreamRef& InStream );

	// PropertySetInterface
	virtual FmXPropertySetInfo* createPropertySetInfo() const;
	virtual BOOL setPropertyValue( UINT16 nId, const XubString& aPropertyName,
								   const UsrAny& aValue,
								   PropertyChangeEventSequence* pSeq,
								   INT32 nIndex );
	virtual UsrAny getPropertyValue( UINT16 nId, const XubString& aPropertyName ) const;
	virtual	void addPropertyChangeListener( const XubString& aPropertyName, const XPropertyChangeListenerRef& aListener );
	virtual void removePropertyChangeListener( const XubString& aPropertyName, const XPropertyChangeListenerRef& aListener );
};

//==================================================================
// FmXGroupModelInfo
// Properties:
//==================================================================
class FmXGroupModelInfo : public FmXControlModelInfo
{
	friend class FmXGroupModel;

protected:
	FmXGroupModelInfo( const FmXGroupModel* pCp );
	virtual void fillProperties( UINT32& nIndex, PropertySequence& aSeq ) const;
};


}//end of namespace binfilter
#endif // _SVX_FMGROUP_HXX

