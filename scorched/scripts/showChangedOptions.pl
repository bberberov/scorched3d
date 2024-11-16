#!/bin/perl

use strict;

if (!defined $ARGV[0])
{
	print "Usaged : $0 <settingsfile>\n";
	exit 0;
}

my ($buffer, $default, $changed) = ("", "", 0);
open (IN, $ARGV[0]) || die "ERROR: Cannot open ".$ARGV[0];
while (<IN>)
{
	if (/default value : "([^"]+)/)
	{
		print $buffer if ($changed);

		$changed = 0;
		$default = $1;
		$buffer = "";
	}
	elsif (/<value>([^<]+)<\/value>/)
	{
		if ($1 ne $default)
		{
			$changed = 1;
		}
	}
	$buffer .= $_;
}
close (IN);
print $buffer if ($changed);

