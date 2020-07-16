#!/usr/bin/perl 

use strict;
use warnings;

# SVG_movie_posterize.pl
# composition of color paths into a single file

# usage
# SVG_movie_posterize.pl --format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

# IN
#
#              n paths

# OUT 
#
#              coposited and colored image

my $USAGE =
"usage: SVG_movie_posterize.pl --nb-layers <integer> --dimming <dimming_coefficient> -o <file_name> -i <file_name> 
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_posterize.pl --nb-layers 4 --dimming 0.4 -o mer.svg -i mer.jpg

ARGUMENTS:
   --nb_layers: number of layers for posterization
   --dimming: coefficient for dimming the threshold values of the layers
   -o: ouput files 
   -i: input files
";

my $numArgs;
$numArgs = $#ARGV + 1;

my $argnum;
my $output_format;
my $nb_layers;
my $dimming;
# my $output_file_name_PATHs;
my $output_file_name_SVGs;
my $input_file_name;

if( 0+@ARGV != 8 ) {
    print "Arguments: ", join(' ',@ARGV), "\n";
    print $USAGE; die"SVG_movie_posterize.pl: expected number of arguments: 8 not ", 0+@ARGV, "\n" ;
}

# nb_layers
if( $ARGV[0] ne "--nb-layers" ) {
    print $USAGE; die"SVG_movie_posterize.pl: --nb-layers expected for argt 1";
}
if ($ARGV[1] < 1) {
    print $USAGE; die "SVG_movie_posterize.pl: ", $ARGV[1]," 1 layer should be produced minimally\n";
}
$nb_layers = $ARGV[1];

# dimming
if( $ARGV[2] ne "--dimming" ) {
    print $USAGE; die"SVG_movie_posterize.pl: --dimming expected for argt 3";
}
if ($ARGV[3] > 1 || $ARGV[3] < 0) {
    print $USAGE; die "SVG_movie_posterize.pl: ", $ARGV[3]," dimming should be between 0 and 1\n";
}
$dimming = $ARGV[3];

# output_file_name_PATHs
if( $ARGV[4] ne "-o" ) {
    print $USAGE; die"SVG_movie_posterize.pl: -o expected for argt 3";
}
# $output_file_name_PATHs = $ARGV[5];
$output_file_name_SVGs = $ARGV[5];

# input_file_names
if( $ARGV[6] ne "-i" ) {
    print $USAGE; die"SVG_movie_posterize.pl: -i expected for argt 7\n";
}
$input_file_name = $ARGV[7];

system( "convert $input_file_name -resize 1920x1080 tmp/tmp.bmp" ); 
print "convert $input_file_name -resize 1920x1080 tmp/tmp.bmp","\n";

################################################
# PATHS GENERATION
if($nb_layers == 4) {
	# -t n means that the pixel darker than this value are taken
	# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
	# my $indLayer = 0;
	my $indLayer = 3;
	my $interval = $dimming / ($nb_layers + 1);
	# my $threshold = 1. - ($indLayer + 1) * $interval;
	my $threshold = ($indLayer + 1) * $interval;
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp1.pbm -x -t ${threshold} -s 2" ); 
	# $indLayer++;
	$indLayer--;
	# $threshold = 1. - ($indLayer + 1) * $interval;
	$threshold = ($indLayer + 1) * $interval;
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp2.pbm -x -t ${threshold} -s 2" ); 
	# $indLayer++;
	$indLayer--;
	# $threshold = 1. - ($indLayer + 1) * $interval;
	$threshold = ($indLayer + 1) * $interval;
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp3.pbm -x -t ${threshold} -s 2" ); 
	# $indLayer++;
	$indLayer--;
	# $threshold = 1. - ($indLayer + 1) * $interval;
	$threshold = ($indLayer + 1) * $interval;
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp4.pbm -x -t ${threshold} -s 2" ); 
	# go from larger and grighter areas to smaller and darker 
	# the layers should be piled in this order in SVG and colored from brighter to darker
	system( "potrace ./tmp/tmp1.pbm -o ./tmp/tmp1.svg --svg -t 24 -O 2 --flat" ); 
	system( "potrace ./tmp/tmp2.pbm -o ./tmp/tmp2.svg --svg -t 24 -O 2 --flat" ); 
	system( "potrace ./tmp/tmp3.pbm -o ./tmp/tmp3.svg --svg -t 24 -O 2 --flat" ); 
	system( "potrace ./tmp/tmp4.pbm -o ./tmp/tmp4.svg --svg -t 24 -O 2 --flat" ); 
	# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
	# to be edited or to its conversion into png or jpg to make a movie from
  	# system( "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_layer_composition.pl --output-format PATHS --nb-files 4 -o ${output_file_name_PATHs} -i ./tmp/tmp1.svg ./tmp/tmp2.svg ./tmp/tmp3.svg ./tmp/tmp4.svg -c \"#c0c0c0\" \"#808080\" \"#404040\" \"#000000\"" ); 
	system( "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_layer_composition.pl --output-format SVG --nb-files 4 -o ${output_file_name_SVGs} -i ./tmp/tmp1.svg ./tmp/tmp2.svg ./tmp/tmp3.svg ./tmp/tmp4.svg -c \"#c0c0c0\" \"#808080\" \"#404040\" \"#000000\"" ); 
}
elsif($nb_layers == 2) {
	# -t n means that the pixel darker than this value are taken
	# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
	# my $indLayer = 0;
	my $indLayer = 1;
	my $interval = $dimming / ($nb_layers + 1);
	# my $threshold = 1. - ($indLayer + 1) * $interval;
	my $threshold = ($indLayer + 1) * $interval;
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp1.pbm -x -t ${threshold} -s 2" ); 
	# $indLayer++;
	$indLayer--;
	# $threshold = 1. - ($indLayer + 1) * $interval;
	$threshold = ($indLayer + 1) * $interval;
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp2.pbm -x -t ${threshold} -s 2" ); 
	# go from larger and grighter areas to smaller and darker 
	# the layers should be piled in this order in SVG and colored from brighter to darker
	system( "potrace ./tmp/tmp1.pbm -o ./tmp/tmp1.svg --svg -t 24 -O 2 --flat" ); 
	system( "potrace ./tmp/tmp2.pbm -o ./tmp/tmp2.svg --svg -t 24 -O 2 --flat" ); 
	# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
	# to be edited or to its conversion into png or jpg to make a movie from
  	# system( "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_layer_composition.pl --output-format PATHS --nb-files 2 -o ${output_file_name_PATHs} -i ./tmp/tmp1.svg ./tmp/tmp2.svg -c \"#808080\" \"#000000\"" ); 
	system( "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_layer_composition.pl --output-format SVG --nb-files 2 -o ${output_file_name_SVGs} -i ./tmp/tmp1.svg ./tmp/tmp2.svg -c \"#808080\" \"#000000\"" ); 
}

