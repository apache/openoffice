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



#include "vbaaxis.hxx"
#include <ooo/vba/excel/XlAxisCrosses.hpp>
#include <ooo/vba/excel/XlAxisType.hpp>
#include <ooo/vba/excel/XlScaleType.hpp>
#include "vbaaxistitle.hxx"
#include "vbachart.hxx"
using namespace ::com::sun::star;
using namespace ::ooo::vba;
using namespace ::ooo::vba::excel::XlAxisCrosses;
using namespace ::ooo::vba::excel::XlAxisType;
using namespace ::ooo::vba::excel::XlScaleType;

const rtl::OUString ORIGIN( RTL_CONSTASCII_USTRINGPARAM("Origin") );
const rtl::OUString AUTOORIGIN( RTL_CONSTASCII_USTRINGPARAM("AutoOrigin") );
const rtl::OUString VBA_MIN( RTL_CONSTASCII_USTRINGPARAM("Max") );
const rtl::OUString VBA_MAX( RTL_CONSTASCII_USTRINGPARAM("Min") );
ScVbaChart*
ScVbaAxis::getChartPtr()
{
	ScVbaChart* pChart = static_cast< ScVbaChart* >( moChartParent.get() );
	if ( !pChart )
		throw uno::RuntimeException( rtl::OUString::createFromAscii("Can't access parent chart impl"), uno::Reference< uno::XInterface >() );
	return pChart;
}

sal_Bool
ScVbaAxis::isValueAxis()
{
	if ( getType() == xlCategory )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return sal_True;
}

ScVbaAxis::ScVbaAxis( const uno::Reference< XHelperInterface >& xParent,const uno::Reference< uno::XComponentContext > & xContext, const uno::Reference< beans::XPropertySet >& _xPropertySet, sal_Int32 _nType, sal_Int32 _nGroup  ) : ScVbaAxis_BASE( xParent, xContext ), mxPropertySet( _xPropertySet ), mnType( _nType ), mnGroup( _nGroup ), bCrossesAreCustomized( sal_False )
{
	oShapeHelper.reset( new ShapeHelper( uno::Reference< drawing::XShape >( mxPropertySet, uno::UNO_QUERY ) ) );
	moChartParent.set( xParent, uno::UNO_QUERY_THROW  );
	setType(_nType);
	setCrosses(xlAxisCrossesAutomatic);
}

void SAL_CALL
ScVbaAxis::Delete(  )
{
	uno::Reference< lang::XComponent > xComponent( mxPropertySet, uno::UNO_QUERY_THROW );
	xComponent->dispose();
}

 uno::Reference< ::ooo::vba::excel::XAxisTitle > SAL_CALL
ScVbaAxis::getAxisTitle(  )
{
	uno::Reference< excel::XAxisTitle > xAxisTitle;
	try
	{
		ScVbaChart* pChart = getChartPtr();

		if (getHasTitle() )
		{
			int nType = getType();
			switch(nType)
			{
				case xlCategory:
					xAxisTitle =  new ScVbaAxisTitle(this, mxContext, pChart->xAxisXSupplier->getXAxisTitle());
					break;
				case xlSeriesAxis:
					xAxisTitle = new ScVbaAxisTitle(this, mxContext, pChart->xAxisZSupplier->getZAxisTitle());
					break;
				default: // xlValue:
					xAxisTitle = new ScVbaAxisTitle(this, mxContext, pChart->xAxisYSupplier->getYAxisTitle());
					break;
			}
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(e);
	}
	return xAxisTitle;

}

void SAL_CALL
ScVbaAxis::setDisplayUnit( ::sal_Int32 /*DisplayUnit*/ )
{
	DebugHelper::exception(SbERR_NOT_IMPLEMENTED, rtl::OUString());
}

::sal_Int32 SAL_CALL
ScVbaAxis::getDisplayUnit(  )
{
	DebugHelper::exception(SbERR_NOT_IMPLEMENTED, rtl::OUString());
	return -1;
}

void SAL_CALL
ScVbaAxis::setCrosses( ::sal_Int32 _nCrosses )
{
	try
	{
		double fNum = 0.0;
		switch (_nCrosses)
		{
			case  xlAxisCrossesAutomatic:       //Microsoft Excel sets the axis crossing point.
				mxPropertySet->setPropertyValue(AUTOORIGIN, uno::makeAny( sal_True ) );
				bCrossesAreCustomized = sal_False;
				return;
			case xlAxisCrossesMinimum:                     // The axis crosses at the minimum value.
				mxPropertySet->getPropertyValue(VBA_MIN) >>= fNum;
				setCrossesAt( fNum );
				bCrossesAreCustomized = sal_False;
				break;
			case xlAxisCrossesMaximum:                     // The axis crosses at the maximum value.
				mxPropertySet->getPropertyValue(VBA_MAX) >>= fNum;
				setCrossesAt(fNum);
				bCrossesAreCustomized = sal_False;
				break;
			default: //xlAxisCrossesCustom
				bCrossesAreCustomized = sal_True;
				break;
		}
		mxPropertySet->setPropertyValue(AUTOORIGIN, uno::makeAny(sal_False) );
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}
::sal_Int32 SAL_CALL
ScVbaAxis::getCrosses(  )
{
	sal_Int32 nCrosses = xlAxisCrossesCustom;
	try
	{
		sal_Bool bisAutoOrigin = sal_False;
		mxPropertySet->getPropertyValue(AUTOORIGIN) >>= bisAutoOrigin;
		if (bisAutoOrigin)
			nCrosses = xlAxisCrossesAutomatic;
		else
		{
			if (bCrossesAreCustomized)
				nCrosses = xlAxisCrossesCustom;
			else
			{
				double forigin = 0.0;
				mxPropertySet->getPropertyValue(ORIGIN) >>= forigin;
//obsolete      double fmax = AnyConverter.toDouble(mxPropertySet.getPropertyValue("Max"));
				double fmin = 0.0;
				mxPropertySet->getPropertyValue(VBA_MIN) >>= fmin;
				if (forigin == fmin)
					nCrosses = xlAxisCrossesMinimum;
				else
					nCrosses = xlAxisCrossesMaximum;
			}
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
	return nCrosses;
}

 void SAL_CALL
ScVbaAxis::setCrossesAt( double _fCrossesAt )
{
	try
	{
//        if (getCrosses() == xlAxisCrossesCustom){
		setMaximumScaleIsAuto( sal_False );
		setMinimumScaleIsAuto( sal_False );
		mxPropertySet->setPropertyValue(ORIGIN, uno::makeAny(_fCrossesAt));
//        }
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(e);
	}
}

 double SAL_CALL
ScVbaAxis::getCrossesAt(  )
{
	double fCrosses = 0.0;
	try
	{
		mxPropertySet->getPropertyValue(ORIGIN) >>= fCrosses;
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return fCrosses;
}

void SAL_CALL
ScVbaAxis::setType( ::sal_Int32 _nType )
{
	mnType = _nType;
}

::sal_Int32 SAL_CALL
ScVbaAxis::getType(  )
{
	return mnType;
}

void SAL_CALL
ScVbaAxis::setHasTitle( ::sal_Bool _bHasTitle )
{
	try
	{
		ScVbaChart* pChart = getChartPtr();
		sal_Int32 nType = getType();
		switch(nType)
		{
			case xlCategory:
				pChart->mxDiagramPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("HasXAxisTitle")), uno::makeAny(_bHasTitle));
				break;
			case xlSeriesAxis:
				pChart->mxDiagramPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("HasZAxisTitle")), uno::makeAny(_bHasTitle));
				break;
			default: // xlValue:
				pChart->mxDiagramPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("HasYAxisTitle")), uno::makeAny(_bHasTitle));
		}

	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(e);
	}
}

 ::sal_Bool SAL_CALL
ScVbaAxis::getHasTitle(  )
{
	sal_Bool bHasTitle = sal_False;
	try
	{
		ScVbaChart* pChart = getChartPtr();
		int nType = getType();
		switch(nType)
		{
			case xlCategory:
				pChart->mxDiagramPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("HasXAxisTitle")) ) >>= bHasTitle;
				break;
			case xlSeriesAxis:
				pChart->mxDiagramPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("HasZAxisTitle")) ) >>= bHasTitle;
				break;
			default: // xlValue:
				pChart->mxDiagramPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("HasYAxisTitle")) ) >>= bHasTitle;
		}
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(e);
	}
	return bHasTitle;
}

void SAL_CALL
ScVbaAxis::setMinorUnit( double _fMinorUnit )
{
	try
	{
		if (isValueAxis())
			mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("StepHelp") ), uno::makeAny(_fMinorUnit));
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}

double SAL_CALL
ScVbaAxis::getMinorUnit(  )
{
	double fMinor = 1.0;
	try
	{
		if (isValueAxis())
			mxPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("StepHelp"))) >>= fMinor;
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return fMinor;
}

void SAL_CALL
ScVbaAxis::setMinorUnitIsAuto( ::sal_Bool _bMinorUnitIsAuto )
{
	try
	{
		if (isValueAxis())
			mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "AutoStepHelp" ) ), uno::makeAny(_bMinorUnitIsAuto));
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
}

 ::sal_Bool SAL_CALL
ScVbaAxis::getMinorUnitIsAuto(  )
{
	sal_Bool bIsAuto = sal_False;
	try
	{
		if (isValueAxis())
		{
			mxPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "AutoStepHelp")) ) >>= bIsAuto;
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return bIsAuto;
}

void SAL_CALL
ScVbaAxis::setReversePlotOrder( ::sal_Bool /*ReversePlotOrder*/ )
{
	DebugHelper::exception(SbERR_NOT_IMPLEMENTED, rtl::OUString());
}

::sal_Bool SAL_CALL
ScVbaAxis::getReversePlotOrder(  )
{
	DebugHelper::exception(SbERR_NOT_IMPLEMENTED, rtl::OUString());
	return sal_False;
}

void SAL_CALL
ScVbaAxis::setMajorUnit( double _fMajorUnit )
{
	try
	{
		if (isValueAxis())
		{
			mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("StepMain")), uno::makeAny(_fMajorUnit));
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}

double SAL_CALL
ScVbaAxis::getMajorUnit(  )
{
	double fMax = 1.0;
	try
	{
		if (isValueAxis())
			mxPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("StepMain"))) >>= fMax;
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
	return fMax;
}

void SAL_CALL
ScVbaAxis::setMajorUnitIsAuto( ::sal_Bool _bMajorUnitIsAuto )
{
	try
	{
		if (isValueAxis())
		{
			mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "AutoStepMain" ) ), uno::makeAny( _bMajorUnitIsAuto ));
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}

::sal_Bool SAL_CALL
ScVbaAxis::getMajorUnitIsAuto(  )
{
	sal_Bool bIsAuto = sal_False;
	try
	{
		if (isValueAxis())
		{
			mxPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "AutoStepMain"))) >>= bIsAuto;
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return bIsAuto;
}

void SAL_CALL
ScVbaAxis::setMaximumScale( double _fMaximumScale )
{
	try
	{
		if ( isValueAxis() )
		{
			mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Max" ) ), uno::makeAny(_fMaximumScale));
		}
	}
	catch ( uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}

double SAL_CALL
ScVbaAxis::getMaximumScale(  )
{
	double fMax = 1.0;
	try
	{
		if (isValueAxis())
		{
			mxPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Max" ))) >>= fMax;
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return fMax;

}

void SAL_CALL
ScVbaAxis::setMaximumScaleIsAuto( ::sal_Bool _bMaximumScaleIsAuto )
{
	try
	{
		if ( isValueAxis() )
			mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "AutoMax" ) ), uno::makeAny( _bMaximumScaleIsAuto ));

	}
	catch ( uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}


::sal_Bool SAL_CALL
ScVbaAxis::getMaximumScaleIsAuto(  )
{
	sal_Bool bIsAuto = sal_False;
	try
	{
		if (isValueAxis())
			mxPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "AutoMax" )) ) >>= bIsAuto;
	}
	catch ( uno::Exception& )
	{
		DebugHelper::exception( SbERR_METHOD_FAILED, rtl::OUString() );
	}
	return bIsAuto;
}

void SAL_CALL
ScVbaAxis::setMinimumScale( double _fMinimumScale )
{
	try
	{
		if (isValueAxis())
			mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Min") ), uno::makeAny( _fMinimumScale )  );
	}
	catch ( uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
}

double SAL_CALL
ScVbaAxis::getMinimumScale(  )
{
	double fMin = 0.0;
	try
	{
		if (isValueAxis())
			mxPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Min") )) >>= fMin;
	}
	catch (uno::Exception& e)
	{
		DebugHelper::exception(e);
	}
	return fMin;
}

void SAL_CALL
ScVbaAxis::setMinimumScaleIsAuto( ::sal_Bool _bMinimumScaleIsAuto )
{
	try
	{
		if (isValueAxis())
		{
			mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("AutoMin") ), uno::makeAny(_bMinimumScaleIsAuto));
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
}

::sal_Bool SAL_CALL
ScVbaAxis::getMinimumScaleIsAuto(  )
{
	sal_Bool bIsAuto = sal_False;
	try
	{
		if (isValueAxis())
		{
			mxPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("AutoMin")) ) >>= bIsAuto;
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return bIsAuto;
}

::sal_Int32 SAL_CALL
ScVbaAxis::getAxisGroup(  )
{
	return mnGroup;
}

void SAL_CALL
ScVbaAxis::setScaleType( ::sal_Int32 _nScaleType )
{
	try
	{
		if (isValueAxis())
		{
			switch (_nScaleType)
			{
				case xlScaleLinear:
					mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Logarithmic" ) ), uno::makeAny( sal_False ) );
					break;
				case xlScaleLogarithmic:
					mxPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Logarithmic" ) ), uno::makeAny( sal_True ) );
					break;
				default:
					// According to MS the parameter is ignored and no Error is thrown
					break;
			}
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString() );
	}
}

::sal_Int32 SAL_CALL
ScVbaAxis::getScaleType(  )
{
	sal_Int32 nScaleType = xlScaleLinear;
	try
	{
		if (isValueAxis())
		{
			sal_Bool bisLogarithmic = sal_False;
			mxPropertySet->getPropertyValue( rtl::OUString( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Logarithmic"))) ) >>= bisLogarithmic;
			if (bisLogarithmic)
				nScaleType = xlScaleLogarithmic;
			else
				nScaleType = xlScaleLinear;
		}
	}
	catch (uno::Exception& )
	{
		DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
	}
	return nScaleType;
}

double SAL_CALL
ScVbaAxis::getHeight(  )
{
	return oShapeHelper->getHeight();
}

void SAL_CALL ScVbaAxis::setHeight( double height )
{
	oShapeHelper->setHeight( height );
}
double SAL_CALL ScVbaAxis::getWidth(  )
{
	return oShapeHelper->getWidth( );
}
void SAL_CALL ScVbaAxis::setWidth( double width )
{
	oShapeHelper->setWidth( width );
}
double SAL_CALL ScVbaAxis::getTop(  )
{
	return oShapeHelper->getTop( );
}
void SAL_CALL ScVbaAxis::setTop( double top )
{
	oShapeHelper->setTop( top );
}
double SAL_CALL ScVbaAxis::getLeft(  )
{
	return oShapeHelper->getLeft( );
}
void SAL_CALL ScVbaAxis::setLeft( double left )
{
	oShapeHelper->setLeft( left );
}

rtl::OUString&
ScVbaAxis::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaAxis") );
	return sImplName;
}

uno::Sequence< rtl::OUString >
ScVbaAxis::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.excel.Axis" ) );
	}
	return aServiceNames;
}
