#!/usr/bin/perl
mkdir("/tmp/mydir$$");
my ($base) = ($0 =~ /(^.*\/)[^\/]+$/);
@lines = split(/\n/, `diff -x .svn -x Makefile -urN /tmp/mydir$$ $base/../loader`);
rmdir("/tmp/mydir$$");
foreach (@lines) {
  if(/^\+\+\+ $base\/..\/loader/) {
    s/^\+\+\+ $base\/..\/loader/+++ libs\/libdshowloader/;
  } elsif(/^--- \/tmp\/mydir$$/) {
    s/--- \/tmp\/mydir$$\/\S+/--- \/dev\/null/;
  }
  print "$_\n";
}
system("cat $base/libdshowloader.patch");   
