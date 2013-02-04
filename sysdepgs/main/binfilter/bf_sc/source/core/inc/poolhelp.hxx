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



#ifndef SC_POOLHELP_HXX
#define SC_POOLHELP_HXX

#ifndef _VOS_REFERNCE_HXX_
#include <vos/refernce.hxx>
#endif

namespace binfilter {

class SvNumberFormatter;
class SfxItemPool;
class ScDocument;
class ScDocumentPool;
class ScStyleSheetPool;


class ScPoolHelper : public vos::OReference
{
private:
	ScDocumentPool*		pDocPool;
	ScStyleSheetPool*	pStylePool;
	SvNumberFormatter*	pFormTable;
	SfxItemPool*		pEditPool;						// EditTextObjectPool
	SfxItemPool*		pEnginePool;					// EditEnginePool

public:
				ScPoolHelper( ScDocument* pSourceDoc );
	virtual		~ScPoolHelper();

				// called in dtor of main document
	void		SourceDocumentGone();

				// access to pointers (are never 0):
	ScDocumentPool*		GetDocPool() const		{ return pDocPool; }
	ScStyleSheetPool*	GetStylePool() const	{ return pStylePool; }
	SvNumberFormatter*	GetFormTable() const	{ return pFormTable; }
	SfxItemPool*		GetEditPool() const		{ return pEditPool; }
	SfxItemPool*		GetEnginePool() const	{ return pEnginePool; }
};

} //namespace binfilter
#endif

