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
#include "precompiled_codemaker.hxx"

#include <stdio.h>

#include "sal/main.h"

#include <codemaker/typemanager.hxx>
#include <codemaker/dependency.hxx>

#include "corbaoptions.hxx"
#include "corbatype.hxx"

using namespace rtl;

sal_Bool produceAllTypes(const OString& typeName,
						TypeManager& typeMgr,
						TypeDependency& typeDependencies,
						CorbaOptions* pOptions,
						sal_Bool bFullScope,
						FileStream& o,
						TypeSet* pAllreadyDumped,
						TypeSet* generatedConversion)

	throw( CannotDumpException )
{
	if (!produceType(typeName, typeMgr,	typeDependencies, pOptions, o, pAllreadyDumped, generatedConversion))
	{
		fprintf(stderr, "%s ERROR: %s\n",
				pOptions->getProgramName().getStr(),
				OString("cannot dump Type '" + typeName + "'").getStr());
		exit(99);
	}

	RegistryKey	typeKey = typeMgr.getTypeKey(typeName);
	RegistryKeyNames subKeys;

	if (typeKey.getKeyNames(OUString(), subKeys))
		return sal_False;

	OString tmpName;
	for (sal_uInt32 i=0; i < subKeys.getLength(); i++)
	{
		tmpName = OUStringToOString(subKeys.getElement(i), RTL_TEXTENCODING_UTF8);

		if (pOptions->isValid("-B"))
			tmpName = tmpName.copy(tmpName.indexOf('/', 1) + 1);
		else
			tmpName = tmpName.copy(1);

		if (bFullScope)
		{
			if (!produceAllTypes(tmpName, typeMgr, typeDependencies, pOptions, sal_True, o, pAllreadyDumped, generatedConversion))
				return sal_False;
		} else
		{
			if (!produceType(tmpName, typeMgr, typeDependencies, pOptions, o, pAllreadyDumped, generatedConversion))
				return sal_False;
		}
	}

	return sal_True;
}

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv)
{
	CorbaOptions options;

	try
	{
		if (!options.initOptions(argc, argv))
		{
			exit(1);
		}
	}
	catch( IllegalArgument& e)
	{
		fprintf(stderr, "Illegal option: %s\n", e.m_message.getStr());
		exit(99);
	}

	RegistryTypeManager typeMgr;
	TypeDependency		typeDependencies;

	if (!typeMgr.init(!options.isValid("-T"), options.getInputFiles()))
	{
		fprintf(stderr, "%s : init registries failed, check your registry files.\n", options.getProgramName().getStr());
		exit(99);
	}

	if (options.isValid("-B"))
	{
		typeMgr.setBase(options.getOption("-B"));
	}

	try
	{
		TypeSet generatedConversion;
		FileStream cppFile;
		OString outPath;
		if (options.isValid("-O"))
			outPath = options.getOption("-O");

		cppFile.open(outPath);

		if(!cppFile.isValid())
		{
			OString message("cannot open ");
			message += outPath + " for writing";
			throw CannotDumpException(message);
		}

		if (options.isValid("-H"))
		{
			OString corbaHeader = options.getOption("-H");

			cppFile << "#include <"
					<< corbaHeader
					<< ">\n\n";

			CorbaType::dumpDefaultHxxIncludes(cppFile);
			cppFile << "\n";
		}

		if (options.isValid("-T"))
		{
			OString tOption(options.getOption("-T"));

			OString typeName, tmpName;
			sal_Bool ret = sal_False;
			sal_Int32 nIndex = 0;
			do
			{
				typeName = tOption.getToken(0, ';', nIndex);

				sal_Int32 nPos = typeName.lastIndexOf( '.' );
				tmpName = typeName.copy( nPos != -1 ? nPos+1 : 0 );
				if (tmpName == "*")
				{
					// produce this type and his scope, but the scope is not recursively generated.
					if (typeName.equals("*"))
					{
						tmpName = "/";
					} else
					{
						tmpName = typeName.copy(0, typeName.lastIndexOf('.')).replace('.', '/');
						if ( tmpName.isEmpty() )
							tmpName = "/";
						else
							tmpName.replace('.', '/');
					}
					ret = produceAllTypes(tmpName, typeMgr, typeDependencies, &options, sal_False, cppFile, NULL, &generatedConversion);
				} else
				{
					// produce only this type
					ret = produceType(typeName.replace('.', '/'), typeMgr, typeDependencies, &options, cppFile, NULL, &generatedConversion);
				}

				if (!ret)
				{
					fprintf(stderr, "%s ERROR: %s\n",
							options.getProgramName().getStr(),
							OString("cannot dump Type '" + typeName + "'").getStr());
					exit(99);
				}
			} while( nIndex != -1 );
		} else
		{
			// produce all types
			if (!produceAllTypes("/", typeMgr, typeDependencies, &options, sal_True, cppFile, NULL, &generatedConversion))
			{
				fprintf(stderr, "%s ERROR: %s\n",
						options.getProgramName().getStr(),
						"an error occurs while dumping all types.");
				exit(99);
			}
		}

		cppFile << "namespace bonobobridge {\n"
				<< "const ConversionInfo* get_conversion_functions() {\n"
				<< "  static ConversionInfo allFunctions[" << generatedConversion.size()+1<< "] = {\n";

		for (TypeSet::iterator iter = generatedConversion.begin(); iter != generatedConversion.end(); iter++)
		{
			cppFile << "    {\"" << (*iter).getStr() << "\""
					<< ", &TC_" << (*iter).replace('/','_').getStr() << "_struct"
					<< ", sizeof(" << (*iter).replace('/','_').getStr() << ")"
					<< ", convert_b2u_" << (*iter).replace('/','_').getStr()
					<< ", convert_u2b_" <<  (*iter).replace('/','_').getStr()
					<< " },\n";
		}

		cppFile << "    {NULL, NULL, 0 , NULL, NULL} };\n"
				<< "  return allFunctions;\n"
				<< "}\n"
				<< "}; // namespace bonobobridge\n";

		cppFile.close();
	}
	catch( CannotDumpException& e)
	{
		fprintf(stderr, "%s ERROR: %s\n",
				options.getProgramName().getStr(),
				e.m_message.getStr());
		exit(99);
	}

	return 0;
}

/* vim: set noet sw=4 ts=4: */
