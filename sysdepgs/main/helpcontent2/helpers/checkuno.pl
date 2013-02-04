#!/usr/bin/perl
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

$uno_hid = "/handbuch/WORKBENCH/helpcontent2/helpers/uno_hid.lst";
$uno_cmd = "/handbuch/WORKBENCH/helpcontent2/helpers/unocmds.txt";
$startdir = "/handbuch/WORKBENCH/helpcontent2/source/text/";

if (open HID, $uno_hid) {
	while (<HID>) {
		chomp;
		($shid,$nhid,$uno) = split ",";
		$hid{lc($uno)} = $shid;		
	}
	close HID;
} else {
	die "Cannot open $uno_hid: $!\n";
}
print scalar keys(%hid) . " entries read\n";

if (open UNO, $uno_cmd) {
	while (<UNO>) {
		chomp;
		($uno,$file) = split ";";
		#print "$file\n";
		if (defined $hid{lc($uno)}) {
			if (open XML, "$startdir$file") {
				undef $/;
				$xml = <XML>;
				$/ = "\n";
				close XML;
				if ($xml =~ /hid\/$hid{lc($uno)}/ig) {
					print "OK    $startdir$file\n";
				} else {
					print "MISS  $startdir$file\n";
					open XML, ">$startdir$file.org";
					print XML $xml;
					close XML;
					open XML, ">$startdir$file";
					$id = "bm_id" . int(rand(10000000));
					$xml =~ s/(<bookmark[^>]*branch=\"hid\/$uno\"[^>]*>)/<bookmark xml-lang=\"en-US\" branch=\"hid\/$hid{lc($uno)}\" id=\"$id\" localize=\"false\"\/><!-- HID added by script -->\n$1/ig;
					print XML $xml;
					close XML;
				}
			} else {
				print "Cannot open $startdir$file: $!\n";
			}
		}
	}
} else {
	die "Cannot open $uno_cmd: $!\n";
}



