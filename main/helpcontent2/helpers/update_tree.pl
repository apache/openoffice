:
eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;
#**************************************************************
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
#**************************************************************



use Cwd 'abs_path';
use File::Find;
use File::Copy qw/cp mv/;
use File::Basename;
use Benchmark;

$t0 = new Benchmark;
# update the tree files in <platform>/misc/*

$| = 1;

my $prj = $ENV{ENVPRJ};

my $inpath = $ENV{INPATH};
terminate() if ( ! defined $inpath );

my $destpath = $inpath;
my $with_lang = $ENV{WITH_LANG};


# Always use / directory separators
$prj =~ s/\\/\//g if defined($prj);
$inpath =~ s/\\/\//g;
$destpath =~ s/\\/\//g;


if ( ! defined $prj ) {
# do someting that works for manual call
    ($scriptname = `pwd`) =~ s/\n/\/$0/;
    ($tree_src = $scriptname) =~ s/\/update_tree.pl/\/..\/source\/auxiliary/;
    ($tree_dest = $scriptname) =~ s/\/update_tree.pl/\/..\/$destpath\/misc/;
    ($source_dir = $scriptname) =~ s/\/update_tree.pl/\/..\/source/;
    ($source_dir_xhp = $scriptname) =~ s/\/update_tree.pl/\/..\/source/;

    if ( defined $ENV{TRYSDF} || defined $ENV{LOCALIZESDF} )
    {
        if( defined $ENV{LOCALIZATION_FOUND} && $ENV{LOCALIZATION_FOUND} eq "YES" )
        {
            $source_dir = $ENV{TRYSDF};
        }
        else 
        {
            $source_dir = $ENV{LOCALIZESDF};
        }
        $source_dir =~ s/\/auxiliary\/localize.sdf$// ;
    }
    #else {die "ERROR: The env variables TRYSDF LOCALIZATION_FOUND LOCALIZESDF not found ... something is wrong!\n";}


    $treestrings = "$source_dir/text/shared/tree_strings.xhp";
} else {
    $tree_src = "$prj\/source\/auxiliary";
    $tree_dest = "$prj\/$destpath\/misc";
    $source_dir = "$prj\/source";
    $source_dir_xhp = "$prj\/source";
    $treestrings = "$source_dir/text/shared/tree_strings.xhp";

    if( defined $ENV{LOCALIZATION_FOUND} && $ENV{LOCALIZATION_FOUND} eq "YES" )
    {
        $source_dir = $ENV{TRYSDF};
    }
    else 
    {
        $source_dir = $ENV{LOCALIZESDF};
    }
    $source_dir =~ s/\/auxiliary\/localize.sdf$// ;
    #else {die "ERROR: The env variables TRYSDF LOCALIZATION_FOUND LOCALIZESDF not found ... something is wrong!\n";}

}

# Get the English tree files as master
#-------------------------------
# Update English from xhp
#-------------------------------
&do_english;
#-------------------------------
# Update localizations from sdf
#-------------------------------

@langs = split /\s+/, $with_lang;
&read_loc;
print "################\nUpdating the treefiles for @langs \n";
for $l(@langs) {
    if ($l ne "en-US") {
        &do_lang($l);
    }
}
print "jan slutter\n";

#-------------------------------
#
$t1 = new Benchmark;
$td = timediff($t1, $t0);
print timestr($td),"\n";

####################
# SUBS
####################
sub terminate {
    $err = shift;
    print "$err\n\n";
    $msg = <<"MSG";

update_tree.pl
   all languages in WITH_LANG are processed. WITH_LANG=ALL is
   not supported in manual calls.

   Updates the *.tree files.
   At first, the English file is updated based on the English
   help topic titles as read from the help files. Then, the
   localized tree files are written based on the English tree
   file and the localized help topic titles.

   Requires a valid SO/OOo environment.
MSG
   print "$msg\n";
   exit( -1 );
   # die "$msg\n";
}

#---------------------------------------------------

sub do_english {
    print "Processing en-US\n";
    undef %helpsection; undef %node;
    &readtreestrings;
    &gettreefiles;
    &processtreefiles('en-US');    
}

#---------------------------------------------------
sub do_lang {
    $lng = shift;
    print "\n---------------------------------------------------\nProcessing $lng\n";
    &processtreefiles($lng);
    print "\n";
}

#---------------------------------------------------
sub readtreestrings {
    print "Reading tree strings for en-US...";
    if (open TREE, $treestrings) {
        while (<TREE>) {
            chomp;
            s/<\/*help:productname>//gis;    
            if (/help_section/) {
                s/^\s*<.*help_section//;
                s/<\/.*$//;
                ($id = $_) =~ s/^.*id=&quot;(\d+)&quot;.*$/$1/;
                ($title = $_) =~ s/^.*title=&quot;(.*)&quot;.*$/$1/;
                $helpsection{$id} = $title; 
            }

            if (/node id=/) {
                s/^\s*<.*node //;
                s/<\/.*$//;
                ($id = $_) =~ s/^.*id=&quot;(\d+)&quot;.*$/$1/;
                ($title = $_) =~ s/^.*title=&quot;(.*)&quot;.*$/$1/;
                $node{$id} = $title;
            }
        }
        close TREE;
    } else {
	    &terminate("Error opening $treestrings");
    }
    print "done\n";
}

#------------------------------------
sub gettreefiles {
    # Read the tree files from the directory
    # this list is also used for all foreign languages
    print "Reading tree files...";
    if (opendir ENUS, "$tree_src") {
        @treeviews = grep /\.tree/, readdir ENUS;
        closedir ENUS;
    } else {
        &terminate("Cannot open directory $tree_src");
    }
    print "done\n";
}

#------------------------------------
sub processtreefiles {
    $lng = shift;
	use File::Temp qw/ tempfile /; 
	use File::Spec;

	for $tv(@treeviews) {
        print "\nProcessing $tv\n";
        @lines = &readtv("$tree_src/$tv");
        print "Read ".scalar @lines." lines\n";
        for $l(@lines) {
            if ($l =~ /topic/) {
                ($id = $l) =~ s/^.*id="([^"]*)".*$/$1/gis;
                ($module = $id) =~ s/^([^\/]*).*$/$1/;
                $id =~ s/^.*?\///;
                $file = "$source_dir_xhp/$id";

                if ($lng eq 'en-US') { # english comes from the file
                    if (open F,$file) {
                        print ".";
                        undef $/; $cnt = <F>; close F;
                        $cnt =~ s/^.*<title[^>]+id="tit"[^>]*>([^<]*)<\/title>.*$/$1/gis;
                        $cnt =~ s/&apos;/\'/gis; $cnt =~ s/&amp;/+/gis;
                        $cnt =~ s/&quot;/\'/gis; $cnt =~ s/&/+/gis;
                        $l = "<topic id=\"$module/$id\">$cnt</topic>\n";
                    } else {
                        print "!";
                        $l = "<!-- removed $module/$id -->\n";
                    }
                } else { # localized comes from the localize sdf
                    #print "\nid: $id";
                    if (defined($loc_title{$lng}->{$id})) {
                        print ".";
                        $l = "<topic id=\"$module/$id\">$loc_title{$lng}->{$id}</topic>\n";
                    } else {
                        print "!";
                    }
                }
            }
    
            if ($l =~/<node/) {
                ($id = $l) =~ s/^.*id="(\d+)".*$/$1/gis;
                if ($lng eq 'en-US') {
                    if (defined($node{$id})) {
                        $l =~ s/title="(.*)"/title="$node{$id}"/;
                    } else {
                        $l =~ s/title="(.*)"/title="NOTFOUND:$id"/;
                    }
                } else {
                    if (defined($node{$lng}->{$id})) {
                        $l =~ s/title="(.*)"/title="$node{$lng}->{$id}"/;
                    }
                }
            }
    
            if ($l =~/<help_section/) {
                ($id = $l) =~ s/^.*id="(\d+)".*$/$1/gis;
                if ($lng eq 'en-US') {
                    if (defined($helpsection{$id})) {
                        $l =~ s/title="(.*)"/title="$helpsection{$id}"/;
                    } else {
                        print "#";
                        $l =~ s/title="(.*)"/title="NOTFOUND:$id"/;
                    }
                } else {
                    if (defined($helpsection{$lng}->{$id})) {
                        $l =~ s/title="(.*)"/title="$helpsection{$lng}->{$id}"/;
                    }
                }
            }
        }
 		if ( ! -d "$tree_dest/$lng" ) { 
	        mkdir "$tree_dest/$lng" or die "\nCouldn't create directory \"$tree_dest/$lng\"";
		}
		my $treeoutdir = "$tree_dest/$lng";
		my $tmpname_template=$tv."_XXXXX";
		my ( $treetmpfilehandle, $treetmpfile ) = tempfile($tmpname_template , DIR => File::Spec->tmpdir() );
		close $treetmpfilehandle ;
        if (open TV, ">$treetmpfile") {
            for $line(@lines) { 
                $line =~ s/\$\[officename\]/%PRODUCTNAME/g;
                $line =~ s/\$\[officeversion\]/%PRODUCTVERSION/g;
                print TV $line;    
            }  
            close TV;
			chmod 0664, $treetmpfile or &terminate("Cannot change rights on $treetmpfile");
			if( $^O eq 'MSWin32' )
			{
				$tree_dest =~ s/\//\\/g ;
				unlink "$tree_dest\\$lng\\$tv" ;
                mv $treetmpfile , "$tree_dest\\$lng\\$tv" or &terminate("Cannot mv $treetmpfile to $tree_dest\\$lng\\$tv" );
			}
			else
			{
				unlink "$tree_dest/$lng/$tv" ;
                my $ret=mv $treetmpfile , "$tree_dest/$lng/$tv$inpath" or &terminate("Cannot write to $tree_dest/$lng/$tv$inpath - Error $!");
                my $ret=mv "$tree_dest/$lng/$tv$inpath" , "$tree_dest/$lng/$tv" or &terminate("Cannot write to $tree_dest/$lng/$tv - Error $!");
			}
      } else {
            &terminate("Cannot write to $tvout");
        }
    }
}

#------------------------------------
sub readtv {
    my $f = shift;
    if (open TV, $f) {
        $/ = "\n";
        my @l = <TV>;
        close TV;
        return @l;
    } else { 
        &terminate("Error opening $f"); 
    }
}

#------------------------------------
# read entries form localize.sdf files
#------------------------------------
sub read_loc {
    print "\n\nReading localized titles...";
    $/ = "\n";
    my $path = "$source_dir/text";
    print " in $source_dir/text\n";
    @files = `find $source_dir/text -name localize.sdf`;
    for my $fname (@files) {
        $FS = '\t';
        print ".";
        open(LOCALIZE_SDF, $fname) || die 'Cannot open "localize.sdf".'."$fname";
        while (<LOCALIZE_SDF>) {
            my $sdf_line = $_;
	    my ($Fld1,$file,$Fld3,$Fld4,$id,$Fld6,$Fld7,$Fld8,$Fld9,$lang,$text) = split($FS, $sdf_line , 12);
            next if ( $Fld1 =~ /^#/); 
	    if ($id eq 'tit') {
                #strip filename
                $file =~ s/.*text\\/text\\/g;
                #convert \ to / in filename
                $file =~ s/\\/\//g;
                #fpe: i46823 - need to encode &s, added encoding
                $text =~ s/&(?!amp;)/&amp;/g;
                # add entry to the hash
                $loc_title{$lang}->{$file} = $text;
            }
            if ($file =~ /tree_strings.xhp/) {
                #strip filename
                $file =~ s/.*text/text/g;
                #convert \ to / in filename
                $file =~ s/\\/\//g;
                if ($text =~ /^<help_section/) {
                    #example: <help_section application="scalc" id="08" title="表計算ドキュメント"> 
                    my ($fld1,$app,$fld3,$id,$fld5,$sec_title) = split('"', $text, 7);
                    #fpe: i46823 - need to encode &s, added encoding
                    if( defined $sec_title )
		            {
		    	        $sec_title =~ s/&(?!amp;)/&amp;/g;
		    	        #unquot \<item ... /\>
			            terminate( "\n\nERROR: Bad string in file '$fname' will cause invalid xml tree file \n---\n'$sdf_line'\n---\nPlease remove or replace < = '&lt;' and  > = '&gt;' within the title attribute '$sec_title'\n") , if( $sec_title =~ /[\<\>]/ );
		    	        $helpsection{$lang}->{$id} = $sec_title; 
		            }
                } elsif ($text =~/<node id=/) {
                    # example: <node id="0205" title="Tabelas em documentos de texto"> 
                    # BEWARE: title may contain escaped '"' so only match " not preceded by \
                    # using a zero‐width negative look‐behind assertion.
                    my ($fld1,$id,$fld3,$node_title,$Fld5) = split(/(?<!\\)"/, $text, 5);
                    #fpe: i46823 - need to encode &s, added encoding
		            if( defined $node_title )
		            {
			            $node_title =~ s/&(?!amp;)/&amp;/g;
		   	            terminate( "\n\nERROR: Bad string in '$fname' will cause invalid xml tree file \n---\n'$sdf_line'\n---\nPlease remove or replace < = '&lt;' and  > = '&gt;' within the title attribute '$node_title'\n") , if( $node_title =~ /[\<\>]/ );
		            }
		            $node{$lang}->{$id} = $node_title;
                }
            }
        }
        close LOCALIZE_SDF;
    }
    # statistics
    $total_elements=0;
    foreach $lang (keys %loc_title) {
        $no_elements = scalar(keys(%{$loc_title{$lang}}));
        push(@langstat, "$lang:\t ".$no_elements." matches\n");
        $total_elements += $no_elements;
    }
    print "\ndone reading a total of ".$total_elements." localized titles for ".scalar(keys(%loc_title))." languages from ".scalar @files ." files\n";
    print sort(@langstat);
}

