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

use File::Find;
use File::Basename;

# creates the help2 makefile for a given 
# directory including all help xhp files
# in that and the subordinate directories
# Only help files with the following 
#
# status values are included:
# PUBLISH DEPRECATED
#
# The following status values are 
# disregarded:
# DRAFT FINAL STALLED

$makefiletemplate = 'helpers/makefile.template';
$linkmakefiletemplate = 'helpers/linkmakefile.template';
$prj = "helpcontent2";
$helpdirprefix = "$prj/source/";

undef @sbasic;
undef @scalc;
undef @schart;
undef @sdraw;
undef @shared;
undef @simpress;
undef @smath;
undef @swriter;

$params = join "|", "",@ARGV,"";
($params =~ /-dir/) ? ($startdir = $params) =~ (s/.*-dir=([^\|]*).*$/$1/gs) : (($startdir = `pwd`) =~ s/\n//gs);
($startdir = $startdir."/source/text") if ($startdir =~ /$prj$/);
($params =~ /-linkdir/) ? ($linkdir = $params) =~ (s/.*-linkdir=([^\|]*).*$/$1/gs) : (($linkdir = `pwd`) =~ s/\n//gs);
($linkdir = $linkdir."/util") if ($linkdir =~ /$prj$/);
$recursive = $params =~ /-recursive/ || 0;
$all = $params =~ /-all/ || 0;

if (open TMPL, $makefiletemplate) {
	undef $/;
	$tmpl = <TMPL>;
	close TMPL;
} else {
	&terminate("Cannot open $makefiletemplate");
}

if (open LINKTMPL, $linkmakefiletemplate) {
	undef $/;
	$linktmpl = <LINKTMPL>;
	close LINKTMPL;
} else {
	&terminate("Cannot open $linkmakefiletemplate");
}

print "Start Directory: $startdir\n";
print "Link Directory : $linkdir\n";
print "Recursive      : ". ($recursive ? "yes" : "no") . "\n";
print "All files      : ". ($all ? "yes" : "no") . "\n";

if ($recursive) {	
	find(sub{push @dirs, $File::Find::name if (-d and ($File::Find::name!~/\/CVS/));},$startdir); 
} else { 
	push @dirs, $startdir;
}

#print join "\n", @dirs;
#die;

for $d(@dirs) {
	opendir DIR, $d;
	@files = grep {/xhp$/} readdir DIR;
	undef @files2;
	closedir DIR;
	
	($helpdir = $d) =~ s/.*\/$helpdirprefix/source\//gis;
	
	($package = $helpdir) =~ s/^source\///gi; 
	 
	($target = $package) =~ s/\//_/g; $target =~ s/_$//;
	($module = $package) =~ s/^\/*text\/([^\/]+)\/*.*$/$1/;

	for $f(@files) {
		($n,$p,$e) = fileparse($f,".xhp");
		if (not $all) {
			if (open XML, $d.'/'.$f) {
				undef $/;
				($status = <XML>) =~ s/.*<topic[^>]*status="([^"]*)"[^>]*>.*$/$1/gs;
				close XML;
			} else {
				die "Error: Cannot open ${d}/$f:$!\n";
			}
			if ($status =~ /(DEPRECATED|PUBLISH)/i) {
				($p2=$package) =~ s,/,\$/,gis;
				push @{$module}, $p2.'$/'.$n.'.xhp';
				push @files2, '    '.$n.'.xhp ';
			}
		} else {
			($p2=$package) =~ s,/,\$/,gis;
			push @{$module}, $p2.'$/'.$n.'.xhp';
			push @files2, '   '.$n.'.xhp ';
		}
	}
	
	printf "%s: %4d files, %4d included in makefile -> ",$d,scalar @files,scalar @files2;
		
	if (scalar @files2 > 0) { # don't write makefiles where there are no files to make
		($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
	
		$auth = "script";
		$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
		$prj = '..$/' x ((split "/", $helpdir) -1); $prj = $prj . "..";

		
		$xhpfiles = join "\\\n", sort @files2;

		($makefile = $tmpl) =~ s/%([^%]*)%/$$1/gise;
	
		if (open(MK, ">$d/makefile.mk")) {
			print MK $makefile;
			close MK;
			print "makefile created\n";
		} else {
			&terminate("Cannot write to ${d}/makefile.mk\n");
		}
	} else {
		print "NO makefile created\n";
	}
}

@sbasic = sort @sbasic;
@schart = sort @schart;
@scalc = sort @scalc;
@shared = sort @shared;
@sdraw = sort @sdraw;
@simpress = sort @simpress;
@smath = sort @smath;
@swriter = sort @swriter;

# now create the link makefiles

#----------------------------------
# sbasic
	$module = "sbasic";
	
	$linkaddedfiles = <<"LAF";
   -add $module.cfg \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   -add $module.tree \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAD
	
	$linklinkfiles = '';
	for (@sbasic) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@shared) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	
	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	&writelinkmakefile($module,$linkmakefile);

	
#-------------------------------------
# scalc
	$module = "scalc";
	
	$linkaddedfiles = <<"LAF";
   -add $module.cfg \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   -add $module.tree \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAD
	
	$linklinkfiles = '';
	for (@scalc)  { $linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@shared) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@schart) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}

	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	&writelinkmakefile($module,$linkmakefile);

#--------------------------------	
# schart
	$module = "schart";
	
	$linkaddedfiles = <<"LAF";
   -add $module.cfg \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   -add $module.tree \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAD
	
	$linklinkfiles = '';

	for (@shared) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@schart) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}


	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	&writelinkmakefile($module,$linkmakefile);

#------------------------------
# sdraw
	$module = "sdraw";
	
	$linkaddedfiles = <<"LAF";
   -add $module.cfg \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAD
	
	$linklinkfiles = '';
	for (@sdraw) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@simpress) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@shared) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@schart) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}

	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	&writelinkmakefile($module,$linkmakefile);
		
#-------------------------------------
# shared
 
	$module = "shared";
	
	$linkaddedfiles = <<"LAF";
   -add $module.tree \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip \\
   -add default.css  \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/default.css \\
   -add highcontrast1.css  \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/highcontrast1.css \\
   -add highcontrast2.css  \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/highcontrast2.css \\
   -add highcontrastwhite.css  \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/highcontrastwhite.css \\
   -add highcontrastblack.css  \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/highcontrastblack.css \\
   -add err.html  \$(COMMONMISC)\$/LANGUAGE\$/text\$/shared\$/05\$/err_html.xhp
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip \\
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/default.css \\
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/highcontrast1.css \\
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/highcontrast2.css \\
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/highcontrastwhite.css \\
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/highcontrastblack.css \\
   \$(COMMONMISC)\$/LANGUAGE\$/text\$/shared\$/05\$/err_html.xhp
LAD
	
	$linklinkfiles = '';

	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	&writelinkmakefile($module,$linkmakefile);

#-------------------------------
# simpress
	$module = "simpress";
	
	$linkaddedfiles = <<"LAF";
   -add $module.cfg \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   -add $module.tree \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAD
	
	$linklinkfiles = '';

	for (@sdraw) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@simpress) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@shared) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@schart) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	
	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	&writelinkmakefile($module,$linkmakefile);

#-------------------------------------
# smath
	$module = "smath";
	
	$linkaddedfiles = <<"LAF";
   -add $module.cfg \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   -add $module.tree \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAD
	
	$linklinkfiles = '';
	for (@smath) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@shared) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	
	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	&writelinkmakefile($module,$linkmakefile);

#-------------------------------
# swriter
	$module = "swriter";
	
	$linkaddedfiles = <<"LAF";
   -add $module.cfg \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   -add $module.tree \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   \$(COMMONMISC)\$/LANGUAGE\$/$module.tree \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAD
	
	$linklinkfiles = '';
	for (@swriter) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@shared) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	for (@schart) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	&writelinkmakefile($module,$linkmakefile);

#-------------------------------
# sdatabase
	$module = "sdatabase";
	
	$linkaddedfiles = <<"LAF";
   -add $module.cfg \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   -add $module.jar  \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAF
	
	$linkaddeddeps = <<"LAD";
   \$(PRJ)\$/source\$/auxiliary\$/LANGUAGE\$/$module.cfg \\
   \$(COMMONBIN)\$/xhp_${module}_LANGUAGE.zip
LAD
	
	$linklinkfiles = '';
	for (@shared) {	$linklinkfiles = $linklinkfiles . "   $_ \\\n";	}
	$auth = "script";
	$date = sprintf "%4d/%02d/%02d %02d:%02d:%02d",$year+1900,$mon,$mday,$hour,$min,$sec;
	$prj = '..$/..' ;
	
	($linkmakefile = $linktmpl) =~ s/%([^%]*)%/$$1/gise;
	
	# remove zip1 targets
	$linkmakefile =~ s,\n(ZIP1LIST=.*)\n,\nZIP1LIST=\$(LANGDIR)\$/text\$/shared\$/explorer\$/database\$/main.xhp\n,gi;
	&writelinkmakefile($module,$linkmakefile);
	
	
	
print "sbasic: $#sbasic\n";
print "scalc : $#scalc\n";
print "schart: $#schart\n";
print "sdraw : $#sdraw\n";
print "shared: $#shared\n";
print "simpr : $#simpress\n";
print "smath : $#smath\n";
print "swrit : $#swriter\n";
print "sdbase: $#sdatabase\n";

sub terminate {
	$err = shift;
	print "$err\n\n";
	$msg = <<"MSG";
createmakefile.pl -dir=[directory name] -linkdir=[directory name] [-recursive] [-all] 
  -dir        Directory to start
  -linkdir    Directory to write the link makefiles
  -recursive  Write makefiles recursively
  -all        include files with all status values
MSG
	die "$msg\n";
}


sub writelinkmakefile {
	$module = shift;
	$content = shift;
	if (open(MK, ">$linkdir/$module/makefile.mk")) {
		print MK $content;	
		close MK;
	} else {
		&terminate("Cannot write makefile to ${linkdir}/$module\n");
	}
}
