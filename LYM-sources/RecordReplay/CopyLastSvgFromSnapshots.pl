#!/usr/bin/perl 
 
# ShowOnlyOnePathAndAddPhoto - v.0.0.11
# last revised Dec 11, 1998
 
# A program for suppressing headers of slides

# usage
# CopyLastSvgFromSnapshots.pl

format USAGE =
Usage: CopyLastSvgFromSnapshots.pl
.

if( @ARGV != 0 ) {
    $~ = USAGE; write; exit;
}

# target file opening
use File::Path qw/make_path/;
use Cwd;
use strict;

my $pathToSnapshots = "D:/sync.com/Sync/LYM-projects/snapshots";
opendir (DIR, $pathToSnapshots) or die "Unable to open current directory! $!/n";
my @subdirs = readdir (DIR) or die "Unable to read directory! $!/n";
closedir DIR;

my $sourceSubDir;
foreach my $subdir ( reverse sort @subdirs ) {
	if ($subdir =~ /pic_core/ ) {
		$sourceSubDir = $subdir;
		last;
	}
}

my $pathToSvgFile = $pathToSnapshots . "/" . $sourceSubDir;
opendir (DIR, $pathToSvgFile) or die "Unable to open current directory! $!/n";
@subdirs = readdir (DIR) or die "Unable to read directory! $!/n";
closedir DIR;

my $sourceSvgFile;
foreach my $subdir ( reverse sort @subdirs ) {
	if ($subdir =~ /svg$/ ) {
		$sourceSvgFile = $subdir;
		last;
	}
}

$sourceSvgFile = $pathToSvgFile . "/" . $sourceSvgFile;
$sourceSvgFile =~ tr/\//\\/;

print "copy $sourceSvgFile snapshotFullPerformance.svg" , "\n";
system( "copy $sourceSvgFile snapshotFullPerformance.svg");

