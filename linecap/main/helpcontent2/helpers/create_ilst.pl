:
eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

# creates the ilst control file for the
# help specific images

use File::Find;

$params = join "|", "",@ARGV,"";
($params =~ /-dir/) ? ($startdir = $params) =~ (s/.*-dir=([^\|]*).*$/$1/gs) : (($startdir = `pwd`) =~ s/\n//gs);
($params =~ /-pre/) ? ($pre = $params) =~ (s/.*-pre=([^\|]*).*$/$1/gs) : ($pre = "helpimg");

my $startdir_regexp = quotemeta($startdir);


if ( -d $startdir ) {
	find(sub{push @files, $File::Find::name if (($File::Find::name=~/\.png$/));},$startdir);
	foreach ( @files ) { s#.*$startdir_regexp[\\/]##; };
	for (sort(@files)) {
		print "%GLOBALRES%/$pre/$_\n";
	}
} else {
	&terminate("Cannot find $startdir.");
}


sub terminate {
	$err = shift;
	printf STDERR "$err\n\n";
	$msg = <<"MSG";
create_ilst.pl -dir=directory [-pre=string] 
-dir  root directory for the help images to
	be searched (default=current dir)
	Only *.png will be found.
-pre  define directory prefix (default=helpimg)
  
MSG
	die "$msg\n";
}
