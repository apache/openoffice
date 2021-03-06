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

#include <precomp.h>
#include <all_toks.hxx>


// NOT FULLY DEFINED SERVICES
#include <cpp/ctokdeal.hxx>
#include "c_dealer.hxx"
#include "tokintpr.hxx"



namespace cpp {


void
Token::DealOut( ::TokenDealer & o_rDealer )
{
    // KORR_FUTURE HACK (casting to derivation cpp::TokenDealer)
    o_rDealer.AsDistributor()->Deal_CppCode(*this);
}


#define DEF_TOKEN_CLASS(name) \
void \
Tok_##name::Trigger( TokenInterpreter & io_rInterpreter ) const \
{ io_rInterpreter.Hdl_##name(*this); } \
INT16 \
Tok_##name::TypeId() const { return Tid_##name; } \
const char * \
Tok_##name::Text() const { return #name; }

#define DEF_TOKEN_CLASS_WITHTEXT(name, text ) \
void \
Tok_##name::Trigger( TokenInterpreter & io_rInterpreter ) const \
{ io_rInterpreter.Hdl_##name(*this); } \
INT16 \
Tok_##name::TypeId() const { return Tid_##name; } \
const char * \
Tok_##name::Text() const { return text; }


DEF_TOKEN_CLASS_WITHTEXT(Identifier,sText)
DEF_TOKEN_CLASS_WITHTEXT(Operator,sText)

DEF_TOKEN_CLASS(operator)
DEF_TOKEN_CLASS(class)
DEF_TOKEN_CLASS(struct)
DEF_TOKEN_CLASS(union)
DEF_TOKEN_CLASS(enum)
DEF_TOKEN_CLASS(typedef)
DEF_TOKEN_CLASS(public)
DEF_TOKEN_CLASS(protected)
DEF_TOKEN_CLASS(private)
DEF_TOKEN_CLASS(template)
DEF_TOKEN_CLASS(virtual)
DEF_TOKEN_CLASS(friend)
DEF_TOKEN_CLASS_WITHTEXT(Tilde,"~")
DEF_TOKEN_CLASS(const)
DEF_TOKEN_CLASS(volatile)
DEF_TOKEN_CLASS(extern)
DEF_TOKEN_CLASS(static)
DEF_TOKEN_CLASS(mutable)
DEF_TOKEN_CLASS(register)
DEF_TOKEN_CLASS(inline)
DEF_TOKEN_CLASS(explicit)
DEF_TOKEN_CLASS(namespace)
DEF_TOKEN_CLASS(using)
DEF_TOKEN_CLASS(throw)
DEF_TOKEN_CLASS_WITHTEXT(SwBracket_Left,"{")
DEF_TOKEN_CLASS_WITHTEXT(SwBracket_Right,"}")
DEF_TOKEN_CLASS_WITHTEXT(ArrayBracket_Left,"[")
DEF_TOKEN_CLASS_WITHTEXT(ArrayBracket_Right,"]")
DEF_TOKEN_CLASS_WITHTEXT(Bracket_Left,"(")
DEF_TOKEN_CLASS_WITHTEXT(Bracket_Right,")")
DEF_TOKEN_CLASS_WITHTEXT(DoubleColon,"::")
DEF_TOKEN_CLASS_WITHTEXT(Semicolon,";")
DEF_TOKEN_CLASS_WITHTEXT(Comma,",")
DEF_TOKEN_CLASS_WITHTEXT(Colon,":")
DEF_TOKEN_CLASS_WITHTEXT(Assign,"=")
DEF_TOKEN_CLASS_WITHTEXT(Less,"<")
DEF_TOKEN_CLASS_WITHTEXT(Greater,">")
DEF_TOKEN_CLASS_WITHTEXT(Asterix,"*")
DEF_TOKEN_CLASS_WITHTEXT(AmpersAnd,"&")
DEF_TOKEN_CLASS_WITHTEXT(Ellipse,"...")
DEF_TOKEN_CLASS(typename)

DEF_TOKEN_CLASS_WITHTEXT(DefineName,sText)
DEF_TOKEN_CLASS_WITHTEXT(MacroName,sText)
DEF_TOKEN_CLASS_WITHTEXT(MacroParameter,sText)
// DEF_TOKEN_CLASS_WITHTEXT(PreProDefinition,sText)

void
Tok_PreProDefinition::Trigger( TokenInterpreter &   io_rInterpreter ) const
{ io_rInterpreter.Hdl_PreProDefinition(*this); }

INT16
Tok_PreProDefinition::TypeId() const { return Tid_PreProDefinition; }

const char *
Tok_PreProDefinition::Text() const
{
    return sText;
}



DEF_TOKEN_CLASS_WITHTEXT(BuiltInType,sText)
DEF_TOKEN_CLASS_WITHTEXT(TypeSpecializer,sText)
DEF_TOKEN_CLASS_WITHTEXT(Constant,sText)

const char *
Tok_UnblockMacro::Text() const
{
    return sMacroName;
}

void
Tok_UnblockMacro::DealOut( ::TokenDealer & o_rDealer )
{
    // KORR_FUTURE HACK (casting to derivation cpp::TokenDealer)
    o_rDealer.AsDistributor()->Deal_Cpp_UnblockMacro(*this);
}

}   // namespace cpp
