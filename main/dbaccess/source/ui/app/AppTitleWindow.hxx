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


#ifndef DBAUI_TITLE_WINDOW_HXX
#define DBAUI_TITLE_WINDOW_HXX

#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif

namespace dbaui
{
	class OTitleWindow : public Window
	{
		FixedText	m_aSpace1;
		FixedText	m_aSpace2;
		FixedText	m_aTitle;
		Window*		m_pChild;
		sal_Bool		m_bShift;
		void ImplInitSettings( sal_Bool bFont, sal_Bool bForeground, sal_Bool bBackground );
	protected:
		virtual void DataChanged(const DataChangedEvent& rDCEvt);
	public:
		OTitleWindow(Window* _pParent,sal_uInt16 _nTitleId,WinBits _nBits,sal_Bool _bShift = sal_True);
		virtual ~OTitleWindow();
		
		// window overloads
		virtual void Resize();
		virtual void GetFocus();

		/** sets the child window which should be displayed below the title. It will be destroyed at the end.
			@param	_pChild
				The child window.
		*/
		void setChildWindow(Window* _pChild);

		/** gets the child window.
			
			@return
				The child window.
		*/
		inline Window* getChildWindow() const { return m_pChild; }

		/** sets the title text out of the resource
			@param	_nTitleId
				The resource id of the title text.
		*/
		void setTitle(sal_uInt16 _nTitleId);


		/** Gets the min Width in Pixel which is needed to display the whole
			
			@return
				the min width
		*/
		long GetWidthPixel() const;
	};
// .............................................................
} // namespace dbaui
// .............................................................
#endif // DBAUI_TITLE_WINDOW_HXX

