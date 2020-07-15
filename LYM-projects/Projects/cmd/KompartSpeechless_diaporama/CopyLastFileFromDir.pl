#!/usr/bin/perl 
 
# ShowOnlyOnePathAndAddPhoto - v.0.0.11
# last revised Dec 11, 1998
 
# A program for suppressing headers of slides

# usage
# CopyLastFileFromDir.pl <source-dir> <file-extension> <local-name>

use File::stat;
#use warnings;

format USAGE =
Usage: CopyLastFileFromDir.pl <source-dir> <file-extension> <local-name>
A program for copying the newest file in a directory with a given extension
.

if( @ARGV != 3 ) {
    $~ = USAGE; write; exit;
}

my $pathToDir = $ARGV[0]; # "D:/sync.com/Sync/LYM-projects/snapshots"
my $file_extension = $ARGV[1];  # svg txt...
my $local_name = $ARGV[2]; # snapshotFullPerformance.svg

# target file opening
use File::Path qw/make_path/;
use Cwd;
use strict;

opendir (DIR, $pathToDir) or die "Unable to open current directory! $!/n";
my @subdirs = readdir (DIR) or die "Unable to read directory! $!/n";
closedir DIR;

my $sourceSubDir = "";
foreach my $subdir ( reverse sort @subdirs ) {
	if ($subdir =~ /pic_core/ ) {
		$sourceSubDir = $subdir;
		last;
	}
}

my $pathToFile = $pathToDir . "/" . $sourceSubDir;
opendir (DIR, $pathToFile) or die "Unable to open current directory! $!/n";
@subdirs = sort {  -M $a <=> -M $b } # Sort by modification time
               map  { "$pathToFile/$_" } # We need full paths for sorting
               readdir DIR or die "Unable to read directory! $!/n";
closedir DIR;

my $sourceFile;
for my $subdir (@subdirs) {
	if ($subdir =~ /${file_extension}$/ ) {
		$sourceFile = $subdir;
		# print "File ", $subdir, "\n";
		last;
	}
}

$sourceFile = $sourceFile;
$sourceFile =~ tr/\//\\/;

print "copy $sourceFile $local_name" , "\n";
system( "copy $sourceFile $local_name");

