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


#ifndef _SCH_DATALOG
#define _SCH_DATALOG

#include "memchrt.hxx"
#include <float.h>
#include <math.h>
#define SCH_DATALOG_ANY -1
namespace binfilter {

class SchDataLogBook
{
	long* mpRowCoordinates;
	long* mpColCoordinates;

	long dummy;

	long mnRows;
	long mnCols;
	long mnColsInitial;
	long mnRowsInitial;

	long mnRowsAdded;
	long mnColsAdded;
	long mnRowsLeft;
	long mnColsLeft;

	BOOL mbValid;
	BOOL mbRowChanged;
	BOOL mbColChanged;
	BOOL mbGetCol;

	void IncreaseRowCount();
	void IncreaseColCount();

public:

	 void Reset();
	~SchDataLogBook();
};

} //namespace binfilter
#endif
