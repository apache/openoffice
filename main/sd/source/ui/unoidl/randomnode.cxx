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
#include "precompiled_sd.hxx"
#include <com/sun/star/animations/XTimeContainer.hpp>
#include <com/sun/star/presentation/ParagraphTarget.hpp>
#include <com/sun/star/animations/AnimationFill.hpp>
#include <com/sun/star/animations/XAnimate.hpp>
#include <com/sun/star/animations/AnimationRestart.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/util/XCloneable.hpp>
#include <comphelper/processfactory.hxx>

#include <cppuhelper/implbase5.hxx>
#include <osl/mutex.hxx>
#include "CustomAnimationPreset.hxx"

using ::osl::Mutex;
using ::osl::Guard;
using ::rtl::OUString;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::XInterface;
using ::com::sun::star::lang::NoSupportException;
using ::com::sun::star::beans::NamedValue;
using ::com::sun::star::lang::IllegalArgumentException;
using ::com::sun::star::container::NoSuchElementException;
using ::com::sun::star::container::ElementExistException;
using ::com::sun::star::lang::WrappedTargetException;
using ::com::sun::star::container::XEnumeration;
using ::com::sun::star::container::XEnumerationAccess;
using ::com::sun::star::util::XCloneable;
using ::com::sun::star::lang::XServiceInfo;
using ::com::sun::star::lang::XInitialization;
using ::com::sun::star::uno::Type;
using ::com::sun::star::uno::XWeak;
using ::com::sun::star::lang::XMultiServiceFactory;
using ::com::sun::star::presentation::ParagraphTarget;
using ::com::sun::star::drawing::XShape;

using namespace ::com::sun::star::animations;
namespace sd
{

typedef ::cppu::WeakImplHelper5< XTimeContainer, XEnumerationAccess, XCloneable, XServiceInfo, XInitialization > RandomAnimationNodeBase;
class RandomAnimationNode : public RandomAnimationNodeBase
{
public:
	RandomAnimationNode( const RandomAnimationNode& rNode );
	RandomAnimationNode( sal_Int16 nPresetClass );
	RandomAnimationNode();

	void init( sal_Int16 nPresetClass );

	// XInitialization
    void SAL_CALL initialize( const Sequence< Any >& aArguments );

    // XChild
    Reference< XInterface > SAL_CALL getParent(  );
    void SAL_CALL setParent( const Reference< XInterface >& Parent );

    // XCloneable
    virtual Reference< XCloneable > SAL_CALL createClone();

	// XServiceInfo
    OUString SAL_CALL getImplementationName() throw();
    Sequence< OUString > SAL_CALL getSupportedServiceNames(void) throw();
    sal_Bool SAL_CALL supportsService(const OUString& ServiceName) throw();

    // XAnimationNode
    ::sal_Int16 SAL_CALL getType();
    Any SAL_CALL getBegin();
    void SAL_CALL setBegin( const Any& _begin );
    Any SAL_CALL getDuration();
    void SAL_CALL setDuration( const Any& _duration );
    Any SAL_CALL getEnd();
    void SAL_CALL setEnd( const Any& _end );
    Any SAL_CALL getEndSync();
    void SAL_CALL setEndSync( const Any& _endsync );
    Any SAL_CALL getRepeatCount();
    void SAL_CALL setRepeatCount( const Any& _repeatcount );
    Any SAL_CALL getRepeatDuration();
    void SAL_CALL setRepeatDuration( const Any& _repeatduration );
    ::sal_Int16 SAL_CALL getFill();
    void SAL_CALL setFill( ::sal_Int16 _fill );
    ::sal_Int16 SAL_CALL getFillDefault();
    void SAL_CALL setFillDefault( ::sal_Int16 _filldefault );
    ::sal_Int16 SAL_CALL getRestart();
    void SAL_CALL setRestart( ::sal_Int16 _restart );
    ::sal_Int16 SAL_CALL getRestartDefault();
	void SAL_CALL setRestartDefault( ::sal_Int16 _restartdefault );
    double SAL_CALL getAcceleration();
    void SAL_CALL setAcceleration( double _acceleration );
    double SAL_CALL getDecelerate();
    void SAL_CALL setDecelerate( double _decelerate );
    ::sal_Bool SAL_CALL getAutoReverse();
    void SAL_CALL setAutoReverse( ::sal_Bool _autoreverse );
    Sequence< NamedValue > SAL_CALL getUserData();
    void SAL_CALL setUserData( const Sequence< NamedValue >& _userdata );

	// XElementAccess
	virtual Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

	// XEnumerationAccess
    virtual Reference< XEnumeration > SAL_CALL createEnumeration();

    // XTimeContainer
    Reference< XAnimationNode > SAL_CALL insertBefore( const Reference< XAnimationNode >& newChild, const Reference< XAnimationNode >& refChild );
    Reference< XAnimationNode > SAL_CALL insertAfter( const Reference< XAnimationNode >& newChild, const Reference< XAnimationNode >& refChild );
    Reference< XAnimationNode > SAL_CALL replaceChild( const Reference< XAnimationNode >& newChild, const Reference< XAnimationNode >& oldChild );
    Reference< XAnimationNode > SAL_CALL removeChild( const Reference< XAnimationNode >& oldChild );
    Reference< XAnimationNode > SAL_CALL appendChild( const Reference< XAnimationNode >& newChild );

private:
	// our first, last and only protection from mutli-threads!
	Mutex maMutex;

	sal_Int16 mnPresetClass;
	Reference< XInterface > mxParent;

	Any maBegin, maDuration, maEnd, maEndSync, maRepeatCount, maRepeatDuration, maTarget;
	sal_Int16 mnFill, mnFillDefault, mnRestart, mnRestartDefault;
	double mfAcceleration, mfDecelerate;
	sal_Bool mbAutoReverse;
	Sequence< NamedValue > maUserData;

	Reference< XAnimate > mxFirstNode;
};

// --------------------------------------------------------------------

static const sal_Char* pSN1 = "com.sun.star.animations.ParallelTimeContainer";
static const sal_Char* pSN2 = "com.sun.star.comp.sd.RandomAnimationNode";

// --------------------------------------------------------------------

SD_DLLPUBLIC Reference< XInterface > RandomAnimationNode_createInstance( sal_Int16 nPresetClass )
{
	Reference< XInterface > xInt( static_cast<XWeak*>( new RandomAnimationNode( nPresetClass ) ) );
	return xInt;
}

// --------------------------------------------------------------------

Reference< XInterface > SAL_CALL RandomNode_createInstance( const Reference< XMultiServiceFactory > &  )
{
	Reference< XInterface > xInt( static_cast<XWeak*>( new RandomAnimationNode() ) );
	return xInt;
}

// --------------------------------------------------------------------

OUString RandomNode__getImplementationName()
{
	return OUString( RTL_CONSTASCII_USTRINGPARAM( "sd::RandomAnimationNode" ) );
}

// --------------------------------------------------------------------

Sequence< OUString > SAL_CALL RandomNode_getSupportedServiceNames()
{
//	static const OUString aSN1( OUString::createFromAscii( pSN1 ) );
	static const OUString aSN2( OUString::createFromAscii( pSN2 ) );
	Sequence< OUString > aSeq( &aSN2, 1 );
//	aSeq[0] = aSN1;
///	aSeq[1] = aSN2;
	return aSeq;
}

// --------------------------------------------------------------------

RandomAnimationNode::RandomAnimationNode( const RandomAnimationNode& rNode )
:	RandomAnimationNodeBase(),
	mnPresetClass( rNode.mnPresetClass ),
	maBegin( rNode.maBegin ),
	maDuration( rNode.maDuration ),
	maEnd( rNode.maEnd ),
	maEndSync( rNode.maEndSync ),
	maRepeatCount( rNode.maRepeatCount ),
	maRepeatDuration( rNode.maRepeatDuration ),
	maTarget( rNode.maTarget ),
	mnFill( rNode.mnFill ),
	mnFillDefault( rNode.mnFillDefault ),
	mnRestart( rNode.mnRestart ),
	mnRestartDefault( rNode.mnRestartDefault ),
	mfAcceleration( rNode.mfAcceleration ),
	mfDecelerate( rNode.mfDecelerate ),
	mbAutoReverse( rNode.mbAutoReverse ),
	maUserData( rNode.maUserData )
{
}

// --------------------------------------------------------------------

RandomAnimationNode::RandomAnimationNode( sal_Int16 nPresetClass )
{
	init( nPresetClass );
}

RandomAnimationNode::RandomAnimationNode()
{
	init( 1 );
}

void RandomAnimationNode::init( sal_Int16 nPresetClass )
{
	mnPresetClass = nPresetClass;
	mnFill = AnimationFill::DEFAULT;
	mnFillDefault = AnimationFill::INHERIT;
	mnRestart = AnimationRestart::DEFAULT;
	mnRestartDefault = AnimationRestart::INHERIT;
	mfAcceleration = 0.0;
	mfDecelerate = 0.0;
	mbAutoReverse = sal_False;
}

// --------------------------------------------------------------------

// XInitialization
void SAL_CALL RandomAnimationNode::initialize( const Sequence< Any >& aArguments )
{
	if( aArguments.getLength() != 1 )
		throw IllegalArgumentException();

	if( aArguments[0].getValueType() == ::getCppuType((const sal_Int16*)0) )
	{
		aArguments[0] >>= mnPresetClass;
	}
	else if( aArguments[0].getValueType() != ::getCppuType((const ParagraphTarget*)0) )
	{
		Reference< XShape > xShape;
		aArguments[0] >>= xShape;
		if( !xShape.is() )
			throw IllegalArgumentException();
	}
	maTarget = aArguments[0];
}

// --------------------------------------------------------------------

// XAnimationNode
sal_Int16 SAL_CALL RandomAnimationNode::getType()
{
	Guard< Mutex > aGuard( maMutex );
	return ::com::sun::star::animations::AnimationNodeType::PAR;
}

// --------------------------------------------------------------------

// XAnimationNode
Any SAL_CALL RandomAnimationNode::getBegin()
{
	Guard< Mutex > aGuard( maMutex );
	return maBegin;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setBegin( const Any& _begin )
{
	Guard< Mutex > aGuard( maMutex );
	maBegin = _begin;
}

// --------------------------------------------------------------------

// XAnimationNode
Any SAL_CALL RandomAnimationNode::getDuration()
{
	Guard< Mutex > aGuard( maMutex );
	return maDuration;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setDuration( const Any& _duration )
{
	Guard< Mutex > aGuard( maMutex );
	maDuration = _duration;
}

// --------------------------------------------------------------------

// XAnimationNode
Any SAL_CALL RandomAnimationNode::getEnd()
{
	Guard< Mutex > aGuard( maMutex );
	return maEnd;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setEnd( const Any& _end )
{
	Guard< Mutex > aGuard( maMutex );
	maEnd = _end;
}

// --------------------------------------------------------------------

// XAnimationNode
Any SAL_CALL RandomAnimationNode::getEndSync()
{
	Guard< Mutex > aGuard( maMutex );
	return maEndSync;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setEndSync( const Any& _endsync )
{
	Guard< Mutex > aGuard( maMutex );
	maEndSync = _endsync;
}

// --------------------------------------------------------------------

// XAnimationNode
Any SAL_CALL RandomAnimationNode::getRepeatCount()
{
	Guard< Mutex > aGuard( maMutex );
	return maRepeatCount;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setRepeatCount( const Any& _repeatcount )
{
	Guard< Mutex > aGuard( maMutex );
	maRepeatCount = _repeatcount;
}

// --------------------------------------------------------------------

// XAnimationNode
Any SAL_CALL RandomAnimationNode::getRepeatDuration()
{
	Guard< Mutex > aGuard( maMutex );
	return maRepeatDuration;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setRepeatDuration( const Any& _repeatduration )
{
	Guard< Mutex > aGuard( maMutex );
	maRepeatDuration = _repeatduration;
}

// --------------------------------------------------------------------

// XAnimationNode
sal_Int16 SAL_CALL RandomAnimationNode::getFill()
{
	Guard< Mutex > aGuard( maMutex );
	return mnFill;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setFill( sal_Int16 _fill )
{
	Guard< Mutex > aGuard( maMutex );
	mnFill = _fill;
}

// --------------------------------------------------------------------

// XAnimationNode
sal_Int16 SAL_CALL RandomAnimationNode::getFillDefault()
{
	Guard< Mutex > aGuard( maMutex );
	return mnFillDefault;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setFillDefault( sal_Int16 _filldefault )
{
	Guard< Mutex > aGuard( maMutex );
	mnFillDefault = _filldefault;
}

// --------------------------------------------------------------------

// XAnimationNode
sal_Int16 SAL_CALL RandomAnimationNode::getRestart()
{
	Guard< Mutex > aGuard( maMutex );
	return mnRestart;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setRestart( sal_Int16 _restart )
{
	Guard< Mutex > aGuard( maMutex );
	mnRestart = _restart;
}

// --------------------------------------------------------------------

// XAnimationNode
sal_Int16 SAL_CALL RandomAnimationNode::getRestartDefault()
{
	Guard< Mutex > aGuard( maMutex );
	return mnRestartDefault;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setRestartDefault( sal_Int16 _restartdefault )
{
	Guard< Mutex > aGuard( maMutex );
	mnRestartDefault = _restartdefault;
}

// --------------------------------------------------------------------

// XAnimationNode
double SAL_CALL RandomAnimationNode::getAcceleration()
{
	Guard< Mutex > aGuard( maMutex );
	return mfAcceleration;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setAcceleration( double _acceleration )
{
	Guard< Mutex > aGuard( maMutex );
	mfAcceleration = _acceleration;
}

// --------------------------------------------------------------------

// XAnimationNode
double SAL_CALL RandomAnimationNode::getDecelerate()
{
	Guard< Mutex > aGuard( maMutex );
	return mfDecelerate;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setDecelerate( double _decelerate )
{
	Guard< Mutex > aGuard( maMutex );
	mfDecelerate = _decelerate;
}

// --------------------------------------------------------------------

// XAnimationNode
sal_Bool SAL_CALL RandomAnimationNode::getAutoReverse()
{
	Guard< Mutex > aGuard( maMutex );
	return mbAutoReverse;
}

// --------------------------------------------------------------------

// XAnimationNode
void SAL_CALL RandomAnimationNode::setAutoReverse( sal_Bool _autoreverse )
{
	Guard< Mutex > aGuard( maMutex );
	mbAutoReverse = _autoreverse;
}

// --------------------------------------------------------------------

Sequence< NamedValue > SAL_CALL RandomAnimationNode::getUserData()
{
	Guard< Mutex > aGuard( maMutex );
	return maUserData;
}

// --------------------------------------------------------------------

void SAL_CALL RandomAnimationNode::setUserData( const Sequence< NamedValue >& _userdata )
{
	Guard< Mutex > aGuard( maMutex );
	maUserData = _userdata;
}

// --------------------------------------------------------------------

// XChild
Reference< XInterface > SAL_CALL RandomAnimationNode::getParent()
{
	Guard< Mutex > aGuard( maMutex );
	return mxParent;
}

// --------------------------------------------------------------------

// XChild
void SAL_CALL RandomAnimationNode::setParent( const Reference< XInterface >& Parent )
{
	Guard< Mutex > aGuard( maMutex );
	mxParent = Parent;
}

// --------------------------------------------------------------------

// XCloneable
Reference< XCloneable > SAL_CALL RandomAnimationNode::createClone()
{
	Reference< XCloneable > xNewNode( new RandomAnimationNode( *this ) );
	return xNewNode;
}

// --------------------------------------------------------------------

// XElementAccess
Type SAL_CALL RandomAnimationNode::getElementType()
{
	return ::getCppuType((const Reference< XAnimationNode >*)0);
}

// --------------------------------------------------------------------

// XElementAccess
sal_Bool SAL_CALL RandomAnimationNode::hasElements()
{
	return sal_True;
}

// --------------------------------------------------------------------

// XEnumerationAccess
Reference< XEnumeration > SAL_CALL RandomAnimationNode::createEnumeration()
{
	Guard< Mutex > aGuard( maMutex );

	if( !maTarget.hasValue() && mxFirstNode.is() )
	{
		Any aTarget( mxFirstNode->getTarget() );
		if( aTarget.hasValue() )
		{
			maTarget = aTarget;
			mxFirstNode.clear();
		}
	}

	Reference< XEnumeration > xEnum;

	Reference< XEnumerationAccess > aEnumAccess( CustomAnimationPresets::getCustomAnimationPresets().getRandomPreset( mnPresetClass ), UNO_QUERY );

	if( aEnumAccess.is() )
	{
		Reference< XEnumeration > xEnumeration( aEnumAccess->createEnumeration(), UNO_QUERY );
		if( xEnumeration.is() )
		{
			while( xEnumeration->hasMoreElements() )
			{
				Reference< XAnimate > xAnimate( xEnumeration->nextElement(), UNO_QUERY );
				if( xAnimate.is() )
					xAnimate->setTarget( maTarget );
			}
		}
		xEnum = aEnumAccess->createEnumeration();
	}
	else
	{
		// no presets? give empty node!
		Reference< XMultiServiceFactory > xServiceFactory( comphelper::getProcessServiceFactory() );
		Reference< XEnumerationAccess > aEmptyEnumAccess(
			xServiceFactory->createInstance(
				OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.animations.ParallelTimeContainer" ))),
			UNO_QUERY );

		if( aEmptyEnumAccess.is() )
			xEnum = aEmptyEnumAccess->createEnumeration();
	}

	return xEnum;
}

// --------------------------------------------------------------------


// XTimeContainer
Reference< XAnimationNode > SAL_CALL RandomAnimationNode::insertBefore( const Reference< XAnimationNode >& newChild, const Reference< XAnimationNode >&  )
{
	return appendChild( newChild );
}

// --------------------------------------------------------------------

// XTimeContainer
Reference< XAnimationNode > SAL_CALL RandomAnimationNode::insertAfter( const Reference< XAnimationNode >& newChild, const Reference< XAnimationNode >&  )
{
	return appendChild( newChild );
}

// --------------------------------------------------------------------

// XTimeContainer
Reference< XAnimationNode > SAL_CALL RandomAnimationNode::replaceChild( const Reference< XAnimationNode >& newChild, const Reference< XAnimationNode >&  )
{
	return appendChild( newChild );
}

// --------------------------------------------------------------------

// XTimeContainer
Reference< XAnimationNode > SAL_CALL RandomAnimationNode::removeChild( const Reference< XAnimationNode >& oldChild )
{
	return oldChild;
}

// --------------------------------------------------------------------

// XTimeContainer
Reference< XAnimationNode > SAL_CALL RandomAnimationNode::appendChild( const Reference< XAnimationNode >& newChild )
{
	Reference< XAnimate > xAnimate( newChild, UNO_QUERY );
	if( xAnimate.is() )
	{
		Any aTarget( xAnimate->getTarget() );
		if( aTarget.hasValue() )
			maTarget = aTarget;
	}

	if( !maTarget.hasValue() && !mxFirstNode.is() )
		mxFirstNode = xAnimate;

	return newChild;
}

// XServiceInfo
OUString RandomAnimationNode::getImplementationName() throw()
{
	return RandomNode__getImplementationName();
}

// XServiceInfo
sal_Bool RandomAnimationNode::supportsService(const OUString& ServiceName) throw()
{
	return ServiceName.equalsAscii( pSN1 ) || ServiceName.equalsAscii( pSN2 );
}

// XServiceInfo
Sequence< OUString > RandomAnimationNode::getSupportedServiceNames(void) throw()
{
	return RandomNode_getSupportedServiceNames();
}

}
