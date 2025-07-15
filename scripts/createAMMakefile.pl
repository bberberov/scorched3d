#! /usr/bin/perl

use strict;

sub getFiles
{
	my ($vcfile) = @_;

	open (IN, $vcfile) || die $vcfile;
	my @files = grep { /^[[:space:]]+<Cl/ and ( /\.cpp\"/ or /\.c\"/ or /\.h\"/ ) } <IN>;
	close (IN);

	for (my $i=0; $i<=$#files; $i++)
	{
		chomp $files[$i];
		$files[$i] =~ s/^[^\"]*\"/\t/;
		$files[$i] =~ s/\".*$//;
		$files[$i] =~ s/\\/\//g;

		$files[$i] .= " \\" if ($i < $#files);
		$files[$i] .= "\n";
	}

	return @files;
}

sub locatefiles
{
	my ($dir, $basetypedir, $destdir) = @_;
	opendir(DIR, "../$dir") || die "ERROR: DIR ../$dir";
	my @files = grep { !/^\./ && !/CVS/ } readdir(DIR);
	closedir(DIR);

	my @newdirs = ();
	my @newfiles = ();
	my $file;
	foreach $file (@files)
	{
		if (-d "../$dir/$file")
		{
			push @newdirs, "$file";
		}
		else
		{
			push @newfiles,"$dir/$file";
		}
	}

	my $newdir = $dir;
	$newdir =~ s!/!!g;
	$newdir =~ s!-!!g;
	print CLIENT "scorched${newdir}dir = $destdir\n";
	print CLIENT "scorched${newdir}_DATA = " . join(" \\\n\t", @newfiles) . "\n";
	foreach $file (@newdirs)
	{
		locatefiles("$dir/$file", $basetypedir, "$destdir/$file");
	}
}

sub createBinaryMakefile
{
	my ($input, $output, $binary, $flags, $libs) = @_;

	my @clientfiles = getFiles($input);

	open (CLIENT, ">$output") || die $output;

	print CLIENT "bin_PROGRAMS = $binary\n\n";
	print CLIENT $binary."_SOURCES = \\\n";
	print CLIENT @clientfiles;
	print CLIENT "\n";
	print CLIENT "AM_CXXFLAGS = -std=c++11\n";
	print CLIENT "AM_CPPFLAGS = -I../../common/porting -I../../common ${flags}\n";
	print CLIENT "LDADD = ${libs}\n";
	print CLIENT "\n";

	close(CLIENT);
}

sub createInstallMakefile
{
	open (CLIENT, ">../Makefile.am") || die "ERROR: Failed to write to ../Makefile.am";
	print CLIENT "SUBDIRS = src\n\n";
	print CLIENT "docdir = \@docdir\@\n\n";
	locatefiles("documentation", "\$\{docdir\}", "\$\{docdir\}");
	locatefiles("data", "\$\{datadir\}", "\$\{datadir\}/data");
	close(CLIENT);
}

createInstallMakefile();
createBinaryMakefile(
	"../src/launcher/scorched/scorched.vcxproj",
	"../src/launcher/scorched/Makefile.am",
	"scorched3d",
	(
		''
		. ' -I../../client'
		. ' -I../../server'
		. ' -I../../launcher'
		. ' -DS3D_BROWSER=\'"@USE_BROWSER@"\''
		. ' @WX_CFLAGS@'
		. ' @FT2_CFLAGS@'
		. ' @SDL_CFLAGS@ @SDL_NET_CFLAGS@'
	),
	(
		''
		. ' @WX_LIBS@'
		. ' @FT2_LIBS@'
		. ' @SDL_LIBS@ @SDL_NET_LIBS@'
	)
);
createBinaryMakefile(
	"../src/client/scorchedc/scorchedc.vcxproj",
	"../src/client/scorchedc/Makefile.am",
	"scorched3dc",
	(
		''
		. ' -I../../client'
		. ' -I../../server'
		. ' -DS3D_BROWSER=\'"@USE_BROWSER@"\''
		. ' @OPENAL_CFLAGS@ @FREEALUT_CFLAGS@'
		. ' @FFTW_CFLAGS@'
		. ' @FT2_CFLAGS@'
		. ' @OGG_CFLAGS@ @VORBIS_CFLAGS@'
		. ' @SDL_CFLAGS@ @SDL_NET_CFLAGS@'
		. ' @LUA_CFLAGS@'
	),
	(
		''
		. ' @OPENAL_LIBS@ @FREEALUT_LIBS@'
		. ' @FFTW_LIBS@'
		. ' @FT2_LIBS@'
		. ' @GL_LIBS@'
		. ' @OGG_LIBS@ @VORBIS_LIBS@ @VORBISFILE_LIBS@'
		. ' @SDL_LIBS@ @SDL_NET_LIBS@'
		. ' @LUA_LIBS@'
	)

);
createBinaryMakefile(
	"../src/server/scorcheds/scorcheds.vcxproj",
	"../src/server/scorcheds/Makefile.am",
	"scorched3ds",
	(
		''
		. ' -I../../server'
		. ' -DS3D_SERVER=1'
		. ' @SDL_CFLAGS@ @SDL_NET_CFLAGS@'
		. ' @MYSQL_CFLAGS@'
		. ' @LUA_CFLAGS@'
	),
	(
		''
		. ' @SDL_LIBS@ @SDL_NET_LIBS@'
		. ' @MYSQL_LIBS@'
		. ' @LUA_LIBS@'
	)
);
