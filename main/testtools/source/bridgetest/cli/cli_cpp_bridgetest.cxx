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
#include "precompiled_testtools.hxx"

#using <mscorlib.dll>
#using <System.dll>
#using <cli_basetypes.dll>
#using <cli_uretypes.dll>
#using <cli_ure.dll>
#using <cli_types_bridgetest.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Reflection;
using namespace System::Threading;
using namespace uno;
using namespace uno::util;
using namespace unoidl::com::sun::star::uno;
using namespace unoidl::com::sun::star::lang;
//using namespace unoidl::com::sun::star::test::bridge;
using namespace unoidl::test::testtools::bridgetest;
namespace foo
{
    public interface class MyInterface
    {
    };
}

namespace cpp_bridgetest
{
    ref class ORecursiveCall: public WeakBase, public XRecursiveCall
    {
        public:
        // Implementing an interface method needs an explicit virtual in
        // C++/CLI; MC++ inferred it from the base list.
        virtual void  callRecursivly(XRecursiveCall ^ xCall, int nToCall)
		{
			Monitor::Enter(this);
            try
            {
                {
                    if (nToCall > 0)
                    {
                        nToCall --;
                        xCall->callRecursivly(this, nToCall);
                    }
                }
            }
            __finally
            {
                Monitor::Exit(this);
            }

        }
    };

public ref class Constants
{
public:
    static String ^ STRING_TEST_CONSTANT  = gcnew String("\" paco\' chorizo\\\' \"\'");
};

public ref class BridgeTest : public WeakBase, public XMain
{
	static bool compareData(Object ^ val1, Object ^ val2)
	{
		if (val1 == nullptr && val2 == nullptr || val1 == val2)
			return true;
		if ((val1 == nullptr && val2 != nullptr) ||
			(val1 != nullptr && val2 == nullptr) || val1->GetType() != val2->GetType())
			return false;

		bool ret = false;
		Type ^ t1  = val1->GetType();
			//Sequence
		if (t1->IsArray)
		{
			ret = compareSequence(static_cast<Array ^>(val1),
                                  static_cast<Array ^>(val2));
		}
			//String
		else if (t1 == String::typeid)
		{
			ret = val1->Equals(val2);
		}
			// Interface implementation
		else if (t1->GetInterfaces()->Length > 0 && ! t1->IsValueType)
		{
			ret = val1 == val2;
		}
			// Struct
		else if ( ! t1->IsValueType)
		{
			ret = compareStruct(val1, val2);
		}
		else if (t1 == Any::typeid)
		{
			Any a1 = (Any) val1;
			Any a2 = (Any) val2;
			ret = a1.Type == a2.Type && compareData(a1.Value, a2.Value);
		}
		else if (t1->IsValueType)
		{
			//Any, enum, int, bool char, float, double etc.
			ret = val1->Equals(val2);
		}
		else
		{
			Debug::Assert(false);
		}
		return ret;
	}

	// Arrays have only one dimension
	static bool compareSequence(Array ^ ar1, Array ^ ar2)
	{
		Debug::Assert(ar1 != nullptr && ar2 != nullptr);
		Type ^ t1 = ar1->GetType();
		Type ^ t2 = ar2->GetType();

		if (!(ar1->Rank == 1 && ar2->Rank == 1
			&& ar1->Length == ar2->Length && t1->GetElementType() == t2->GetElementType()))
			return false;

		//arrays have same rank and size and element type.
		int len  = ar1->Length;
		bool ret = true;
		for (int i = 0; i < len; i++)
		{
			if (compareData(ar1->GetValue(i), ar2->GetValue(i)) == false)
			{
				ret = false;
				break;
			}
		}
		return ret;
	}

	static bool compareStruct(Object ^ val1, Object ^ val2)
	{
		Debug::Assert(val1 != nullptr && val2 != nullptr);
		Type ^ t1 = val1->GetType();
		Type ^ t2 = val2->GetType();
		if (t1 != t2)
			return false;
		cli::array< FieldInfo ^ > ^ fields = t1->GetFields();
		int cFields = fields->Length;
		bool ret = true;
		for (int i = 0; i < cFields; i++)
		{
			Object ^ fieldVal1 = fields[i]->GetValue(val1);
			Object ^ fieldVal2 = fields[i]->GetValue(val2);
			if ( ! compareData(fieldVal1, fieldVal2))
			{
				ret = false;
				break;
			}
		}
		return ret;
	}

    static bool check( bool b , String ^ message )
    {
        if ( ! b)
        Console::WriteLine("{0} failed\n" , message);
        return b;
    }

    static bool equals(TestElement ^ rData1, TestElement ^  rData2)
    {
        check( rData1->Bool == rData2->Bool, "### bool does not match!" );
        check( rData1->Char == rData2->Char, "### char does not match!" );
        check( rData1->Byte == rData2->Byte, "### byte does not match!" );
        check( rData1->Short == rData2->Short, "### short does not match!" );
        check( rData1->UShort == rData2->UShort, "### unsigned short does not match!" );
        check( rData1->Long == rData2->Long, "### long does not match!" );
        check( rData1->ULong == rData2->ULong, "### unsigned long does not match!" );
        check( rData1->Hyper == rData2->Hyper, "### hyper does not match!" );
        check( rData1->UHyper == rData2->UHyper, "### unsigned hyper does not match!" );
        check( rData1->Float == rData2->Float, "### float does not match!" );
        check( rData1->Double == rData2->Double, "### double does not match!" );
        check( rData1->Enum == rData2->Enum, "### enum does not match!" );
        check( rData1->String == rData2->String, "### string does not match!" );
        check( rData1->Interface == rData2->Interface, "### interface does not match!" );
        check( compareData((::System::Object ^)(rData1->Any), (::System::Object ^)(rData2->Any)), "### any does not match!" );

        return (rData1->Bool == rData2->Bool &&
                rData1->Char == rData2->Char &&
                rData1->Byte == rData2->Byte &&
                rData1->Short == rData2->Short &&
                rData1->UShort == rData2->UShort &&
                rData1->Long == rData2->Long &&
                rData1->ULong == rData2->ULong &&
                rData1->Hyper == rData2->Hyper &&
                rData1->UHyper == rData2->UHyper &&
                rData1->Float == rData2->Float &&
                rData1->Double == rData2->Double &&
                rData1->Enum == rData2->Enum &&
                rData1->String == rData2->String &&
                rData1->Interface == rData2->Interface &&
                compareData((::System::Object ^)(rData1->Any), (::System::Object ^)(rData2->Any)));
    }

static void assign( TestElement ^ rData,
					bool bBool, Char cChar, Byte nByte,
					Int16 nShort, UInt16 nUShort,
					Int32 nLong, UInt32 nULong,
					Int64 nHyper, UInt64 nUHyper,
					float fFloat, double fDouble,
					TestEnum eEnum, String ^ rStr,
					Object ^ xTest,
					uno::Any rAny )
{
	rData->Bool = bBool;
	rData->Char = cChar;
	rData->Byte = nByte;
	rData->Short = nShort;
	rData->UShort = nUShort;
	rData->Long = nLong;
	rData->ULong = nULong;
	rData->Hyper = nHyper;
	rData->UHyper = nUHyper;
	rData->Float = fFloat;
	rData->Double = fDouble;
	rData->Enum = eEnum;
	rData->String = rStr;
	rData->Interface = xTest;
	rData->Any = rAny;
}

static void assign( TestDataElements ^ rData,
					bool bBool, Char cChar, Byte nByte,
					Int16 nShort, UInt16 nUShort,
					Int32 nLong, UInt32 nULong,
					Int64 nHyper, UInt64 nUHyper,
					float fFloat, double fDouble,
					TestEnum eEnum, String ^ rStr,
					Object ^ xTest,
					Any rAny,
					cli::array< TestElement ^ > ^ rSequence)
{
	assign( static_cast<TestElement ^>(rData),
			bBool, cChar, nByte, nShort, nUShort, nLong, nULong, nHyper, nUHyper, fFloat, fDouble,
			eEnum, rStr, xTest, rAny );
	rData->Sequence = rSequence;
}

static bool testAny(Type ^ typ, Object ^  value, XBridgeTest ^ xLBT )
{
	Any any;
	if (typ == nullptr)
		any = Any(value->GetType(), value);
	else
		any = Any(typ, value);

	Any any2 = xLBT->transportAny(any);
	bool ret = compareData((::System::Object ^)(any), (::System::Object ^)(any2));
	if (!ret)
    {
        Console::WriteLine("any is different after roundtrip: in {0}, out {1}\n",
                          any.Type->FullName, any2.Type->FullName);
    }
	return ret;
}



static bool performAnyTest(XBridgeTest ^ xLBT,  TestDataElements ^ data)
{
	bool bReturn = true;
	bReturn = testAny( nullptr, (::System::Object ^)(data->Byte), xLBT ) && bReturn;
	bReturn = testAny( nullptr, (::System::Object ^)(data->Short), xLBT ) && bReturn;
	bReturn = testAny(  nullptr, (::System::Object ^)(data->UShort), xLBT ) && bReturn;
	bReturn = testAny(  nullptr, (::System::Object ^)(data->Long), xLBT ) && bReturn;
	bReturn = testAny(  nullptr, (::System::Object ^)(data->ULong), xLBT ) && bReturn;
	bReturn = testAny(  nullptr, (::System::Object ^)(data->Hyper), xLBT ) && bReturn;
	bReturn = testAny(  nullptr, (::System::Object ^)(data->UHyper), xLBT ) && bReturn;
	bReturn = testAny( nullptr, (::System::Object ^)(data->Float), xLBT ) && bReturn;
	bReturn = testAny( nullptr, (::System::Object ^)(data->Double),xLBT ) && bReturn;
	bReturn = testAny( nullptr, (::System::Object ^)(data->Enum), xLBT ) && bReturn;
	bReturn = testAny( nullptr, data->String,xLBT ) && bReturn;
	bReturn = testAny(XWeak::typeid, data->Interface,xLBT ) && bReturn;
	bReturn = testAny(nullptr, data, xLBT ) && bReturn;

	{
        Any a1(true);
		Any a2 = xLBT->transportAny( a1 );
		bReturn = compareData((::System::Object ^)(a2), (::System::Object ^)(a1)) && bReturn;
	}

	{
        Any a1('A');
		Any a2 = xLBT->transportAny(a1);
		bReturn = compareData( (::System::Object ^)(a2), (::System::Object ^)(a1)) && bReturn;
	}
	return bReturn;
}

static bool performSequenceOfCallTest(XBridgeTest ^ xLBT)
{
	int i,nRounds;
	int nGlobalIndex = 0;
	const int nWaitTimeSpanMUSec = 10000;
	for( nRounds = 0 ; nRounds < 10 ; nRounds ++ )
	{
		for( i = 0 ; i < nRounds ; i ++ )
		{
			// fire oneways
			xLBT->callOneway(nGlobalIndex, nWaitTimeSpanMUSec);
			nGlobalIndex++;
		}

		// call synchron
		xLBT->call(nGlobalIndex, nWaitTimeSpanMUSec);
		nGlobalIndex++;
	}
 	return xLBT->sequenceOfCallTestPassed();
}




static bool performRecursiveCallTest(XBridgeTest ^  xLBT)
{
	xLBT->startRecursiveCall(gcnew ORecursiveCall(), 50);
	// on failure, the test would lock up or crash
	return true;
}

static bool performQueryForUnknownType(XBridgeTest ^ xLBT)
{
    bool bRet = false;
    // test queryInterface for an unknown type
    try
    {
        safe_cast< foo::MyInterface ^ >(xLBT);
    }
    catch( System::InvalidCastException ^)
    {
        bRet = true;
    }

    return bRet;
}

// //==================================================================================================
static bool performTest(XBridgeTest ^ xLBT)
{
	check( xLBT != nullptr, "### no test interface!" );
    bool bRet = true;
	if (xLBT != nullptr)
	{
		// this data is never ever granted access to by calls other than equals(), assign()!
		TestDataElements ^ aData = gcnew TestDataElements(); // test against this data

		Object ^ xI= gcnew WeakBase();

        Any aAny( Object::typeid, xI);
		assign( static_cast<TestElement ^>(aData),
				true, '@', 17, 0x1234, 0xfedc, 0x12345678, 0xfedcba98,
				0x123456789abcdef0, 0xfedcba9876543210,
				17.0815f, 3.1415926359, TestEnum::LOLA,
				Constants::STRING_TEST_CONSTANT, xI,
				aAny);

		bRet = check( aData->Any.Value == xI, "### unexpected any!" ) && bRet;
		bRet = check( !(aData->Any.Value != xI), "### unexpected any!" ) && bRet;

		aData->Sequence = gcnew cli::array< TestElement ^ >(2);
        aData->Sequence[0] = gcnew TestElement(
			aData->Bool, aData->Char, aData->Byte, aData->Short,
			aData->UShort, aData->Long, aData->ULong,
			aData->Hyper, aData->UHyper, aData->Float,
			aData->Double, aData->Enum, aData->String,
			aData->Interface, aData->Any); //(TestElement) aData;
        aData->Sequence[1] = gcnew TestElement(); //is empty

		// aData complete
		//
		// this is a manually copy of aData for first setting...
		TestDataElements ^ aSetData = gcnew TestDataElements();
		Any aAnySet(Object::typeid, xI);
		assign( static_cast<TestElement ^>(aSetData),
				aData->Bool,
				aData->Char,
				aData->Byte,
				aData->Short,
				aData->UShort,
				aData->Long, aData->ULong, aData->Hyper, aData->UHyper, aData->Float, aData->Double,
				aData->Enum,
				aData->String,
				xI,
				aAnySet);

		aSetData->Sequence = gcnew cli::array< TestElement ^ >(2);
        aSetData->Sequence[0] = gcnew TestElement(
			aSetData->Bool, aSetData->Char, aSetData->Byte, aSetData->Short,
			aSetData->UShort, aSetData->Long, aSetData->ULong,
			aSetData->Hyper, aSetData->UHyper, aSetData->Float,
			aSetData->Double, aSetData->Enum, aSetData->String,
			aSetData->Interface, aSetData->Any); //TestElement) aSetData;
        aSetData->Sequence[1] = gcnew TestElement(); // empty struct

		xLBT->setValues(
				aSetData->Bool, aSetData->Char, aSetData->Byte, aSetData->Short, aSetData->UShort,
				aSetData->Long, aSetData->ULong, aSetData->Hyper, aSetData->UHyper, aSetData->Float, aSetData->Double,
				aSetData->Enum, aSetData->String, aSetData->Interface, aSetData->Any, aSetData->Sequence, aSetData );

		{
		TestDataElements ^ aRet = gcnew TestDataElements();
        TestDataElements ^ aRet2 = gcnew TestDataElements();
		xLBT->getValues(
			aRet->Bool, aRet->Char, aRet->Byte, aRet->Short, aRet->UShort,
			aRet->Long, aRet->ULong, aRet->Hyper, aRet->UHyper,
            aRet->Float, aRet->Double, aRet->Enum, aRet->String,
            aRet->Interface, aRet->Any, aRet->Sequence, aRet2 );

		bRet = check( compareData( aData, aRet ) && compareData( aData, aRet2 ) , "getValues test") && bRet;

		// set last retrieved values
		TestDataElements ^ aSV2ret = xLBT->setValues2(
			aRet->Bool, aRet->Char, aRet->Byte, aRet->Short, aRet->UShort,
			aRet->Long, aRet->ULong, aRet->Hyper, aRet->UHyper, aRet->Float,
            aRet->Double, aRet->Enum, aRet->String, aRet->Interface, aRet->Any,
            aRet->Sequence, aRet2 );

        // check inout sequence order
        // => inout sequence parameter was switched by test objects
		TestElement ^ temp = aRet->Sequence[ 0 ];
        aRet->Sequence[ 0 ] = aRet->Sequence[ 1 ];
        aRet->Sequence[ 1 ] = temp;

		bRet = check(
            compareData( aData, aSV2ret ) && compareData( aData, aRet2 ),
            "getValues2 test") && bRet;
		}
		{
		TestDataElements ^ aRet = gcnew TestDataElements();
        TestDataElements ^ aRet2 = gcnew TestDataElements();
		TestDataElements ^ aGVret = xLBT->getValues(
			aRet->Bool, aRet->Char, aRet->Byte, aRet->Short,
            aRet->UShort, aRet->Long, aRet->ULong, aRet->Hyper,
            aRet->UHyper, aRet->Float, aRet->Double, aRet->Enum,
            aRet->String, aRet->Interface, aRet->Any, aRet->Sequence,
            aRet2 );

		bRet = check( compareData( aData, aRet ) && compareData( aData, aRet2 ) && compareData( aData, aGVret ), "getValues test" ) && bRet;

		// set last retrieved values
		xLBT->Bool = aRet->Bool;
		xLBT->Char = aRet->Char;
		xLBT->Byte = aRet->Byte;
		xLBT->Short = aRet->Short;
		xLBT->UShort = aRet->UShort;
        xLBT->Long = aRet->Long;
		xLBT->ULong = aRet->ULong;
		xLBT->Hyper = aRet->Hyper;
		xLBT->UHyper = aRet->UHyper;
		xLBT->Float = aRet->Float;
		xLBT->Double = aRet->Double;
		xLBT->Enum = aRet->Enum;
		xLBT->String = aRet->String;
		xLBT->Interface = aRet->Interface;
		xLBT->Any = aRet->Any;
		xLBT->Sequence = aRet->Sequence;
		xLBT->Struct = aRet2;
		}
		{
		TestDataElements ^ aRet = gcnew TestDataElements();
        TestDataElements ^ aRet2 = gcnew TestDataElements();
		aRet->Hyper = xLBT->Hyper;
		aRet->UHyper = xLBT->UHyper;
		aRet->Float = xLBT->Float;
		aRet->Double = xLBT->Double;
		aRet->Byte = xLBT->Byte;
		aRet->Char = xLBT->Char;
		aRet->Bool = xLBT->Bool;
		aRet->Short = xLBT->Short;
		aRet->UShort = xLBT->UShort;
		aRet->Long = xLBT->Long;
		aRet->ULong = xLBT->ULong;
		aRet->Enum = xLBT->Enum;
		aRet->String = xLBT->String;
		aRet->Interface = xLBT->Interface;
		aRet->Any = xLBT->Any;
		aRet->Sequence = xLBT->Sequence;
		aRet2 = xLBT->Struct;

		bRet = check( compareData( aData, aRet ) && compareData( aData, aRet2 ) , "struct comparison test") && bRet;

		bRet = check(performSequenceTest(xLBT), "sequence test") && bRet;

		// any test
		bRet = check( performAnyTest( xLBT , aData ) , "any test" ) && bRet;

		// sequence of call test
		bRet = check( performSequenceOfCallTest( xLBT ) , "sequence of call test" ) && bRet;

		// recursive call test
		bRet = check( performRecursiveCallTest( xLBT ) , "recursive test" ) && bRet;

		bRet = (compareData( aData, aRet ) && compareData( aData, aRet2 )) && bRet ;

        // check setting of null reference.  XInterface maps to Object^, and
        // "= 0" on one BOXES the literal into an Int32 instead of storing
        // null -- so the property ends up non-null and the comparison below
        // is false whatever happens.  Neither has a check() message, which is
        // why this failed as a bare "standard test failed".
        xLBT->Interface = nullptr;
        aRet->Interface = xLBT->Interface;
        bRet = (aRet->Interface == nullptr) && bRet;

        }


	}
        return bRet;
 }
static bool performSequenceTest(XBridgeTest ^ xBT)
{
    bool bRet = true;
    XBridgeTest2 ^  xBT2 = dynamic_cast<XBridgeTest2 ^>(xBT);
    if ( xBT2 == nullptr)
        return false;

    // perform sequence tests (XBridgeTest2)
    // create the sequence which are compared with the results
    cli::array< bool > ^ arBool = gcnew cli::array< bool >(3);
    arBool[0] = true; arBool[1] = false; arBool[2] = true;
    cli::array< Char > ^ arChar = gcnew cli::array< Char >(3);
    arChar[0] = 'A'; arChar[1] = 'B'; arChar[2] = 'C';
    cli::array< Byte > ^ arByte = gcnew cli::array< Byte >(3);
    arByte[0] =  1; arByte[1] = 2; arByte[2] = 0xff;
    cli::array< Int16 > ^ arShort = gcnew cli::array< Int16 >(3);
    arShort[0] = Int16::MinValue; arShort[1] = 1; arShort[2] = Int16::MaxValue;
    cli::array< UInt16 > ^ arUShort = gcnew cli::array< UInt16 >(3);
    arUShort[0] = UInt16::MinValue; arUShort[1] = 1; arUShort[2] = UInt16::MaxValue;
    cli::array< Int32 > ^ arLong = gcnew cli::array< Int32 >(3);
    arLong[0] = Int32::MinValue; arLong[1] = 1; arLong[2] = Int32::MaxValue;
    cli::array< UInt32 > ^ arULong = gcnew cli::array< UInt32 >(3);
    arULong[0] = UInt32::MinValue; arULong[1] = 1; arULong[2] = UInt32::MaxValue;
    cli::array< Int64 > ^ arHyper = gcnew cli::array< Int64 >(3);
    arHyper[0] = Int64::MinValue; arHyper[1] = 1; arHyper[2] = Int64::MaxValue;
    cli::array< UInt64 > ^ arUHyper = gcnew cli::array< UInt64 >(3);
    arUHyper[0] = UInt64::MinValue; arUHyper[1] = 1;
    arUHyper[2] = UInt64::MaxValue;
    cli::array< Single > ^ arFloat = gcnew cli::array< Single >(3);
    arFloat[0] = 1.1f; arFloat[1] = 2.2f; arFloat[2] = 3.3f;
    cli::array< Double > ^ arDouble = gcnew cli::array< Double >(3);
    arDouble[0] = 1.11; arDouble[1] = 2.22; arDouble[2] = 3.33;
    cli::array< String ^ > ^ arString = gcnew cli::array< String ^ >( 3 );
    arString[0] = gcnew String("String 1");
    arString[1] = gcnew String("String 2");
    arString[2] = gcnew String("String 3");

    cli::array< Any > ^ arAny = gcnew cli::array< Any >( 3 );
    arAny[0] = Any(true); arAny[1] = Any(11111); arAny[2] = Any(3.14);
    cli::array< Object ^ > ^ arObject = gcnew cli::array< Object ^ >( 3 );
    arObject[0] = gcnew WeakBase(); arObject[1] =  gcnew WeakBase();
    arObject[1] = gcnew WeakBase();

    //TestEnum arEnum[] = new TestEnum[3];
    //arEnum[0] = TestEnum::ONE; arEnum[1] = TestEnum::TWO;
    //arEnum[2] = TestEnum::CHECK;
	Console::WriteLine(gcnew String("cli_cpp_bridgetest: Workaround for C++ compiler bug:"
		" using Array of Int32 instead of Array of enums w"));
	cli::array< Int32 > ^ arEnum = gcnew cli::array< Int32 >(3);
	arEnum[0] = static_cast<Int32>(TestEnum::ONE);
	arEnum[1] = static_cast<Int32>(TestEnum::TWO);
	arEnum[2] = static_cast<Int32>(TestEnum::CHECK);

    cli::array< TestElement ^ > ^ arStruct = gcnew cli::array< TestElement ^ >(3);
    arStruct[0] = gcnew TestElement(); arStruct[1] = gcnew TestElement();
    arStruct[2] = gcnew TestElement();
    assign( arStruct[0], true, '@', 17, 0x1234, 0xfedc, 0x12345678, 0xfedcba98,
 			0x123456789abcdef0, 0xfedcba9876543210, 17.0815f, 3.1415926359,
            TestEnum::LOLA, Constants::STRING_TEST_CONSTANT, arObject[0],
            Any( Object::typeid,  arObject[0]) );
    assign( arStruct[1], true, 'A', 17, 0x1234, 0xfedc, 0x12345678, 0xfedcba98,
			0x123456789abcdef0, 0xfedcba9876543210, 17.0815f, 3.1415926359,
            TestEnum::TWO, Constants::STRING_TEST_CONSTANT, arObject[1],
            Any( Object::typeid, arObject[1]) );
    assign( arStruct[2], true, 'B', 17, 0x1234, 0xfedc, 0x12345678, 0xfedcba98,
			0x123456789abcdef0, 0xfedcba9876543210, 17.0815f, 3.1415926359,
            TestEnum::CHECK, Constants::STRING_TEST_CONSTANT, arObject[2],
            Any( Object::typeid, arObject[2] ) );


//     int[][][] arLong3 = new int[][][]{
//         new int[][]{new int[]{1,2,3},new int[]{4,5,6}, new int[]{7,8,9} },
//         new int [][]{new int[]{1,2,3},new int[]{4,5,6}, new int[]{7,8,9}},
//         new int[][]{new int[]{1,2,3},new int[]{4,5,6}, new int[]{7,8,9}}};

    {

//		Console::WriteLine(gcnew String("cli_cpp_bridgetest:
//     int[][] seqSeqRet = xBT2->setDim2(arLong3[0]);
//     bRet = check( compareData(seqSeqRet, arLong3[0]), "sequence test") && bRet;
//     int[][][] seqSeqRet2 = xBT2->setDim3(arLong3);
//     bRet = check( compareData(seqSeqRet2, arLong3), "sequence test") && bRet;

    cli::array< Any > ^ seqAnyRet = xBT2->setSequenceAny(arAny);
    bRet = check( compareData(seqAnyRet, arAny), "sequence test") && bRet;
    cli::array< Boolean > ^ seqBoolRet = xBT2->setSequenceBool(arBool);
    bRet = check( compareData(seqBoolRet, arBool), "sequence test") && bRet;
    cli::array< Byte > ^ seqByteRet = xBT2->setSequenceByte(arByte);
    bRet = check( compareData(seqByteRet, arByte), "sequence test") && bRet;
    cli::array< Char > ^ seqCharRet = xBT2->setSequenceChar(arChar);
    bRet = check( compareData(seqCharRet, arChar), "sequence test") && bRet;
    cli::array< Int16 > ^ seqShortRet = xBT2->setSequenceShort(arShort);
    bRet = check( compareData(seqShortRet, arShort), "sequence test") && bRet;
    cli::array< Int32 > ^ seqLongRet = xBT2->setSequenceLong(arLong);
    bRet = check( compareData(seqLongRet, arLong), "sequence test") && bRet;
    cli::array< Int64 > ^ seqHyperRet = xBT2->setSequenceHyper(arHyper);
    bRet = check( compareData(seqHyperRet,arHyper), "sequence test") && bRet;
    cli::array< Single > ^ seqFloatRet = xBT2->setSequenceFloat(arFloat);
    bRet = check( compareData(seqFloatRet, arFloat), "sequence test") && bRet;
    cli::array< Double > ^ seqDoubleRet = xBT2->setSequenceDouble(arDouble);
    bRet = check( compareData(seqDoubleRet, arDouble), "sequence test") && bRet;
    xBT2->setSequenceEnum(
        safe_cast< cli::array< TestEnum > ^ >( (Object ^) arEnum ) );
	//comparing seqEnumRet with arEnum will fail since they are of different
	//types because of workaround. arEnum is Int32[].
	Console::WriteLine(gcnew String("cli_cpp_bridgetest: Test omitted because "
		"of C++ compiler bug. XBridgeTest2::setSequenceEnum(sequence<TestEnum>)"));
//    bRet = check( compareData(seqEnumRet, arEnum), "sequence test") && bRet;
    cli::array< UInt16 > ^ seqUShortRet = xBT2->setSequenceUShort(arUShort);
    bRet = check( compareData(seqUShortRet, arUShort), "sequence test") && bRet;
    cli::array< UInt32 > ^ seqULongRet = xBT2->setSequenceULong(arULong);
    bRet = check( compareData(seqULongRet, arULong), "sequence test") && bRet;
    cli::array< UInt64 > ^ seqUHyperRet = xBT2->setSequenceUHyper(arUHyper);
    bRet = check( compareData(seqUHyperRet, arUHyper), "sequence test") && bRet;
    cli::array< Object ^ > ^ seqObjectRet = xBT2->setSequenceXInterface(arObject);
    bRet = check( compareData(seqObjectRet, arObject), "sequence test") && bRet;
    cli::array< String ^ > ^ seqStringRet = xBT2->setSequenceString(arString);
    bRet = check( compareData(seqStringRet, arString), "sequence test") && bRet;
    cli::array< TestElement ^ > ^ seqStructRet = xBT2->setSequenceStruct(arStruct);
    bRet = check( compareData(seqStructRet, arStruct), "sequence test") && bRet;
    }
    {
//     Boolean arBoolTemp[] = static_cast<Boolean[]>( arBool->Clone());
//     Char arCharTemp[] = static_cast<Char[]>(arChar->Clone());
//     Byte arByteTemp[] = static_cast<Byte[]>(arByte->Clone());
//     Int16 arShortTemp[] = static_cast<Int16[]>(arShort->Clone());
//     UInt16 arUShortTemp[] = static_cast<UInt16[]>(arUShort->Clone());
//     Int32 arLongTemp[] = static_cast<Int32[]>(arLong->Clone());
//     UInt32 arULongTemp[] = static_cast<UInt32[]>(arULong->Clone());
//     Int64 arHyperTemp[] = static_cast<Int64[]>(arHyper->Clone());
//     UInt64 arUHyperTemp[] = static_cast<UInt64[]>(arUHyper->Clone());
//     Single arFloatTemp[] = static_cast<Single[]>(arFloat->Clone());
//     Double arDoubleTemp[] = static_cast<Double[]>(arDouble->Clone());
//     TestEnum arEnumTemp[] = static_cast<TestEnum[]>(arEnum->Clone());
//     cli::array< String ^ > ^ arStringTemp = static_cast<String ^[]>(arString->Clone());
//     Object ^ arObjectTemp = static_cast<Object ^[]>(arObject->Clone());
//     cli::array< Any > ^ arAnyTemp = static_cast<Any[]>(arAny->Clone());
//     // make sure this are has the same contents as arLong3[0]
//     int[][] arLong2Temp = new int[][]{new int[]{1,2,3},new int[]{4,5,6}, new int[]{7,8,9} };
//     // make sure this are has the same contents as arLong3
//     int[][][] arLong3Temp = new int[][][]{
//         new int[][]{new int[]{1,2,3},new int[]{4,5,6}, new int[]{7,8,9} },
//         new int [][]{new int[]{1,2,3},new int[]{4,5,6}, new int[]{7,8,9}},
//         new int[][]{new int[]{1,2,3},new int[]{4,5,6}, new int[]{7,8,9}}};
	Console::WriteLine(gcnew String("cli_cpp_bridgetest: no test of "
		"XBridgeTest2::setSequencesInOut and XBridgeTest2.setSequencesOut "
		"because jagged arrays are not supported by C++ compiler"));
//     xBT2->setSequencesInOut(& arBoolTemp, & arCharTemp, & arByteTemp,
//                            & arShortTemp, & arUShortTemp, & arLongTemp,
//                            & arULongTemp,& arHyperTemp, & arUHyperTemp,
//                            & arFloatTemp,& arDoubleTemp, & arEnumTemp,
//                            & arStringTemp, &  arObjectTemp,
//                            & arAnyTemp, & arLong2Temp, & arLong3Temp);
//     bRet = check(
//         compareData(arBoolTemp, arBool) &&
//         compareData(arCharTemp , arChar) &&
//         compareData(arByteTemp , arByte) &&
//         compareData(arShortTemp , arShort) &&
//         compareData(arUShortTemp , arUShort) &&
//         compareData(arLongTemp , arLong) &&
//         compareData(arULongTemp , arULong) &&
//         compareData(arHyperTemp , arHyper) &&
//         compareData(arUHyperTemp , arUHyper) &&
//         compareData(arFloatTemp , arFloat) &&
//         compareData(arDoubleTemp , arDouble) &&
//         compareData(arEnumTemp , arEnum) &&
//         compareData(arStringTemp , arString) &&
//         compareData(arObjectTemp , arObject) &&
//         compareData(arAnyTemp , arAny) &&
//         compareData(arLong2Temp , arLong3[0]) &&
//         compareData(arLong3Temp , arLong3), "sequence test") && bRet;

    //Boolean arBoolOut[];
    //Char arCharOut[];
    //Byte arByteOut[];
    //Int16 arShortOut[];
    //UInt16 arUShortOut[];
    //Int32 arLongOut[];
    //UInt32 arULongOut[];
    //Int64 arHyperOut[];
    //UInt64 arUHyperOut[];
    //Single arFloatOut[];
    //Double arDoubleOut[];
    //TestEnum arEnumOut[];
    //cli::array< String ^ > ^ arStringOut;
    //cli::array< Object ^ > ^ arObjectOut;
    //cli::array< Any > ^ arAnyOut;
//     int[][] arLong2Out;
//     int[][][] arLong3Out;

//     xBT2->setSequencesOut(out arBoolOut, out arCharOut, out arByteOut,
//                          out arShortOut, out arUShortOut, out arLongOut,
//                          out arULongOut, out arHyperOut, out arUHyperOut,
//                          out arFloatOut, out arDoubleOut, out arEnumOut,
//                          out arStringOut, out arObjectOut, out arAnyOut,
//                          out arLong2Out, out arLong3Out);
//     bRet = check(
//         compareData(arBoolOut, arBool) &&
//         compareData(arCharOut, arChar) &&
//         compareData(arByteOut, arByte) &&
//         compareData(arShortOut, arShort) &&
//         compareData(arUShortOut, arUShort) &&
//         compareData(arLongOut, arLong) &&
//         compareData(arULongOut, arULong) &&
//         compareData(arHyperOut, arHyper) &&
//         compareData(arUHyperOut, arUHyper) &&
//         compareData(arFloatOut, arFloat) &&
//         compareData(arDoubleOut, arDouble) &&
//         compareData(arEnumOut, arEnum) &&
//         compareData(arStringOut, arString) &&
//         compareData(arObjectOut, arObject) &&
//         compareData(arAnyOut, arAny) &&
//         compareData(arLong2Out, arLong3[0]) &&
//         compareData(arLong3Out, arLong3), "sequence test") && bRet;
    }
    {
    //test with empty sequences
   //  int[][] _arLong2 = new int[0][];
//     int[][] seqSeqRet = xBT2->setDim2(_arLong2);
//     bRet = check( compareData(seqSeqRet, _arLong2), "sequence test") && bRet;
//     int[][][] _arLong3 = new int[0][][];
//     int[][][] seqSeqRet2 = xBT2->setDim3(_arLong3);
//    bRet = check( compareData(seqSeqRet2, _arLong3), "sequence test") && bRet;
    cli::array< Any > ^ _arAny = gcnew cli::array< Any >( 0 );
    cli::array< Any > ^ seqAnyRet = xBT2->setSequenceAny(_arAny);
    bRet = check( compareData(seqAnyRet, _arAny), "sequence test") && bRet;
    cli::array< Boolean > ^ _arBool = gcnew cli::array< Boolean >(0);
    cli::array< Boolean > ^ seqBoolRet = xBT2->setSequenceBool(_arBool);
    bRet = check( compareData(seqBoolRet, _arBool), "sequence test") && bRet;
    cli::array< Byte > ^ _arByte = gcnew cli::array< Byte >(0);
    cli::array< Byte > ^ seqByteRet = xBT2->setSequenceByte(_arByte);
    bRet = check( compareData(seqByteRet, _arByte), "sequence test") && bRet;
    cli::array< Char > ^ _arChar = gcnew cli::array< Char >(0);
    cli::array< Char > ^ seqCharRet = xBT2->setSequenceChar(_arChar);
    bRet = check( compareData(seqCharRet, _arChar), "sequence test") && bRet;
    cli::array< Int16 > ^ _arShort = gcnew cli::array< Int16 >(0);
    cli::array< Int16 > ^ seqShortRet = xBT2->setSequenceShort(_arShort);
    bRet = check( compareData(seqShortRet, _arShort), "sequence test") && bRet;
    cli::array< Int32 > ^ _arLong = gcnew cli::array< Int32 >(0);
    cli::array< Int32 > ^ seqLongRet = xBT2->setSequenceLong(_arLong);
    bRet = check( compareData(seqLongRet, _arLong), "sequence test") && bRet;
    cli::array< Int64 > ^ _arHyper = gcnew cli::array< Int64 >(0);
    cli::array< Int64 > ^ seqHyperRet = xBT2->setSequenceHyper(_arHyper);
    bRet = check( compareData(seqHyperRet, _arHyper), "sequence test") && bRet;
    cli::array< Single > ^ _arFloat = gcnew cli::array< Single >(0);
    cli::array< Single > ^ seqFloatRet = xBT2->setSequenceFloat(_arFloat);
    bRet = check( compareData(seqFloatRet, _arFloat), "sequence test") && bRet;
    cli::array< Double > ^ _arDouble = gcnew cli::array< Double >(0);
    cli::array< Double > ^ seqDoubleRet = xBT2->setSequenceDouble(_arDouble);
    bRet = check( compareData(seqDoubleRet, _arDouble), "sequence test") && bRet;
    cli::array< TestEnum > ^ _arEnum = gcnew cli::array< TestEnum >(0);
    xBT2->setSequenceEnum(_arEnum);
//	compiler bug: _arEnum has type System.Enum and not TestEnum
//    bRet = check( compareData(seqEnumRet, _arEnum), "sequence test") && bRet;
    cli::array< UInt16 > ^ _arUShort = gcnew cli::array< UInt16 >(0);
    cli::array< UInt16 > ^ seqUShortRet = xBT2->setSequenceUShort(_arUShort);
    bRet = check( compareData(seqUShortRet, _arUShort), "sequence test") && bRet;
    cli::array< UInt32 > ^ _arULong = gcnew cli::array< UInt32 >(0);
    cli::array< UInt32 > ^ seqULongRet = xBT2->setSequenceULong(_arULong);
    bRet = check( compareData(seqULongRet, _arULong), "sequence test") && bRet;
    cli::array< UInt64 > ^ _arUHyper = gcnew cli::array< UInt64 >(0);
    cli::array< UInt64 > ^ seqUHyperRet = xBT2->setSequenceUHyper(_arUHyper);
    bRet = check( compareData(seqUHyperRet, _arUHyper), "sequence test") && bRet;
    cli::array< Object ^ > ^ _arObject = gcnew cli::array< Object ^ >( 0 );
    cli::array< Object ^ > ^ seqObjectRet = xBT2->setSequenceXInterface(_arObject);
    bRet = check( compareData(seqObjectRet, _arObject), "sequence test") && bRet;
    cli::array< String ^ > ^ _arString = gcnew cli::array< String ^ >( 0 );
    cli::array< String ^ > ^ seqStringRet = xBT2->setSequenceString(_arString);
    bRet = check( compareData(seqStringRet, _arString), "sequence test") && bRet;
    cli::array< TestElement ^ > ^ _arStruct = gcnew cli::array< TestElement ^ >(0);
    cli::array< TestElement ^ > ^ seqStructRet = xBT2->setSequenceStruct(_arStruct);
    bRet = check( compareData(seqStructRet, _arStruct), "sequence test") && bRet;

    }
    return bRet;
}
/** Test the System::Object method on the proxy object
 */
static bool testObjectMethodsImplemention(XBridgeTest ^ xLBT)
{
    bool ret = false;
    Object ^ obj = gcnew Object();
	XBridgeTestBase ^ xBase = dynamic_cast<XBridgeTestBase ^>(xLBT);
	if (xBase == nullptr)
		return false;
	// Object.Equals
	ret = xLBT->Equals(obj) == false;
	ret = xLBT->Equals(xLBT) && ret;
	ret = Object::Equals(obj, obj) && ret;
	ret = Object::Equals(xLBT, xBase) && ret;
	//Object.GetHashCode
	// Don't know how to verify this. Currently it is not possible to get the object id from a proxy
	int nHash = xLBT->GetHashCode();
	ret = nHash == xBase->GetHashCode() && ret;

	//Object.ToString
    // Don't know how to verify this automatically.
	String ^ s = xLBT->ToString();
    ret = (s->Length > 0) && ret;
    return ret;
}


static bool raiseOnewayException(XBridgeTest ^ xLBT)
{
    bool bReturn = true;
	String ^ sCompare = Constants::STRING_TEST_CONSTANT;
	try
	{
		// Note : the exception may fly or not (e.g. remote scenario).
		//        When it flies, it must contain the correct elements.
		xLBT->raiseRuntimeExceptionOneway(sCompare, xLBT->Interface );
	}
	catch (RuntimeException ^  e )
	{
		bReturn = ( xLBT->Interface == e->Context );
	}
    return bReturn;
}

// //==================================================================================================
static bool raiseException(XBridgeTest ^ xLBT )
{
	int nCount = 0;
	try
	{
		try
		{
			try
			{
				xLBT->raiseException(
					5, Constants::STRING_TEST_CONSTANT, xLBT->Interface );
			}
			catch (unoidl::com::sun::star::lang::IllegalArgumentException ^ aExc)
			{
				if (aExc->ArgumentPosition == 5 &&
                    aExc->Context == xLBT->Interface)
				{
					++nCount;
				}
				else
				{
					check( false, "### unexpected exception content!" );
				}

				/** it is certain, that the RuntimeException testing will fail,
                    if no */
				xLBT->RuntimeException = 0;
			}
		}
		catch (unoidl::com::sun::star::uno::RuntimeException ^ rExc)
		{
			if (rExc->Context == xLBT->Interface )
			{
				++nCount;
			}
			else
			{
				check( false, "### unexpected exception content!" );
			}

			/** it is certain, that the RuntimeException testing will fail, if no */
            xLBT->RuntimeException = (int) 0xcafebabe;
		}
	}
	catch (unoidl::com::sun::star::uno::Exception ^  rExc)
	{
		if (rExc->Context == xLBT->Interface)
		{
			++nCount;
		}
		else

		{
			check( false, "### unexpected exception content!" );
		}
		return (nCount == 3);
	}
    return false;
}

    static private void perform_test( XBridgeTest ^ xLBT )
    {
        bool bRet= true;
       bRet = check( performTest( xLBT ), "standard test" ) && bRet;
       bRet = check( raiseException( xLBT ) , "exception test" )&& bRet;
       bRet = check( raiseOnewayException( xLBT ), "oneway exception test" ) && bRet;
       bRet = check( testObjectMethodsImplemention(xLBT), "object methods test") && bRet;
       bRet = performQueryForUnknownType( xLBT ) && bRet;
        if (! bRet)
        {
            throw gcnew unoidl::com::sun::star::uno::RuntimeException(
                gcnew String("error: test failed!"), nullptr);
        }
    }
    XComponentContext ^ m_xContext;

    public:
    BridgeTest( XComponentContext ^ xContext )
    {
        m_xContext = xContext;
    }



    virtual int run( cli::array< String ^ > ^ args )
    {
        try
        {
            if (args->Length < 1)
            {
                throw gcnew RuntimeException(
                    "missing argument for bridgetest!", this );
            }
            Object ^ test_obj =
                m_xContext->getServiceManager()->createInstanceWithContext(
                    args[ 0 ], m_xContext );
            if (test_obj == nullptr)
                test_obj = m_xContext->getValueByName( args[ 0 ] ).Value;

            Console::WriteLine(
                "cli target bridgetest obj: {0}", test_obj->ToString() );
            XBridgeTest ^ xTest = safe_cast< XBridgeTest ^ >(test_obj) ;
            perform_test( xTest );
            Console::WriteLine( "\n### cli_uno C++  bridgetest succeeded." );
            return 0;
        }
        catch (unoidl::com::sun::star::uno::RuntimeException ^ )
        {
            throw;
        }
        catch (System::Exception ^ exc)
        {
            System::Text::StringBuilder ^ s = gcnew System::Text::StringBuilder();
            s->Append("cli_cpp_bridgetest: unexpected exception occurred in XMain::run. Original exception: ");
            s->Append(exc->GetType()->Name);
            s->Append("\n Message: ");
            s->Append(exc->Message);
            throw gcnew unoidl::com::sun::star::uno::RuntimeException(
                s->ToString(), nullptr);
        }
    }
};

}
