use strict;

my @dirs =
(
	"3dsparse",
	"actions",
	"client",
	"common",
	"coms",
	"dialogs",
	"engine",
	"GLEXT",
	"cgext",
	"GLW",
	"landscape",
	"landscapedef",
	"scorched",
	"ships",
	"server",
	"target",
	"sprites",
	"tank",
	"placement",
	"tankai",
	"tankgraph",
	"boids",
	"sound",
	"weapons"
);

my $text = "////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2006
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

";

my $dir;
foreach $dir (@dirs)
{
	opendir(IN, "../src/$dir") || die "ERROR: DIR \"$dir\"";
	my @files = grep { /\.h/ or /\.cpp/ } readdir(IN);
	closedir(IN);

	my $file;
	foreach $file (@files)
	{
		open (INFILE, "../src/$dir/$file") || die "ERROR: File \"../$dir/$file\"";
		my @filelines = <INFILE>;
		close (INFILE);

		my $lines = join("", @filelines);
		if ($lines !~ /MERCHANTABILITY or FITNESS/)
		{
			$lines = $text.$lines;

			print "../src/$dir/$file\n";
			open (OUTFILE, ">../src/$dir/$file") || die "ERROR: Out File \"../$dir/$file\"";
			print OUTFILE $lines;
			close (OUTFILE);
		}
	}
}
