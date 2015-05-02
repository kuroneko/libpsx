#!/usr/bin/perl
#
# make_psx_codes.pl

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