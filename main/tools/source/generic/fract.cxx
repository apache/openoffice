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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_tools.hxx"

#ifndef _LIMITS_H
#include <limits.h>
#endif
#include <tools/debug.hxx>
#include <tools/fract.hxx>
#include <tools/stream.hxx>

#include <tools/bigint.hxx>

/*************************************************************************
|*
|*    GetGGT()
|*
|*    Beschreibung      Berechnet den größten gemeinsamen Teiler von
|*                      nVal1 und nVal2
|*    Parameter         long nVal1, long nVal2
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   DV 21.12.92
|*
*************************************************************************/

// Die Funktion GetGGT berechnet den größten gemeinsamen Teiler der
// beiden als Parameter übergebenen Werte nVal1 und nVal2 nach dem
// Algorithmus von Euklid. Hat einer der beiden Parameter den Wert 0 oder
// 1, so wird als Ergebnis der Wert 1 zurückgegeben. Da der Algorithmus
// nur mit positiven Zahlen arbeitet, werden die beiden Parameter
// entsprechend umgewandelt.
// Zum Algorithmus: die beiden Parameter werden solange durcheinander
//              geteilt, bis sie beide gleich sind oder bis bei der Division
//              kein Rest bleibt. Der kleinere der beiden Werte ist dann der
//              GGT.

static long GetGGT( long nVal1, long nVal2 )
{
	nVal1 = Abs( nVal1 );
	nVal2 = Abs( nVal2 );

	if ( nVal1 <= 1 || nVal2 <= 1 )
		return 1;

	while ( nVal1 != nVal2 )
	{
		if ( nVal1 > nVal2 )
		{
			nVal1 %= nVal2;
			if ( nVal1 == 0 )
				return nVal2;
		}
		else
		{
			nVal2 %= nVal1;
			if ( nVal2 == 0 )
				return nVal1;
		}
	}

	return nVal1;
}

static void Reduce( BigInt &rVal1, BigInt &rVal2 )
{
	BigInt nA( rVal1 );
	BigInt nB( rVal2 );
	nA.Abs();
	nB.Abs();

	if ( nA.IsOne() || nB.IsOne() || nA.IsZero() || nB.IsZero() )
		return;

	while ( nA != nB )
	{
		if ( nA > nB )
		{
			nA %= nB;
			if ( nA.IsZero() )
			{
				rVal1 /= nB;
				rVal2 /= nB;
				return;
			}
		}
		else
		{
			nB %= nA;
			if ( nB.IsZero() )
			{
				rVal1 /= nA;
				rVal2 /= nA;
				return;
			}
		}
	}

	rVal1 /= nA;
	rVal2 /= nB;
}

/*************************************************************************
|*
|*    Fraction::Fraction()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    WP 07.03.97
|*    Letzte Änderung
|*
*************************************************************************/

Fraction::Fraction( long nN1, long nN2, long nD1, long nD2 )
{
	long n;
	int  i = 1;

	if( nN1 < 0 ) { i = -i; nN1 = -nN1; }
	if( nN2 < 0 ) { i = -i; nN2 = -nN2; }
	if( nD1 < 0 ) { i = -i; nD1 = -nD1; }
	if( nD2 < 0 ) { i = -i; nD2 = -nD2; }

	n = GetGGT( nN1, nD1 ); if( n > 1 ) { nN1 /= n; nD1 /= n; }
	n = GetGGT( nN1, nD2 ); if( n > 1 ) { nN1 /= n; nD2 /= n; }
	n = GetGGT( nN2, nD1 ); if( n > 1 ) { nN2 /= n; nD1 /= n; }
	n = GetGGT( nN2, nD2 ); if( n > 1 ) { nN2 /= n; nD2 /= n; }

	BigInt nN( nN1 );
	nN *= BigInt( nN2 );

	BigInt nD( nD1 );
	nD *= BigInt( nD2 );

	while ( nN.bIsBig || nD.bIsBig )
	{
		BigInt n1 = 1;
		BigInt n2 = 2;

		nN += n1;
		nN /= n2;
		nD += n1;
		nD /= n2;

		// Kürzen über Größte Gemeinsame Teiler
		Reduce( nN, nD );
	}

	nNumerator   = i * (long)nN;
	nDenominator = (long)nD;
}

/*************************************************************************
|*
|*    Fraction::Fraction()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   DV 21.12.92
|*
*************************************************************************/

// Zur Initialisierung eines Bruches wird nNum dem Zähler und nDen dem
// Nenner zugewiesen. Da negative Werte des Nenners einen Bruch als
// ungültig kennzeichnen, wird bei der Eingabe eines negativen Nenners
// sowohl das Vorzeichen des Nenners und des Zählers invertiert um wieder
// einen gültigen Wert für den Bruch zu erhalten.

Fraction::Fraction( long nNum, long nDen )
{
	nNumerator = nNum;
	nDenominator = nDen;
	if ( nDenominator < 0 )
	{
		nDenominator = -nDenominator;
		nNumerator   = -nNumerator;
	}

	// Kürzen über Größte Gemeinsame Teiler
	long n = GetGGT( nNumerator, nDenominator );
	nNumerator   /= n;
	nDenominator /= n;
}

/*************************************************************************
|*
|*    Fraction::Fraction()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   DV 21.12.92
|*
*************************************************************************/

// Wenn der Wert von dVal größer ist als LONG_MAX, dann wird der Bruch
// auf den Wert ungültig gesetzt, ansonsten werden dVal und der Nenner
// solange mit 10 multipliziert, bis entweder der Zähler oder der Nenner
// größer als LONG_MAX / 10 ist. Zum Schluss wird der so entstandene Bruch
// gekürzt.

Fraction::Fraction( double dVal )
{
	long nDen = 1;
	long nMAX = LONG_MAX / 10;

	if ( dVal > LONG_MAX || dVal < LONG_MIN )
	{
		nNumerator   = 0;
		nDenominator = -1;
		return;
	}

	while ( Abs( (long)dVal ) < nMAX && nDen < nMAX )
	{
		dVal *= 10;
		nDen *= 10;
	}
	nNumerator   = (long)dVal;
	nDenominator = nDen;

	// Kürzen über Größte Gemeinsame Teiler
	long n = GetGGT( nNumerator, nDenominator );
	nNumerator   /= n;
	nDenominator /= n;
}

/*************************************************************************
|*
|*    Fraction::operator double()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   DV 14.05.91
|*
*************************************************************************/

Fraction::operator double() const
{
	if ( nDenominator > 0 )
		return (double)nNumerator / (double)nDenominator;
	else
		return (double)0;
}

/*************************************************************************
|*
|*    Fraction::operator+=()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   DV 21.12.92
|*
*************************************************************************/

// Zunächst werden die beiden Parameter auf ihre Gültigkeit überprüft.
// Ist einer der Parameter ungültig, dann ist auch des Ergebnis
// ungültig. Zur Addition werden die beiden Brüche erst durch
// Erweiterung mit den Nenner des jeweils anderen Bruches auf einen
// gemeinsamen Nenner gebracht. Anschließend werden die beiden Zähler
// addiert und das Ergebnis gekürzt (durch Division von Zähler und
// Nenner mit nGGT). Innerhalb der Funktion wird mit dem Datentyp SLong
// gerechnet, um einen möglichen Überlauf erkennen zu können. Bei
// einem Überlauf wird das Ergebnis auf den Wert ungültig gesetzt.

Fraction& Fraction::operator += ( const Fraction& rVal )
{
	if ( !rVal.IsValid() )
	{
		nNumerator   = 0;
		nDenominator = -1;
	}
	if ( !IsValid() )
		return *this;

	// (a/b) + (c/d) = ( (a*d) + (c*b) ) / (b*d)
	BigInt nN( nNumerator );
	nN *= BigInt( rVal.nDenominator );
	BigInt nW1Temp( nDenominator );
	nW1Temp *= BigInt( rVal.nNumerator );
	nN += nW1Temp;

	BigInt nD( nDenominator );
	nD *= BigInt( rVal.nDenominator );

	Reduce( nN, nD );

	if ( nN.bIsBig || nD.bIsBig )
	{
		nNumerator   = 0;
		nDenominator = -1;
	}
	else
	{
		nNumerator   = (long)nN,
		nDenominator = (long)nD;
	}

	return *this;
}

/*************************************************************************
|*
|*    Fraction::operator-=()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   DV 21.12.92
|*
*************************************************************************/

// Zunächst werden die beiden Parameter auf ihre Gültigkeit überprüft.
// Ist einer der Parameter ungültig, dann ist auch des Ergebnis
// ungültig. Zur Subtraktion werden die beiden Brüche erst durch
// Erweiterung mit den Nenner des jeweils anderen Bruches auf einen
// gemeinsamen Nenner gebracht. Anschließend werden die beiden Zähler
// subtrahiert und das Ergebnis gekürzt (durch Division von Zähler und
// Nenner mit nGGT). Innerhalb der Funktion wird mit dem Datentyp BigInt
// gerechnet, um einen möglichen Überlauf erkennen zu können. Bei
// einem Überlauf wird das Ergebnis auf den Wert ungültig gesetzt.

Fraction& Fraction::operator -= ( const Fraction& rVal )
{
	if ( !rVal.IsValid() )
	{
		nNumerator   = 0;
		nDenominator = -1;
	}
	if ( !IsValid() )
		return *this;

	// (a/b) - (c/d) = ( (a*d) - (c*b) ) / (b*d)
	BigInt nN( nNumerator );
	nN *= BigInt( rVal.nDenominator );
	BigInt nW1Temp( nDenominator );
	nW1Temp *= BigInt( rVal.nNumerator );
	nN -= nW1Temp;

	BigInt nD( nDenominator );
	nD *= BigInt( rVal.nDenominator );

	Reduce( nN, nD );

	if ( nN.bIsBig || nD.bIsBig )
	{
		nNumerator   = 0;
		nDenominator = -1;
	}
	else
	{
		nNumerator   = (long)nN,
		nDenominator = (long)nD;
	}

	return *this;
}

/*************************************************************************
|*
|*    Fraction::operator*=()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   TH 19.08.92
|*
*************************************************************************/

// Zunächst werden die beiden Parameter auf ihre Gültigkeit überprüft.
// Ist einer der Parameter ungültig, dann ist auch des Ergebnis
// ungültig. Zur Multiplikation werden jeweils die beiden Zähler und
// Nenner miteinander multipliziert. Um Überläufe zu vermeiden, werden
// vorher jeweils der GGT zwischen dem Zähler des einen und dem Nenner
// des anderen Bruches bestimmt und bei der Multiplikation Zähler und
// Nenner durch die entsprechenden Werte geteilt.
// Innerhalb der Funktion wird mit dem Datentyp BigInt gerechnet, um
// einen möglichen Überlauf erkennen zu können. Bei einem Überlauf
// wird das Ergebnis auf den Wert ungültig gesetzt.

Fraction& Fraction::operator *= ( const Fraction& rVal )
{
	if ( !rVal.IsValid() )
	{
		nNumerator   = 0;
		nDenominator = -1;
	}
	if ( !IsValid() )
		return *this;

	long nGGT1 = GetGGT( nNumerator, rVal.nDenominator );
	long nGGT2 = GetGGT( rVal.nNumerator, nDenominator );
	BigInt nN( nNumerator / nGGT1 );
	nN *= BigInt( rVal.nNumerator / nGGT2 );
	BigInt nD( nDenominator / nGGT2 );
	nD *= BigInt( rVal.nDenominator / nGGT1 );

	if ( nN.bIsBig || nD.bIsBig )
	{
		nNumerator   = 0;
		nDenominator = -1;
	}
	else
	{
		nNumerator   = (long)nN,
		nDenominator = (long)nD;
	}

	return *this;
}

/*************************************************************************
|*
|*    Fraction::operator/=()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   DV 21.12.92
|*
*************************************************************************/

// Zunächst werden die beiden Parameter auf ihre Gültigkeit überprüft.
// Ist einer der Parameter ungültig, dann ist auch des Ergebnis
// ungültig.
// Um den Bruch a durch b zu teilen, wird a mit dem Kehrwert von b
// multipliziert. Analog zu Multiplikation wird jetzt jeweils der Zähler
// des einen Bruches mit dem Nenner des anderen multipliziert.
// Um Überläufe zu vermeiden, werden vorher jeweils der GGT zwischen den
// beiden Zählern und den beiden Nennern bestimmt und bei der
// Multiplikation Zähler und Nenner durch die entsprechenden Werte
// geteilt.
// Innerhalb der Funktion wird mit dem Datentyp BigInt gerechnet, um
// einen möglichen Überlauf erkennen zu können. Bei einem Überlauf
// wird das Ergebnis auf den Wert ungültig gesetzt.

Fraction& Fraction::operator /= ( const Fraction& rVal )
{
	if ( !rVal.IsValid() )
	{
		nNumerator   = 0;
		nDenominator = -1;
	}
	if ( !IsValid() )
		return *this;

	long nGGT1 = GetGGT( nNumerator, rVal.nNumerator );
	long nGGT2 = GetGGT( rVal.nDenominator, nDenominator );
	BigInt nN( nNumerator / nGGT1 );
	nN *= BigInt( rVal.nDenominator / nGGT2 );
	BigInt nD( nDenominator / nGGT2 );
	nD *= BigInt( rVal.nNumerator / nGGT1 );

	if ( nN.bIsBig || nD.bIsBig )
	{
		nNumerator   = 0;
		nDenominator = -1;
	}
	else
	{
		nNumerator   = (long)nN,
		nDenominator = (long)nD;
		if ( nDenominator < 0 )
		{
			nDenominator = -nDenominator;
			nNumerator   = -nNumerator;
		}
	}

	return *this;
}

/*************************************************************************
|*
|*    Fraction::ReduceInaccurate()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    JOE 17.09.95
|*    Letzte Änderung   kendy 2007-06-13
|*
*************************************************************************/


// Similar to clz_table that can be googled
const char nbits_table[32] =
{
	32,  1, 23,  2, 29, 24, 14,  3,
	30, 27, 25, 18, 20, 15, 10,  4,
	31, 22, 28, 13, 26, 17, 19,  9,
	21, 12, 16,  8, 11,  7,  6,  5
};

static int impl_NumberOfBits( unsigned long nNum )
{
	// https://en.wikipedia.org/wiki/De_Bruijn_sequence
	//
	// background paper: Using de Bruijn Sequences to Index a 1 in a
	// Computer Word (1998) Charles E. Leiserson,
	// Harald Prokop, Keith H. Randall
	// (e.g. http://citeseer.ist.psu.edu/leiserson98using.html)
	const sal_uInt32 nDeBruijn = 0x7DCD629;

	if ( nNum == 0 )
		return 0;

	// Get it to form like 0000001111111111b
	nNum |= ( nNum >>  1 );
	nNum |= ( nNum >>  2 );
	nNum |= ( nNum >>  4 );
	nNum |= ( nNum >>  8 );
	nNum |= ( nNum >> 16 );

	sal_uInt32 nNumber;
	int nBonus = 0;

#if SAL_TYPES_SIZEOFLONG == 4
	nNumber = nNum;
#elif SAL_TYPES_SIZEOFLONG == 8
	nNum |= ( nNum >> 32 );

	if ( nNum & 0x80000000 )
	{
		nNumber = sal_uInt32( nNum >> 32 );
		nBonus = 32;

		if ( nNumber == 0 )
			return 32;
	}
	else
		nNumber = sal_uInt32( nNum & 0xFFFFFFFF );
#else
#error "Unknown size of long!"
#endif

	// De facto shift left of nDeBruijn using multiplication (nNumber
	// is all ones from topmost bit, thus nDeBruijn + (nDeBruijn *
	// nNumber) => nDeBruijn * (nNumber+1) clears all those bits to
	// zero, sets the next bit to one, and thus effectively shift-left
	// nDeBruijn by lg2(nNumber+1). This generates a distinct 5bit
	// sequence in the msb for each distinct position of the last
	// leading 0 bit - that's the property of a de Bruijn number.
	nNumber = nDeBruijn + ( nDeBruijn * nNumber );

	// 5-bit window indexes the result
	return ( nbits_table[nNumber >> 27] ) + nBonus;
}

/** Inaccurate cancellation for a fraction.

    Clip both nominator and denominator to said number of bits. If
    either of those already have equal or less number of bits used,
    this method does nothing.

    @param nSignificantBits denotes, how many significant binary
    digits to maintain, in both nominator and denominator.

    @example ReduceInaccurate(8) has an error <1% [1/2^(8-1)] - the
    largest error occurs with the following pair of values:

    binary    1000000011111111111111111111111b/1000000000000000000000000000000b
    =         1082130431/1073741824
    = approx. 1.007812499

    A ReduceInaccurate(8) yields 1/1.
*/
void Fraction::ReduceInaccurate( unsigned nSignificantBits )
{
	if ( !nNumerator || !nDenominator )
		return;

	// Count with unsigned longs only
	const bool bNeg = ( nNumerator < 0 );
	unsigned long nMul = (unsigned long)( bNeg? -nNumerator: nNumerator );
	unsigned long nDiv = (unsigned long)( nDenominator );

	DBG_ASSERT(nSignificantBits<65, "More than 64 bit of significance is overkill!");

	// How much bits can we lose?
	const int nMulBitsToLose = Max( ( impl_NumberOfBits( nMul ) - int( nSignificantBits ) ), 0 );
	const int nDivBitsToLose = Max( ( impl_NumberOfBits( nDiv ) - int( nSignificantBits ) ), 0 );

	const int nToLose = Min( nMulBitsToLose, nDivBitsToLose );

	// Remove the bits
	nMul >>= nToLose;
	nDiv >>= nToLose;

	if ( !nMul || !nDiv )
	{
		// Return without reduction
		DBG_ERROR( "Oops, we reduced too much..." );
		return;
	}

	// Reduce
	long n1 = GetGGT( nMul, nDiv );
	if ( n1 != 1 )
	{
		nMul /= n1;
		nDiv /= n1;
	}

	nNumerator = bNeg? -long( nMul ): long( nMul );
	nDenominator = nDiv;
}

/*************************************************************************
|*
|*    Fraction::operator ==()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   TH 19.08.92
|*
*************************************************************************/

sal_Bool operator == ( const Fraction& rVal1, const Fraction& rVal2 )
{
	if ( !rVal1.IsValid() || !rVal2.IsValid() )
		return sal_False;

	return rVal1.nNumerator == rVal2.nNumerator
		   && rVal1.nDenominator == rVal2.nDenominator;
}

/*************************************************************************
|*
|*    Fraction::operator <()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   DV 21.12.92
|*
*************************************************************************/

// Beide Operanden werden zunächst auf ihre Gültigkeit überprüft und
// anschließend zur Sicherheit noch einmal gekürzt. Um die Brüche
// (a/b) und (c/d) zu vergleichen, werden sie zunächst auf einen
// gemeinsamen Nenner gebracht (b*d), um dann die beiden Zähler (a*d)
// und (c*b) zu vergleichen. Das Ergebnis dieses Vergleichs wird
// zurückgegeben.

sal_Bool operator < ( const Fraction& rVal1, const Fraction& rVal2 )
{
	if ( !rVal1.IsValid() || !rVal2.IsValid() )
		return sal_False;

	BigInt nN( rVal1.nNumerator );
	nN *= BigInt( rVal2.nDenominator );
	BigInt nD( rVal1.nDenominator );
	nD *= BigInt( rVal2.nNumerator );

	return nN < nD;
}

/*************************************************************************
|*
|*    Fraction::operator >()
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    DV 20.09.90
|*    Letzte Änderung   TH 19.08.92
|*
*************************************************************************/

// Beide Operanden werden zunächst auf ihre Gültigkeit überprüft und
// anschließend zur Sicherheit noch einmal gekürzt. Um die Brüche
// (a/b) und (c/d) zu vergleichen, werden sie zunächst auf einen
// gemeinsamen Nenner gebracht (b*d), um dann die beiden Zähler (a*d)
// und (c*b) zu vergleichen. Das Ergebnis dieses Vergleichs wird
// zurückgegeben.

sal_Bool operator > ( const Fraction& rVal1, const Fraction& rVal2 )
{
	if ( !rVal1.IsValid() || !rVal2.IsValid() )
		return sal_False;

	BigInt nN( rVal1.nNumerator );
	nN *= BigInt( rVal2.nDenominator );
	BigInt nD( rVal1.nDenominator);
	nD *= BigInt( rVal2.nNumerator );

	return nN > nD;
}

/*************************************************************************
|*
|*    SvStream& operator>>( SvStream& rIStream, Fraction& rFract )
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    MM 08.01.96
|*    Letzte Änderung   MM 08.01.96
|*
*************************************************************************/
SvStream& operator >> ( SvStream& rIStream, Fraction& rFract )
{
	rIStream >> rFract.nNumerator;
	rIStream >> rFract.nDenominator;
	return rIStream;
}

/*************************************************************************
|*
|*    SvStream& operator<<( SvStream& rIStream, Fraction& rFract )
|*
|*    Beschreibung      FRACT.SDW
|*    Ersterstellung    MM 08.01.96
|*    Letzte Änderung   MM 08.01.96
|*
*************************************************************************/
SvStream& operator << ( SvStream& rOStream, const Fraction& rFract )
{
	rOStream << rFract.nNumerator;
	rOStream << rFract.nDenominator;
	return rOStream;
}

/* vim: set noet sw=4 ts=4: */
