use strict;

my @dirs =
(
	"3dsparse",
	"common",
	"coms",
	"dialogs",
	"engine",
	"GLEXT",
	"GLW",
	"landscape",
	"landscapedef",
	"client",
	"server",
	"actions",
	"cgext",
	"scorched",
	"sprites",
	"tankai",
	"target",
	"XML",
	"ships",
	"tankgraph",
	"tank",
	"boids",
	"placement",
	"sound",
	"weapons"
);

my $dir;
foreach $dir (@dirs)
{
	opendir(IN, "../src/$dir") || die "ERROR: DIR \"$dir\"";
	my @files = grep { /\.h/ } readdir(IN);
	closedir(IN);

	my $file;
	foreach $file (@files)
	{
		open (INFILE, "../src/$dir/$file") || die "ERROR: File \"../$dir/$file\"";
		my @filelines = <INFILE>;
		close (INFILE);

		my $index = -1;
		for (my $i=0; $i<=$#filelines; $i++)
		{
			if ($filelines[$i] =~ /pragma once/)
			{
				$index = $i;
			}
		}

		if ($index != -1)
		{
			my $incline = "__INCLUDE_".$file."_INCLUDE__";
			$incline =~ s/\.//g;

			print "$dir/$file\n";

			$filelines[$index] = "#if !defined($incline)\n#define $incline\n";
			push @filelines, "\n#endif // $incline\n";

			open(OUTFILE, ">../src/$dir/$file") || die "ERROR: Out $file";
			print OUTFILE @filelines;
			close (OUTFILE);
		}
	}
}
