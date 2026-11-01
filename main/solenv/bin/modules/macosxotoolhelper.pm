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



package macosxotoolhelper;
require Exporter;
our @ISA = Exporter;
our @EXPORT = otoolD;

use File::Basename;
$::CC_PATH=(fileparse( $ENV{"CC"}))[1];

sub otoolD($) {
    my ($file) = @_;
    my $call = "otool -D $file";
    open(IN, "-|", $call) or die "cannot $call";
    my @lines = <IN>;
    close(IN);

    # A fat/universal binary (e.g. a Homebrew-installed library bundling
    # x86_64 and arm64 slices) makes otool -D print one
    # "<path> (architecture <arch>):" stanza per slice instead of the
    # single "<path>:" header a thin binary gets. Every slice of the same
    # library reports the same install name, so take it from the first
    # stanza and ignore the rest.
    #
    # A slice with no install name (e.g. a loadable module/bundle, such as
    # a PKCS#11 provider that is only ever dlopen()ed) prints its header
    # with nothing after it -- callers rely on getting undef back for that
    # case, same as otool -D on a thin file with no install name.
    my $header_re = qr/^\Q$file\E(?: \(architecture [^)]+\))?:\n$/;
    my @names;
    my $i = 0;
    while ($i < @lines) {
        $lines[$i] =~ $header_re or
            die "unexpected otool -D output (\"$lines[$i]\", expecting \"$file:\")";
        ++$i;
        if ($i < @lines && $lines[$i] !~ $header_re) {
            push @names, $lines[$i];
            ++$i;
        }
    }
    return undef unless @names;
    grep($_ ne $names[0], @names) and
        die "otool -D reported differing install names across architectures for $file";
    return $names[0];
}
