#!/usr/bin/perl 
 
# ShowOnlyOnePathAndAddPhoto - v.0.0.11
# last revised Dec 11, 1998
 
# A program for suppressing headers of slides

# usage
# ShowOnlyOnePathAndAddPhoto <input-svg> <input_png> <path_color> <pathIndexFrom1> <output-svg>

format USAGE =
Usage: ShowOnlyOnePathAndAddPhoto.pl <input-svg> <input_png> <pathIndexFrom1> <path_color> <with-or-without-photo> <output-svg>
ShowOnlyOnePathAndAddPhoto: A program for making SVG paths in/visible and adding a photo layer
.

if( @ARGV != 6 ) {
    $~ = USAGE; write; exit;
}

$input_svg = @ARGV[0];
$pathIndexFrom1 = @ARGV[1];
$input_png = @ARGV[2];
$path_color = @ARGV[3];
$with_or_without_photo = @ARGV[4];
$output_svg = @ARGV[5];

# target file opening
open( FILEin , "<$input_svg" ) 
    ||  die "Can't open input file $input_svg!" ;
open( FILEout , ">$output_svg" ) 
     ||  die "Can't open output file $output_svg!" ;

# SVG FILE PARSING
&main;

# XML FILE PRINTING
close( FILEout );
close( FILEin );

##############################################################
# SVG FILE PARSING
##############################################################

sub main {
	my $count_paths = 0;
	my $printout = 1;

    while( $line = <FILEin> ) {
    	# path printed or not for porphyrograph
    	if($count_paths != ($pathIndexFrom1 - 1)  && $line =~ /<path/) {
			$printout = 0;
    	}

		# COLOR STYLE
		if( $line =~ /stroke:#000000/ ) {
		    $line =~  s/#000000/$path_color/;
		}

		# IMAGE
		if( $line =~ /\<g/ && $with_or_without_photo eq "true") {
			print FILEout "    <image
			     xlink:href=\"".$input_png."\"
			     width=\"1920\"
			     height=\"1080\"
			     preserveAspectRatio=\"none\"
			     id=\"imageDancer\"
			     x=\"0\"
			     y=\"0\" />\n";
     	}

    	# prints current line
    	if($printout == 1) {
    		print FILEout $line;
    	}

    	# path printed or not for porphyrograph
		if( $line =~ /stroke:/ ) {
			$printout = 1;
		}

    	# path counting
		if( $line =~ /stroke:/ ) {
		    $count_paths++;
		}

    }
}
