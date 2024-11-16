#!/usr/local/bin/perl

use strict;

opendir(DIR, ".") || die "ERROR: Cannot open dir";
my @files = grep { /cpp$/ } readdir(DIR);
closedir(DIR);

foreach my $file (@files)
{
	my %result = parseFile($file);
	if (defined $result{"Class"})
	{
		#print "----------\n";
		print "<accessoryaction type=\"".$result{"Class"}."\">\n";

		#if ($result{"Extends"} ne "AccessoryPart")
		if (($result{"Extends"} ne "") & ($result{"Extends"} ne "AccessoryPart"))
		{
			print "Extends - ".$result{"Extends"}."\n";
		}

		if (defined $result{"Attribute"})
		{
			my @attrs = @{$result{"Attribute"}};
			foreach my $attr (@attrs)
			{	
				my %attrh = %{$attr};	
				print "=========" if ($attrh{"Type"} eq "");
				#print "Attribute : ".$attrh{"Type"}." ".$attrh{"Name"}." ".$attrh{"Rest"}."\n";
				print "\t<".$attrh{"Name"}.">".$attrh{"Type"}."</".$attrh{"Name"}.">\n";
			}
		}
		print "</accessory>\n\n"
	}
}

sub parseFile
{
	my ($file) = @_;
	
	open (FILE, "$file") || die "ERROR: Cannot open file $file";
	my @fileconts = <FILE>;
	close (FILE);	
	
	my @fns = grep { /\s+(\w+)::(parse|read)XML/ } @fileconts;
	return if ($#fns == -1);
	
	my %result = ();
	
	$fns[0] =~ /\s+(\w+)::(parse|read)XML/ or die "ERROR1";
	my $class = $1;
	$result{"Class"} = $class;
	
	my $fileh = $file;
	$fileh =~ s/cpp$/h/;
	
	my $extends = getExtends($fileh);
	$result{"Extends"} = $extends;
	if ($extends ne "")
	{
		if ($extends eq "PlacementModelDefinition")
		{	
			$extends = "../placement/PlacementModelDefinition";
		}
		my %newResult = parseFile($extends.".cpp");
		$result{"Attribute"} = $newResult{"Attribute"};
		
		if ($newResult{"Extends"} ne "")
		{
			$result{"Extends"} = $newResult{"Extends"};
		}
	}
	
	my @nodes = grep { /getNamedChild/ } @fileconts;
	foreach my $node (@nodes)
	{
		if ($node =~ /getNamedChild\(\"(\w+)\"\s*,\s*([\w_]+)(\s*,\s*false|)/)
		{
			my ($nodename, $var, $rest) = ($1, $2, $3);
			
			my $type = getType($fileh, $nodename);
			$type = getType($fileh, $var) if ($type eq "");
			$type = getType($file, $var) if ($type eq "");

			$rest = "(optional)" if ($rest ne "");
			
			my %attr = ();
			$attr{"Name"} = $nodename;
			$attr{"Type"} = $type;
			$attr{"Rest"} = $rest;
			
			push @{$result{"Attribute"}}, { %attr };
		}
	}
	@nodes = grep { /\.readXML\(/ } @fileconts;
	foreach my $node (@nodes)
	{
		if ($node =~ /(\w+)\.readXML\(/)
		{
			my $var = ($1);
			my $type = getType($fileh, $var);
						
			if ($type eq "TargetDefinition")
			{
				my %newResult = parseFile("../target/TargetDefinition.cpp");
				push @{$result{"Attribute"}}, @{$newResult{"Attribute"}};
			}
		}
	}
	
	return %result;
}

sub getExtends
{
	my ($file) = @_;

	open (FILE, "$file") || die "ERROR: Cannot open file $file";
	my @fileconts = <FILE>;
	close (FILE);		
	
	my @extends = grep { /public \w+/i } @fileconts;
	foreach (@extends)
	{
		if (/public (\w+)/)
		{
			my $extend = $1;
			$extend =~ s/Callback//;
		
			return $extend;
		}
	}
	return "";
}

sub getType
{
	my ($file, $var) = @_;
	
	return "float" if (length($var) == 1);
	
	open (FILE, "$file") || die "ERROR: Cannot open file $file";
	my @fileconts = <FILE>;
	close (FILE);		
	
	my @types = grep { /$var[_,\s;]+/i } @fileconts;
	foreach (@types)
	{
		if (/^\s*([_\*\w:]+)/i)
		{
			return $1 if ($1 eq "float");
			return $1 if ($1 eq "bool");
			return $1 if ($1 eq "Explosion::DeformType");
			return $1 if ($1 eq "std::string");
			return $1 if ($1 eq "Vector");
			return $1 if ($1 eq "int");
			return $1 if ($1 eq "Weapon");
			return $1 if ($1 eq "TargetDefinition");
			return $1 if ($1 eq "AccessoryPart");
			return $1 if ($1 eq "XMLNode");
		}
	}
	return "ModelID" if ($var =~/model/);
	
	return "";
}
