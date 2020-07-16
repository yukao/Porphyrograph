#!/usr/bin/perl 
 
# ShowOnlyOnePathAndAddPhoto - v.0.0.11
# last revised Dec 11, 1998
 
# A program for combining scenes and offsetting the time

# usage
# MergeScenes.pl <nb_inputs> <input-txt> (<input-txt>)+ <output-txt>

use strict;
use warnings;


my $USAGE =
"Usage: MergeScenes.pl (sequence|parallel) <nb_inputs> <input-txt> (<input-txt>)+ <output-txt>
MergeScenes: A program for combining scenes and offsetting the time
.";

if( @ARGV < 5 ) {
	println("At least 4 arguments should be provided");
    $~ = $USAGE; write; exit;
}

my $mergingMode = $ARGV[0];
my $mergingModeValue = 0;
my $sequence = 1;
my $parallel = 2;
if( $mergingMode eq "sequence" ) {
	$mergingModeValue = $sequence;
}
elsif( $mergingMode eq "parallel" ) {
	$mergingModeValue = $parallel;
}
else {
	printf("Merging mode should be sequence or parallel, not %s\n", $mergingMode);
    $~ = $USAGE; write; exit;
}

my $nbFilesIn = $ARGV[1];
if( @ARGV != $nbFilesIn + 3 ) {
	printf("%d arguments are expected for %d input files", $nbFilesIn + 3, $nbFilesIn);
    $~ = $USAGE; write; exit;
}
my $output_txt = $ARGV[$nbFilesIn + 2];

# target file opening
open( FILEout , ">$output_txt" ) 
     ||  die "Can't open output file $output_txt!" ;

# TXT FILE PARSING
my $localInitialTime = 0;
my $currentTime = 0;

if($mergingModeValue == $sequence) {
	print("Sequence Merging\n");
	for(my $ind = 0; $ind < $nbFilesIn; $ind++) {
		my $fileInName = $ARGV[$ind + 2];
		my $firstline = 1;
		my $line;

		open( FILEin , "<$fileInName" ) 
		    ||  die "Can't open input file $fileInName!" ;

		while( $line = <FILEin> ) {
			if($firstline == 1) {
				$localInitialTime = $currentTime;
				$firstline = 0;
			}

			# prints current line
	 		my @lineElements = split /\t/, $line;
	 		$currentTime = $lineElements[0] + $localInitialTime;

			if($lineElements[1] eq "/quit") {
		 		my $formattedTime = sprintf("%.6f", $currentTime);
				print FILEout "$formattedTime\t/new_scene\t1\ti\t4\n";

				$currentTime += 10;
		 		$formattedTime = sprintf("%.6f", $currentTime);
				print FILEout "$formattedTime\t/new_scene\t1\ti\t5\n";

				$currentTime += 10;
		 		$formattedTime = sprintf("%.6f", $currentTime);
				print FILEout "$formattedTime\t/new_scene\t1\ti\t6\n";

				$currentTime += 10;
		 		$formattedTime = sprintf("%.6f", $currentTime);
				print FILEout "$formattedTime\t/new_scene\t1\ti\t7\n";

				$currentTime += 10;
		 		$formattedTime = sprintf("%.6f", $currentTime);
				print FILEout "$formattedTime\t/new_scene\t1\ti\t8\n";

				$currentTime += 10;
			}
	 		$lineElements[0] = sprintf("%.6f", $currentTime);
			print FILEout join("\t", @lineElements);
		}

		close( FILEin );
	}
}
elsif($mergingModeValue == $parallel) {
	print("Parallel Merging\n");
	my @fileInName = ();
	my @FILEin = ();
	for(my $ind = 0; $ind < $nbFilesIn; $ind++) {
		$fileInName[$ind] = $ARGV[$ind + 2];
		open( $FILEin[$ind] , "<$fileInName[$ind]" ) 
		    ||  die "Can't open input file $fileInName[$ind]!" ;
	}
	my @lines = ();
	my @currentTimes = ();
	for(my $ind = 0; $ind < $nbFilesIn; $ind++) {
		$lines[$ind] = readline($FILEin[$ind]);
		# print("line file #$ind ", $lines[$ind]);
		if($lines[$ind]) {
 			my @lineElements = split /\t/, $lines[$ind];
 			$currentTimes[$ind] = $lineElements[0];
 		}
 		else {
 			$currentTimes[$ind] = -1;
 		}
	}
	while(1) {
		# searches for lowest time
		# index of the earliest file
		my $earliestFile = -1;
		# time of the earliest file
		my $earliestTime = -1;
		for(my $ind = 0; $ind < $nbFilesIn; $ind++) {
			if($currentTimes[$ind] >= 0) {
				# first file processed
				if($earliestFile < 0) {
					$earliestFile = $ind;
					$earliestTime = $currentTimes[$ind];
				}
				# earlier file
				elsif($earliestFile >= 0 
	 					&& $currentTimes[$ind] < $earliestTime) {
					$earliestFile = $ind;
					$earliestTime = $currentTimes[$ind];
				}
	 		}
		}
		# print("earliest file ", $earliestFile, "\n");
		# prints current line
		if($earliestFile >= 0) {
 			my @lineElements = split /\t/, $lines[$earliestFile];
 			# adds a number in case of pen position, to have several pens drawing simultaneously
 			if($lineElements[1] eq "/abs_pen_xy") {
 				$lineElements[1] = $lineElements[1]."_".$earliestFile;
 			}
			print FILEout join("\t", @lineElements);
			$lines[$earliestFile] = readline($FILEin[$earliestFile]);
			if($lines[$earliestFile]) {
	 			my @lineElements = split /\t/, $lines[$earliestFile];
	 			$currentTimes[$earliestFile] = $lineElements[0];
	 		}
	 		else {
	 			$currentTimes[$earliestFile] = -1;
	 		}
		}
		else {
			last;
		}
	}
	for(my $ind = 0; $ind < $nbFilesIn; $ind++) {
		close( $FILEin[$ind] );
	}
}
close( FILEout );

