use strict;

my @dirs =
(
	"3dsparse",
	"actions",
	"common",
	"coms",
	"dialogs",
	"engine",
	"GLEXT",
	"GLW",
	"ode",
	"landscape",
	"client",
	"server",
	"tankgraph",
	"sprites",
	"tank",
	"weapons"
);

my @stdinc =
(
	"..\\include\\SDL",
 	"F:\\Microsoft Visual Studio .NET\\Vc7\\include",
	"F:\\Microsoft Visual Studio .NET\\Vc7\\PlatformSDK\\Include"
);

my $dir;
my %allheaderfiles = ();
foreach $dir (@dirs)
{
	opendir(IN, "..\\src\\$dir") || die "ERROR: DIR \"$dir\"";
	my @headerfiles = grep { /\.h/ } readdir(IN);
	closedir(IN);

	my $file;
	foreach $file (@headerfiles)
	{
		#print "$dir/$file\n";
		$allheaderfiles{"$dir/$file"} = 1;
	}
}		

foreach $dir (@stdinc)
{
	opendir(IN, "$dir") || die "ERROR: DIR \"$dir\"";
	my @headerfiles = readdir(IN);
	closedir(IN);

	my $file;
	foreach $file (@headerfiles)
	{
		$file = lc($file);
		#print "$file\n";
		$allheaderfiles{"$file"} = 1;
	}
}

print "Not found headers......\n";
print "-----------------------\n";

foreach $dir (@dirs)
{
	opendir(IN, "..\\src\\$dir") || die "ERROR: DIR \"$dir\"";
	my @files = grep { /\.h/ || /\.cpp/ } readdir(IN);
	closedir(IN);

	my $file;
	foreach $file (@files)
	{
		open (INFILE, "..\\src\\$dir\\$file") || die "ERROR: File \"..\\src\\$dir/$file\"";
		my @filelines = <INFILE>;
		close (INFILE);

		for my $line (@filelines)
		{
			if ($line =~ /\#include\s+\<([^\>]+)\>/)
			{
				if (!exists $allheaderfiles{$1})
				{
					print "$dir/$file -> \"$1\"\n";
				}
			}
		}
	}
}
