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



#ifndef SVX_UNOSHTXT_HXX
#define SVX_UNOSHTXT_HXX

#include <bf_svtools/bf_solar.h>

#include <memory>

#ifndef _SVX_UNOEDSRC_HXX
#include <bf_svx/unoedsrc.hxx>
#endif
class Window;
namespace binfilter {

class SvxTextForwarder;
class SdrObject;
class SdrView;
class SvxTextEditSourceImpl;

class SvxTextEditSource : public SvxEditSource, public SvxViewForwarder
{
public:
	SvxTextEditSource( SdrObject* pObj );

	virtual ~SvxTextEditSource();

	virtual SvxEditSource*			Clone() const;
	virtual SvxTextForwarder*		GetTextForwarder();
 	virtual SvxViewForwarder*		GetViewForwarder();
 	virtual SvxEditViewForwarder*	GetEditViewForwarder( sal_Bool bCreate = sal_False );
	virtual void					UpdateData();

    virtual SfxBroadcaster&			GetBroadcaster() const;

	void lock();
	void unlock();

	static sal_Bool hasLevels( const SdrObject* pObject );

    // the viewforwarder interface
	virtual BOOL		IsValid() const;
    virtual Rectangle	GetVisArea() const;
    virtual Point		LogicToPixel( const Point&, const MapMode& ) const;
    virtual Point		PixelToLogic( const Point&, const MapMode& ) const;

private:
	SvxTextEditSource( SvxTextEditSourceImpl* pImpl );

	SvxTextEditSourceImpl*	mpImpl;
};

}//end of namespace binfilter
#endif
