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



package SvnRevision;

#old SVN code unchanged
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
			$id = DetectRevisionIdFromGit($path);
			last;
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
	#test if path points to a git repository. if true return is 0 else positive number.
	my $isNotGit= `[ -d .git ] || git rev-parse --git-dir > /dev/null 2>&1`;
	if ($isNotGit)
	{
		$id = DetectRevisionIdFromSVN ($path);
	}
	else
	{
		#returns directly the hash of the current checkout.
		$id = `git log -1 --pretty=format:%h --abbrev=10`;
	}

	return $id;
}

1;
