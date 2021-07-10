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



package RepoRevision;

sub DetectRevisionIdFromFile ($)
{
	my $path = shift;
	my $id = undef;

	open( my $fh, '<', $path ) || return undef;
	$id = <$fh>;
	close $fh;
	return $id;
}

sub DetectRevisionIdFromGit ($)
{
	my $path = shift;
	my $id = undef;

	$id = `git log -1 --pretty=format:%h --abbrev=10`;
	return $id;
}

sub DetectRevisionIdFromSVN ($)
{
	my $path = shift;
	my $id = undef;

	open my $proc, "cd $path && svn info 2>\&1 |";
	while (<$proc>)
	{
		if (/svn: E155007:/ || /svn: '.' is not a working copy/)
		{
			# Not in an SVN repository.
			return undef;
		}
		else
		{
			if (/Last Changed Rev:\s+([0-9]+)/)
			{
				$id = $1;
				last;
			}
		}
	}
	close $proc;
	return $id;
}


sub DetectRevisionId ($)
{
	my $path = shift;
	my $id = undef;

	#NOTE: Magic cookie file 'reporevision.lst' created by aoo_srcrelease
	$id = DetectRevisionIdFromFile ("$ENV{'SOLARENV'}/inc/reporevision.lst");
	if ($id) { return $id };

	my $NotGit = `cd $path && git rev-parse --git-dir > /dev/null 2>&1`;
	if (!$NotGit || -d ".git" || -d "$path/.git")
	{
		$id = DetectRevisionIdFromGit ($path);
	}
	else
	{
		$id = DetectRevisionIdFromSVN ($path);
	}

	if (!$id) { $id = "unknown-rev" };
	return $id;
}

1;
