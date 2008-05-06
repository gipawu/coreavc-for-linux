#!/usr/bin/perl
if(scalar @ARGV != 1 || ! -d ($mplayer = shift @ARGV) ||
                        ! -d "$mplayer/loader") {
  printf "Usage:\n$0 <path to mplayer source>\n";
  exit 1;
}
my ($base) = ($0 =~ /(^.*)\/[^\/]+$/);
$base =~ s/\/+/\//g;
@lines = split(/\n/, `diff -x .svn -x Makefile -x compat -ur --unidirectional-new-file $mplayer/loader $base/../loader`);
#print "$base\n";
foreach (@lines) {
  next if(/^Binary / || /^Only in /);
  s/^\+\+\+ $base\/\.\.\//+++ /;
  s/^--- $mplayer\//--- /;
  print "$_\n";
}
system("cat $base/mplayer.patch");   
