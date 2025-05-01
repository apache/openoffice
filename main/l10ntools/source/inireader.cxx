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

#define U_SHOW_CPLUSPLUS_API 0
#define U_SHOW_CPLUSPLUS_HEADER_API 0
#include <unicode/uregex.h>
//#include <unicode/unistr.h>

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <rtl/ustring.hxx>
#include "inireader.hxx"

using namespace std;
namespace transex3
{

bool INIreader::read( INImap& myMap , string& filename )
{
    ifstream aFStream( filename.c_str() ); 
    if( aFStream && aFStream.is_open())		
    {
        string line;
        string section;
        string param_key;
        string param_value;
        stringmap* myvalues = 0;

        while( std::getline( aFStream , line ) )
        {
            trim( line );
            if( line.empty() ){
            }
            else if( is_section( line , section ) )
            {
                //cerr << "[" << section << "]\n"; 
                myvalues = new stringmap();
                myMap[ section ] = myvalues ;
            }
            else if ( is_parameter( line , param_key , param_value ) )
            {
                //cerr << "" << param_key << " = " << param_value << "\n";
                if( myvalues )
                {
                    (*myvalues)[ param_key ] = param_value ;
                }
                else
                {
                    cerr << "ERROR: The INI file " << filename << " appears to be broken ... parameters without a section?!?\n";
                    if( aFStream.is_open() ) aFStream.close();
                    return false;
                }
            }
        }

        if( aFStream.is_open() ) 
            aFStream.close();
        
        return true;
    }
    else
    {
        cerr << "ERROR: Can't open file '" << filename << "'\n";
    }
    return false;
}

bool INIreader::is_section( string& line , string& section_str )
{
    // Error in regex ?
    check_status( section_status );
    ::rtl::OUString target = ::rtl::OUString::createFromAscii( line.c_str() );
    
    uregex_setText( section_match, (UChar*) target.getStr(), target.getLength(), &section_status );
    check_status( section_status );

    if( uregex_find(section_match, 0, &section_status) )
    {
        check_status( section_status );

        ::std::vector<UChar> group( 1024 );
        int32_t size = uregex_group( section_match, 1, group.data(), group.capacity(), &section_status );
        if (group.capacity() < size) {
            group.reserve( size );
            size = uregex_group( section_match, 1, group.data(), group.capacity(), &section_status );
        }
        check_status( section_status );
        toStlString( group , section_str );
        
        return true;
    }
    return false;
}

bool INIreader::is_parameter( string& line , string& parameter_key , string& parameter_value )
{
    // Error in regex ?
    check_status( parameter_status );
    ::rtl::OUString target = ::rtl::OUString::createFromAscii( line.c_str() );
    
    uregex_setText(parameter_match, (const UChar*)target.getStr(), target.getLength(), &parameter_status);
    check_status( parameter_status );

    if( uregex_find(parameter_match, 0, &parameter_status) )
    {
        check_status( parameter_status );

        ::std::vector<UChar> group(1024);
        int32_t size = uregex_group(parameter_match, 1 , group.data(), group.capacity(), &parameter_status);
        if (group.capacity() < size) {
            group.reserve(size);
            uregex_group(parameter_match, 1 , group.data(), group.capacity(), &parameter_status);
        }
        check_status( parameter_status );
        toStlString( group , parameter_key );

        size = uregex_group(parameter_match, 2 , group.data(), group.capacity(), &parameter_status);
        if (group.capacity() < size) {
            group.reserve(size);
            size = uregex_group(parameter_match, 2 , group.data(), group.capacity(), &parameter_status);
        }
        check_status( parameter_status );
        toStlString( group , parameter_value );
        
        return true;
    }
    return false;
}

void INIreader::check_status( UErrorCode status )
{
    if( U_FAILURE( status) )
    {   
        cerr << "Error in or while using regex: " << u_errorName( status ) << "\n";
        exit(-1);
    }
}

void INIreader::toStlString( const ::std::vector<UChar>& str , string& stl_str )
{
    // convert to string
    ::rtl::OUString ouString( (sal_Unicode*) str.data(), str.capacity() );
    stl_str = string( ::rtl::OUStringToOString( ouString, RTL_TEXTENCODING_ASCII_US ).getStr() );
}

void INIreader::trim( string& str )
{
    string str1 = str.substr( 0 , str.find_last_not_of(' ') + 1 );
    str = str1.empty() ? str1 : str1.substr( str1.find_first_not_of(' ') );
}

}
