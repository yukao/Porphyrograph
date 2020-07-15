#!/usr/bin/perl 
 
# ShowOnlyOnePathAndAddPhoto - v.0.0.11
# last revised Dec 11, 1998
 
# A program for suppressing headers of slides

# usage
# ShowOnlyOnePathAndAddPhoto <input-svg> <input_png> <path_color> <pathIndexFrom1> <output-svg>

format USAGE =
Usage: RecordOnlyOneScene.pl <input-txt> <pathIndexFrom1> <output-txt>
RecordOnlyOneScene: A program for selecting a scene in a recording
.

if( @ARGV != 3 ) {
    $~ = USAGE; write; exit;
}

my $input_txt = @ARGV[0];
my $pathIndexFrom1 = @ARGV[1];
my $output_txt = @ARGV[2];

# target file opening
open( FILEin , "<$input_txt" ) 
    ||  die "Can't open input file $input_txt!" ;
open( FILEout , ">$output_txt" ) 
     ||  die "Can't open output file $output_txt!" ;

# SVG FILE PARSING
my $initialTime = 0;
my $printout = 0;

while( $line = <FILEin> ) {
	# path printed or not for porphyrograph
	if($line =~ /\/new_scene/) {
		@lineElements = split /\t/, $line;
		my $sceneNo = $lineElements[4];
		if(defined($sceneNo) && $sceneNo == ($pathIndexFrom1 - 1)) {
			$printout = 1;
			$initialTime = $lineElements[0];
		}
		else {
			$printout = 0;
		}
	}
	# prints current line
	if($printout == 1) {
 		@lineElements = split /\t/, $line;
 		$lineElements[0] = sprintf("%.6f", $lineElements[0] - $initialTime);
		print FILEout join("\t", @lineElements);
	}
	if($line =~ /\/quit/) {
			$printout = 0;
	}
}

# XML FILE PRINTING
close( FILEout );
close( FILEin );

