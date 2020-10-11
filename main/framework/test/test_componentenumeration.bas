rem *************************************************************
rem  
rem  Licensed to the Apache Software Foundation (ASF) under one
rem  or more contributor license agreements.  See the NOTICE file
rem  distributed with this work for additional information
rem  regarding copyright ownership.  The ASF licenses this file
rem  to you under the Apache License, Version 2.0 (the
rem  "License"); you may not use this file except in compliance
rem  with the License.  You may obtain a copy of the License at
rem  
rem    http://www.apache.org/licenses/LICENSE-2.0
rem  
rem  Unless required by applicable law or agreed to in writing,
rem  software distributed under the License is distributed on an
rem  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
rem  KIND, either express or implied.  See the License for the
rem  specific language governing permissions and limitations
rem  under the License.
rem  
rem *************************************************************
rem	_______________________________________________________________________________________________________________________________________
rem	Test script for helper class "framework/helper/OComponentAccess and OComponentEnumeration.
rem	These two classes are used for "framework/baeh_services/Desktop::getComponents()" only.
rem	_______________________________________________________________________________________________________________________________________


Sub Main

	rem	___________________________________________________________________________________________________________________________________
	rem	Get all current components of the frame tree as an enumeration access object.
	rem	The return value must be a valid reference!
	xComponentAccess = StarDesktop.Components
	if( isNull(xComponentAccess) = TRUE ) then
		msgbox "Error: Desktop return null reference as enumeration access to all tree components!"
		exit Sub
	endif

	rem	___________________________________________________________________________________________________________________________________
	rem	Control service specification of helper class "framework/helper/OComponentAccess".
	rem	The follow output must occur:	com.sun.star.lang.XTypeProvider
	rem									com.sun.star.container.XEnumerationAccess -> com.sun.star.container.XElementAccess
	msgbox xComponentAccess.dbg_supportedInterfaces

	rem	___________________________________________________________________________________________________________________________________
	rem	Test interface XElementAccess of helper OComponentAcces.

	rem	Method hasElements() must return TRUE, because if you call this from the basic IDE at least one task must exist ...
	rem	the IDE by himself. Normally two tasks exist - an empty writer document and a basic frame.
	rem Attention: Not all tasks or frames must support a full implemented component!
	if( xComponentAccess.hasElements <> TRUE ) then
		msgbox "Error: xComponentAccess has no elements - but I can't believe it!"
		exit Sub
	endif		

	rem	Method getElementType() must return the cppu type of XComponent.
	rem	Otherwise something is wrong or implementation has changed.
	if( xComponentAccess.getElementType.Name <> "com.sun.star.lang.XComponent" ) then
		msgbox "Error: xComponentAccess return wrong type as element type! - Has implementation changed?"
		exit Sub
	endif
	
	rem	___________________________________________________________________________________________________________________________________
	rem	Test interface XEnumerationAccess of helper OComponentAcces.
	rem	The return value must be a valid reference!
	xComponentEnumeration = xComponentAccess.createEnumeration
	if( isNull(xComponentEnumeration) = TRUE ) then
		msgbox "Error: Could not create a component enumeration!"
		exit Sub
	endif
	
	rem ___________________________________________________________________________________________________________________________________
	rem	Control service specification of helper class "framework/helper/OComponentEnumeration".
	rem	The follow output must occur:	com.sun.star.lang.XTypeProvider
	rem									com.sun.star.lang.XEventListener
	rem									com.sun.star.container.XEnumeration
	msgbox xComponentEnumeration.dbg_supportedInterfaces
	
	rem ___________________________________________________________________________________________________________________________________
	rem	Test interface XEnumeration of helper OComponentEnumeration.
	nElementCounter = 0
	while( xComponentEnumeration.hasMoreElements = TRUE )
		xElement = xComponentEnumeration.nextElement
		if( isNull(xElement) = TRUE ) then
			msgbox "Error: An empty component in enumeration detected! Whats wrong?"
			exit Sub
		endif
		nElementCounter = nElementCounter + 1
	wend
	if( nElementCounter < 1 ) then
		msgbox "Warning: The enumeration was empty. I think it's wrong ... please check it again."
	endif
	msgbox "Info: An enumeration with " + nElementCounter + " element(s) was detected."
	
	rem ___________________________________________________________________________________________________________________________________
	rem If this point arrived our test was successful.
	msgbox "Test of framework/helper/OComponentAccess & OComponentEnumeration was successful!"
	
End Sub
