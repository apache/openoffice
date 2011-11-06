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



//------------------------------------------------------------------

#ifndef SC_CBUTTON_HXX
#define SC_CBUTTON_HXX

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif


class Window;
namespace binfilter {


//==================================================================

class ScDDComboBoxButton
{
public:
			ScDDComboBoxButton( Window* pOutputDevice );
			~ScDDComboBoxButton();




	void	SetOptSizePixel();

	void	SetPosPixel( const Point& rNewPos )  { aBtnPos = rNewPos; }
	Point	GetPosPixel() const				 	 { return aBtnPos; }

	void	SetSizePixel( const Size& rNewSize ) { aBtnSize = rNewSize; }
	Size	GetSizePixel() const				 { return aBtnSize; }

private:

protected:
	Window*	pOut;
	Point	aBtnPos;
	Size	aBtnSize;
};


} //namespace binfilter
#endif // SC_CBUTTON_HXX


