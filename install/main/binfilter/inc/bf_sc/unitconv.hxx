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



#ifndef SC_UNITCONV_HXX
#define SC_UNITCONV_HXX

#include <bf_svtools/bf_solar.h>

#include "collect.hxx"
namespace binfilter {


class ScUnitConverterData : public StrData
{
			double			fValue;


public:
							ScUnitConverterData( const String& rFromUnit,
								const String& rToUnit, double fValue = 1.0 );
							ScUnitConverterData( const ScUnitConverterData& );
	virtual					~ScUnitConverterData() {};

	virtual	DataObject*		Clone() const;

			double			GetValue() const	{ return fValue; }

	static	void			BuildIndexString( String& rStr,
								const String& rFromUnit, const String& rToUnit );

};


class ScUnitConverter : public StrCollection
{
							// not implemented

public:
	ScUnitConverter( USHORT nInit = 16, USHORT nDelta = 4 );
	virtual					~ScUnitConverter() {};

		BOOL				GetValue( double& fValue, const String& rFromUnit,
								const String& rToUnit ) const;
};


} //namespace binfilter
#endif

