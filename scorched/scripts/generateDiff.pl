use strict;
use File::Compare;

if (!defined $ARGV[0])
{
	die "Usage : $0 <newdir> <olddir>";
}

my ($dir1, $dir2) = @ARGV;

open(OUT, ">diff.txt") || die "ERROR: Cannot open diff.txt";

checkDir($dir1, ".", $dir2, ".");

sub checkDir()
{
	my ($dir1, $offset1, $dir2, $offset2) = @_;

	my $actualdir1 = $dir1."\\".$offset1;
	my $actualdir2 = $dir2."\\".$offset2;

	# Src dir
	opendir(DIR, $actualdir1) || die "ERROR $actualdir1";
	my @files = readdir(DIR);
	closedir(DIR);

	if (! -d $actualdir2)
	{
		# New Dir
		print "New Dir : $actualdir2\n";
		print OUT "CreateDirectory \"\$INSTDIR\\$offset2\"\n";
	}

	foreach my $file (@files)
	{
		my $file1 = $actualdir1."\\".$file;
		my $file2 = $actualdir2."\\".$file;
		my $offfile = $offset1."\\".$file;

		if (-f $file1)
		{
			if (! -f $file2)
			{			
				# New file
				print "New : $offfile\n";
				print OUT "File \"/oname=\$INSTDIR\\$offfile\" \"$offfile\" \n";
			}
			else
			{
				if (compare($file1, $file2) != 0)
				{
					# Different file
					print "Diff : $offfile\n";
					print OUT "File \"/oname=\$INSTDIR\\$offfile\" \"$offfile\" \n";
				}
			}
		}
		elsif (-d $file1 && $file !~ /^\./)
		{
			checkDir($dir1, $offset1."\\".$file, $dir2, $offset2."\\".$file);
		}
	}
	
	if (-d $actualdir2)
	{
		# Dest dir
		opendir(DIR, $actualdir2) || die "ERROR $actualdir2";
		my @files = readdir(DIR);
		closedir(DIR);

		foreach my $file (@files)
		{
			my $file1 = $actualdir1."\\".$file;
			my $file2 = $actualdir2."\\".$file;
			my $offfile = $offset2."\\".$file;

			if (-f $file2)
			{
				if (! -f $file1)
				{
					# Old file

					print "Old : $offfile\n";
					print OUT "Delete \"\$INSTDIR\\$offfile\" \n";
				}
			}
			elsif (-d $file2 && $file !~ /^\./)
			{
				if (! -d $file1)
				{
					# Old dir
					print "Old Dir : $offfile\n";
					print OUT "RMDir /r \"\$INSTDIR\\$offfile\" \n";
				}
			}
		}
	}
}
