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
print "<html>";

# get current and last shown picture id
$sCurrPic = join( "", common::File_read( "currpic.txt" ) );

%aRequestMap = common::HTTP_getRequest();
$sLastPic = $aRequestMap{ "LastPic" };

print "<head>";
    print "<meta http-equiv=\"refresh\" content=\"" . $common::REFRESH_TIME . "; URL=poll.pl?LastPic=" . $sCurrPic . "\">";
print "</head>";

#' a new picture was chosen ?
if( $sLastPic ne $sCurrPic )
{
    # then show the new picture
    print "<body bgcolor=\"red\" onLoad=\"parent.frame1.location.href='./show.pl?" . $sCurrPic . "'\">";
}
else
{
    # otherwise do nothing
    print "<body bgcolor=\"green\">";
}

print "</body>";

print "</html>";
