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


#ifndef MATH_COMMAND_HXX
#define MATH_COMMAND_HXX



#ifndef _BITMAP_HXX //autogen
#include <vcl/bitmap.hxx>
#endif
class String; 
namespace binfilter {



class SmCommandDesc: public Resource
{
protected:
	String		*pSample;
	String		*pCommand;
	String		*pText;
	String		*pHelp;
	Bitmap		*pGraphic;

public:
	SmCommandDesc(const ResId& rResId);
	~SmCommandDesc();

	const String  &GetSample()	const { return (*pSample);	}
	const String  &GetCommand() const { return (*pCommand); }
	const String  &GetText()  	const { return (*pText); }
	const String  &GetHelp()  	const { return (*pHelp); }
	const Bitmap  &GetGraphic() const { return (*pGraphic); }
};


} //namespace binfilter
#endif

