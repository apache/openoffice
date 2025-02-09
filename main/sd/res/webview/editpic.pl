#!/usr/bin/env perl
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

require "common.pl";

print "Content-type: text/html\n\n";
print "<html>\n";

print "<head>\n";
print "<title>editpic.pl</title>";
print "</head>\n";

print "<body>\n";
    $sCurrPic = join( "", common::File_read( "currpic.txt" ) );
    @aPictureArray = common::File_read( "picture.txt" );
    $nPictureArrayLen = @aPictureArray;
    print "<form action=\"savepic.pl\" method=get>\n";
        if( abs( $sCurrPic ) > 1 )
        {
            print "<input type=submit name=\"Auswahl\" value=\"-\"></input>\n";
        }
        else
        {
            print "<input type=button value=\" \"></input>\n";
        }
        print "<input type=text name=\"CurrPic\" value=\"";
        print $sCurrPic;
        print "\" size=3></input>\n";
        if( abs( $sCurrPic ) < ( $nPictureArrayLen - 1 ) )
        {
            print "<input type=submit name=\"Auswahl\" value=\"+\"></input>\n";
        }
        else
        {
            print "<input type=button value=\" \"></input>\n";
        }
        print "<input type=submit name=\"Auswahl\" value=\"$$2\"></input>\n";
    print "</form>\n";
print "</body>\n";

print "</html>\n";
