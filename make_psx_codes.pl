#!/usr/bin/perl
#
# make_psx_codes.pl
#
# This is part of libpsx
#
# Copyright (C) 2015, Christopher Collins
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License version 2.1 as published by the Free Software Foundation
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA

use strict;

my @Qs = ();
my @Qh = ();
my @Qi = ();

while (my $line = <>) {
  chomp $line;
  next if $line =~ m/^\[/;  

  if ($line =~ /^Qs/) {
    my ($code, $name) = $line =~ /^Qs(\d+)="([^"]+)"/;
    $Qs[$code] = $name;
  } elsif ($line =~ /^Qi/) {
    my ($code, $name) = $line =~ /^Qi(\d+)="([^"]+)"/;
    $Qi[$code] = $name;
  } elsif ($line =~ /^Qh/) {
    my ($code, $name) = $line =~ /^Qh(\d+)="([^"]+)"/;
    $Qh[$code] = $name;
  }
}

sub emitEnum($$) {
  my $enumName = shift;
  my $arRef = shift;

  my $nextVal = undef;
  print "enum class $enumName : int {\n";
  foreach my $idx (0 .. ($#{$arRef}-1)) {
    if (defined($nextVal) and $nextVal == $idx) {
      print "\t".$arRef->[$idx].",\n";
    } else {
      $nextVal = $idx;
      print "\t".$arRef->[$idx]." = $nextVal,\n";
    }
    $nextVal++;
  }
  print "};\n\n";
}

emitEnum("Qs", \@Qs);
emitEnum("Qh", \@Qh);
emitEnum("Qi", \@Qi);

0;