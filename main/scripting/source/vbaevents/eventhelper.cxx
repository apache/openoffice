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
#include "precompiled_scripting.hxx"
#include <comphelper/processfactory.hxx>
#include <comphelper/uno3.hxx>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>

#include <ooo/vba/XVBAToOOEventDescGen.hpp>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XIntrospection.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>

#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XServiceName.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>

#include <com/sun/star/util/XCloseListener.hpp>
#include <com/sun/star/util/XCloseBroadcaster.hpp>

#include <com/sun/star/frame/XModel.hpp>

#include <com/sun/star/script/XLibraryContainer.hpp>
#include <com/sun/star/script/ScriptEventDescriptor.hpp>
#include <com/sun/star/script/provider/XScriptProviderSupplier.hpp>

#include <com/sun/star/drawing/XControlShape.hpp>

#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XDialog.hpp>
#include <com/sun/star/awt/KeyEvent.hpp>
#include <com/sun/star/awt/MouseEvent.hpp>
#include <com/sun/star/awt/XFixedText.hpp> //liuchen 2009-6-5
#include <com/sun/star/awt/XTextComponent.hpp> //liuchen 2009-6-5
#include <com/sun/star/awt/XComboBox.hpp> //liuchen 2009-6-18
#include <com/sun/star/awt/XRadioButton.hpp> //liuchen 2009-7-30

#include <msforms/ReturnInteger.hpp>

#include <sfx2/objsh.hxx>
#include <basic/sbstar.hxx>
#include <basic/basmgr.hxx>
#include <basic/sbmeth.hxx>
#include <basic/sbmod.hxx>
#include <basic/sbx.hxx>




// for debug
#include <comphelper/anytostring.hxx>


#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/script/XScriptListener.hpp>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase3.hxx>
#include <comphelper/evtmethodhelper.hxx>

#include <set>
#include <list>
#include <hash_map>

using namespace ::com::sun::star;
using namespace ::com::sun::star::script;
using namespace ::com::sun::star::uno;
using namespace ::ooo::vba;

#define MAP_CHAR_LEN(x) ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(x))//liuchen 2009-6-8
#define GET_TYPE(x) ::getCppuType((uno::Reference< x > *)0);

// Some constants 
const static rtl::OUString DELIM = rtl::OUString::createFromAscii( "::" );
const static sal_Int32 DELIMLEN = DELIM.getLength();

#if 0
void dumpListeners( const Reference< beans::XIntrospection >& xIntrospection, const Reference<XInterface>& xIfc)
{
    Reference< beans::XIntrospectionAccess > xIntrospectionAccess;
    if ( xIntrospection.is() )
    {
        xIntrospectionAccess = xIntrospection->inspect( 
            makeAny( xIfc ) );
        Sequence< Type > aControlListeners = 
            xIntrospectionAccess->getSupportedListeners();
        sal_Int32 nLength = aControlListeners.getLength();
             
        for ( sal_Int32 i = 0; i< nLength; ++i )
        {
            Type& listType = aControlListeners[ i ];
            rtl::OUString sFullTypeName = listType.getTypeName();
            rtl::OUString sTypeName = listType.getTypeName();
            sal_Int32 lastDotIndex = -1;
            if ( ( lastDotIndex = sFullTypeName.lastIndexOf( '.' ) ) > -1 )
            {
                sTypeName = sFullTypeName.copy( lastDotIndex + 1 );    
            } 
            Sequence< ::rtl::OUString > sMeths = comphelper::getEventMethodsForType( listType );    
            sal_Int32 sMethLen = sMeths.getLength();
            for ( sal_Int32 j=0 ; j < sMethLen; ++j )
            {
                OSL_TRACE("**Listener [%d] Type[%s] Method[%s]",j,
                    rtl::OUStringToOString( sTypeName, 
                        RTL_TEXTENCODING_UTF8 ).getStr(),
                    rtl::OUStringToOString( sMeths[ j ], 
                        RTL_TEXTENCODING_UTF8 ).getStr() );
            }
        }
       
    }
}

void dumpEvent( const ScriptEvent& evt )
{
    OSL_TRACE("dumpEvent: Source %s",
        rtl::OUStringToOString( comphelper::anyToString( makeAny(evt.Source)), 
            RTL_TEXTENCODING_UTF8 ).getStr() );

    OSL_TRACE("dumpEvent: ScriptType %s",
        rtl::OUStringToOString( evt.ScriptType, 
            RTL_TEXTENCODING_UTF8 ).getStr() );
    
    OSL_TRACE("dumpEvent: ScriptCode %s",
        rtl::OUStringToOString( evt.ScriptCode, 
            RTL_TEXTENCODING_UTF8 ).getStr() );

    OSL_TRACE("dumpEvent: ListenerType %s",
        rtl::OUStringToOString( evt.ListenerType.getTypeName(), 
            RTL_TEXTENCODING_UTF8 ).getStr() );
    
    OSL_TRACE("dumpEvent: Listener methodname %s",
        rtl::OUStringToOString( evt.MethodName, 
            RTL_TEXTENCODING_UTF8 ).getStr() );

    OSL_TRACE("dumpEvent: arguments;");
    sal_Int32 nLen = evt.Arguments.getLength();
    for ( sal_Int32 index=0; index < nLen; ++index )
    {
        OSL_TRACE("\t [%d] %s", index, 
        rtl::OUStringToOString( comphelper::anyToString( evt.Arguments[ index ] ), 
            RTL_TEXTENCODING_UTF8 ).getStr() );

    }
}

#endif

bool isKeyEventOk( awt::KeyEvent& evt, const Sequence< Any >& params )
{
    if ( !( params.getLength() > 0 ) ||
        !( params[ 0 ] >>= evt ) )
        return false;
    return true;
}

bool isMouseEventOk( awt::MouseEvent& evt, const Sequence< Any >& params )
{
    if ( !( params.getLength() > 0 ) ||
        !( params[ 0 ] >>= evt ) )
        return false;
    return true;
}

Sequence< Any > ooMouseEvtToVBADblClick( const Sequence< Any >& params )
{
    Sequence< Any > translatedParams;
    awt::MouseEvent evt;

    if ( !( isMouseEventOk(evt, params)) || 
        (evt.ClickCount != 2) )
        return Sequence< Any >(); 
    // give back orig params, this will signal that the event is good
    return params;
}

Sequence< Any > ooMouseEvtToVBAMouseEvt( const Sequence< Any >& params )
{
    Sequence< Any > translatedParams;
    awt::MouseEvent evt;

    if ( !isMouseEventOk(evt, params) )
        return Sequence< Any >(); 

    translatedParams.realloc(4);

    // Buttons
    translatedParams[ 0 ] <<= evt.Buttons;
    // Shift
    translatedParams[ 1 ] <<= evt.Modifiers;
    // X
    translatedParams[ 2 ] <<= evt.X;
    // Y
    translatedParams[ 3 ] <<= evt.Y;
    return translatedParams;
}

Sequence< Any > ooKeyPressedToVBAKeyPressed( const Sequence< Any >& params )
{
    Sequence< Any > translatedParams;
    awt::KeyEvent evt;

    if ( !isKeyEventOk( evt, params ) )
        return Sequence< Any >(); 

    translatedParams.realloc(1);

    msforms::ReturnInteger keyCode;
    keyCode.Value = evt.KeyCode; 
    translatedParams[0] <<= keyCode;
    return  translatedParams;
}

Sequence< Any > ooKeyPressedToVBAKeyUpDown( const Sequence< Any >& params )
{
    Sequence< Any > translatedParams;
    awt::KeyEvent evt;

    if ( !isKeyEventOk( evt, params ) )
        return Sequence< Any >(); 

    translatedParams.realloc(2);

    msforms::ReturnInteger keyCode;
    sal_Int8 shift = sal::static_int_cast<sal_Int8>( evt.Modifiers );

    // #TODO check whether values from OOO conform to values generated from vba
    keyCode.Value = evt.KeyCode; 
    translatedParams[0] <<= keyCode;
    translatedParams[1] <<= shift;
    return  translatedParams;
}

typedef Sequence< Any > (*Translator)(const Sequence< Any >&);

//liuchen 2009-6-23
//expand the "TranslateInfo" struct to support more kinds of events 
struct TranslateInfo
{
    rtl::OUString sVBAName; //vba event name
    Translator toVBA;       //the method to convert OO event parameters to VBA event parameters	
	bool (*ApproveRule)(const ScriptEvent& evt, void* pPara); //this method is used to determine which types of controls should execute the event 
	void *pPara;			//Parameters for the above approve method
};


typedef std::hash_map< rtl::OUString, 
std::list< TranslateInfo >, 
::rtl::OUStringHash,
::std::equal_to< ::rtl::OUString > > EventInfoHash;

//liuchen 2009-6-23
struct TranslatePropMap
{
	rtl::OUString sEventInfo;   //OO event name
	TranslateInfo aTransInfo;   
};
 
bool ApproveAll(const ScriptEvent& evt, void* pPara); //allow all types of controls to execute the event
bool ApproveType(const ScriptEvent& evt, void* pPara); //certain types of controls should execute the event, those types are given by pPara
bool DenyType(const ScriptEvent& evt, void* pPara);    //certain types of controls should not execute the event, those types are given by pPara
bool DenyMouseDrag(const ScriptEvent& evt, void* pPara); //used for VBA MouseMove event when "Shift" key is pressed

struct TypeList
{
	uno::Type* pTypeList;
	int nListLength;
};

Type typeXFixedText = GET_TYPE(awt::XFixedText)
Type typeXTextComponent = GET_TYPE(awt::XTextComponent)
Type typeXComboBox = GET_TYPE(awt::XComboBox)
Type typeXRadioButton = GET_TYPE(awt::XRadioButton)


TypeList fixedTextList = {&typeXFixedText, 1};
TypeList textCompList = {&typeXTextComponent, 1};
TypeList radioButtonList = {&typeXRadioButton, 1};
TypeList comboBoxList = {&typeXComboBox, 1};

//this array stores the OO event to VBA event translation info
static TranslatePropMap aTranslatePropMap_Impl[] = 
{
	// actionPerformed ooo event
	{ MAP_CHAR_LEN("actionPerformed"), { MAP_CHAR_LEN("_Click"), NULL, ApproveAll, NULL } },	
	{ MAP_CHAR_LEN("actionPerformed"), { MAP_CHAR_LEN("_Change"), NULL, DenyType, (void*)(&radioButtonList) } },  //liuchen 2009-7-30, OptionalButton_Change event is not the same as OptionalButton_Click event

	// itemStateChanged ooo event
	{ MAP_CHAR_LEN("itemStateChanged"), { MAP_CHAR_LEN("_Click"), NULL, ApproveType, (void*)(&comboBoxList) } },  //liuchen, add to support VBA ComboBox_Click event
	{ MAP_CHAR_LEN("itemStateChanged"), { MAP_CHAR_LEN("_Change"), NULL, ApproveType, (void*)(&radioButtonList) } }, //liuchen 2009-7-30, OptionalButton_Change event should be triggered when the button state is changed
	
	// changed ooo event
	{ MAP_CHAR_LEN("changed"), { MAP_CHAR_LEN("_Change"), NULL, ApproveAll, NULL } },	

	// focusGained ooo event
	{ MAP_CHAR_LEN("focusGained"), { MAP_CHAR_LEN("_GotFocus"), NULL, ApproveAll, NULL } },

	// focusLost ooo event
	{ MAP_CHAR_LEN("focusLost"), { MAP_CHAR_LEN("_LostFocus"), NULL, ApproveAll, NULL } },
	{ MAP_CHAR_LEN("focusLost"), { MAP_CHAR_LEN("_Exit"), NULL, ApproveType, (void*)(&textCompList) } }, //liuchen, add to support VBA TextBox_Exit event

	// adjustmentValueChanged ooo event
	{ MAP_CHAR_LEN("adjustmentValueChanged"), { MAP_CHAR_LEN("_Scroll"), NULL, ApproveAll, NULL } },
	{ MAP_CHAR_LEN("adjustmentValueChanged"), { MAP_CHAR_LEN("_Change"), NULL, ApproveAll, NULL } },

	// textChanged ooo event
	{ MAP_CHAR_LEN("textChanged"), { MAP_CHAR_LEN("_Change"), NULL, ApproveAll, NULL } },

	// keyReleased ooo event
	{ MAP_CHAR_LEN("keyReleased"), { MAP_CHAR_LEN("_KeyUp"), ooKeyPressedToVBAKeyUpDown, ApproveAll, NULL } },

	// mouseReleased ooo event
	{ MAP_CHAR_LEN("mouseReleased"), { MAP_CHAR_LEN("_Click"), ooMouseEvtToVBAMouseEvt, ApproveType, (void*)(&fixedTextList) } }, //liuchen, add to support VBA Label_Click event
	{ MAP_CHAR_LEN("mouseReleased"), { MAP_CHAR_LEN("_MouseUp"), ooMouseEvtToVBAMouseEvt, ApproveAll, NULL } },

	// mousePressed ooo event
	{ MAP_CHAR_LEN("mousePressed"), { MAP_CHAR_LEN("_MouseDown"), ooMouseEvtToVBAMouseEvt, ApproveAll, NULL } },
	{ MAP_CHAR_LEN("mousePressed"), { MAP_CHAR_LEN("_DblClick"), ooMouseEvtToVBADblClick, ApproveAll, NULL } },

	// mouseMoved ooo event	
	{ MAP_CHAR_LEN("mouseMoved"), { MAP_CHAR_LEN("_MouseMove"), ooMouseEvtToVBAMouseEvt, ApproveAll, NULL } },
	{ MAP_CHAR_LEN("mouseDragged"), { MAP_CHAR_LEN("_MouseMove"), ooMouseEvtToVBAMouseEvt, DenyMouseDrag, NULL } }, //liuchen, add to support VBA MouseMove event when the "Shift" key is pressed

	// keyPressed ooo event
	{ MAP_CHAR_LEN("keyPressed"), { MAP_CHAR_LEN("_KeyDown"), ooKeyPressedToVBAKeyPressed, ApproveAll, NULL } },
	{ MAP_CHAR_LEN("keyPressed"), { MAP_CHAR_LEN("_KeyPress"), ooKeyPressedToVBAKeyPressed, ApproveAll, NULL } }
};

EventInfoHash& getEventTransInfo()
{
    static bool initialised = false;
    static EventInfoHash eventTransInfo;
    if ( !initialised )
    {
        rtl::OUString sEventInfo = MAP_CHAR_LEN("");
        TranslatePropMap* pTransProp = aTranslatePropMap_Impl;
        int nCount = sizeof(aTranslatePropMap_Impl) / sizeof(aTranslatePropMap_Impl[0]);

        int i = 0;
        while (i < nCount)
        {		
            sEventInfo = pTransProp->sEventInfo;
            std::list< TranslateInfo > infoList;
            do
            {									
                infoList.push_back( pTransProp->aTransInfo );
                pTransProp++;
                i++;
            }while(i < nCount && sEventInfo == pTransProp->sEventInfo);
            eventTransInfo[sEventInfo] = infoList;			
        }	
        initialised = true;
    }
    return eventTransInfo;
}
//liuchen 2009-6-23 end

// Helper class

class ScriptEventHelper
{
public:
    ScriptEventHelper( const Reference< XInterface >& xControl );
    Sequence< ScriptEventDescriptor > createEvents( const rtl::OUString& sCodeName );   
    Sequence< rtl::OUString > getEventListeners();
private:
    Reference< XComponentContext > m_xCtx;
    Reference< XInterface > m_xControl;
};

bool
eventMethodToDescriptor( const ::rtl::OUString& rEventMethod, ScriptEventDescriptor& evtDesc, const ::rtl::OUString& sCodeName )
{
    // format of ControlListener is TypeName::methodname e.g.
    // "com.sun.star.awt.XActionListener::actionPerformed" or
    // "XActionListener::actionPerformed

    ::rtl::OUString sMethodName;
    ::rtl::OUString sTypeName;
    sal_Int32 nDelimPos = rEventMethod.indexOf( DELIM );
    if ( nDelimPos == -1 )
    {
        return false;
    }
    sMethodName = rEventMethod.copy( nDelimPos + DELIMLEN );
    sTypeName = rEventMethod.copy( 0, nDelimPos );
            
    EventInfoHash& infos = getEventTransInfo();

    // Only create an ScriptEventDescriptor for an event we can translate
    // or emulate
    if ( sMethodName.getLength() 
         && sTypeName.getLength()
         && ( infos.find( sMethodName ) != infos.end() ) )
    {
        // just fill in CodeName, when the event fires the other 
	// info is gathered from the event source to determine what
	// event handler we try to call
        evtDesc.ScriptCode = sCodeName;
        evtDesc.ListenerType = sTypeName;
        evtDesc.EventMethod = sMethodName;

        // set this it VBAInterop, ensures that it doesn't
        // get persisted or shown in property editors
        evtDesc.ScriptType = rtl::OUString::createFromAscii(
            "VBAInterop" );	
        return true;
    }
    return false;

}

ScriptEventHelper::ScriptEventHelper( const Reference< XInterface >& xControl ) : m_xControl( xControl )
{
    Reference < beans::XPropertySet > xProps(
        ::comphelper::getProcessServiceFactory(), UNO_QUERY_THROW );
    m_xCtx.set( xProps->getPropertyValue( rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))), 
        uno::UNO_QUERY_THROW );
}

Sequence< rtl::OUString > 
ScriptEventHelper::getEventListeners()
{
    Reference< lang::XMultiComponentFactory > xMFac( 
        m_xCtx->getServiceManager(), UNO_QUERY );
    std::list< rtl::OUString > eventMethods;

    if ( xMFac.is() )
    {
        Reference< beans::XIntrospection > xIntrospection( 
            xMFac->createInstanceWithContext( rtl::OUString( 
                RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.beans.Introspection"  ) ), m_xCtx ), UNO_QUERY );
#if 0
        dumpListeners( xIntrospection, m_xControl );
        dumpListeners( xIntrospection, m_xControl->getModel() );
#endif 
        Reference< beans::XIntrospectionAccess > xIntrospectionAccess;
	if  ( xIntrospection.is() )
	{
            xIntrospectionAccess = xIntrospection->inspect( 
                makeAny( m_xControl ) );
            Sequence< Type > aControlListeners = 
                xIntrospectionAccess->getSupportedListeners();
            sal_Int32 nLength = aControlListeners.getLength();             
            for ( sal_Int32 i = 0; i< nLength; ++i )
            {
                Type& listType = aControlListeners[ i ];
                rtl::OUString sFullTypeName = listType.getTypeName();
                Sequence< ::rtl::OUString > sMeths = 
                    comphelper::getEventMethodsForType( listType );    
                sal_Int32 sMethLen = sMeths.getLength();
                for ( sal_Int32 j=0 ; j < sMethLen; ++j )
                {
                    rtl::OUString sEventMethod = sFullTypeName;
                    sEventMethod += DELIM;
                    sEventMethod += sMeths[ j ];
                    eventMethods.push_back( sEventMethod );
                }
            }
            
        }
    }

    Sequence< rtl::OUString > sEventMethodNames( eventMethods.size() );
    std::list< rtl::OUString >::const_iterator it = eventMethods.begin();
    rtl::OUString* pDest = sEventMethodNames.getArray();

    for ( ; it != eventMethods.end(); ++it, ++pDest )
        *pDest = *it;         

    return sEventMethodNames;
}

Sequence< ScriptEventDescriptor > 
ScriptEventHelper::createEvents( const rtl::OUString& sCodeName )
{
    Sequence< rtl::OUString > aControlListeners = getEventListeners();
    rtl::OUString* pSrc = aControlListeners.getArray();
    sal_Int32 nLength = aControlListeners.getLength();             

    Sequence< ScriptEventDescriptor > aDest( nLength );
    sal_Int32 nEvts = 0; 
    for ( sal_Int32 i = 0; i< nLength; ++i, ++pSrc )
    {
        // from getListeners eventName is of form
        // "com.sun.star.awt.XActionListener::actionPerformed"
        // we need to strip "com.sun.star.awt." from that for form
        // controls
    	ScriptEventDescriptor evtDesc;
        if ( eventMethodToDescriptor( *pSrc, evtDesc, sCodeName ) )
        {
            sal_Int32 dIndex = nEvts;
            ++nEvts;
            if ( nEvts > aDest.getLength() )
                aDest.realloc( nEvts );// should never happen
            aDest[ dIndex ] = evtDesc;
        }
    }
    aDest.realloc( nEvts );

    return aDest;
}


typedef ::cppu::WeakImplHelper1< container::XNameContainer > NameContainer_BASE;

class ReadOnlyEventsNameContainer : public NameContainer_BASE
{
public:
    ReadOnlyEventsNameContainer( const Sequence< rtl::OUString >& eventMethods, const rtl::OUString& sCodeName );
    // XNameContainer

    virtual void SAL_CALL insertByName( const ::rtl::OUString&, const Any& ) throw (lang::IllegalArgumentException, container::ElementExistException, lang::WrappedTargetException, RuntimeException)
    {
        throw RuntimeException( rtl::OUString::createFromAscii( "ReadOnly container" ), Reference< XInterface >() );
 
    }
    virtual void SAL_CALL removeByName( const ::rtl::OUString& ) throw (::com::sun::star::container::NoSuchElementException, lang::WrappedTargetException, RuntimeException)
    {
        throw RuntimeException( rtl::OUString::createFromAscii( "ReadOnly container" ), Reference< XInterface >() );
    }

    // XNameReplace
    virtual void SAL_CALL replaceByName( const ::rtl::OUString&, const Any& ) throw (lang::IllegalArgumentException, container::NoSuchElementException, lang::WrappedTargetException, RuntimeException)
    {
        throw RuntimeException( rtl::OUString::createFromAscii( "ReadOnly container" ), Reference< XInterface >() );
 
    }
                                                                                
    // XNameAccess
    virtual Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw (container::NoSuchElementException, lang::WrappedTargetException, RuntimeException);
    virtual Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw (RuntimeException);
    virtual ::sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (RuntimeException);

    // XElementAccess
    virtual Type SAL_CALL getElementType(  ) throw (RuntimeException)
    { return getCppuType(static_cast< const rtl::OUString * >(0) ); }
    virtual ::sal_Bool SAL_CALL hasElements(  ) throw (RuntimeException)
    { return ( ( m_hEvents.size() > 0 ? sal_True : sal_False ) ); }
private:

typedef std::hash_map< rtl::OUString, Any, ::rtl::OUStringHash,
::std::equal_to< ::rtl::OUString > > EventSupplierHash;

    EventSupplierHash m_hEvents;
};

ReadOnlyEventsNameContainer::ReadOnlyEventsNameContainer( const Sequence< rtl::OUString >& eventMethods, const rtl::OUString& sCodeName )
{
    const rtl::OUString* pSrc = eventMethods.getConstArray();
    sal_Int32 nLen = eventMethods.getLength();
    for ( sal_Int32 index = 0; index < nLen; ++index, ++pSrc )
    {
        Any aDesc;
        ScriptEventDescriptor evtDesc;
        if (  eventMethodToDescriptor( *pSrc, evtDesc, sCodeName ) )
        {
            aDesc <<= evtDesc; 
            m_hEvents[ *pSrc ] = aDesc;
        }
    }
}

Any SAL_CALL 
ReadOnlyEventsNameContainer::getByName( const ::rtl::OUString& aName ) throw (container::NoSuchElementException, lang::WrappedTargetException, RuntimeException){
    EventSupplierHash::const_iterator it = m_hEvents.find( aName );
    if ( it == m_hEvents.end() )
        throw container::NoSuchElementException();
    return it->second;
}

Sequence< ::rtl::OUString > SAL_CALL 
ReadOnlyEventsNameContainer::getElementNames(  ) throw (RuntimeException)
{
    Sequence< ::rtl::OUString > names(m_hEvents.size());
    rtl::OUString* pDest = names.getArray();
    EventSupplierHash::const_iterator it = m_hEvents.begin();
    EventSupplierHash::const_iterator it_end = m_hEvents.end();
    for ( sal_Int32 index = 0; it != it_end; ++index, ++pDest, ++it )
        *pDest = it->first;
    return names;
}

sal_Bool SAL_CALL 
ReadOnlyEventsNameContainer::hasByName( const ::rtl::OUString& aName ) throw (RuntimeException)
{
    EventSupplierHash::const_iterator it = m_hEvents.find( aName );
    if ( it == m_hEvents.end() )
        return sal_False;
    return sal_True;
}

typedef ::cppu::WeakImplHelper1< XScriptEventsSupplier > EventsSupplier_BASE;

class ReadOnlyEventsSupplier : public EventsSupplier_BASE
{
public:
    ReadOnlyEventsSupplier( const Sequence< ::rtl::OUString >& eventMethods, const rtl::OUString& sCodeName )
    { m_xNameContainer = new ReadOnlyEventsNameContainer( eventMethods, sCodeName ); }
    
    // XScriptEventSupplier
    virtual Reference< container::XNameContainer > SAL_CALL getEvents(  ) throw (RuntimeException){ return m_xNameContainer; }
private:
    Reference< container::XNameContainer > m_xNameContainer;
};

typedef ::cppu::WeakImplHelper3< XScriptListener, util::XCloseListener, lang::XInitialization > EventListener_BASE;

#define EVENTLSTNR_PROPERTY_ID_MODEL         1
#define EVENTLSTNR_PROPERTY_MODEL            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Model" ) )

class EventListener : public EventListener_BASE
    ,public ::comphelper::OMutexAndBroadcastHelper
    ,public ::comphelper::OPropertyContainer
    ,public ::comphelper::OPropertyArrayUsageHelper< EventListener >

{
    
public:
    EventListener( const Reference< XComponentContext >& rxContext );
    // XEventListener
    virtual void SAL_CALL disposing(const lang::EventObject& Source) throw( RuntimeException );
	using cppu::OPropertySetHelper::disposing;

    // XScriptListener
    virtual void SAL_CALL firing(const ScriptEvent& evt) throw(RuntimeException);
    virtual Any SAL_CALL approveFiring(const ScriptEvent& evt) throw(reflection::InvocationTargetException, RuntimeException);
    // XCloseListener
    virtual void SAL_CALL queryClosing( const lang::EventObject& Source, ::sal_Bool GetsOwnership ) throw (util::CloseVetoException, uno::RuntimeException);
    virtual void SAL_CALL notifyClosing( const lang::EventObject& Source ) throw (uno::RuntimeException);
    // XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (::com::sun::star::uno::RuntimeException);    
    // XInitialization
    virtual void SAL_CALL initialize( const Sequence< Any >& aArguments ) throw (Exception, RuntimeException);
    // XInterface
    DECLARE_XINTERFACE()

    // XTypeProvider
    DECLARE_XTYPEPROVIDER()
    virtual void SAL_CALL setFastPropertyValue( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
    {
        if ( nHandle == EVENTLSTNR_PROPERTY_ID_MODEL )
        {
            uno::Reference< frame::XModel > xModel( rValue, uno::UNO_QUERY );
            if( xModel != m_xModel)
            {
                // Remove the listener from the old XCloseBroadcaster.
                uno::Reference< util::XCloseBroadcaster > xCloseBroadcaster( m_xModel, uno::UNO_QUERY );
                if (xCloseBroadcaster.is())
                {
                    xCloseBroadcaster->removeCloseListener( this );
                }
                // Add the listener into the new XCloseBroadcaster.
                xCloseBroadcaster = uno::Reference< util::XCloseBroadcaster >( xModel, uno::UNO_QUERY );
                if (xCloseBroadcaster.is())
                {
                    xCloseBroadcaster->addCloseListener( this );
                }
            }
        }
        OPropertyContainer::setFastPropertyValue( nHandle, rValue );
        if ( nHandle == EVENTLSTNR_PROPERTY_ID_MODEL )
            setShellFromModel();
    }

protected:
    // OPropertySetHelper
    virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper(  );

    // OPropertyArrayUsageHelper
    virtual ::cppu::IPropertyArrayHelper* createArrayHelper(  ) const;

private:
    void setShellFromModel();
    void firing_Impl( const  ScriptEvent& evt, Any *pSyncRet=NULL ) throw( RuntimeException );

    Reference< XComponentContext > m_xContext;
    Reference< frame::XModel > m_xModel;
    sal_Bool m_bDocClosed;
    SfxObjectShell* mpShell;
    
};

EventListener::EventListener( const Reference< XComponentContext >& rxContext ) :
OPropertyContainer(GetBroadcastHelper()), m_xContext( rxContext ), m_bDocClosed(sal_False), mpShell( 0 )
{
    registerProperty( EVENTLSTNR_PROPERTY_MODEL, EVENTLSTNR_PROPERTY_ID_MODEL,
        beans::PropertyAttribute::TRANSIENT, &m_xModel, ::getCppuType( &m_xModel ) );

}

void
EventListener::setShellFromModel()
{
	// reset mpShell
	mpShell = 0;
	SfxObjectShell* pShell = SfxObjectShell::GetFirst();
	while ( m_xModel.is() && pShell )
	{
		if ( pShell->GetModel() == m_xModel )
		{
			mpShell = pShell;
			break;
		}
		pShell = SfxObjectShell::GetNext( *pShell );
	}    
}

//XEventListener
void
EventListener::disposing(const lang::EventObject&)  throw( RuntimeException )
{
}

//XScriptListener

void SAL_CALL 
EventListener::firing(const ScriptEvent& evt) throw(RuntimeException)
{
    firing_Impl( evt );
}

Any SAL_CALL 
EventListener::approveFiring(const ScriptEvent& evt) throw(reflection::InvocationTargetException, RuntimeException)
{
    Any ret;
    firing_Impl( evt, &ret );
    return ret;
}

// XCloseListener
void SAL_CALL 
EventListener::queryClosing( const lang::EventObject& Source, ::sal_Bool GetsOwnership ) throw (util::CloseVetoException, uno::RuntimeException)
{
    //Nothing to do
}

void SAL_CALL 
EventListener::notifyClosing( const lang::EventObject& Source ) throw (uno::RuntimeException)
{
    m_bDocClosed = sal_True;
    uno::Reference< util::XCloseBroadcaster > xCloseBroadcaster( m_xModel, uno::UNO_QUERY );
    if (xCloseBroadcaster.is())
    {
        xCloseBroadcaster->removeCloseListener( this );
    }
}

// XInitialization
void SAL_CALL 
EventListener::initialize( const Sequence< Any >& aArguments ) throw (Exception, RuntimeException)
{
    if ( aArguments.getLength() == 1 )
        aArguments[0] >>= m_xModel;
    OSL_TRACE("EventListener::initialize() args %d m_xModel %d", aArguments.getLength(), m_xModel.is() );
}

// XInterface

IMPLEMENT_FORWARD_XINTERFACE2( EventListener, EventListener_BASE, OPropertyContainer )

// XTypeProvider

IMPLEMENT_FORWARD_XTYPEPROVIDER2( EventListener, EventListener_BASE, OPropertyContainer )

// OPropertySetHelper

::cppu::IPropertyArrayHelper& 
EventListener::getInfoHelper(  )
{
    return *getArrayHelper();
}

// OPropertyArrayUsageHelper

::cppu::IPropertyArrayHelper* 
EventListener::createArrayHelper(  ) const
{
    Sequence< beans::Property > aProps;
    describeProperties( aProps );
    return new ::cppu::OPropertyArrayHelper( aProps );
}

// XPropertySet
Reference< beans::XPropertySetInfo > 
EventListener::getPropertySetInfo(  ) throw (RuntimeException)
{
    Reference< beans::XPropertySetInfo > xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}

//liuchen 2009-6-23
//decide if the control should execute the event
bool ApproveAll(const ScriptEvent&, void* )
{
	return true;
}

//for the given control type in evt.Arguments[0], look for if it appears in the type list in pPara
bool FindControl(const ScriptEvent& evt, void* pPara)
{
	lang::EventObject aEvent;
	evt.Arguments[ 0 ] >>= aEvent;
	uno::Reference< uno::XInterface > xInterface( aEvent.Source, uno::UNO_QUERY );

	TypeList* pTypeListInfo = static_cast<TypeList*>(pPara);
	Type* pType = pTypeListInfo->pTypeList;
	int nLen = pTypeListInfo->nListLength;

	for (int i = 0; i < nLen; i++)
	{
		if ( xInterface->queryInterface( *pType ).hasValue() )
		{
			return true;
		}
		pType++;
	}

	return false;
}

//if the given control type in evt.Arguments[0] appears in the type list in pPara, then approve the execution
bool ApproveType(const ScriptEvent& evt, void* pPara)
{
	return FindControl(evt, pPara);
}

//if the given control type in evt.Arguments[0] appears in the type list in pPara, then deny the execution
bool DenyType(const ScriptEvent& evt, void* pPara)
{
	return !FindControl(evt, pPara);
}

//when mouse is moving, either the mouse button is pressed or some key is pressed can trigger the OO mouseDragged event,
//the former should be denyed, and the latter allowed, only by doing so can the VBA MouseMove event when the "Shift" key is 
//pressed can be correctly triggered
bool DenyMouseDrag(const ScriptEvent& evt, void* )
{
	awt::MouseEvent aEvent;
	evt.Arguments[ 0 ] >>= aEvent;
	if (aEvent.Buttons == 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}



//liuchen 2009-6-23
// EventListener

void
EventListener::firing_Impl(const ScriptEvent& evt, Any* /*pRet*/ ) throw(RuntimeException)
{
	OSL_TRACE("EventListener::firing_Impl( FAKE VBA_EVENTS )");
    static const ::rtl::OUString vbaInterOp =
        ::rtl::OUString::createFromAscii("VBAInterop");

    // let default handlers deal with non vba stuff
    if ( !evt.ScriptType.equals( vbaInterOp ) )
        return;
    lang::EventObject aEvent;
    evt.Arguments[ 0 ] >>= aEvent;
	OSL_TRACE("Argument[0] is  %s", rtl::OUStringToOString( comphelper::anyToString( evt.Arguments[0] ), RTL_TEXTENCODING_UTF8 ).getStr() );
	OSL_TRACE("Getting Control");
    uno::Reference< awt::XControl > xControl( aEvent.Source, uno::UNO_QUERY_THROW );
	OSL_TRACE("Getting properties");
    uno::Reference< beans::XPropertySet > xProps( xControl->getModel(), uno::UNO_QUERY_THROW );

    rtl::OUString sName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("UserForm") );
	OSL_TRACE("Getting Name");

    uno::Reference< awt::XDialog > xDlg( aEvent.Source, uno::UNO_QUERY );
    if ( !xDlg.is() )
        xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Name") ) ) >>= sName;
    //dumpEvent( evt );
    EventInfoHash& infos = getEventTransInfo();
    EventInfoHash::const_iterator eventInfo_it = infos.find( evt.MethodName );
    EventInfoHash::const_iterator it_end = infos.end();
    if ( eventInfo_it == it_end )
    {
        OSL_TRACE("Bogus event for %s",
            rtl::OUStringToOString( evt.ScriptType, RTL_TEXTENCODING_UTF8 ).getStr() );
        return;
    }

    uno::Reference< script::provider::XScriptProviderSupplier > xSPS( m_xModel, uno::UNO_QUERY );
    uno::Reference< script::provider::XScriptProvider > xScriptProvider;
    if ( xSPS.is() )
        xScriptProvider =  xSPS->getScriptProvider();
    if ( xScriptProvider.is() && mpShell )
    {
        std::list< TranslateInfo > matchingMethods;
        std::list< TranslateInfo >::const_iterator txInfo =
            eventInfo_it->second.begin();
        std::list< TranslateInfo >::const_iterator txInfo_end = eventInfo_it->second.end();
        rtl::OUString sMacroLoc = rtl::OUString::createFromAscii("Standard.").concat( evt.ScriptCode ).concat( rtl::OUString::createFromAscii(".") );
       
        StarBASIC* pBasic = mpShell->GetBasic();
        SbModule* pModule = pBasic->FindModule( evt.ScriptCode );
        for ( ; pModule && txInfo != txInfo_end; ++txInfo )
        {
	    // #i106270#: If the document is closed, we should not execute macro.
	    if (m_bDocClosed)
	    {
		break;
	    }
            // see if we have a match for the handlerextension
            // where ScriptCode is methodname_handlerextension 
            rtl::OUString sTemp = sName.concat( (*txInfo).sVBAName ); 
            
            OSL_TRACE("*** trying to invoke %s ",
                rtl::OUStringToOString( sTemp, RTL_TEXTENCODING_UTF8 ).getStr() );
            SbMethod* pMeth = static_cast< SbMethod* >( pModule->Find( sTemp, SbxCLASS_METHOD ) );
            if ( pMeth )
            {
				//liuchen 2009-6-8
				if (! txInfo->ApproveRule(evt, txInfo->pPara) )
				{
					continue;
				}
				//liuchen 2009-6-8
                // !! translate arguments & emulate events where necessary 
                Sequence< Any > aArguments;
                if  ( (*txInfo).toVBA )
                    aArguments = (*txInfo).toVBA( evt.Arguments );
                else
                    aArguments = evt.Arguments;
                if ( aArguments.getLength() )
                {
                    // call basic event handlers for event

                    static rtl::OUString part1 = rtl::OUString::createFromAscii( "vnd.sun.star.script:");
                    static rtl::OUString part2 = rtl::OUString::createFromAscii("?language=Basic&location=document"); 

                    // create script url
                    rtl::OUString url = part1 + sMacroLoc + sTemp + part2;
                 
                    OSL_TRACE("script url = %s",
                        rtl::OUStringToOString( url, 
                            RTL_TEXTENCODING_UTF8 ).getStr() );
                    Sequence< sal_Int16 > aOutArgsIndex;
                    Sequence< Any > aOutArgs;
                    try
                    {
                        if ( mpShell )
                        {
                            uno::Any aRet;
                            mpShell->CallXScript( url,
                                aArguments, aRet, aOutArgsIndex, aOutArgs, false );
                        }
                    }
                    catch ( uno::Exception& e )
                    {
                        OSL_TRACE("event script raised %s", rtl::OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
                    }
               } 
           }
       }
    }
}

typedef ::cppu::WeakImplHelper1< XVBAToOOEventDescGen > VBAToOOEventDescGen_BASE;


class VBAToOOEventDescGen : public VBAToOOEventDescGen_BASE
{
public:
    VBAToOOEventDescGen( const Reference< XComponentContext >& rxContext );

    // XVBAToOOEventDescGen
    virtual Sequence< ScriptEventDescriptor > SAL_CALL getEventDescriptions( const Reference< XInterface >& control, const rtl::OUString& sCodeName ) throw (RuntimeException);
    virtual Reference< XScriptEventsSupplier > SAL_CALL getEventSupplier( const Reference< XInterface >& xControl,  const rtl::OUString& sCodeName ) throw (::com::sun::star::uno::RuntimeException);
private:
    Reference< XComponentContext > m_xContext;
  
};

VBAToOOEventDescGen::VBAToOOEventDescGen( const Reference< XComponentContext >& rxContext ):m_xContext( rxContext ) {}

Sequence< ScriptEventDescriptor > SAL_CALL 
VBAToOOEventDescGen::getEventDescriptions( const Reference< XInterface >& xControl, const rtl::OUString& sCodeName ) throw (RuntimeException)
{
    ScriptEventHelper evntHelper( xControl ); 
    return evntHelper.createEvents( sCodeName ); 
}

Reference< XScriptEventsSupplier > SAL_CALL 
VBAToOOEventDescGen::getEventSupplier( const Reference< XInterface >& xControl, const rtl::OUString& sCodeName  ) throw (::com::sun::star::uno::RuntimeException)
{
    ScriptEventHelper evntHelper( xControl ); 
    Reference< XScriptEventsSupplier > xSupplier = 
        new ReadOnlyEventsSupplier( 
            evntHelper.getEventListeners(), sCodeName ) ;
    return xSupplier;
}

// Component related 

namespace evtlstner
{
    ::rtl::OUString SAL_CALL getImplementationName()
    {
        static ::rtl::OUString* pImplName = 0;
        if ( !pImplName )
        {
            ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
            if ( !pImplName )
            {
                static ::rtl::OUString aImplName( RTL_CONSTASCII_USTRINGPARAM( "ooo.vba.EventListener" ) );
                pImplName = &aImplName;
            }
        }
        return *pImplName;
    }

    uno::Reference< XInterface > SAL_CALL create(
    Reference< XComponentContext > const & xContext )
    SAL_THROW( () )
    {
        return static_cast< lang::XTypeProvider * >( new EventListener( xContext ) );
    }

    Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
    {
        const ::rtl::OUString strName( ::evtlstner::getImplementationName() );
        return Sequence< ::rtl::OUString >( &strName, 1 );
    }	
}
namespace ooevtdescgen 
{
    ::rtl::OUString SAL_CALL getImplementationName()
    {
        static ::rtl::OUString* pImplName = 0;
        if ( !pImplName )
        {
            ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
            if ( !pImplName )
            {
                static ::rtl::OUString aImplName( RTL_CONSTASCII_USTRINGPARAM( "ooo.vba.VBAToOOEventDesc" ) );
                pImplName = &aImplName;
            }
        }
        return *pImplName;
    }

    uno::Reference< XInterface > SAL_CALL create(
        Reference< XComponentContext > const & xContext )
        SAL_THROW( () )
    {
        return static_cast< lang::XTypeProvider * >( new VBAToOOEventDescGen( xContext ) );
    }

    Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
    {
    	const ::rtl::OUString strName( ::ooevtdescgen::getImplementationName() );
        return Sequence< ::rtl::OUString >( &strName, 1 );
    }	
}
