# *************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
# *************************************************************
BEGIN {
	ExitRet = 0
	outfiles = 4
	if ( ARGC != (outfiles+2) ) {
		printf( "Syntax: AWK ... Input.RC IconBitmapCursorOut.RC Output2.RC " )
		printf( "Output3.RC Output4.RC\n" )
		ExitRet = 1
		exit ExitRet
	}
	printf( "splitting %s => %s, %s, %s, %s\n", ARGV[1], ARGV[2], ARGV[3], ARGV[4], ARGV[5] )
	IcoBitCu = ARGV[2]
	rc2 = ARGV[3]
	rc3 = ARGV[4]
	rc4 = ARGV[5]
	printf("") >IcoBitCu
	printf("") >rc2
	printf("") >rc3
	printf("") >rc4
	ARGC -= outfiles
	out = 2
}

{
	if ( $2 == "ICON" || $2 == "BITMAP" || $2 == "CURSOR" ) {
		print >>IcoBitCu
		if ( $2 == "ICON" )
			printf("i")
		else if ( $2 == "BITMAP" )
			printf("b")
		else if ( $2 == "CURSOR" )
			printf("c")
		else
			printf("?")
	}
	else {
		if ( 2 == out )
			print >>rc2
		else if ( 3 == out )
			print >>rc3
		else if ( 4 == out )
			print >>rc4
		if ( "END" == $0 ) {
			printf(".")
			out++
			if ( out > outfiles )
				out = 2
		}
	}
}

END {
	if ( ExitRet == 0 )
		printf(" :-)\n")
	else
		printf(" :-(\n")
	exit ExitRet
}
