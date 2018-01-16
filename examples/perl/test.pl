#! /usr/bin/env perl
#use strict;
#use warnings;

my $name="xxxx";

# This is a comment.
print "Hello,
	world";
print "name: $name\n";
print 'name: $name\n';

print 42;
print (42);

print "\n";

my @animals = ("dog", "cat", "pig");

print $animals[0];
print $animals[2];
print $animals[1];
print $#animals;
print "\n";
print @animals, "\n";

my @sorted = sort @animals;
print @sorted, "\n";
my @backwards = reverse @animals;
print @backwards, "\n";

CORE::say "xxx";

my $tmp = "abcdefg";
$tmp =~ y/ace/135/;
print $tmp,"\n";

print <<EOF;
dsfjladkf
dgfsag
ggdla
      jlkjxljxx

xx  xx   xx x  x
EOF

print "a";
