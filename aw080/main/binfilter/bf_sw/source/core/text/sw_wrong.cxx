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




#ifdef _MSC_VER
#pragma hdrstop
#endif


#include "wrong.hxx"
namespace binfilter {

/*************************************************************************
 * sal_Bool SwWrongList::InWrongWord() gibt den Anfang und die Laenge des Wortes
 * zurueck, wenn es als falsch markiert ist.
 *************************************************************************/

/*************************************************************************
 * sal_Bool SwWrongList::Check() liefert den ersten falschen Bereich
 *************************************************************************/

/*************************************************************************
 * xub_StrLen SwWrongList::NextWrong() liefert die naechste Fehlerposition
 *************************************************************************/


/*************************************************************************
 * xub_StrLen SwWrongList::LastWrong() liefert die letzte Fehlerposition
 *************************************************************************/


/*************************************************************************
 *				   MSHORT SwWrongList::GetPos( xub_StrLen nValue )
 *  sucht die erste Position im Array, die groessergleich nValue ist,
 * dies kann natuerlich auch hinter dem letzten Element sein!
 *************************************************************************/


/*************************************************************************
 *				   void SwWrongList::Invalidate()
 *************************************************************************/

/*N*/ void SwWrongList::_Invalidate( xub_StrLen nBegin, xub_StrLen nEnd )
/*N*/ {
/*N*/ 	if ( nBegin < GetBeginInv() )
/*N*/ 		nBeginInvalid = nBegin;
/*N*/ 	if ( nEnd > GetEndInv() )
/*N*/ 		nEndInvalid = nEnd;
/*N*/ }

/*************************************************************************
 *				   		SwWrongList::Move( xub_StrLen nPos, long nDiff )
 *  veraendert alle Positionen ab nPos um den angegebenen Wert,
 *  wird nach Einfuegen oder Loeschen von Buchstaben benoetigt.
 *************************************************************************/


/*************************************************************************
 *				   		SwWrongList::Clear()/( xub_StrLen nBegin, xub_StrLen nEnd )
 *  loescht das Array im angegebenen Bereich
 *************************************************************************/



/*************************************************************************
 *                      SwWrongList::Fresh
 *
 * In this method the wrong list is updated, new wrong words are inserted,
 * and by evaluating the postiztion of wrong words, we also know, which
 * words are not wrong any longer and have to be removed.
 * Note: Fresh has to be called at the end of the check of the invalid region,
 * in order to find words, which are behind the last wrong word but not wrong
 * any longer
 *************************************************************************/

/*N*/ sal_Bool SwWrongList::InvalidateWrong( )
/*N*/ {
/*N*/ 	if( Count() )
/*N*/ 	{
/*N*/ 		xub_StrLen nFirst = WRPOS( 0 );
/*N*/ 		xub_StrLen nLast = WRPOS( Count() - 1 ) + WRLEN( Count() - 1 );
/*N*/ 		Invalidate( nFirst, nLast );
/*N*/ 		return sal_True;
/*N*/ 	}
/*N*/ 	else
/*N*/ 		return sal_False;
/*N*/ }



}
