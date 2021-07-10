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



package installer::epmfile;

use Cwd;
use installer::converter;
use installer::existence;
use installer::exiter;
use installer::files;
use installer::globals;
use installer::logger;
use installer::packagelist;
use installer::pathanalyzer;
use installer::remover;
use installer::scriptitems;
use installer::systemactions;
use installer::worker;
use POSIX qw(uname);

############################################################################
# Reading the package map to find Solaris package names for 
# the corresponding abbreviations
############################################################################

sub read_packagemap
{
	my ($allvariables, $includepatharrayref, $languagesarrayref) = @_;
	
	my $packagemapname = "";
	if ( $allvariables->{'PACKAGEMAP'} ) { $packagemapname = $allvariables->{'PACKAGEMAP'}; }
	if ( $packagemapname eq "" ) { installer::exiter::exit_program("ERROR: Property PACKAGEMAP must be defined!", "read_packagemap"); }

    $installer::logger::Lang->printf("\n");
    $installer::logger::Lang->printf("\n");
    $installer::logger::Lang->printf("nCollected abbreviations and package names:\n");

	# Can be a comma separated list. All files have to be found in include paths
	my $allpackagemapnames = installer::converter::convert_stringlist_into_hash(\$packagemapname, ",");
	foreach my $onepackagemapname ( keys %{$allpackagemapnames} )
	{
		my $packagemapref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$onepackagemapname, $includepatharrayref, 0);

		if ( $$packagemapref eq "" ) { installer::exiter::exit_program("ERROR: Could not find package map file \"$onepackagemapname\" (propery PACKAGEMAP)!", "read_packagemap"); }

		my $packagemapcontent = installer::files::read_file($$packagemapref);

		for ( my $i = 0; $i <= $#{$packagemapcontent}; $i++ )
		{
			my $line = ${$packagemapcontent}[$i];
			
			if ( $line =~ /^\s*\#/ ) { next; }  # comment line
			if ( $line =~ /^\s*$/ ) { next; }  # empty line
		
			if ( $line =~ /^\s*(.*?)\t(.*?)\s*$/ )
			{
				my $abbreviation = $1;
				my $packagename = $2;
				installer::packagelist::resolve_packagevariables(\$abbreviation, $allvariables, 0);
				installer::packagelist::resolve_packagevariables(\$packagename, $allvariables, 0);
				
				# Special handling for language strings %LANGUAGESTRING

				if (( $abbreviation =~ /\%LANGUAGESTRING/ ) || ( $packagename =~ /\%LANGUAGESTRING/ ))
				{
					foreach my $onelang ( @{$languagesarrayref} )
					{
                        my $local_abbreviation = $abbreviation;
                        my $local_packagename = $packagename;
						$local_abbreviation =~ s/\%LANGUAGESTRING/$onelang/g;
						$local_packagename =~ s/\%LANGUAGESTRING/$onelang/g;

                        # Logging all abbreviations and packagenames
                        $installer::logger::Lang->printf("%s : %s : %s\n",
                            $onelang,
                            $local_abbreviation,
                            $local_packagename);

						if ( exists($installer::globals::dependfilenames{$local_abbreviation}) )
						{
							installer::exiter::exit_program("ERROR: Packagename for  Solaris package $local_abbreviation already defined ($installer::globals::dependfilenames{$local_abbreviation})!", "read_packagemap");
						}
						else
						{
							$installer::globals::dependfilenames{$local_abbreviation} = $local_packagename;
						}
					}
				}
				else
				{
                    # Logging all abbreviations and packagenames
                    $installer::logger::Lang->printf("%s : %s\n", $abbreviation, $packagename);

					if ( exists($installer::globals::dependfilenames{$abbreviation}) )
					{
						installer::exiter::exit_program("ERROR: Packagename for  Solaris package $abbreviation already defined ($installer::globals::dependfilenames{$abbreviation})!", "read_packagemap");
					}
					else
					{
						$installer::globals::dependfilenames{$abbreviation} = $packagename;
					}
				}
			}
			else
			{
				my $errorline = $i + 1;
				installer::exiter::exit_program("ERROR: Wrong syntax in file \"$onepackagemapname\" (line $errorline)!", "read_packagemap");
			}
		}
	}

    $installer::logger::Lang->print("\n");
    $installer::logger::Lang->print("\n");
}

############################################################################
# The header file contains the strings for the epm header in all languages
############################################################################

sub get_string_from_headerfile
{
	my ($searchstring, $language, $fileref) = @_;
	
	my $returnstring  = "";
	my $onestring  = "";
	my $englishstring  = "";
	my $foundblock = 0;
	my $foundstring = 0;
	my $foundenglishstring = 0;
	my $englishidentifier = "01";
	
	$searchstring = "[" . $searchstring . "]";
	
	for ( my $i = 0; $i <= $#{$fileref}; $i++ )
	{
		my $line = ${$fileref}[$i];
		
		if ( $line =~ /^\s*\Q$searchstring\E\s*$/ )
		{
			$foundblock = 1;
			my $counter = $i + 1;
			
			$line = ${$fileref}[$counter];

			# Beginning of the next block oder Dateiende

			while ((!($line =~ /^\s*\[\s*\w+\s*\]\s*$/ )) && ( $counter <= $#{$fileref} ))
			{
				if ( $line =~ /^\s*\Q$language\E\s+\=\s*\"(.*)\"\s*$/ )
				{
					$onestring = $1;
					$foundstring = 1;
					last;					
				}

				if ( $line =~ /^\s*\Q$englishidentifier\E\s+\=\s*\"(.*)\"\s*$/ )
				{
					$englishstring = $1;
					$foundenglishstring = 1;					
				}
				
				$counter++;
				$line = ${$fileref}[$counter];
			}
		}
	}	
	
	if ( $foundstring )
	{
		$returnstring = $onestring;
	}
	else
	{
		if ( $foundenglishstring )
		{
			$returnstring = $englishstring;
		}
		else
		{
			installer::exiter::exit_program("ERROR: No string found for $searchstring in epm header file (-h)", "get_string_from_headerfile");
		}
	}
	
	return \$returnstring;
}

##########################################################
# Filling the epm file with directories, files and links
##########################################################

sub put_directories_into_epmfile
{
	my ($directoriesarrayref, $epmfileref, $allvariables, $packagerootpath) = @_;
	my $group = "bin";
	
	if ( $installer::globals::islinuxbuild )
	{
		$group = "root";
	}
	
	for ( my $i = 0; $i <= $#{$directoriesarrayref}; $i++ )
	{
		my $onedir = ${$directoriesarrayref}[$i];
		my $dir = "";

		if ( $onedir->{'Dir'} ) { $dir = $onedir->{'Dir'}; }
		
		# if (!($dir =~ /\bPREDEFINED_/ ))
		if ((!($dir =~ /\bPREDEFINED_/ )) || ( $dir =~ /\bPREDEFINED_PROGDIR\b/ ))
		{
			my $hostname = $onedir->{'HostName'};
			
			# not including simple directory "/opt"
			# if (( $allvariables->{'SETSTATICPATH'} ) && ( $hostname eq $packagerootpath )) { next; }
						
			my $line = "d 755 root $group $hostname -\n";
			
			push(@{$epmfileref}, $line)  
		} 
	}
}

sub put_files_into_epmfile
{
	my ($filesinproductarrayref, $epmfileref) = @_;

	for ( my $i = 0; $i <= $#{$filesinproductarrayref}; $i++ )
	{
		my $onefile = ${$filesinproductarrayref}[$i];
		
		my $unixrights = $onefile->{'UnixRights'};
		my $destination = $onefile->{'destination'};
		my $sourcepath = $onefile->{'sourcepath'};

		my $filetype = "f";
		my $styles = "";
		if ( $onefile->{'Styles'} ) { $styles = $onefile->{'Styles'}; }
		if ( $styles =~ /\bCONFIGFILE\b/ ) { $filetype = "c"; }

		my $group = "bin";
		if ( $installer::globals::islinuxbuild ) { $group = "root"; }
		if (( $installer::globals::issolarisbuild ) && ( $onefile->{'SolarisGroup'} )) { $group = $onefile->{'SolarisGroup'}; }

		my $line = "$filetype $unixrights root $group $destination $sourcepath\n";
			
		push(@{$epmfileref}, $line);
	}
}

sub put_links_into_epmfile
{
	my ($linksinproductarrayref, $epmfileref) = @_;
	my $group = "bin";
	
	if ( $installer::globals::islinuxbuild )
	{
		$group = "root";
	}
	
	
	for ( my $i = 0; $i <= $#{$linksinproductarrayref}; $i++ )
	{
		my $onelink = ${$linksinproductarrayref}[$i];
		my $destination = $onelink->{'destination'};
		my $destinationfile = $onelink->{'destinationfile'};
						
		my $line = "l 000 root $group $destination $destinationfile\n";
			
		push(@{$epmfileref}, $line)  
	}
}

sub put_unixlinks_into_epmfile
{
	my ($unixlinksinproductarrayref, $epmfileref) = @_;
	my $group = "bin";
	
	if ( $installer::globals::islinuxbuild ) { $group = "root";	}

	for ( my $i = 0; $i <= $#{$unixlinksinproductarrayref}; $i++ )
	{
		my $onelink = ${$unixlinksinproductarrayref}[$i];
		my $destination = $onelink->{'destination'};
		my $target = $onelink->{'Target'};
						
		my $line = "l 000 root $group $destination $target\n";
			
		push(@{$epmfileref}, $line)  
	}
}

###############################################
# Creating epm header file
###############################################

sub create_epm_header
{
	my ($variableshashref, $filesinproduct, $languagesref, $onepackage) = @_;  

	my @epmheader = ();
	
	my ($licensefilename, $readmefilename);
	
	my $foundlicensefile = 0;
	my $foundreadmefile = 0;

	my $line = "";
	my $infoline = "";
		
	# %product Apache OpenOffice Software
	# %version 2.0
	# %description A really great software
	# %copyright 1999-2003 by OOo
	# %vendor Apache OpenOffice project
	# %license /test/replace/01/LICENSE01
	# %readme /test/replace/01/README01
	# %requires foo
	# %provides bar

	# The first language in the languages array determines the language of license and readme file
	
	my $searchlanguage = ${$languagesref}[0];

	# using the description for the %product line in the epm list file
	
	my $productnamestring = $onepackage->{'description'};
	installer::packagelist::resolve_packagevariables(\$productnamestring, $variableshashref, 0);
	if ( $variableshashref->{'PRODUCTEXTENSION'} ) { $productnamestring = $productnamestring . " " . $variableshashref->{'PRODUCTEXTENSION'}; }
	
	$line = "%product" . " " . $productnamestring . "\n";
	push(@epmheader, $line);

	# Determining the release version 
	# This release version has to be listed in the line %version : %version versionnumber releasenumber
	
	# if ( $variableshashref->{'PACKAGEVERSION'} ) { $installer::globals::packageversion = $variableshashref->{'PACKAGEVERSION'}; }
	if ( ! $onepackage->{'packageversion'} ) { installer::exiter::exit_program("ERROR: No packageversion defined for package: $onepackage->{'module'}!", "create_epm_header"); }
	$installer::globals::packageversion = $onepackage->{'packageversion'};
	installer::packagelist::resolve_packagevariables(\$installer::globals::packageversion, $variableshashref, 0);
	if ( $variableshashref->{'PACKAGEREVISION'} ) { $installer::globals::packagerevision = $variableshashref->{'PACKAGEREVISION'}; }

	$line = "%version" . " " . $installer::globals::packageversion . "\n";
	push(@epmheader, $line);

	$line = "%release" . " " . $installer::globals::packagerevision . "\n";
	if ( $installer::globals::islinuxrpmbuild ) { $line = "%release" . " " . $installer::globals::buildid . "\n"; }
	push(@epmheader, $line);
	
	# Description, Copyright and Vendor are multilingual and are defined in
	# the string file for the header file ($headerfileref)

	my $descriptionstring = $onepackage->{'description'};
	installer::packagelist::resolve_packagevariables(\$descriptionstring, $variableshashref, 0);
	$line = "%description" . " " . $descriptionstring . "\n";
	push(@epmheader, $line);

	my $copyrightstring = $onepackage->{'copyright'};
	installer::packagelist::resolve_packagevariables(\$copyrightstring, $variableshashref, 0);
	$line = "%copyright" . " " . $copyrightstring . "\n";
	push(@epmheader, $line);

	my $vendorstring = $onepackage->{'vendor'};
	installer::packagelist::resolve_packagevariables(\$vendorstring, $variableshashref, 0);
	$line = "%vendor" . " " . $vendorstring . "\n";
	push(@epmheader, $line);

	# License and Readme file can be included automatically from the file list
	
	if ( $installer::globals::iswindowsbuild || $installer::globals::isos2 )
	{
		$licensefilename = "LICENSE.txt";
		$readmefilename = "readme.txt";
	}
	else
	{
		$licensefilename = "LICENSE";
		$readmefilename = "README";	
	}
	
	if (( $installer::globals::languagepack )	# in language packs the files LICENSE and README are removed, because they are not language specific
		|| ( $variableshashref->{'NO_README_IN_ROOTDIR'} ))
	{
		if ( $installer::globals::iswindowsbuild || $installer::globals::isos2 )
		{
			$licensefilename = "LICENSE.txt";
			$readmefilename = "readme_$searchlanguage.txt";
		}
		else
		{
			$licensefilename = "LICENSE";
			$readmefilename = "README_$searchlanguage";
		}
	}

	my $license_in_package_defined = 0;
	
	if ( $installer::globals::issolarisbuild )
	{
		if ( $onepackage->{'solariscopyright'} )
		{
			$licensefilename = $onepackage->{'solariscopyright'};
			$license_in_package_defined = 1;
		}
	}
	
	# Process for Linux packages, in which only a very basic license file is
	# included into the package.

	if ( $installer::globals::islinuxbuild )
	{
		if ( $variableshashref->{'COPYRIGHT_INTO_LINUXPACKAGE'} )
		{
			$licensefilename = "linuxcopyrightfile";
			$license_in_package_defined = 1;
		}
	}	
	# searching for and readme file
	
	for ( my $i = 0; $i <= $#{$filesinproduct}; $i++ )
	{
		my $onefile = ${$filesinproduct}[$i];
		my $filename = $onefile->{'Name'};
		if ( $filename eq $readmefilename )
		{
			$foundreadmefile = 1;
			$line = "%readme" . " " . $onefile->{'sourcepath'} . "\n";
			push(@epmheader, $line);
			last;
		}
	}

	# searching for and license file

	if ( $license_in_package_defined )
	{
		my $fileref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$licensefilename, "" , 0);

		if ( $$fileref eq "" ) { installer::exiter::exit_program("ERROR: Could not find license file $licensefilename (A)!", "create_epm_header"); }

		# Special handling to add the content of the file "license_en-US" to the solaris copyrightfile. But not for all products
		
		if (( $installer::globals::issolarispkgbuild ) && ( ! $variableshashref->{'NO_LICENSE_INTO_COPYRIGHT'} ))
		{
			if ( ! $installer::globals::englishlicenseset ) { installer::worker::set_english_license() }

			# The location for the new file
			my $languagestring = "";
			for ( my $i = 0; $i <= $#{$languagesref}; $i++ ) { $languagestring = $languagestring . "_" . ${$languagesref}[$i]; }
			$languagestring =~ s/^\s*_//;
			
			my $copyrightdir = installer::systemactions::create_directories("copyright", \$languagestring);
			
			my $copyrightfile = installer::files::read_file($$fileref);

			# Adding license content to copyright file
			push(@{$copyrightfile}, "\n");
			for ( my $i = 0; $i <= $#{$installer::globals::englishlicense}; $i++ ) { push(@{$copyrightfile}, ${$installer::globals::englishlicense}[$i]); }

			# New destination for $$fileref
			$$fileref = $copyrightdir . $installer::globals::separator . "solariscopyrightfile_" . $onepackage->{'module'};
			if ( -f $$fileref ) { unlink $$fileref; }
			installer::files::save_file($$fileref, $copyrightfile);
		}

        $installer::logger::Lang->printf("Using license file: \"%s\"!\n", $$fileref);

		$foundlicensefile = 1;
		$line = "%license" . " " . $$fileref . "\n";
		push(@epmheader, $line);
	}
	else
	{
		# refer to the license in the matching AOO installation
		# TODO: sync AOO dest license full path with lpacks/sdks/exts
		my $licpath = "openoffice4/program/" . $licensefilename;
		$foundlicensefile = 1;
		$line = "%license " . $licpath . "\n";
		push(@epmheader, $line);
	}

	if (!($foundlicensefile))
	{
		installer::exiter::exit_program("ERROR: Could not find license file $licensefilename (B)", "create_epm_header");
	}

	if (!($foundreadmefile))
	{
		installer::exiter::exit_program("ERROR: Could not find readme file $readmefilename (C)", "create_epm_header");
	}

	# including %replaces

	my $replaces = "";

	if (( $installer::globals::issolarispkgbuild ) && ( ! $installer::globals::patch ))
	{
		$replaces = "solarisreplaces";	 # the name in the packagelist	
	}
	elsif (( $installer::globals::islinuxbuild ) && ( ! $installer::globals::patch ))
	{
		$replaces = "linuxreplaces";	# the name in the packagelist	
	}
	
	if (( $replaces ) && ( ! $installer::globals::patch ))
	{
		if ( $onepackage->{$replaces} )
		{
			my $replacesstring = $onepackage->{$replaces};

			my $allreplaces = installer::converter::convert_stringlist_into_array(\$replacesstring, ",");

			for ( my $i = 0; $i <= $#{$allreplaces}; $i++ )
			{
				my $onereplaces = ${$allreplaces}[$i];
				$onereplaces =~ s/\s*$//;
				installer::packagelist::resolve_packagevariables(\$onereplaces, $variableshashref, 1);
				if ( $installer::globals::linuxlinkrpmprocess ) { $onereplaces = $onereplaces . "u"; }
				if ( $installer::globals::debian ) { $onereplaces =~ s/_/-/g; } # Debian allows no underline in package name
				$line = "%replaces" . " " . $onereplaces . "\n";
				push(@epmheader, $line);
			}

			if ( $installer::globals::debian && $variableshashref->{'UNIXPRODUCTNAME'} eq 'openoffice' )
			{
				$line = "%provides" . " openoffice.org-unbundled\n";
				push(@epmheader, $line);
				$line = "%replaces" . " openoffice.org-bundled\n";
				push(@epmheader, $line);
			}
		}
	}
	
	# including the directives for %requires and %provides
	
	my $provides = "";
	my $requires = "";

	if ( $installer::globals::issolarispkgbuild )
	{
		$provides = "solarisprovides";	 # the name in the packagelist	
		$requires = "solarisrequires";	 # the name in the packagelist	
	}
	elsif ( $installer::globals::isfreebsdpkgbuild )
	{
		$provides = "freebsdprovides";	 # the name in the packagelist	
		$requires = "freebsdrequires";	 # the name in the packagelist	
	}
	elsif (( $installer::globals::islinuxrpmbuild ) && 
			( $installer::globals::patch ) && 
			( exists($onepackage->{'linuxpatchrequires'}) )) 
	{
		$provides = "provides";	 # the name in the packagelist	
		$requires = "linuxpatchrequires";	 # the name in the packagelist	
	}
	else
	{
		$provides = "provides";	 		# the name in the packagelist	
		$requires = "requires";	 		# the name in the packagelist	
	}

	# if ( $installer::globals::patch )
	# {
	#	$onepackage->{$provides} = "";
		my $isdict = 0;
		if ( $onepackage->{'packagename'} =~ /-dict-/ ) { $isdict = 1;  }

	#	$onepackage->{$requires} = "";
	# }

	if ( $onepackage->{$provides} )
	{
		my $providesstring = $onepackage->{$provides};

		my $allprovides = installer::converter::convert_stringlist_into_array(\$providesstring, ",");

		for ( my $i = 0; $i <= $#{$allprovides}; $i++ )
		{
			my $oneprovides = ${$allprovides}[$i];
			$oneprovides =~ s/\s*$//;
			installer::packagelist::resolve_packagevariables(\$oneprovides, $variableshashref, 1);
			if ( $installer::globals::linuxlinkrpmprocess ) { $oneprovides = $oneprovides . "u"; }
			if ( $installer::globals::debian ) { $oneprovides =~ s/_/-/g; } # Debian allows no underline in package name
			$line = "%provides" . " " . $oneprovides . "\n";
			push(@epmheader, $line);
		}
	}

	if ( $onepackage->{$requires} )
	{
		my $requiresstring = $onepackage->{$requires};

		if ( $installer::globals::add_required_package ) { $requiresstring = $requiresstring . "," . $installer::globals::add_required_package; }
		
		# The requires string can contain the separator "," in the names (descriptions) of the packages
		# (that are required for Solaris depend files). Therefore "," inside such a description has to
		# masked with a backslash.
		# This masked separator need to be found and replaced, before the stringlist is converted into an array.
		# This replacement has to be turned back after the array is created.
		
		my $replacementstring = "COMMAREPLACEMENT";
		$requiresstring = installer::converter::replace_masked_separator($requiresstring, ",", "$replacementstring");
		
		my $allrequires = installer::converter::convert_stringlist_into_array(\$requiresstring, ",");

		installer::converter::resolve_masked_separator($allrequires, ",", $replacementstring);
		
		for ( my $i = 0; $i <= $#{$allrequires}; $i++ )
		{
			my $onerequires = ${$allrequires}[$i];
			$onerequires =~ s/\s*$//;
			installer::packagelist::resolve_packagevariables2(\$onerequires, $variableshashref, 0, $isdict);
			if ( $installer::globals::debian ) { $onerequires =~ s/_/-/g; } # Debian allows no underline in package name
			
			# Special handling for Solaris. In depend files, the names of the packages are required, not 
			# only the abbreviation. Therefore there is a special syntax for names in packagelist:
			# solarisrequires = "SUNWcar (Name="Package name of SUNWcar"),SUNWkvm (Name="Package name of SUNWcar"), ...
			# if ( $installer::globals::issolarispkgbuild )
			# {
			#	if ( $onerequires =~ /^\s*(.*?)\s+\(\s*Name\s*=\s*\"(.*?)\"\s*\)\s*$/ )
			#	{
			#		$onerequires = $1;
			#		$packagename = $2;
			#		$installer::globals::dependfilenames{$onerequires} = $packagename;
			#	}
			# }
			
			$line = "%requires" . " " . $onerequires . "\n";
			push(@epmheader, $line);
		}
	}
	else
	{
		if ( $installer::globals::add_required_package )
		{
			my $requiresstring = $installer::globals::add_required_package;

			my $replacementstring = "COMMAREPLACEMENT";
			$requiresstring = installer::converter::replace_masked_separator($requiresstring, ",", "$replacementstring");
			my $allrequires = installer::converter::convert_stringlist_into_array(\$requiresstring, ",");
			installer::converter::resolve_masked_separator($allrequires, ",", $replacementstring);
		
			for ( my $i = 0; $i <= $#{$allrequires}; $i++ )
			{
				my $onerequires = ${$allrequires}[$i];
				$onerequires =~ s/\s*$//;
				installer::packagelist::resolve_packagevariables(\$onerequires, $variableshashref, 0);
				if ( $installer::globals::debian ) { $onerequires =~ s/_/-/g; } # Debian allows no underline in package name

				# Special handling for Solaris. In depend files, the names of the packages are required, not 
				# only the abbreviation. Therefore there is a special syntax for names in packagelist:
				# solarisrequires = "SUNWcar (Name="Package name of SUNWcar"),SUNWkvm (Name="Package name of SUNWcar"), ...
				# if ( $installer::globals::issolarispkgbuild )
				# {
				#	if ( $onerequires =~ /^\s*(.*?)\s+\(\s*Name\s*=\s*\"(.*?)\"\s*\)\s*$/ )
				#	{
				#		$onerequires = $1;
				#		$packagename = $2;
				#		$installer::globals::dependfilenames{$onerequires} = $packagename;
				#	}
				# }

				$line = "%requires" . " " . $onerequires . "\n";
				push(@epmheader, $line);
			}
		}
	}

	return \@epmheader;
}

#######################################
# Adding header to epm file
#######################################

sub adding_header_to_epm_file
{
	my ($epmfileref, $epmheaderref) = @_;
	
	for ( my $i = 0; $i <= $#{$epmheaderref}; $i++ )
	{
		push( @{$epmfileref}, ${$epmheaderref}[$i] );	
	}

	push( @{$epmfileref}, "\n\n" );	
}

#####################################################
# Replace one in shell scripts ( ${VARIABLENAME} )
#####################################################

sub replace_variable_in_shellscripts
{
	my ($scriptref, $variable, $searchstring) = @_;
	
	for ( my $i = 0; $i <= $#{$scriptref}; $i++ )
	{
		${$scriptref}[$i] =~ s/\$\{$searchstring\}/$variable/g;
	}
}

###################################################
# Replace one in shell scripts ( %VARIABLENAME )
###################################################

sub replace_percent_variable_in_shellscripts
{
	my ($scriptref, $variable, $searchstring) = @_;
	
	for ( my $i = 0; $i <= $#{$scriptref}; $i++ )
	{
		${$scriptref}[$i] =~ s/\%$searchstring/$variable/g;
	}
}

################################################
# Replacing many variables in shell scripts
################################################

sub replace_many_variables_in_shellscripts
{
	my ($scriptref, $variableshashref) = @_;

	my $key;

	foreach $key (keys %{$variableshashref})
	{
		my $value = $variableshashref->{$key};
		# $value = lc($value); 	# lowercase !
		# if ( $installer::globals::issolarisbuild) { $value =~ s/\.org/org/g; }	# openofficeorg instead of openoffice.org
		replace_variable_in_shellscripts($scriptref, $value, $key);
	}
}

#######################################
# Adding shell scripts to epm file
#######################################

sub adding_shellscripts_to_epm_file
{
	my ($epmfileref, $shellscriptsfilename, $localrootpath, $allvariableshashref, $filesinpackage) = @_;

	# $installer::globals::shellscriptsfilename

	push( @{$epmfileref}, "\n\n" );	

	my $shellscriptsfileref = installer::files::read_file($shellscriptsfilename);
	
	replace_variable_in_shellscripts($shellscriptsfileref, $localrootpath, "rootpath");

	replace_many_variables_in_shellscripts($shellscriptsfileref, $allvariableshashref);

	for ( my $i = 0; $i <= $#{$shellscriptsfileref}; $i++ )
	{
		push( @{$epmfileref}, ${$shellscriptsfileref}[$i] );	
	}

	push( @{$epmfileref}, "\n" );	
}

#################################################
# Determining the epm on the system
#################################################

sub find_epm_on_system
{
	my ($includepatharrayref) = @_;

	installer::logger::include_header_into_logfile("Check epm on system");

	my $epmname = "epm";

	# epm should be defined through the configure script but we need to
	# check for it to be defined because of the Sun environment.
	# Check the environment variable first and if it is not defined,
	# or if it is but the location is not executable, search further.
	# It has to be found in the solver or it has to be in the path 
	# (saved in $installer::globals::epm_in_path) or we get the specified 
	# one through the environment (i.e. when --with-epm=... is specified)

	if ($ENV{'EPM'})
	{
		if (($ENV{'EPM'} ne "") && (-x "$ENV{'EPM'}"))
		{
			$epmname = $ENV{'EPM'};
		}
		elsif ( ($ENV{'EPM'} eq "no") || ($ENV{'EPM'} eq "internal") )
		{
			$epmname = "epm";		
			my $epmref = installer::scriptitems::get_sourcepath_from_filename_and_includepath( \$epmname, $includepatharrayref, 0);
			if ($$epmref eq "") { installer::exiter::exit_program("ERROR: Could not find program $epmname (EPM set to \"internal\" or \"no\")!", "find_epm_on_system"); }
			$epmname = $$epmref;
		}
		else
		{
			installer::exiter::exit_program("Environment variable EPM set (\"$ENV{'EPM'}\"), but file does not exist or is not executable!", "find_epm_on_system");
		}
	}
	else
	{
		my $epmfileref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$epmname, $includepatharrayref, 0);

		if (($$epmfileref eq "") && (!($installer::globals::epm_in_path))) { installer::exiter::exit_program("ERROR: Could not find program $epmname!", "find_epm_on_system"); }
		if (($$epmfileref eq "") && ($installer::globals::epm_in_path)) { $epmname = $installer::globals::epm_path; }
		if (!($$epmfileref eq "")) { $epmname = $$epmfileref; }
	}

    $installer::logger::Lang->printf("Using epmfile: %s\n", $epmname);
	
	return $epmname;	
}

#################################################
# Determining the epm patch state
# saved in $installer::globals::is_special_epm
# NOTE: W/ EPM 5.0.0 and later, it is ALWAYS special
#################################################

sub set_patch_state
{
	my ($epmexecutable) = @_;

	my $infoline = "";
	
	my $systemcall = "$epmexecutable |";
	open (EPMPATCH, "$systemcall");
	
	while (<EPMPATCH>)
	{
		chop;
		if ( $_ =~ /Patched for .*OpenOffice/ ) { $installer::globals::is_special_epm = 1; last; }
		if ( $_ =~ /Apache OpenOffice compatible/ ) { $installer::globals::is_special_epm = 1; last; }
	}
			
	close (EPMPATCH);

	if ( $installer::globals::is_special_epm )
	{
        $installer::logger::Lang->print("\n");
        $installer::logger::Lang->print("Patch state: This is a patched version of epm!\n");
        $installer::logger::Lang->print("\n");
	}
	else	
	{
        $installer::logger::Lang->print("\n");
        $installer::logger::Lang->print("Patch state: This is an unpatched version of epm!\n");
        $installer::logger::Lang->print("\n");
	}

	if ( ( $installer::globals::is_special_epm ) && (($installer::globals::islinuxrpmbuild) || ($installer::globals::issolarispkgbuild)) )
	{
		# Special postprocess handling only for Linux RPM and Solaris packages
		$installer::globals::postprocess_specialepm = 1;
		$installer::globals::postprocess_standardepm = 0;		
	}
	else
	{
		$installer::globals::postprocess_specialepm = 0;
		$installer::globals::postprocess_standardepm = 1;
	}
}

#################################################
# LD_PRELOAD string for Debian packages
#################################################

sub get_ld_preload_string
{
	return $ENV{'FAKEROOT'};
}

#################################################
# Calling epm to create the installation sets
#################################################

sub call_epm
{
	my ($epmname, $epmlistfilename, $packagename, $includepatharrayref) = @_;
	
	installer::logger::include_header_into_logfile("epm call for $packagename");

	my $packageformat = $installer::globals::packageformat;
	
	my $localpackagename = $packagename;
	# Debian allows only lowercase letters in package name
	if ( $installer::globals::debian ) { $localpackagename = lc($localpackagename); } 
	
	my $outdirstring = "";
	if ( $installer::globals::epmoutpath ne "" ) { $outdirstring = " --output-dir $installer::globals::epmoutpath"; }
	
	# Debian package build needs fakeroot which we check for at configure time
	# NOTE: EPM 5.0.0 or later also uses fakeroot w/ dpkg if available
	
	my $ldpreloadstring = "";
	
	if ( $installer::globals::debian ) { $ldpreloadstring = get_ld_preload_string() . " "; }

	my $extraflags = "";
    if ($ENV{'EPM_FLAGS'}) { $extraflags = $ENV{'EPM_FLAGS'}; }

    my $verboseflag = "-v";
    if ( ! $installer::globals::quiet ) { $verboseflag = "-v2"; };
	
	my $systemcall = $ldpreloadstring . $epmname . " -f " . $packageformat . " " . $extraflags . " " . $localpackagename . " " . $epmlistfilename . $outdirstring . " " . $verboseflag . " " . " 2\>\&1 |";

    $installer::logger::Info->printf("... %s ...\n", $systemcall);

	my $maxepmcalls = 3;

	for ( my $i = 1; $i <= $maxepmcalls; $i++ )
	{
		my @epmoutput = ();

		open (EPM, "$systemcall");
		while (<EPM>) {push(@epmoutput, $_); }
		close (EPM);

		my $returnvalue = $?;	# $? contains the return value of the systemcall

        $installer::logger::Lang->printf("Systemcall  (Try %d): \n", $i, $systemcall);

		for ( my $j = 0; $j <= $#epmoutput; $j++ )
		{
			if ( $i < $maxepmcalls ) { $epmoutput[$j] =~ s/\bERROR\b/PROBLEM/ig; }
            $installer::logger::Lang->print($epmoutput[$j]);
		}
	
		if ($returnvalue)
		{
            $installer::logger::Lang->printf("Try %d : Could not execute \"%s\"!\n", $i, $systemcall);
			if ( $i == $maxepmcalls ) { installer::exiter::exit_program("ERROR: \"$systemcall\"!", "call_epm"); }
		}
		else
		{
            $installer::logger::Info->printf("Success: Executed (Try %d): \"%s\" successfully\n", $i, $systemcall);
            $installer::logger::Lang->printf("Success: Executed (Try %d): \"%s\" successfully\n", $i, $systemcall);
			last;
		}	
	}
}

#####################################################################
# Adding the new line for relocatables into pkginfo file (Solaris)
# or spec file (Linux) created by epm
#####################################################################

sub add_one_line_into_file
{
	my ($file, $insertline, $filename) = @_;
	
	if ( $installer::globals::issolarispkgbuild )
	{
		push(@{$file}, $insertline);		# simply adding at the end of pkginfo file
	}	

	if ( $installer::globals::islinuxrpmbuild )
	{
		# Adding behind the line beginning with: Group:
		
		my $inserted_line = 0;
		
		for ( my $i = 0; $i <= $#{$file}; $i++ )
		{
			if ( ${$file}[$i] =~ /^\s*Group\:\s*/ )
			{
				splice(@{$file},$i+1,0,$insertline);
				$inserted_line = 1;
				last;
			}	
		}
		
		if (! $inserted_line) { installer::exiter::exit_program("ERROR: Did not find string \"Group:\" in file: $filename", "add_one_line_into_file"); }
	}

	$insertline =~ s/\s*$//;	# removing line end for correct logging
    $installer::logger::Lang->printf("Success: Added line %s into file !\n", $insertline, $filename);
}

#####################################################################
# Setting the revision VERSION=1.9,REV=66  .
# Also adding the new line: "AutoReqProv: no"
#####################################################################

sub set_revision_in_pkginfo
{
	my ($file, $filename, $variables, $packagename) = @_;

	my $revisionstring = "\,REV\=" . $installer::globals::packagerevision;
	
	# Adding also a time string to the revision. Syntax: VERSION=8.0.0,REV=66.2005.01.24
	
	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);

	$mday = $mday;
	$mon = $mon + 1;
	$year = $year + 1900;

	if ( $mday < 10 ) { $mday = "0" . $mday; } 
	if ( $mon < 10 ) { $mon = "0" . $mon; } 
	$datestring = $year . "." . $mon . "." . $mday;
	$revisionstring = $revisionstring . "." . $datestring;
	
	for ( my $i = 0; $i <= $#{$file}; $i++ )
	{
		if ( ${$file}[$i] =~ /^\s*(VERSION\=.*?)\s*$/ )
		{
			my $oldstring = $1;
			my $newstring = $oldstring . $revisionstring;	# also adding the date string
			${$file}[$i] =~ s/$oldstring/$newstring/;
            $installer::logger::Lang->printf("Info: Changed in %s file: \"%s\" to \"\"!\n",
                $filename,
                $oldstring,
                $newstring);
			last;
		}	
	}

	# For Update and Patch reasons, this string can also be kept constant
	
	my $pkgversion = "SOLSPARCPKGVERSION";
	if ( $installer::globals::issolarisx86build ) { $pkgversion = "SOLIAPKGVERSION"; }
	
	if (( $variables->{$pkgversion} ) && ( $variables->{$pkgversion} ne "" ))
	{
		if ( $variables->{$pkgversion} ne "FINALVERSION" )
		{
			# In OOo 3.x timeframe, this string is no longer unique for all packages, because of the three layer.
			# In the string: "3.0.0,REV=9.2008.09.30" only the part "REV=9.2008.09.30" can be unique for all packages
			# and therefore be set as $pkgversion.
			# The first part "3.0.0" has to be derived from the 
			
			my $version = $installer::globals::packageversion;
			if ( $version =~ /^\s*(\d+)\.(\d+)\.(\d+)\s*$/ )
			{
				my $major = $1;
				my $minor = $2;
				my $micro = $3;
				
				my $finalmajor = $major;
				my $finalminor = $minor;
				my $finalmicro = 0;
				
				# if (( $packagename =~ /-ure\s*$/ ) && ( $finalmajor == 1 )) { $finalminor = 4; }

				$version = "$finalmajor.$finalminor.$finalmicro"; 
			}
			
			my $datestring = $variables->{$pkgversion};
			
			# Allowing some packages to have another date of creation.
			# They can be defined in product definition using a key like "SOLSPARCPKGVERSION_$packagename"
			
			my $additionalkey = $pkgversion . "_" . $packagename;
			if (( $variables->{$additionalkey} ) && ( $variables->{$additionalkey} ne "" )) { $datestring = $variables->{$additionalkey}; }
			
			my $versionstring = "$version,$datestring";

			for ( my $i = 0; $i <= $#{$file}; $i++ )
			{
				if ( ${$file}[$i] =~ /^\s*(VERSION\=).*?\s*$/ )
				{
					my $start = $1;
					my $newstring = $start . $versionstring . "\n";	# setting the complete new string	
					my $oldstring = ${$file}[$i];
					${$file}[$i] = $newstring;
					$oldstring =~ s/\s*$//;
					$newstring =~ s/\s*$//;
                    $installer::logger::Lang->printf("Info: Changed in %s file: \"%s\" to \"\"!\n", $filename, $oldstring, $newstring);
					last;
				}
			}	
		}
	}
}

########################################################
# Setting Patch information for Respin versions
# into pkginfo file. This prevents Respin versions
# from patching.
########################################################

sub set_patchlist_in_pkginfo_for_respin
{
	my ($changefile, $filename, $allvariables, $packagename) = @_;
	
	my $patchlistname = "SOLSPARCPATCHLISTFORRESPIN";
	if ( $installer::globals::issolarisx86build ) { $patchlistname = "SOLIAPATCHLISTFORRESPIN"; }	
	
	if ( $allvariables->{$patchlistname} )
	{
		# patchlist separator is a blank
		my $allpatchesstring = $allvariables->{$patchlistname};
		my @usedpatches = ();

		# Analyzing the patchlist
		# Syntax: 120186-10 126411-01(+core-01) -> use 126411-01 only for core-01
		# Syntax: 120186-10 126411-01(-core-01) -> use 126411-01 for all packages except for core-01
		my $allpatches = installer::converter::convert_whitespace_stringlist_into_array(\$allpatchesstring);

		for ( my $i = 0; $i <= $#{$allpatches}; $i++ )
		{
			my $patchdefinition = ${$allpatches}[$i];

			my $patchid = "";
			my $symbol = "";
			my $constraint = "";
			my $isusedpatch = 0;
			
			if ( $patchdefinition =~ /^\s*(.+)\(([+-])(.+)\)\s*$/ )
			{
				$patchid = $1;
				$symbol = $2;
				$constraint = $3;
			}
			elsif (( $patchdefinition =~ /\(/ ) || ( $patchdefinition =~ /\)/ ))	# small syntax check
			{
				# if there is a bracket in the $patchdefinition, but it does not 
				# match the if-condition, this is an erroneous definition.
				installer::exiter::exit_program("ERROR: Unknown patch string: $patchdefinition", "set_patchlist_in_pkginfo_for_respin");
			}
			else
			{
				$patchid = $patchdefinition;
				$isusedpatch = 1; # patches without constraint are always included
			}
			
			if ( $symbol ne "" )
			{
				if ( $symbol eq "+" )
				{
					if ( $packagename =~ /^.*\Q$constraint\E\s*$/ ) { $isusedpatch = 1; }
				}
				
				if ( $symbol eq "-" )
				{
					if ( ! ( $packagename =~ /^.*\Q$constraint\E\s*$/ )) { $isusedpatch = 1; }					
				}
			}

			if ( $isusedpatch ) { push(@usedpatches, $patchid); }
		}

		if ( $#usedpatches > -1 )
		{
			my $patchstring = installer::converter::convert_array_to_space_separated_string(\@usedpatches);

			my $newline = "PATCHLIST=" . $patchstring . "\n";
			add_one_line_into_file($changefile, $newline, $filename);

			# Adding patch info for each used patch in the patchlist

			for ( my $i = 0; $i <= $#usedpatches; $i++ )
			{				
				my $patchid = $usedpatches[$i];
				my $key = "PATCH_INFO_" . $patchid;
				$key =~ s/\s*$//;

				if ( ! $allvariables->{$key} ) { installer::exiter::exit_program("ERROR: No Patch info available in zip list file for $key", "set_patchlist_in_pkginfo"); }
				my $value = set_timestamp_in_patchinfo($allvariables->{$key});
				$newline = $key . "=" . $value . "\n";		

				add_one_line_into_file($changefile, $newline, $filename);
			}
		}
	}
}

########################################################
# Solaris requires, that the time of patch installation
# must not be empty.
# Format: Mon Mar 24 11:20:56 PDT 2008
# Log file: Tue Apr 29 23:26:19 2008 (04:31 min.)
# Replace string: ${TIMESTAMP}
########################################################

sub set_timestamp_in_patchinfo
{
	my ($value) = @_;
	
	my $currenttime = localtime();
	
	if ( $currenttime =~ /^\s*(.+?)(\d\d\d\d)\s*$/ )
	{
		my $start = $1;
		my $year = $2;
		$currenttime = $start . "CET " . $year;
	}
	
	$value =~ s/\$\{TIMESTAMP\}/$currenttime/;

	return $value;
}

########################################################
# Setting MAXINST=1000 into the pkginfo file.
########################################################

sub set_maxinst_in_pkginfo
{
	my ($changefile, $filename) = @_;
	
	my $newline = "MAXINST\=1000\n";

	add_one_line_into_file($changefile, $newline, $filename);
}

#############################################################
# Setting several Solaris variables into the pkginfo file.
#############################################################

sub set_solaris_parameter_in_pkginfo
{
	my ($changefile, $filename, $allvariables) = @_;

	my $newline = "";

	# SUNW_PRODNAME
	# SUNW_PRODVERS
	# SUNW_PKGVERS
	# Not: SUNW_PKGTYPE
	# HOTLINE
	# EMAIL
	
	my $productname = $allvariables->{'PRODUCTNAME'};
	$newline = "SUNW_PRODNAME=$productname\n";
	add_one_line_into_file($changefile, $newline, $filename);

	my $productversion = "";
	if ( $allvariables->{'PRODUCTVERSION'} )
	{
		$productversion = $allvariables->{'PRODUCTVERSION'};
		if ( $allvariables->{'PRODUCTEXTENSION'} ) { $productversion = $productversion . "/" . $allvariables->{'PRODUCTEXTENSION'}; }
	}
	$newline = "SUNW_PRODVERS=$productversion\n";
	add_one_line_into_file($changefile, $newline, $filename);

	$newline = "SUNW_PKGVERS=1\.0\n";
	add_one_line_into_file($changefile, $newline, $filename);

	if ( $allvariables->{'SUNW_PKGTYPE'} )
	{
		$newline = "SUNW_PKGTYPE=$allvariables->{'SUNW_PKGTYPE'}\n";
		add_one_line_into_file($changefile, $newline, $filename);		
	}
    else
    {
        $newline = "SUNW_PKGTYPE=\n";
        add_one_line_into_file($changefile, $newline, $filename);
    }

	$newline = "HOTLINE=Please contact your local service provider\n";
	add_one_line_into_file($changefile, $newline, $filename);

	$newline = "EMAIL=\n";
	add_one_line_into_file($changefile, $newline, $filename);
	
}

#####################################################################
# epm uses as archtecture for Solaris x86 "i86pc". This has to be
# changed to "i386".
#####################################################################

sub fix_architecture_setting
{
	my ($changefile) = @_;
	
	for ( my $i = 0; $i <= $#{$changefile}; $i++ )
	{
		if ( ${$changefile}[$i] =~ /^\s*ARCH=i86pc\s*$/ )
		{
			${$changefile}[$i] =~ s/i86pc/i386/;
			last;
		}

	}
}

#####################################################################
# Adding a new line for topdir into specfile, removing old
# topdir if set.
#####################################################################

sub set_topdir_in_specfile
{
	my ($changefile, $filename, $newepmdir) = @_;

	# $newepmdir =~ s/^\s*\.//;	# removing leading "."
	$newepmdir = cwd() . $installer::globals::separator . $newepmdir; # only absolute path allowed

	# removing "%define _topdir", if existing
	
	for ( my $i = 0; $i <= $#{$changefile}; $i++ )
	{
		if ( ${$changefile}[$i] =~ /^\s*\%define _topdir\s+/ )
		{
			my $removeline = ${$changefile}[$i];
			$removeline =~ s/\s*$//;
			splice(@{$changefile},$i,1);
            $installer::logger::Lang->printf("Info: Removed line \"%s\" from file %s!\n", $removeline, $filename);
			last;
		}	
	}

	# Adding "topdir" behind the line beginning with: Group:
		
	my $inserted_line = 0;

	my $topdirline = "\%define _topdir $newepmdir\n";
		
	for ( my $i = 0; $i <= $#{$changefile}; $i++ )
	{
		if ( ${$changefile}[$i] =~ /^\s*Group\:\s*/ )
		{
			splice(@{$changefile},$i+1,0,$topdirline);
			$inserted_line = 1;
			$topdirline =~ s/\s*$//;
            $installer::logger::Lang->printf("Success: Added line %s into file %s!\n", $topdirline, $filename);
		}	
	}
		
	if (! $inserted_line) { installer::exiter::exit_program("ERROR: Did not find string \"Group:\" in file: $filename", "set_topdir_in_specfile"); }

}

#####################################################################
# Setting the packager in the spec file
# Syntax: Packager: abc@def
#####################################################################

sub set_packager_in_specfile
{
	my ($changefile) = @_;

	my $packager = $installer::globals::longmanufacturer; 

	for ( my $i = 0; $i <= $#{$changefile}; $i++ )
	{
		if ( ${$changefile}[$i] =~ /^\s*Packager\s*:\s*(.+?)\s*$/ )
		{
			my $oldstring = $1;
			${$changefile}[$i] =~ s/\Q$oldstring\E/$packager/;
            $installer::logger::Lang->printf("Info: Changed Packager in spec file from %s to %s!\n",
                $oldstring,
                $packager);
			last;			
		}	
	}
}

#####################################################################
# Setting the requirements in the spec file (i81494)
# Syntax: PreReq: "requirements" (only for shared extensions)
#####################################################################

sub set_prereq_in_specfile
{
	my ($changefile) = @_;

	my $prereq = "PreReq:"; 

	for ( my $i = 0; $i <= $#{$changefile}; $i++ )
	{
		if ( ${$changefile}[$i] =~ /^\s*Requires:\s*(.+?)\s*$/ )
		{
			my $oldstring = ${$changefile}[$i];
			${$changefile}[$i] =~ s/Requires:/$prereq/;
            $installer::logger::Lang->printf("Info: Changed requirements in spec file from %s to %s!\n",
                $oldstring,
                ${$changefile}[$i]);
		}	
	}
}

#####################################################################
# Setting the Auto[Req]Prov line and __find_requires
#####################################################################

sub set_autoprovreq_in_specfile
{
    my ($changefile, $findrequires, $bindir) = @_;
    
	my $autoreqprovline;
    
    if ( $findrequires )
    {
        $autoreqprovline = "AutoProv\: no\n%define __find_requires $bindir/$findrequires\n";
    }
    else
    {
        $autoreqprovline = "AutoReqProv\: no\n";
    }

    $autoreqprovline .= "%define _binary_filedigest_algorithm 1\n%define _binary_payload w9.gzdio\n";

	for ( my $i = 0; $i <= $#{$changefile}; $i++ )
	{
        # Adding "autoreqprov" behind the line beginning with: Group:
		if ( ${$changefile}[$i] =~ /^\s*Group\:\s*/ )
		{
			splice(@{$changefile},$i+1,0,$autoreqprovline);
			$autoreqprovline =~ s/\s*$//;
            $installer::logger::Lang->printf("Success: Added line %s into spec file!\n", $autoreqprovline);
			last;
        }
    }
}

#####################################################################
# Replacing Copyright with License in the spec file
# Syntax: License: ALv2 (older usages were LGPL, SISSL)
#####################################################################

sub set_license_in_specfile
{
	my ($changefile, $variableshashref) = @_;

	my $license = $variableshashref->{'LICENSENAME'};

	for ( my $i = 0; $i <= $#{$changefile}; $i++ )
	{
		if ( ${$changefile}[$i] =~ /^\s*Copyright\s*:\s*(.+?)\s*$/ )
		{
			${$changefile}[$i] = "License: $license\n";
            $installer::logger::Lang->printf("Info: Replaced Copyright with License: %s !\n", $license);
			last;			
		}	
	}
}

#########################################################
# Building relocatable Solaris packages means:
# 1. Add "BASEDIR=/opt" into pkginfo
# 2. Remove "/opt/" from all objects in prototype file
# For step2 this function exists
# Sample: d none /opt/openofficeorg20/help 0755 root other
# -> d none openofficeorg20/help 0755 root other
#########################################################

sub make_prototypefile_relocatable
{
	my ($prototypefile, $relocatablepath) = @_;
			
	for ( my $i = 0; $i <= $#{$prototypefile}; $i++ )
	{
		if ( ${$prototypefile}[$i] =~ /^\s*\w\s+\w+\s+\/\w+/ )	# this is an object line
		{
			${$prototypefile}[$i] =~ s/$relocatablepath//;	# Important: $relocatablepath has a "/" at the end. Example "/opt/"
		}
	}
	
	# If the $relocatablepath is "/opt/openoffice20/" the line "d none /opt/openoffice20" was not changed.
	# This line has to be removed now
	
	if ( $relocatablepath ne "/" ) { $relocatablepath =~ s/\/\s*$//; }		# removing the ending slash
	
	for ( my $i = 0; $i <= $#{$prototypefile}; $i++ )
	{
		if ( ${$prototypefile}[$i] =~ /^\s*d\s+\w+\s+\Q$relocatablepath\E/ )
		{
			my $line = ${$prototypefile}[$i];
			splice(@{$prototypefile},$i,1);	# removing the line
			$line =~ s/\s*$//;
            $installer::logger::Lang->printf("Info: Removed line \"%s\" from prototype file!\n", $line);
			last;
		}
	}
	
	# Making "\$" to "$" in prototype file. "\$" was created by epm.

	for ( my $i = 0; $i <= $#{$prototypefile}; $i++ )
	{
		if ( ${$prototypefile}[$i] =~ /\\\$/ )
		{
			${$prototypefile}[$i] =~ s/\\\$/\$/g;
            $installer::logger::Lang->printf("Info: Changed line in prototype file: %s !\n", ${$prototypefile}[$i]);
		}
	}
}


#########################################################################
# In scp the flag VOLATEFILE can be used. This shall lead to style "v"
# in Solaris prototype file. This is not supported by epm and has
# therefore to be included in prototypefile, not in epm list file.
#########################################################################

sub set_volatilefile_into_prototypefile
{
	my ($prototypefile, $filesref) = @_;

	for ( my $i = 0; $i <= $#{$filesref}; $i++ )
	{
		my $onefile = ${$filesref}[$i];

		my $styles = "";
		if ( $onefile->{'Styles'} ) { $styles = $onefile->{'Styles'}; }
		
		if ( $styles =~ /\bVOLATILEFILE\b/ )
		{
			my $sourcepath = $onefile->{'sourcepath'};
				
			for ( my $j = 0; $j <= $#{$prototypefile}; $j++ )
			{
				if (( ${$prototypefile}[$j] =~ /^\s*f\s+none\s+/ ) && ( ${$prototypefile}[$j] =~ /\=\Q$sourcepath\E\s+/ ))
				{
					my $oldline = ${$prototypefile}[$j];
					${$prototypefile}[$j] =~ s/^\s*f/v/;
					my $newline = ${$prototypefile}[$j];
					$oldline =~ s/\s*$//;
					$newline =~ s/\s*$//;
                    $installer::logger::Lang->printf(
                        "Volatile file: Changing content from \"%s\" to \"%s\" .\n",
                        $oldline,
                        $newline);
					last;
				}
			}
		}
	}
}

#########################################################################
# Replacing the variables in the Solaris patch shell scripts.
# Taking care, that multiple slashes are not always removed.
#########################################################################

sub replace_variables_in_shellscripts_for_patch
{
	my ($scriptfile, $scriptfilename, $oldstring, $newstring) = @_;

	for ( my $i = 0; $i <= $#{$scriptfile}; $i++ )
	{
		if ( ${$scriptfile}[$i] =~ /\Q$oldstring\E/ )
		{
			my $oldline = ${$scriptfile}[$i];
			if (( $oldstring eq "PRODUCTDIRECTORYNAME" ) && ( $newstring eq "" )) { $oldstring = $oldstring . "/"; }
			${$scriptfile}[$i] =~ s/\Q$oldstring\E/$newstring/g;
            $installer::logger::Lang->printf("Info: Substituting in %s %s by %s\n",
                $scriptfilename, $oldstring, $newstring);
		}
	}	
}

#########################################################################
# Replacing the variables in the shell scripts or in the epm list file
# Linux: spec file
# Solaris: preinstall, postinstall, preremove, postremove
# If epm is used in the original version (not relocatable)
# the variables have to be exchanged in the list file, 
# created for epm.
#########################################################################

sub replace_variables_in_shellscripts
{
	my ($scriptfile, $scriptfilename, $oldstring, $newstring) = @_;

	my $debug = 0;
	if ( $oldstring eq "PRODUCTDIRECTORYNAME" ) { $debug = 1; }

	for ( my $i = 0; $i <= $#{$scriptfile}; $i++ )
	{
		if ( ${$scriptfile}[$i] =~ /\Q$oldstring\E/ )
		{
			my $oldline = ${$scriptfile}[$i];
			${$scriptfile}[$i] =~ s/\Q$oldstring\E/$newstring/g;
			${$scriptfile}[$i] =~ s/\/\//\//g;	# replacing "//" by "/" , if path $newstring is empty!
            $installer::logger::Lang->printf("Info: Substituting in %s %s by %s\n",
                $scriptfilename,
                $oldstring,
                $newstring);
			if ( $debug )
			{
                $installer::logger::Lang->printf("Old Line: %s", $oldline);
                $installer::logger::Lang->printf("New Line: %s", ${$scriptfile}[$i]);
			}
		}
	}
}

############################################################
# Determinig the directory created by epm, in which the
# RPMS or Solaris packages are created.
############################################################

sub determine_installdir_ooo
{
	# A simple "ls" command returns the directory name 

	my $dirname = "";
	
	my $systemcall = "ls |";
	open (LS, "$systemcall");
	$dirname = <LS>;
	close (LS);

	$dirname =~ s/\s*$//;

    $installer::logger::Lang->printf("Info: Directory created by epm: %s\n", $dirname);
	
	return $dirname;
}

############################################################
# Setting the tab content into the file container
############################################################

sub set_tab_into_datafile
{
	my ($changefile, $filesref) = @_;

	my @newclasses = ();
	my $newclassesstring = "";

	if ( $installer::globals::issolarispkgbuild )
	{
		for ( my $i = 0; $i <= $#{$filesref}; $i++ )
		{
			my $onefile = ${$filesref}[$i];

			if ( $onefile->{'SolarisClass'} )
			{
				my $sourcepath = $onefile->{'sourcepath'};
				
				for ( my $j = 0; $j <= $#{$changefile}; $j++ )
				{
					if (( ${$changefile}[$j] =~ /^\s*f\s+none\s+/ ) && ( ${$changefile}[$j] =~ /\=\Q$sourcepath\E\s+/ ))
					{
						my $oldline = ${$changefile}[$j];
						${$changefile}[$j] =~ s/f\s+none/e $onefile->{'SolarisClass'}/;
						my $newline = ${$changefile}[$j];
						$oldline =~ s/\s*$//;
						$newline =~ s/\s*$//;

                        $installer::logger::Lang->printf("TAB: Changing content from \"%s\" to \"%s\" .\n",
                            $oldline, $newline);

						# collecting all new classes
						if (! installer::existence::exists_in_array($onefile->{'SolarisClass'}, \@newclasses))
						{
							push(@newclasses, $onefile->{'SolarisClass'});
						}
						
						last;
					}
				}
			}
		}

		$newclassesstring = installer::converter::convert_array_to_space_separated_string(\@newclasses);
	}

	if ( $installer::globals::islinuxrpmbuild )
	{
		for ( my $i = 0; $i <= $#{$filesref}; $i++ )
		{
			my $onefile = ${$filesref}[$i];

			if ( $onefile->{'SpecFileContent'} )
			{
				my $destination = $onefile->{'destination'};
				
				for ( my $j = 0; $j <= $#{$changefile}; $j++ )
				{
					if ( ${$changefile}[$j] =~ /^\s*(\%attr\(.*\))\s+(\".*?\Q$destination\E\"\s*)$/ )
					{
						my $begin = $1;
						my $end = $2;
					
						my $oldline = ${$changefile}[$j];
						${$changefile}[$j] = $begin . " " . $onefile->{'SpecFileContent'} . " " . $end;
						my $newline = ${$changefile}[$j];
						
						$oldline =~ s/\s*$//;
						$newline =~ s/\s*$//;

                        $installer::logger::Lang->printf(
                            "TAB: Changing content from \"%s\" to \"%s\" .\n", $oldline, $newline);

						last;
					}				
				}			
			}
		}
	}	

	return $newclassesstring;
}

############################################################
# Including additional classes into the pkginfo file
############################################################

sub include_classes_into_pkginfo
{
	my ($changefile, $classesstring) = @_;

	for ( my $i = 0; $i <= $#{$changefile}; $i++ )
	{
		if ( ${$changefile}[$i] =~ /^\s*CLASSES\=none/ )
		{
			${$changefile}[$i] =~ s/\s*$//;
			my $oldline = ${$changefile}[$i];
			${$changefile}[$i] = ${$changefile}[$i] . " " . $classesstring . "\n";
			my $newline = ${$changefile}[$i];
			$newline =~ s/\s*$//;

            $installer::logger::Lang->printf("pkginfo file: Changing content from \"%s\" to \"%s\" .\n",
                $oldline, $newline);
		}
	}
}

##########################################################################################
# Checking, if an extension is included into the package (Linux).
# All extension files have to be installed into directory
# share/extension/install
# %attr(0444,root,root) "/opt/staroffice8/share/extension/install/SunSearchToolbar.oxt"
##########################################################################################

sub is_extension_package
{
	my ($specfile) = @_;
	
	my $is_extension_package = 0;

	for ( my $i = 0; $i <= $#{$specfile}; $i++ )
	{
		my $line = ${$specfile}[$i];
		if ( $line =~ /share\/extension\/install\/.*?\.oxt\"\s*$/ )
		{
			$is_extension_package = 1;
			last;
		}
	}

	return $is_extension_package;
}

######################################################################
# Checking, if an extension is included into the package (Solaris).
# All extension files have to be installed into directory
# share/extension/install
######################################################################

sub contains_extension_dir
{
	my ($prototypefile) = @_;
	
	my $contains_extension_dir = 0;

	# d none opt/openoffice.org3/share/extensions/

	for ( my $i = 0; $i <= $#{$prototypefile}; $i++ )
	{
		my $line = ${$prototypefile}[$i];
		if ( $line =~ /^\s*d\s+none\s.*\/share\/extensions\// )
		{
			$contains_extension_dir = 1;
			last;
		}
	}
	
	return $contains_extension_dir;
}

############################################################
# A Solaris patch contains 7 specific scripts
############################################################

sub add_scripts_into_prototypefile
{
	my ($prototypefile, $prototypefilename, $languagestringref, $staticpath) = @_;
	
	# The files are stored in the directory $installer::globals::patchincludepath
	# The file names are available via @installer::globals::solarispatchscripts
	
	my $path = $installer::globals::patchincludepath;
	$path =~ s/\/\s*$//;
	$path = $path . $installer::globals::separator;

	my @newlines = ();
	my $is_extension_package = contains_extension_dir($prototypefile);

	if ( $is_extension_package )
	{	
		for ( my $i = 0; $i <= $#installer::globals::solarispatchscriptsforextensions; $i++ )
		{
			my $sourcefilename = $path . $installer::globals::solarispatchscriptsforextensions[$i];
			my $destfile = $installer::globals::solarispatchscriptsforextensions[$i];

			# If the sourcepath has "_extension" in its name, this has to be removed
			$destfile =~ s/_extensions\s*$//;  # hard coded renaming of script name

			# Creating unique directory name with $prototypefilename
			my $extensiondir = installer::systemactions::create_directories("extensionscripts", $languagestringref);

			if ( $prototypefilename =~ /\/(\S*?)\s*$/ ) { $prototypefilename = $1; }
			$prototypefilename =~ s/\./_/g;
			my $destdir = $extensiondir . $installer::globals::separator . $prototypefilename;
			if ( ! -d $destdir ) { installer::systemactions::create_directory($destdir); }
			my $destpath = $destdir . $installer::globals::separator . $destfile;
			if ( -f $destpath ) { unlink($destpath); }
			
			# Reading file
			my $scriptfile = installer::files::read_file($sourcefilename);

			# Replacing variables
			my $oldstring = "PRODUCTDIRECTORYNAME";
			replace_variables_in_shellscripts_for_patch($scriptfile, $destpath, $oldstring, $staticpath);
			
			# Saving file
			installer::files::save_file($destpath, $scriptfile);
			
			# Writing file destination into prototype file
			my $line = "i $destfile=" . $destpath . "\n";
			push(@newlines, $line);
		}		
	}
	else
	{
		for ( my $i = 0; $i <= $#installer::globals::solarispatchscripts; $i++ )
		{
			my $line = "i $installer::globals::solarispatchscripts[$i]=" . $path . $installer::globals::solarispatchscripts[$i] . "\n";
			push(@newlines, $line);
		}
	}	
	
	# Including the new lines after the last line starting with "i"

	for ( my $i = 0; $i <= $#{$prototypefile}; $i++ )
	{
		if ( ${$prototypefile}[$i] =~ /^\s*i\s+copyright/ )
		{
			splice(@{$prototypefile}, $i, 1);	# ignoring old copyright text, using patch standard
			next;
		}
		if ( ${$prototypefile}[$i] =~ /^\s*i\s+/ ) { next; }
		splice(@{$prototypefile}, $i, 0, @newlines);
		last;
	}
}

############################################################
# Adding patch infos in pkginfo file
############################################################

sub include_patchinfos_into_pkginfo
{
	my ( $changefile, $filename, $variableshashref ) = @_;
	
	# SUNW_PATCHID=101998-10
	# SUNW_OBSOLETES=114999-01 113999-01
	# SUNW_PKGTYPE=usr
	# SUNW_PKGVERS=1.0
	# SUNW_REQUIRES=126411-01

	my $patchidname = "SOLSPARCPATCHID";
	if ( $installer::globals::issolarisx86build ) { $patchidname = "SOLIAPATCHID"; }

	if ( ! $variableshashref->{$patchidname} ) { installer::exiter::exit_program("ERROR: Variable $patchidname not defined in zip list file!", "include_patchinfos_into_pkginfo"); }

	my $newline = "SUNW_PATCHID=" . $variableshashref->{$patchidname} . "\n";
	add_one_line_into_file($changefile, $newline, $filename);

	my $patchobsoletesname = "SOLSPARCPATCHOBSOLETES";
	if ( $installer::globals::issolarisx86build ) { $patchobsoletesname = "SOLIAPATCHOBSOLETES"; }

	my $obsoletes = "";
	if ( $variableshashref->{$patchobsoletesname} ) { $obsoletes = $variableshashref->{$patchobsoletesname}; }	
	$newline = "SUNW_OBSOLETES=" . $obsoletes . "\n";
	add_one_line_into_file($changefile, $newline, $filename);

	my $patchrequiresname = "SOLSPARCPATCHREQUIRES";
	if ( $installer::globals::issolarisx86build ) { $patchrequiresname = "SOLIAPATCHREQUIRES"; }

	if ( $variableshashref->{$patchrequiresname} )
	{
		my $requires = $variableshashref->{$patchrequiresname};
		$newline = "SUNW_REQUIRES=" . $requires . "\n";
		add_one_line_into_file($changefile, $newline, $filename);
	}
	$newline = "SUNW_PATCH_PROPERTIES=\n"; 
	add_one_line_into_file($changefile, $newline, $filename);
	# $newline = "SUNW_PKGTYPE=usr\n"; 
	# add_one_line_into_file($changefile, $newline, $filename);

	# $newline = "SUNW_PKGVERS=1.0\n"; 
	# add_one_line_into_file($changefile, $newline, $filename);
}

############################################################
# Setting the correct Solaris locales
############################################################

sub get_solaris_language_for_langpack
{
	my ( $onelanguage ) = @_;
	
	my $sollanguage = $onelanguage;
	$sollanguage =~ s/\-/\_/;
	
	if ( $sollanguage eq "de" ) { $sollanguage = "de"; }
	elsif ( $sollanguage eq "en_US" ) { $sollanguage = "en_AU,en_CA,en_GB,en_IE,en_MT,en_NZ,en_US,en_US.UTF-8"; }
	elsif ( $sollanguage eq "es" ) { $sollanguage = "es"; }
	elsif ( $sollanguage eq "fr" ) { $sollanguage = "fr"; }
	elsif ( $sollanguage eq "hu" ) { $sollanguage = "hu_HU"; }
	elsif ( $sollanguage eq "it" ) { $sollanguage = "it"; }
	elsif ( $sollanguage eq "nl" ) { $sollanguage = "nl_BE,nl_NL"; }
	elsif ( $sollanguage eq "pl" ) { $sollanguage = "pl_PL"; }
	elsif ( $sollanguage eq "sv" ) { $sollanguage = "sv"; }
	elsif ( $sollanguage eq "pt" ) { $sollanguage = "pt_PT"; }
	elsif ( $sollanguage eq "pt_BR" ) { $sollanguage = "pt_BR"; }
	elsif ( $sollanguage eq "ru" ) { $sollanguage = "ru_RU"; }
	elsif ( $sollanguage eq "ja" ) { $sollanguage = "ja,ja_JP,ja_JP.PCK,ja_JP.UTF-8"; }
	elsif ( $sollanguage eq "ko" ) { $sollanguage = "ko,ko.UTF-8"; }
	elsif ( $sollanguage eq "zh_CN" ) { $sollanguage = "zh,zh.GBK,zh_CN.GB18030,zh.UTF-8"; }
	elsif ( $sollanguage eq "zh_TW" ) { $sollanguage = "zh_TW,zh_TW.BIG5,zh_TW.UTF-8,zh_HK.BIG5HK,zh_HK.UTF-8"; }
	
	return $sollanguage;
}

############################################################
# Adding language infos in pkginfo file
############################################################

sub include_languageinfos_into_pkginfo
{
	my ( $changefile, $filename, $languagestringref, $onepackage, $variableshashref ) = @_;
	
	# SUNWPKG_LIST=core01
	# SUNW_LOC=de

	my $locallang = $onepackage->{'language'};
	my $solarislanguage = get_solaris_language_for_langpack($locallang);

	my $newline = "SUNW_LOC=" . $solarislanguage . "\n";
	add_one_line_into_file($changefile, $newline, $filename);

	# SUNW_PKGLIST is required, if SUNW_LOC is defined.
	if ( $onepackage->{'pkg_list_entry'} )
	{
		my $packagelistentry = $onepackage->{'pkg_list_entry'};
		installer::packagelist::resolve_packagevariables(\$packagelistentry, $variableshashref, 1);
		$newline = "SUNW_PKGLIST=" . $packagelistentry . "\n";
		add_one_line_into_file($changefile, $newline, $filename);
	}
	else
	{
		# Using default package ooobasis30-core01.
		my $packagelistentry = "%BASISPACKAGEPREFIX%WITHOUTDOTOOOBASEVERSION-core01";
		installer::packagelist::resolve_packagevariables(\$packagelistentry, $variableshashref, 1);
		$newline = "SUNW_PKGLIST=" . $packagelistentry . "\n";
		add_one_line_into_file($changefile, $newline, $filename);	
	}
}

############################################################
# Collecting all files included in patch in 
# @installer::globals::patchfilecollector
############################################################

sub collect_patch_files
{
	my ($file, $packagename, $prefix) = @_;
	
	# $file is the spec file or the prototypefile
	
	$prefix = $prefix . "/";
	my $packagenamestring = "Package " . $packagename . " \:\n";
	push(@installer::globals::patchfilecollector, $packagenamestring);

	for ( my $i = 0; $i <= $#{$file}; $i++ )
	{
		my $line = ${$file}[$i];
	
		if ( $installer::globals::islinuxrpmbuild )
		{
			# %attr(0444,root,root) "/opt/openofficeorg20/program/about.bmp"

			if ( $line =~ /^\s*\%attr\(.*\)\s*\"(.*?)\"\s*$/ )
			{
				my $filename = $1 . "\n";
				$filename =~ s/^\s*\Q$prefix\E//;
				push(@installer::globals::patchfilecollector, $filename);
			}		
		}

		if ( $installer::globals::issolarispkgbuild )
		{
			# f none program/msomrl.rdb=/ab/SRC680/unxsols4.pro/bin/msomrl.rdb 0444 root bin

			if ( $line =~ /^\s*f\s+\w+\s+(.*?)\=/ )
			{
				my $filename = $1 . "\n";
				push(@installer::globals::patchfilecollector, $filename);
			}		
		}
	}

	push(@installer::globals::patchfilecollector, "\n");

}

############################################################
# Including package names into the depend files.
# The package names have to be included into
# packagelist. They are already saved in 
# %installer::globals::dependfilenames.
############################################################

sub put_packagenames_into_dependfile
{
	my ( $file ) = @_;

	for ( my $i = 0; $i <= $#{$file}; $i++ )
	{
		my $line = ${$file}[$i];
		if ( $line =~ /^\s*\w\s+(.*?)\s*$/ )
		{
			my $abbreviation = $1;

            if ( $abbreviation =~ /\%/ ) { installer::exiter::exit_program("ERROR: Could not resolve all properties in Solaris package abbreviation \"$abbreviation\"!", "read_packagemap"); }

			if ( exists($installer::globals::dependfilenames{$abbreviation}) )
			{
                my $packagename = $installer::globals::dependfilenames{$abbreviation};
                if ( $packagename =~ /\%/ ) { installer::exiter::exit_program("ERROR: Could not resolve all properties in Solaris package name \"$packagename\"!", "read_packagemap"); }

				$line =~ s/\s*$//;
				${$file}[$i] = $line . "\t" . $packagename . "\n";
			}
			else
			{
				installer::exiter::exit_program("ERROR: Missing packagename for Solaris package \"$abbreviation\"!", "put_packagenames_into_dependfile");
			}
		}
	}
}

############################################################
# Including the relocatable directory into
# spec file and pkginfo file
# Linux: set topdir in specfile
# Solaris: remove $relocatablepath (/opt/)
# for all objects in prototype file
# and changing "topdir" for Linux
############################################################

sub prepare_packages
{
	my ($loggingdir, $packagename, $staticpath, $relocatablepath, $onepackage, $variableshashref, $filesref, $languagestringref) = @_;
	
	my $filename = "";
	my $newline = "";
	my $newepmdir = $installer::globals::epmoutpath . $installer::globals::separator;
	
	my $localrelocatablepath = $relocatablepath;
	if ( $localrelocatablepath ne "/" ) { $localrelocatablepath =~ s/\/\s*$//; }
		
	if ( $installer::globals::issolarispkgbuild )
	{
		$filename = $packagename . ".pkginfo";
		$newline = "BASEDIR\=" . $localrelocatablepath . "\n";
	}

	if ( $installer::globals::islinuxrpmbuild )
	{
		# if ( $localrelocatablepath =~ /^\s*$/ ) { $localrelocatablepath = "/"; }; # at least the "/"
		$filename =  $packagename . ".spec";
		$newline = "Prefix\:\ " . $localrelocatablepath . "\n";			
	}	

	my $completefilename = $newepmdir . $filename;

	if ( ! -f $completefilename) { installer::exiter::exit_program("ERROR: Did not find file: $completefilename", "prepare_packages"); }
	my $changefile = installer::files::read_file($completefilename);
	if ( $newline ne "" )
	{
		add_one_line_into_file($changefile, $newline, $filename);
		installer::files::save_file($completefilename, $changefile);
	}
	
	# my $newepmdir = $completefilename;
	# installer::pathanalyzer::get_path_from_fullqualifiedname(\$newepmdir);

	# adding new "topdir" and removing old "topdir" in specfile

	if ( $installer::globals::islinuxrpmbuild )
	{
		set_topdir_in_specfile($changefile, $filename, $newepmdir);
		set_autoprovreq_in_specfile($changefile, $onepackage->{'findrequires'}, "$installer::globals::unpackpath" . "/bin");
		set_packager_in_specfile($changefile);
		if ( is_extension_package($changefile) ) { set_prereq_in_specfile($changefile); }
		set_license_in_specfile($changefile, $variableshashref);
		set_tab_into_datafile($changefile, $filesref);
		# check_requirements_in_specfile($changefile);
		installer::files::save_file($completefilename, $changefile);		
		if ( $installer::globals::patch ) { collect_patch_files($changefile, $packagename, $localrelocatablepath); }
	}	

	# removing the relocatable path in prototype file

	if ( $installer::globals::issolarispkgbuild )
	{
		set_revision_in_pkginfo($changefile, $filename, $variableshashref, $packagename);
		set_maxinst_in_pkginfo($changefile, $filename);
		set_solaris_parameter_in_pkginfo($changefile, $filename, $variableshashref);
		if ( $installer::globals::issolarisx86build ) { fix_architecture_setting($changefile); }
		if ( ! $installer::globals::patch ) { set_patchlist_in_pkginfo_for_respin($changefile, $filename, $variableshashref, $packagename); }
		if ( $installer::globals::patch ) { include_patchinfos_into_pkginfo($changefile, $filename, $variableshashref); }
		if (( $onepackage->{'language'} ) && ( $onepackage->{'language'} ne "" ) && ( $onepackage->{'language'} ne "en-US" )) { include_languageinfos_into_pkginfo($changefile, $filename, $languagestringref, $onepackage, $variableshashref); }
		installer::files::save_file($completefilename, $changefile);
	
		my $prototypefilename = $packagename . ".prototype";
		$prototypefilename = $newepmdir . $prototypefilename;
		if (! -f $prototypefilename) { installer::exiter::exit_program("ERROR: Did not find prototype file: $prototypefilename", "prepare_packages"); }

		my $prototypefile = installer::files::read_file($prototypefilename);
		make_prototypefile_relocatable($prototypefile, $relocatablepath);
		set_volatilefile_into_prototypefile($prototypefile, $filesref);
		my $classesstring = set_tab_into_datafile($prototypefile, $filesref);
		if ($classesstring)
		{ 
			include_classes_into_pkginfo($changefile, $classesstring);
			installer::files::save_file($completefilename, $changefile);
		}

		if ( $installer::globals::patch ) { add_scripts_into_prototypefile($prototypefile, $prototypefilename, $languagestringref, $staticpath); }
		
		installer::files::save_file($prototypefilename, $prototypefile);
		if ( $installer::globals::patch ) { collect_patch_files($prototypefile, $packagename, ""); }

		# Adding package names into depend files for Solaris (not supported by epm)
		my $dependfilename = $packagename . ".depend";
		$dependfilename = $newepmdir . $dependfilename;
		if ( -f $dependfilename)
		{
			my $dependfile = installer::files::read_file($dependfilename);
			put_packagenames_into_dependfile($dependfile);		
			installer::files::save_file($dependfilename, $dependfile);
		}
	}

	return $newepmdir;
}

############################################################
# Linux requirement for perl is changed by epm from
# /usr/bin/perl to perl .
# Requires: perl 
############################################################

sub check_requirements_in_specfile
{
	my ( $specfile ) = @_;
	
	for ( my $i = 0; $i <= $#{$specfile}; $i++ )
	{
		if (( ${$specfile}[$i] =~ /^\s*Requires/ ) && ( ${$specfile}[$i] =~ /\bperl\b/ ) && ( ! (  ${$specfile}[$i] =~ /\/usr\/bin\/perl\b/ )))
		{
			my $oldline = ${$specfile}[$i];
			${$specfile}[$i] =~ s/perl/\/usr\/bin\/perl/;
			my $newline = ${$specfile}[$i];

			$oldline =~ s/\s*$//;
			$newline =~ s/\s*$//;
            $installer::logger::Lang->printf("Spec File: Changing content from \"%s\" to \"%s\".\n",
                $oldline, $newline);
		}
	}
}

###############################################################################
# Replacement of PRODUCTINSTALLLOCATION and PRODUCTDIRECTORYNAME in the 
# epm list file.
# The complete rootpath is stored in $installer::globals::rootpath
# or for each package in $onepackage->{'destpath'}
# The static rootpath is stored in $staticpath
# The relocatable path is stored in $relocatablepath
# PRODUCTINSTALLLOCATION is the relocatable part ("/opt") and
# PRODUCTDIRECTORYNAME the static path ("openofficeorg20").
# In standard epm process:
# No usage of package specific variables like $BASEDIR, because
# 1. These variables would be replaced in epm process
# 2. epm version 3.7 does not support relocatable packages
###############################################################################

sub resolve_path_in_epm_list_before_packaging
{
	my ($listfile, $listfilename, $variable, $path) = @_;

	installer::logger::include_header_into_logfile("Replacing variables in epm list file:");

	$path =~ s/\/\s*$//;
	replace_variables_in_shellscripts($listfile, $listfilename, $variable, $path);

}

#################################################################
# Determining the rpm version. Beginning with rpm version 4.0
# the tool to create RPMs is "rpmbuild" and no longer "rpm"
#################################################################

sub determine_rpm_version
{
	my $rpmversion = 0;
	my $rpmout = "";
	my $systemcall = "";
	
	# my $systemcall = "rpm --version |";
	# "rpm --version" has problems since LD_LIBRARY_PATH was removed. Therefore the content of $RPM has to be called.
	# "rpm --version" and "rpmbuild --version" have the same output. Therefore $RPM can be used. Its value 
	# is saved in $installer::globals::rpm
	
	if ( $installer::globals::rpm ne "" )
	{
		$systemcall = "$installer::globals::rpm --version |";
	}
	else
	{
		$systemcall = "rpm --version |";		
	}
	
	open (RPM, "$systemcall");
	$rpmout = <RPM>;
	close (RPM);
	
	if ( $rpmout ne "" )
	{
		$rpmout =~ s/\s*$//g;

        $installer::logger::Lang->printf("Systemcall: %s\n", $systemcall);

		if ( $rpmout eq "" )
        {
            $installer::logger::Lang->printf("ERROR: Could not find file \"rpm\" !\n");
        }
		else
        {
            $installer::logger::Lang->printf("Success: rpm version: %s\n", $rpmout);
        }
	
		if ( $rpmout =~ /(\d+)\.(\d+)\.(\d+)/ ) { $rpmversion = $1; }
		elsif ( $rpmout =~ /(\d+)\.(\d+)/ ) { $rpmversion = $1; }
		elsif ( $rpmout =~ /(\d+)/ ) { $rpmversion = $1; }
		else { installer::exiter::exit_program("ERROR: Unknown format: $rpmout ! Expected: \"a.b.c\", or \"a.b\", or \"a\"", "determine_rpm_version"); }
	}
	
	return $rpmversion;
}

####################################################
# Writing some info about rpm into the log file
####################################################

sub log_rpm_info
{
	my $systemcall = "";
	my $infoline = "";

    $installer::logger::Lang->printf("\n");
    $installer::logger::Lang->printf("Logging rpmrc content using --showrc\n");
    $installer::logger::Lang->printf("\n");

	if ( $installer::globals::rpm ne "" )
	{
		$systemcall = "$installer::globals::rpm --showrc |";
	}
	else
	{
		$systemcall = "rpm --showrc |";		
	}

	my @fullrpmout = ();
	
	open (RPM, "$systemcall");
	while (<RPM>) {push(@fullrpmout, $_); }
	close (RPM);
	
	if ( $#fullrpmout > -1 )
	{
		for ( my $i = 0; $i <= $#fullrpmout; $i++ )
		{
			my $rpmout = $fullrpmout[$i];
			$rpmout =~ s/\s*$//g;

			$infoline = "$rpmout\n";
			$infoline =~ s/error/e_r_r_o_r/gi;  # avoiding log problems
            $installer::logger::Lang->print($infoline);
		}
	}
	else
	{
        $installer::logger::Lang->printf("Problem in systemcall: %s : No return value\n", $systemcall);
	}	

    $installer::logger::Lang->printf("End of logging rpmrc\n");
    $installer::logger::Lang->print("\n");
}

#################################################
# Systemcall to start the packaging process
#################################################

sub create_packages_without_epm
{
	my ($epmdir, $packagename, $includepatharrayref, $allvariables, $languagestringref) = @_;
	
	# Solaris: pkgmk -o -f solaris-2.8-sparc/SUNWso8m34.prototype -d solaris-2.8-sparc
	# Solaris: pkgtrans solaris-2.8-sparc SUNWso8m34.pkg SUNWso8m34
	# Solaris: tar -cf - SUNWso8m34 | gzip > SUNWso8m34.tar.gz

	if ( $installer::globals::issolarispkgbuild )
	{
		my $prototypefile = $epmdir . $packagename . ".prototype";
		if (! -f $prototypefile) { installer::exiter::exit_program("ERROR: Did not find file: $prototypefile", "create_packages_without_epm"); }

		my $destinationdir = $prototypefile;
		installer::pathanalyzer::get_path_from_fullqualifiedname(\$destinationdir);
		$destinationdir =~ s/\/\s*$//;	# removing ending slashes
		
		# my $systemcall = "pkgmk -o -f $prototypefile -d $destinationdir \> /dev/null 2\>\&1";
		my $systemcall = "pkgmk -l 1073741824 -o -f $prototypefile -d $destinationdir 2\>\&1 |";
        $installer::logger::Info->printf("... %s ...\n", $systemcall);

		my $maxpkgmkcalls = 3;

		for ( my $i = 1; $i <= $maxpkgmkcalls; $i++ )
		{
			my @pkgmkoutput = ();

			open (PKGMK, "$systemcall");
			while (<PKGMK>) {push(@pkgmkoutput, $_); }
			close (PKGMK);

			my $returnvalue = $?;	# $? contains the return value of the systemcall

            $installer::logger::Lang->printf("Systemcall (Try %d): %s\n", $i, $systemcall);

			for ( my $j = 0; $j <= $#pkgmkoutput; $j++ )
			{
				if ( $i < $maxpkgmkcalls ) { $pkgmkoutput[$j] =~ s/\bERROR\b/PROBLEM/ig; }
                $installer::logger::Lang->print($pkgmkoutput[$j]);
			}
		
			if ($returnvalue)
			{
                $installer::logger::Lang->printf("Try %s : Could not execute \"%s\"!\n",
                    $i, $systemcall);
				if ( $i == $maxpkgmkcalls ) { installer::exiter::exit_program("ERROR: \"$systemcall\"!", "create_packages_without_epm"); }
			}
			else
			{
                $installer::logger::Info->printf("Success: (Try %d): Executed \"%s\" successfully\n",
                    $i, $systemcall);
                $installer::logger::Lang->printf("Success: (Try %d): Executed \"%s\" successfully\n",
                    $i, $systemcall);
				last;
			}
		}

		# It might be necessary to save uncompressed Solaris packages

		if ( $allvariables->{'JDSBUILD'} )
		{
			if ( ! $installer::globals::jds_language_controlled )
			{
				my $correct_language = installer::worker::check_jds_language($allvariables, $languagestringref);
				$installer::globals::correct_jds_language = $correct_language;
				$installer::globals::jds_language_controlled = 1;
			}

			if ( $installer::globals::correct_jds_language )
			{				
				if ( $installer::globals::saved_packages_path eq "" )
				{
					$packagestempdir = installer::systemactions::create_directories("jds", $languagestringref);
					$installer::globals::saved_packages_path = $packagestempdir;
					push(@installer::globals::jdsremovedirs, $packagestempdir);
				}

				$systemcall = "cd $destinationdir; cp -p -R $packagename $installer::globals::saved_packages_path;";
			 	make_systemcall($systemcall);
                $installer::logger::Info->printf("... %s ...\n", $systemcall);

				# Setting unix rights to "775" for all created directories inside the package,
				# that is saved in temp directory		

				$systemcall = "cd $packagestempdir; find $packagename -type d -exec chmod 775 \{\} \\\;";
                $installer::logger::Info->printf("... %s ...\n", $systemcall);

				$returnvalue = system($systemcall);

                $installer::logger::Lang->printf("Systemcall: %s\n", $systemcall);
		
				if ($returnvalue)
				{
                    $installer::logger::Lang->printf("ERROR: Could not execute \"%s\"!\n", $systemcall);
				}
				else
				{
                    $installer::logger::Lang->printf("Success: Executed \"%s\" successfully!\n", $systemcall);
				}
			}
		}

		# compressing packages
		
		if ( ! $installer::globals::solarisdontcompress )
		{
			my $faspac = "faspac-so.sh";
		
			my $compressorref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$faspac, $includepatharrayref, 0);
			if ($$compressorref ne "")
			{
				# Saving original pkginfo, to set time stamp later
				my $pkginfoorig = "$destinationdir/$packagename/pkginfo";
				my $pkginfotmp = "$destinationdir/$packagename" . ".pkginfo.tmp";
				$systemcall = "cp -p $pkginfoorig $pkginfotmp";
			 	make_systemcall($systemcall);
			
				$faspac = $$compressorref;
                $installer::logger::Lang->printf("Found compressor: %s\n", $faspac);

                $installer::logger::Info->printf("... %s ...\n", $faspac);
				$installer::logger::Lang->add_timestamp("Starting $faspac");

			 	$systemcall = "/bin/sh $faspac -a -q -d $destinationdir $packagename";	 # $faspac has to be the absolute path!
			 	make_systemcall($systemcall);
		 	
			 	# Setting time stamp for pkginfo, because faspac-so.sh changed the pkginfo file, 
			 	# updated the size and checksum, but not the time stamp.
			 	$systemcall = "touch -r $pkginfotmp $pkginfoorig";
			 	make_systemcall($systemcall);
				if ( -f $pkginfotmp ) { unlink($pkginfotmp); }
			
				$installer::logger::Lang->add_timestamp("End of $faspac");
			}
			else
			{
                $installer::logger::Lang->printf("Not found: %s\n", $faspac);
			}
		}

		# Setting unix rights to "775" for all created directories inside the package
		
		$systemcall = "cd $destinationdir; find $packagename -type d -exec chmod 775 \{\} \\\;";
        $installer::logger::Info->printf("... %s ...\n", $systemcall);

		$returnvalue = system($systemcall);

        $installer::logger::Lang->printf("Systemcall: %s\n", $systemcall);
		
		if ($returnvalue)
		{
            $installer::logger::Lang->printf("ERROR: Could not execute \"%s\"!\n", $systemcall);
		}
		else
		{
            $installer::logger::Lang->printf("Success: Executed \"%s\" successfully!\n", $systemcall);
		}
	}

	# Linux: rpm -bb so8m35.spec	( -> dependency check abklemmen? )

	if ( $installer::globals::islinuxrpmbuild )
	{
		my $specfilename = $epmdir . $packagename . ".spec";
		if (! -f $specfilename) { installer::exiter::exit_program("ERROR: Did not find file: $specfilename", "create_packages_without_epm"); }

		# my $rpmcommand = "rpm";
		my $rpmcommand = $installer::globals::rpm;
		my $rpmversion = determine_rpm_version();
		
		# if ( $rpmversion >= 4 ) { $rpmcommand = "rpmbuild"; } 

		# saving globally for later usage
		$installer::globals::rpmcommand = $rpmcommand;
		$installer::globals::rpmquerycommand = "rpm";

		my $target = "";
		if ( $installer::globals::compiler =~ /unxlngi/) { $target = "i586"; }
		elsif ( $installer::globals::compiler =~ /unxlng/) {$target = (POSIX::uname())[4]; }
		
		# rpm 4.6 ignores buildroot tag in spec file

		my $buildrootstring = "";

		if ( $rpmversion >= 4 )
		{
			my $dir = getcwd;
			my $buildroot = $dir . "/" . $epmdir . "buildroot/";
			$buildrootstring = "--buildroot=$buildroot";
			mkdir($buildroot = $dir . "/" . $epmdir . "BUILD/");
		} 

		if ( ! $installer::globals::rpminfologged )
		{
			log_rpm_info();
			$installer::globals::rpminfologged = 1;
		}

		my $systemcall = "$rpmcommand -bb --define \"_unpackaged_files_terminate_build  0\" $specfilename --target $target $buildrootstring 2\>\&1 |";

        $installer::logger::Info->printf("... %s ...\n", $systemcall);

		my $maxrpmcalls = 3;
		my $rpm_failed = 0;

		for ( my $i = 1; $i <= $maxrpmcalls; $i++ )
		{
			my @rpmoutput = ();

			open (RPM, "$systemcall");
			while (<RPM>) {push(@rpmoutput, $_); }
			close (RPM);

			my $returnvalue = $?;	# $? contains the return value of the systemcall

            $installer::logger::Lang->printf("Systemcall (Try %d): %s\n", $i, $systemcall);

			for ( my $j = 0; $j <= $#rpmoutput; $j++ )
			{
				# if ( $i < $maxrpmcalls ) { $rpmoutput[$j] =~ s/\bERROR\b/PROBLEM/ig; }
				$rpmoutput[$j] =~ s/\bERROR\b/PROBLEM/ig;
                $installer::logger::Lang->print($rpmoutput[$j]);
			}
			
			if ($returnvalue)
			{
                $installer::logger::Lang->printf("Try %d : Could not execute \"%s\"!\n", $i, $systemcall);
				$rpm_failed = 1;
			}
			else
			{
                $installer::logger::Info->printf("Success (Try %d): Executed \"%s\" successfully!\n", $i, $systemcall);
                $installer::logger::Lang->printf("Success (Try %d): Executed \"%s\" successfully!\n", $i, $systemcall);
				$rpm_failed = 0;
				last;
			}
		}
		
		if ( $rpm_failed )
		{
			# Because of the problems with LD_LIBARY_PATH, a direct call of local "rpm" or "rpmbuild" might be successful
			my $rpmprog = "";
			if ( -f "/usr/bin/rpmbuild" ) { $rpmprog = "/usr/bin/rpmbuild"; }
			elsif ( -f "/usr/bin/rpm" ) { $rpmprog = "/usr/bin/rpm"; }
			
			if ( $rpmprog ne "" )
			{
                $installer::logger::Info->printf("... %s ...\n", $rpmprog);

				my $helpersystemcall = "$rpmprog -bb $specfilename --target $target $buildrootstring 2\>\&1 |";

				my @helperrpmoutput = ();

				open (RPM, "$helpersystemcall");
				while (<RPM>) {push(@helperrpmoutput, $_); }
				close (RPM);

				my $helperreturnvalue = $?;	# $? contains the return value of the systemcall

                $installer::logger::Lang->printf("\n");
                $installer::logger::Lang->printf("Last try: Using %s directly (problem with LD_LIBARY_PATH)\n",
                    $rpmprog);

                $installer::logger::Lang->printf("\n");
                $installer::logger::Lang->printf("Systemcall: %s\n", $helpersystemcall);

                foreach my $line (@helperrpmoutput)
                {
                    $installer::logger::Lang->print($helperrpmoutput[$j]);
                }
			
				if ($helperreturnvalue)
				{
                    $installer::logger::Lang->printf("Could not execute \"%s\"!\n", $helpersystemcall);
				}
				else
				{
                    $installer::logger::Lang->printf("Success: Executed \"%s\" successfully!\n", $helpersystemcall);
                    $installer::logger::Info->printf("Success: Executed \"%s\" successfully!\n", $helpersystemcall);
					$rpm_failed = 0;
				}
			}
			
			# Now it is really time to exit this packaging process, if the error still occurs
			if ( $rpm_failed ) { installer::exiter::exit_program("ERROR: \"$systemcall\"!", "create_packages_without_epm"); }
		}
	}
}

#################################################
# Removing all temporary files created by epm
#################################################

sub remove_temporary_epm_files
{
	my ($epmdir, $loggingdir, $packagename) = @_;

	# saving the files into the loggingdir

	if ( $installer::globals::issolarispkgbuild )
	{
		my @extensions = ();
		push(@extensions, ".pkginfo");
		push(@extensions, ".prototype");
		push(@extensions, ".postinstall");
		push(@extensions, ".postremove");
		push(@extensions, ".preinstall");
		push(@extensions, ".preremove");
		push(@extensions, ".depend");
		
		for ( my $i = 0; $i <= $#extensions; $i++ )
		{
			my $removefile = $epmdir . $packagename . $extensions[$i];
			my $destfile = $loggingdir . $packagename . $extensions[$i] . ".log";

			if (! -f $removefile) { next; }

			my $systemcall = "mv -f $removefile $destfile";
			system($systemcall);	 # ignoring the return value
            $installer::logger::Lang->printf("Systemcall: %s\n", $systemcall);
		}
	}

	if ( $installer::globals::islinuxrpmbuild )
	{
		my $removefile = $epmdir . $packagename . ".spec";
		my $destfile = $loggingdir . $packagename . ".spec.log";

		 # if (! -f $removefile) { next; }

		my $systemcall = "mv -f $removefile $destfile";
		system($systemcall);	 # ignoring the return value
        $installer::logger::Lang->printf("Systemcall: %s\n", $systemcall);

		# removing the directory "buildroot"
	
		my $removedir = $epmdir . "buildroot";
	
		$systemcall = "rm -rf $removedir";

        $installer::logger::Info->printf("... %s ...\n", $systemcall);
        
		my $returnvalue = system($systemcall);

		$removedir = $epmdir . "BUILD";
	
		$systemcall = "rm -rf $removedir";

        $installer::logger::Info->printf("... %s ...\n", $systemcall);

		$returnvalue = system($systemcall);

        $installer::logger::Lang->printf("Systemcall: %s\n", $systemcall);
		
        if ($returnvalue)
        {
            $installer::logger::Lang->printf("ERROR: Could not execute \"%s\"!\n", $systemcall);
        }
        else
        {
            $installer::logger::Lang->printf("Success: Executed \"%s\" successfully!\n", $systemcall);
        }
	}	
}

######################################################
# Making the systemcall
######################################################

sub make_systemcall
{
	my ($systemcall) = @_;

	my $returnvalue = system($systemcall);

    $installer::logger::Lang->printf("Systemcall: %s\n", $systemcall);
		
	if ($returnvalue)
	{
        $installer::logger::Lang->printf("ERROR: Could not execute \"%s\"!\n", $systemcall);
	}
	else
	{
        $installer::logger::Lang->printf("Success: Executed \"%s\" successfully!\n", $systemcall);
	}
}	

###########################################################
# Creating a better directory structure in the solver.
###########################################################

sub create_new_directory_structure
{
	my ($newepmdir) = @_;
	
	my $newdir = $installer::globals::epmoutpath;
	
	if ( $installer::globals::islinuxrpmbuild )
	{
		my $rpmdir;
                my $machine = "";
		if ( $installer::globals::compiler =~ /unxlngi/) { 
                    $rpmdir = "$installer::globals::epmoutpath/RPMS/i586";
                }
		elsif ( $installer::globals::compiler =~ /unxlng/) { 
                    $machine = (POSIX::uname())[4]; 
                    $rpmdir = "$installer::globals::epmoutpath/RPMS/$machine"; 
                }
                else { installer::exiter::exit_program("ERROR: rpmdir undefined !", "create_new_directory_structure"); }

		my $systemcall = "mv $rpmdir/* $newdir";	# moving the rpms into the directory "RPMS"

		my $returnvalue = system($systemcall);

        $installer::logger::Lang->printf("Systemcall: %s\n", $systemcall);
		
		if ($returnvalue)
		{
            $installer::logger::Lang->printf("ERROR: Could not move content of \"%s\" to \"%s\"!\n",
                $rpmdir,$newdir);
		}
		else
		{
            $installer::logger::Lang->printf("Success: Moved content of \"%s\" to \"%s\"!\n",
                $rpmdir, $newdir);
		}

		# and removing the empty directory

		if ( $machine ne "" )
		{
			installer::systemactions::remove_empty_directory("$installer::globals::epmoutpath/RPMS/$machine");
		}
		installer::systemactions::remove_empty_directory("$installer::globals::epmoutpath/RPMS/x86_64");
		installer::systemactions::remove_empty_directory("$installer::globals::epmoutpath/RPMS/i586");
		installer::systemactions::remove_empty_directory("$installer::globals::epmoutpath/RPMS/i386");
		installer::systemactions::remove_empty_directory("$installer::globals::epmoutpath/RPMS");

	}
		
	# Setting unix rights to "775" for $newdir ("RPMS" or "packages")
		
	my $localcall = "chmod 775 $newdir \>\/dev\/null 2\>\&1";
	my $callreturnvalue = system($localcall);

    $installer::logger::Lang->printf("Systemcall: %s\n", $localcall);
		
	if ($callreturnvalue)
	{
        $installer::logger::Lang->printf("ERROR: Could not execute \"%s\"!\n", $localcall);
	}
	else
	{
        $installer::logger::Lang->printf("Success: Executed \"%s\" successfully!\n", $localcall);
	}
}

######################################################
# Collect modules with product specific styles.
######################################################

sub collect_modules_with_style
{
	my ($style, $modulesarrayref) = @_;

	my @allmodules = ();

	for ( my $i = 0; $i <= $#{$modulesarrayref}; $i++ )
	{				
		my $onemodule = ${$modulesarrayref}[$i];
		my $styles = "";
		if ( $onemodule->{'Styles'} ) { $styles = $onemodule->{'Styles'}; }
		if ( $styles =~ /\b\Q$style\E\b/ )
		{
			push(@allmodules, $onemodule);
		}
	}
	
	return \@allmodules;
}

######################################################
# Remove modules without packagecontent.
######################################################

sub remove_modules_without_package
{
	my ($allmodules) = @_;
	
	my @allmodules = ();

	for ( my $i = 0; $i <= $#{$allmodules}; $i++ )
	{				
		my $onemodule = ${$allmodules}[$i];
		my $packagename = "";
		if ( $onemodule->{'PackageName'} ) { $packagename = $onemodule->{'PackageName'}; }
		if ( $packagename ne "" )
		{
			push(@allmodules, $onemodule);
		}
	}
	
	return \@allmodules;
}

######################################################
# Unpacking tar.gz file and setting new packagename.
######################################################

sub unpack_tar_gz_file
{
	my ($packagename, $destdir) = @_;

	my $newpackagename = "";

	if ( $packagename =~ /\.tar\.gz\s*$/ )
	{
		# Collecting all packages in directory "packages"
		my $oldcontent = installer::systemactions::read_directory($destdir);

		# unpacking gunzip
		my $systemcall = "cd $destdir; cat $packagename | gunzip | tar -xf -";
		make_systemcall($systemcall);
						
		# deleting the tar.gz files
		$systemcall = "cd $destdir; rm -f $packagename";
		make_systemcall($systemcall);
		
		# Finding new content -> that is the package name
		my ($newcontent, $allcontent ) = installer::systemactions::find_new_content_in_directory($destdir, $oldcontent);
		$newpackagename = ${$newcontent}[0];
		installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$newpackagename);
	}
	
	if ( $newpackagename ne "" ) { $packagename = $newpackagename; }
	
	return $packagename;
}

######################################################
# Copying files of child projects.
######################################################

sub copy_childproject_files
{
	my ($allmodules, $sopackpath, $destdir, $modulesarrayref, $allvariables, $subdir, $includepatharrayref, $use_sopackpath) = @_;
	
	for ( my $i = 0; $i <= $#{$allmodules}; $i++ )
	{
		my $localdestdir = $destdir;				
		my $onemodule = ${$allmodules}[$i];
		my $packagename = $onemodule->{'PackageName'};
		my $sourcefile = "";
		if ( $use_sopackpath )
		{
			$sourcefile = $sopackpath . $installer::globals::separator . $installer::globals::compiler . $installer::globals::separator . $subdir . $installer::globals::separator . $packagename;
		}
		else
		{
			my $sourcepathref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$packagename, $includepatharrayref, 1);
			$sourcefile = $$sourcepathref;
		}

		if ( ! -f $sourcefile ) { installer::exiter::exit_program("ERROR: File not found: $sourcefile ($packagename) !", "copy_childproject_files"); }
		if ( $onemodule->{'Subdir'} )
		{
			$localdestdir = $localdestdir . $installer::globals::separator . $onemodule->{'Subdir'};
			if ( ! -d $localdestdir ) { installer::systemactions::create_directory($localdestdir); }
		}
		installer::systemactions::copy_one_file($sourcefile, $localdestdir);
		# Solaris: unpacking tar.gz files and setting new packagename
		if ( $installer::globals::issolarispkgbuild ) { $packagename = unpack_tar_gz_file($packagename, $localdestdir); }

		if (( $installer::globals::isxpdplatform ) && ( $allvariables->{'XPDINSTALLER'} ))
		{
			installer::xpdinstaller::create_xpd_file_for_childproject($onemodule, $localdestdir, $packagename, $allvariableshashref, $modulesarrayref);
		}
	}				

}

######################################################
# Copying files for system integration.
######################################################

sub copy_and_unpack_tar_gz_files
{
	my ($sourcefile, $destdir) = @_;

	my $systemcall = "cd $destdir; cat $sourcefile | gunzip | tar -xf -";
	make_systemcall($systemcall);		
}

######################################################
# Including child packages into the
# installation set.
######################################################

sub put_childprojects_into_installset
{
	my ($newdir, $allvariables, $modulesarrayref, $includepatharrayref) = @_;

	my $infoline = "";

	my $sopackpath = "";
	if ( $ENV{'SO_PACK'} ) { $sopackpath  = $ENV{'SO_PACK'}; }
	else { installer::exiter::exit_program("ERROR: Environment variable SO_PACK not set!", "put_childprojects_into_installset"); }

	my $destdir = "$newdir";

	# adding Java

	my $sourcefile = "";

	# Finding the modules defined in scp (with flag JAVAMODULE, ADAMODULE, ...)
	# Getting name of package from scp-Module
	# Copy file into installation set
	# Create xpd file and put it into xpd directory
	# xpd file has to be created completely from module and package itself (-> no packagelist!)

	if ( $allvariables->{'JAVAPRODUCT'} )
	{
		# Collect all modules with flag "JAVAMODULE"
		my $allmodules = collect_modules_with_style("JAVAMODULE", $modulesarrayref);
		$allmodules = remove_modules_without_package($allmodules);
		copy_childproject_files($allmodules, $sopackpath, $destdir, $modulesarrayref, $allvariables, "jre", $includepatharrayref, 1);
	}

	# Adding additional required packages (freetype).
	# This package names are stored in global array @installer::globals::requiredpackages
	
	if ( $allvariables->{'ADDREQUIREDPACKAGES'} )
	{
		# Collect all modules with flag "REQUIREDPACKAGEMODULE"
		my $allmodules = collect_modules_with_style("REQUIREDPACKAGEMODULE", $modulesarrayref);
		$allmodules = remove_modules_without_package($allmodules);
		copy_childproject_files($allmodules, $sopackpath, $destdir, $modulesarrayref, $allvariables, "requiredpackages", $includepatharrayref, 1);
	}

	# Collect all modules with flag "USERLANDMODULE"
	my $alluserlandmodules = collect_modules_with_style("USERLANDMODULE", $modulesarrayref);
	$alluserlandmodules = remove_modules_without_package($alluserlandmodules);
	copy_childproject_files($alluserlandmodules, $sopackpath, $destdir, $modulesarrayref, $allvariables, "", $includepatharrayref, 0);

}

######################################################
# Checking whether the new content is a directory and 
# not a package. If it is a directory, the complete 
# content of the directory has to be added to the
# array newcontent.
######################################################

sub control_subdirectories
{
	my ($content, $subdir) = @_;
	
	my @newcontent = ();

	for ( my $i = 0; $i <= $#{$content}; $i++ )
	{				
		if ( -d ${$content}[$i] )
		{
			$subdir = ${$content}[$i];
			installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$subdir);
			my $allpackages = installer::systemactions::read_directory(${$content}[$i]);
			for ( my $j = 0; $j <= $#{$allpackages}; $j++ )
			{
				# Currently only Linux rpm is supported, debian packages cannot be installed via xpd installer
				if (( $installer::globals::islinuxbuild ) && ( ! ( ${$allpackages}[$j] =~ /\.rpm\s*$/ ))) { next; }
				push(@newcontent, ${$allpackages}[$j]);
			}
		}
		else
		{
			push(@newcontent, ${$content}[$i]);
		}
	}

	return (\@newcontent, $subdir);
}

######################################################
# Including the system integration files into the
# installation sets.
######################################################

sub put_systemintegration_into_installset
{
	my ($newdir, $includepatharrayref, $allvariables, $modulesarrayref) = @_;

	my $destdir = $newdir;

	# adding System integration files

	my $sourcefile = "";

	# Finding the modules defined in scp (with flag SYSTEMMODULE)
	# Getting name of package from scp-Module
	# Search package in list off all include files
	# Copy file into installation set and unpack it (always tar.gz)	
	# Create xpd file and put it into xpd directory
	# tar.gz can contain a different number of packages -> automatically create hidden sub modules
	# xpd file has to be created completely from module and package itself (-> no packagelist!)

	# Collect all modules with flag "SYSTEMMODULE"
	my $allmodules = collect_modules_with_style("SYSTEMMODULE", $modulesarrayref);
	$allmodules = remove_modules_without_package($allmodules);

	for ( my $i = 0; $i <= $#{$allmodules}; $i++ )
	{	
		my $onemodule = ${$allmodules}[$i];
		my $packagetarfilename = $onemodule->{'PackageName'};

        $installer::logger::Lang->printf("Including into installation set: %s\n", $packagetarfilename);

		my $sourcepathref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$packagetarfilename, $includepatharrayref, 1);
		if ( $$sourcepathref eq "" ) { installer::exiter::exit_program("ERROR: Source path not found for $packagetarfilename!", "copy_systemintegration_files"); }

		# Collecting all packages in directory "packages" or "RPMS"
		my $oldcontent = installer::systemactions::read_directory($destdir);

		copy_and_unpack_tar_gz_files($$sourcepathref, $destdir);
		
		# Finding new content -> that is the package name
		my ($newcontent, $allcontent ) = installer::systemactions::find_new_content_in_directory($destdir, $oldcontent);

		# special handling, if new content is a directory
		my $subdir = "";
		if ( ! $installer::globals::issolarispkgbuild ) { ($newcontent, $subdir) = control_subdirectories($newcontent); }
		
		# Adding license content into Solaris packages
		if (( $installer::globals::issolarispkgbuild ) && ( $installer::globals::englishlicenseset ) && ( ! $variableshashref->{'NO_LICENSE_INTO_COPYRIGHT'} )) { installer::worker::add_license_into_systemintegrationpackages($destdir, $newcontent); }

		if (( $installer::globals::isxpdplatform ) && ( $allvariables->{'XPDINSTALLER'} ))
		{
			installer::xpdinstaller::create_xpd_file_for_systemintegration($onemodule, $newcontent, $modulesarrayref, $subdir);
		}
	}
}

######################################################
# Analyzing the Unix installation path.
# From the installation path /opt/openofficeorg20
# is the part /opt relocatable and the part
# openofficeorg20 static.
######################################################

sub analyze_rootpath
{
	my ($rootpath, $staticpathref, $relocatablepathref, $allvariables) = @_;
	
	$rootpath =~ s/\/\s*$//;	# removing ending slash

	##############################################################
	# Version 1: "/opt" is variable and "openofficeorg20" fixed
	##############################################################
	
	# my $staticpath = $rootpath;
	# installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$staticpath);
	# $$staticpathref = $staticpath;				# will be "openofficeorg20"

	# my $relocatablepath = $rootpath;
	# installer::pathanalyzer::get_path_from_fullqualifiedname(\$relocatablepath);
	# $$relocatablepathref = $relocatablepath;		# will be "/opt/"		

	##############################################################
	# Version 2: "/opt/openofficeorg20" is variable and "" fixed
	##############################################################

	# if ( $$relocatablepathref eq "" )	# relocatablepath is not defined in package list
	# {
	#	$$staticpathref = "";	# will be ""
	#	$$relocatablepathref = $rootpath . "\/"; # relocatable path must end with "/", will be "/opt/openofficeorg20/"
	#	# setting the static path to the hostname of the directory with style OFFICEDIRECTORY
	#	if ( $allvariables->{'SETSTATICPATH'} ) { $$staticpathref = $installer::globals::officedirhostname; }
	#			
	# }
	# else	# relocatablepath is defined in package list
	# {	
	#	$$relocatablepathref =~ s/\/\s*$//;			# removing ending slash
	#	$$relocatablepathref = $$relocatablepathref . "\/";	# relocatable path must end with "/"	
	#	my $staticpath = $rootpath;
	#	$staticpath =~ s/\Q$$relocatablepathref\E//;
	#	$staticpath =~ s/\/\s*$//;
	#	$$staticpathref = $staticpath;
	# }

	##############################################################
	# Version 3: "/" is variable and "/opt/openofficeorg20" fixed
	##############################################################

	$$relocatablepathref = "/";
	# Static path has to contain the office directory name. This is replaced in shellscripts.
	$$staticpathref = $rootpath . $installer::globals::separator . $installer::globals::officedirhostname;
	# For RPM version 3.x it is required, that Prefix is not "/" in spec file. In this case --relocate will not work,
	# because RPM 3.x says, that the package is not relocatable. Therefore we have to use Prefix=/opt and for 
	# all usages of --relocate this path has to be on both sides of the "=": --relocate /opt=<myselectdir>/opt .
	if ( $installer::globals::islinuxrpmbuild )
	{ 
		$$relocatablepathref = $rootpath . "\/"; # relocatable path must end with "/", will be "/opt/"
		$$staticpathref = $installer::globals::officedirhostname; # to be used as replacement in shell scripts
	}

	if ( $installer::globals::islinuxdebbuild )
	{
		$$relocatablepathref = "";
		# $$staticpathref is already "/opt/openoffice.org3", no additional $rootpath required.
		# $$staticpathref = $rootpath . $installer::globals::separator . $$staticpathref;  # no relocatibility for Debian
	}
		
}

######################################################
# Including license and readme into
# Unix installation sets.
######################################################

sub put_installsetfiles_into_installset
{
	my ($destdir) = @_;
	
	# All files for the installation set are saved in the global
	# array @installer::globals::installsetfiles
	
	for ( my $i = 0; $i <= $#installer::globals::installsetfiles; $i++ )
	{
		my $onefile = $installer::globals::installsetfiles[$i];
		my $sourcefile = $onefile->{'sourcepath'};
		my $destfile = "";
		if ( $installer::globals::addjavainstaller ) { $destfile = $onefile->{'Name'}; }
		else { $destfile = $destdir . $installer::globals::separator . $onefile->{'Name'}; }
		installer::systemactions::copy_one_file($sourcefile, $destfile);

        $installer::logger::Lang->printf("Adding to installation set \"%s\" from source \"%s\".\n",
            $destfile, $sourcefile);
	}
}

######################################################
# Replacing one variable in patchinfo file 
######################################################

sub replace_one_variable_in_file
{
	my ( $file, $placeholder, $value ) = @_;

	for ( my $i = 0; $i <= $#{$file}; $i++ )
	{
		${$file}[$i] =~ s/$placeholder/$value/g;
	}
}

######################################################
# Setting variables in the patchinfo file 
######################################################

sub set_patchinfo
{
	my ( $patchinfofile, $patchid, $allvariables ) = @_;

	# Setting: PATCHIDPLACEHOLDER and ARCHITECTUREPLACEHOLDER and PATCHCORRECTSPLACEHOLDER
	
	replace_one_variable_in_file($patchinfofile, "PATCHIDPLACEHOLDER", $patchid);
	
	my $architecture = "";
	if ( $installer::globals::issolarissparcbuild ) { $architecture = "sparc"; } 
	if ( $installer::globals::issolarisx86build ) { $architecture = "i386"; }
	
	replace_one_variable_in_file($patchinfofile, "ARCHITECTUREPLACEHOLDER", $architecture);
	
	if ( ! $allvariables->{'SOLARISPATCHCORRECTS'} ) { installer::exiter::exit_program("ERROR: No setting for PATCH_CORRECTS in zip list file!", "set_patchinfo"); }
	my $patchcorrects = $allvariables->{'SOLARISPATCHCORRECTS'};

	replace_one_variable_in_file($patchinfofile, "PATCHCORRECTSPLACEHOLDER", $patchcorrects);
	
	# Setting also PATCH_REQUIRES in patch info file, if entry in zip list file exists
	my $requiresstring = "";
	if ( $installer::globals::issolarissparcbuild ) { $requiresstring = "SOLSPARCPATCHREQUIRES"; } 
	if ( $installer::globals::issolarisx86build ) { $requiresstring = "SOLIAPATCHREQUIRES"; }

	if ( $allvariables->{$requiresstring} )
	{
		my $newline = "PATCH_REQUIRES=\"" . $allvariables->{$requiresstring} . "\"" . "\n";
		push(@{$patchinfofile}, $newline);
	}
}

######################################################
# Finalizing patch: Renaming directory and 
# including additional patch files.
######################################################

sub finalize_patch
{
	my ( $newepmdir, $allvariables ) = @_;

	my $patchidname = "SOLSPARCPATCHID";
	if ( $installer::globals::issolarisx86build ) { $patchidname = "SOLIAPATCHID"; }

	if ( ! $allvariables->{$patchidname} ) { installer::exiter::exit_program("ERROR: Variable $patchidname not defined in zip list file!", "finalize_patch"); }
	my $patchid = $allvariables->{$patchidname};
	installer::systemactions::rename_directory($newepmdir, $patchid);
	
	# Copying all typical patch files into the patch directory
	# All patch file names are stored in @installer::globals::solarispatchfiles
	# Location of the file is $installer::globals::patchincludepath
	
	my $sourcepath = $installer::globals::patchincludepath;
	$sourcepath =~ s/\/\s*$//;
	
	for ( my $i = 0; $i <= $#installer::globals::solarispatchfiles; $i++ )
	{
		my $sourcefile = $sourcepath . $installer::globals::separator . $installer::globals::solarispatchfiles[$i];
		my $destfile = $patchid . $installer::globals::separator . $installer::globals::solarispatchfiles[$i];
		installer::systemactions::copy_one_file($sourcefile, $destfile);
	}	

	# And editing the patchinfo file
	
	my $patchinfofilename = $patchid . $installer::globals::separator . "patchinfo";
	my $patchinfofile = installer::files::read_file($patchinfofilename);
	set_patchinfo($patchinfofile, $patchid, $allvariables);
	installer::files::save_file($patchinfofilename, $patchinfofile);	
}

######################################################
# Finalizing Linux patch: Renaming directory and 
# including additional patch files.
######################################################

sub finalize_linux_patch
{
	my ( $newepmdir, $allvariables, $includepatharrayref ) = @_;
	
	# Copying the setup into the patch directory
	# and including the list of RPMs into it	

	print "... creating patch setup ...\n";

	installer::logger::include_header_into_logfile("Creating Linux patch setup:");

	# find and read setup script template
	
	my $scriptfilename = "linuxpatchscript.sh";
	my $scriptref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$scriptfilename, $includepatharrayref, 0);
	if ($$scriptref eq "") { installer::exiter::exit_program("ERROR: Could not find patch script template $scriptfilename!", "finalize_linux_patch"); }
	my $scriptfile = installer::files::read_file($$scriptref);

    $installer::logger::Lang->printf("Found  script file %s: %s \n", $scriptfilename, $$scriptref);

	# Collecting all RPMs in the patch directory

	my $fileextension = "rpm";
	my $rpmfiles = installer::systemactions::find_file_with_file_extension($fileextension, $newepmdir);
	if ( ! ( $#{$rpmfiles} > -1 )) { installer::exiter::exit_program("ERROR: Could not find rpm in directory $newepmdir!", "finalize_linux_patch"); } 
	for ( my $i = 0; $i <= $#{$rpmfiles}; $i++ ) { installer::pathanalyzer::make_absolute_filename_to_relative_filename(\${$rpmfiles}[$i]); }

#	my $installline = "";
#
#	for ( my $i = 0; $i <= $#{$rpmfiles}; $i++ )
#	{
#		$installline = $installline . "  rpm --prefix \$PRODUCTINSTALLLOCATION -U $newepmdir/${$rpmfiles}[$i]\n";
#	}	
#
#	$installline =~ s/\s*$//;
#
#	for ( my $j = 0; $j <= $#{$scriptfile}; $j++ )
#	{
#		${$scriptfile}[$j] =~ s/INSTALLLINES/$installline/;
#	}

	# Searching packagename containing -core01
	my $found_package = 0;
	my $searchpackagename = "";
	for ( my $i = 0; $i <= $#{$rpmfiles}; $i++ )
	{
		if ( ${$rpmfiles}[$i] =~ /-core01-/ )
		{
			$searchpackagename = ${$rpmfiles}[$i];
			$found_package = 1;
			if ( $searchpackagename =~ /^\s*(.*?-core01)-.*/ ) { $searchpackagename = $1; }
			last;	
		}
	}
	
	if ( ! $found_package ) { installer::exiter::exit_program("ERROR: No package containing \"-core01\" found in directory \"$newepmdir\"", "finalize_linux_patch"); }

	# Replacing the searchpackagename
	for ( my $j = 0; $j <= $#{$scriptfile}; $j++ ) { ${$scriptfile}[$j] =~ s/SEARCHPACKAGENAMEPLACEHOLDER/$searchpackagename/; }

	# Setting the PRODUCTDIRECTORYNAME to $installer::globals::officedirhostname
	for ( my $j = 0; $j <= $#{$scriptfile}; $j++ ) { ${$scriptfile}[$j] =~ s/PRODUCTDIRECTORYNAME/$installer::globals::officedirhostname/; }

	# Replacing the productname
	my $productname = $allvariables->{'PRODUCTNAME'};
	$productname = lc($productname);
	$productname =~ s/ /_/g;	# abc office -> abc_office

    $installer::logger::Lang->printf("Adding productname %s into Linux patch script\n", $productname);

	for ( my $j = 0; $j <= $#{$scriptfile}; $j++ ) { ${$scriptfile}[$j] =~ s/PRODUCTNAMEPLACEHOLDER/$productname/; }

	# Saving the file

	my $newscriptfilename = "setup"; # $newepmdir . $installer::globals::separator . "setup";
	installer::files::save_file($newscriptfilename, $scriptfile);	

    $installer::logger::Lang->printf("Saved Linux patch setup %s\n", $newscriptfilename);

	# Setting unix rights 755
	my $localcall = "chmod 775 $newscriptfilename \>\/dev\/null 2\>\&1";
	system($localcall);
}

1;
