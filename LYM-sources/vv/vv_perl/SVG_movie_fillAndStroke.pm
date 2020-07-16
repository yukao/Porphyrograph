#!/usr/bin/perl 

use strict;
use warnings;

package SVG_movie_fillAndStroke;

# SVG_movie_fillAndStroke.pm
# composition of color paths into a single file

# usage
# /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_fillAndStroke.pm --nb-layers 4 -o /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_transformed_SVGs/TawTra/Intro_0011.svg -i /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/tmp/tmp_transformed.svg --style #FEFEFE #509D76 #CAB835 #A11A1B #000000 0.791666666666667 0.791666666666667 0.791666666666667 1 #000000 1 2.08333333333333

my $USAGE =
"usage: SVG_movie_fillAndStroke.pm --nb-layers <integer> -o <file_name> -i <file_name> --style <color>5 <float>4 <color> <float>2

ARGUMENTS:
   --nb_layers: number of layers for posterization
   -o: ouput files 
   -i: input files
   --style: fill and stroke style
";

##################################################################
# MAIN SUB
##################################################################
sub MAIN {
	my $numArgs;
	$numArgs = $#ARGV + 1;

	my $argnum;
	my $output_format;
	my $nb_layers;
	my $output_file_name;
	my $input_file_name;

	use POSIX;

	use Math::Vector::Real;
	use List::Util qw[min max];

	##################################################################
	# ARGUMENTS
	##################################################################

	if( 0+@_ != 19) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; 
	    print"SVG_movie_fillAndStroke.pm: expected number of arguments: 19 not ", 0+@_, "\n" ;
	    return 0;
	}

	# nb_layers
	if( $_[0] ne "--nb-layers" ) {
	    print $USAGE; 
	    print"SVG_movie_fillAndStroke.pm: --nb-layers expected for argt 1\n";
	    return 0;
	}
	if ($_[1] < 1) {
	    print $USAGE; 
	    print "SVG_movie_fillAndStroke.pm: ", $_[1]," 1 layer should be produced minimally\n";
	    return 0;
	}
	$nb_layers = $_[1];

	# output_file_name_PATHs
	if( $_[2] ne "-o" ) {
	    print $USAGE; 
	    print"SVG_movie_fillAndStroke.pm: -o expected for argt 3\n";
	    return 0;
	}
	$output_file_name = $_[3];

	# input_file_names
	if( $_[4] ne "-i" ) {
	    print $USAGE; 
	    print"SVG_movie_fillAndStroke.pm: -i expected for argt 5\n";
	    return 0;
	}
	$input_file_name = $_[5];

	#  SVG path transformation
	if( $_[6] ne "--style" ) {
	    print $USAGE; 
	    print"SVG_movie_fillAndStroke.pm: --style expected for argt 7\n";
	    return 0;
	}
	my $indArgt = 7;
	my $fill_color_bg = $_[$indArgt]; $indArgt++;
	my @fill_color;
	my @fill_opacity;
	$fill_color[1] = $_[$indArgt]; $indArgt++;
	$fill_color[2] = $_[$indArgt]; $indArgt++;
	$fill_color[3] = $_[$indArgt]; $indArgt++;
	$fill_color[4] = $_[$indArgt]; $indArgt++;
	$fill_opacity[1] = $_[$indArgt]; $indArgt++;
	$fill_opacity[2] = $_[$indArgt]; $indArgt++;
	$fill_opacity[3] = $_[$indArgt]; $indArgt++;
	$fill_opacity[4] = $_[$indArgt]; $indArgt++;
	my $stroke_color = $_[$indArgt]; $indArgt++;
	my $stroke_opacity = $_[$indArgt]; $indArgt++;
	my $stroke_width = $_[$indArgt]; $indArgt++;

	my $FILEout;
	my $FILEin;
	my $second_FILEin;
	if( open(  $FILEout , ">$output_file_name" ) == 0) {
		print "Can't open file $output_file_name!\n";
	    return 0;
	}
	if( open($FILEin , "<$input_file_name" )  == 0) {
		print "Can't open fill and stroke input file ".$input_file_name."!\n";
	    return 0;
	}

	my $line;
	my $group_no = 1;
	while($line = <$FILEin>){
		chomp($line);
		##################################################################
		# changes fill and stroke
		##################################################################
		if( $line =~ /^<g / ) {
			# background rectangle
			if($group_no == 1) {
				print $FILEout "<rect
	     y=\"-10\"
	     x=\"-10\"
	     height=\"2190\"
	     width=\"3870\"
	     id=\"rect834\"
	     style=\"opacity:1;fill:${fill_color_bg};fill-opacity:1;stroke:none;\" />\n";
			}
			# typical group line
			# <g transform="translate(0.000000,2160.000000) scale(0.100000,-0.100000)" style="fill:#c0c0c0;" stroke="none">
			$line =~ s/stroke="none"/stroke="${stroke_color}" stroke-width="${stroke_width}" stroke-opacity="${stroke_opacity}"/;
			my $fill_color = ${fill_color}[${group_no}];
			my $fill_opacity = ${fill_opacity}[${group_no}];
			$line =~ s/fill:#([0-9a-fA-F]+);/fill:${fill_color}; fill-opacity:${fill_opacity}; /;
			# group for a line beam from the C_LINES transformation
			$line =~ s/(stroke\:url\(\#radialGradient001\);)/$1 stroke-width:15; /;
			# prints the modified group
			print $FILEout "${line}\n";
			$group_no++;
		}
		##########################################################################
		# non path and non group line reprinted
		##########################################################################
		else {
			print $FILEout $line, "\n";
		}
	}
	close($FILEin);
	close($FILEout);
	return 1;
}