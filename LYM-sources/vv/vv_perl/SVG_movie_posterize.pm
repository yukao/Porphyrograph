		#!/usr/bin/perl 

use strict;
use warnings;

package SVG_movie_posterize;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_movie_layer_composition;

# SVG_movie_posterize.pm
# composition of color paths into a single file

my $USAGE =
"usage: SVG_movie_posterize.pm --nb-layers <integer> --dimming <dimming_coefficient> -o <file_name> -i <file_name> 
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_posterize.pm --tmp /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/tmp --nb-layers 4 --dimming 1 -o /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_SVGs/VID_20200307_152223/VID_20200307_152223_0300.svg -i /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_PNGs/VID_20200307_152223/VID_20200307_152223_0300.png

ARGUMENTS:
   --nb_layers: number of layers for posterization
   --dimming: coefficient for dimming the threshold values of the layers
   -o: ouput files 
   -i: input files
   --tmp: tmp directory
";

##################################################################
# MAIN SUB
##################################################################
sub MAIN {
	my $numArgs;
	$numArgs = @_ + 1;

	my $argnum;
	my $output_format;
	my $nb_layers;
	my $dimming;
	# my $output_file_name_PATHs;
	my $output_file_name_SVGs;
	my $input_file_name;
	my $tmp_dir;

	if( 0+@_ != 10 ) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; 
	    print"SVG_movie_posterize.pm: expected number of arguments: 10 not ", 0+@_, "\n" ;
	    return 0;
	}

	# nb_layers
	if( $_[0] ne "--nb-layers" ) {
	    print $USAGE; 
	    print"SVG_movie_posterize.pm: --nb-layers expected for argt 1\n";
	    return 0;
	}
	if ($_[1] < 1) {
	    print $USAGE; 
	    print "SVG_movie_posterize.pm: ", $_[1]," 1 layer should be produced minimally\n";
	    return 0;
	}
	$nb_layers = $_[1];

	# dimming
	if( $_[2] ne "--dimming" ) {
	    print $USAGE; 
	    print"SVG_movie_posterize.pm: --dimming expected for argt 3\n";
	    return 0;
	}
	if ($_[3] > 1 || $_[3] < 0) {
	    print $USAGE; 
	    print "SVG_movie_posterize.pm: ", $_[3]," dimming should be between 0 and 1\n";
	    return 0;
	}
	$dimming = $_[3];

	# output_file_name_PATHs
	if( $_[4] ne "-o" ) {
	    print $USAGE; 
	    print"SVG_movie_posterize.pm: -o expected for argt 5\n";
	    return 0;
	}
	# $output_file_name_PATHs = $_[5];
	$output_file_name_SVGs = $_[5];

	# input_file_names
	if( $_[6] ne "-i" ) {
	    print $USAGE; 
	    print"SVG_movie_posterize.pm: -i expected for argt 7\n";
	    return 0;
	}
	$input_file_name = $_[7];

	# input_file_names
	if( $_[8] ne "--tmp" ) {
	    print $USAGE; print"SVG_movie_posterize.pm: --tmp expected for argt 9\n";
	    return 0;
	}
	$tmp_dir = $_[9];

	system( "convert $input_file_name -crop 1888x1062+16+9 -resize 1920x1080 ${tmp_dir}/tmp.bmp" ); 
	print "convert $input_file_name -crop 1888x1062+16+9 -resize 1920x1080 ${tmp_dir}/tmp.bmp","\n";

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
		system( "mkbitmap ${tmp_dir}/tmp.bmp -o ${tmp_dir}/tmp1.pbm -x -t ${threshold} -s 2" ); 
		# $indLayer++;
		$indLayer--;
		# $threshold = 1. - ($indLayer + 1) * $interval;
		$threshold = ($indLayer + 1) * $interval;
		system( "mkbitmap ${tmp_dir}/tmp.bmp -o ${tmp_dir}/tmp2.pbm -x -t ${threshold} -s 2" ); 
		# $indLayer++;
		$indLayer--;
		# $threshold = 1. - ($indLayer + 1) * $interval;
		$threshold = ($indLayer + 1) * $interval;
		system( "mkbitmap ${tmp_dir}/tmp.bmp -o ${tmp_dir}/tmp3.pbm -x -t ${threshold} -s 2" ); 
		# $indLayer++;
		$indLayer--;
		# $threshold = 1. - ($indLayer + 1) * $interval;
		$threshold = ($indLayer + 1) * $interval;
		system( "mkbitmap ${tmp_dir}/tmp.bmp -o ${tmp_dir}/tmp4.pbm -x -t ${threshold} -s 2" ); 
		# go from larger and grighter areas to smaller and darker 
		# the layers should be piled in this order in SVG and colored from brighter to darker
		system( "potrace ${tmp_dir}/tmp1.pbm -o ${tmp_dir}/tmp1.svg --svg -t 24 -O 2 --flat" ); 
		system( "potrace ${tmp_dir}/tmp2.pbm -o ${tmp_dir}/tmp2.svg --svg -t 24 -O 2 --flat" ); 
		system( "potrace ${tmp_dir}/tmp3.pbm -o ${tmp_dir}/tmp3.svg --svg -t 24 -O 2 --flat" ); 
		system( "potrace ${tmp_dir}/tmp4.pbm -o ${tmp_dir}/tmp4.svg --svg -t 24 -O 2 --flat" ); 
		# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
		# to be edited or to its conversion into png or jpg to make a movie from
		# print "SVG_movie_layer_composition",join(' ', ("--output-format", "SVG", 
	  	#       	"--nb-files", 4, 
	  	#       	"-o", ${output_file_name_SVGs}, 
	  	#       	"-i", "${tmp_dir}/tmp1.svg", "${tmp_dir}/tmp2.svg", "${tmp_dir}/tmp3.svg", "${tmp_dir}/tmp4.svg", 
	  	#       	"-c", "#c0c0c0", "#808080", "#404040", "#000000" )), "\n";
        if( SVG_movie_layer_composition::MAIN("--output-format", "SVG", 
        	"--nb-files", 4, 
        	"-o", ${output_file_name_SVGs}, 
        	"-i", "${tmp_dir}/tmp1.svg", "${tmp_dir}/tmp2.svg", "${tmp_dir}/tmp3.svg", "${tmp_dir}/tmp4.svg", 
        	"-c", "#c0c0c0", "#808080", "#404040", "#000000" ) == 0) {
          return 0;
        }
	}
	elsif($nb_layers == 2) {
		# -t n means that the pixel darker than this value are taken
		# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
		# my $indLayer = 0;
		my $indLayer = 1;
		my $interval = $dimming / ($nb_layers + 1);
		# my $threshold = 1. - ($indLayer + 1) * $interval;
		my $threshold = ($indLayer + 1) * $interval;
		system( "mkbitmap ${tmp_dir}/tmp.bmp -o ${tmp_dir}/tmp1.pbm -x -t ${threshold} -s 2" ); 
		# $indLayer++;
		$indLayer--;
		# $threshold = 1. - ($indLayer + 1) * $interval;
		$threshold = ($indLayer + 1) * $interval;
		system( "mkbitmap ${tmp_dir}/tmp.bmp -o ${tmp_dir}/tmp2.pbm -x -t ${threshold} -s 2" ); 
		# go from larger and grighter areas to smaller and darker 
		# the layers should be piled in this order in SVG and colored from brighter to darker
		system( "potrace ${tmp_dir}/tmp1.pbm -o ${tmp_dir}/tmp1.svg --svg -t 24 -O 2 --flat" ); 
		system( "potrace ${tmp_dir}/tmp2.pbm -o ${tmp_dir}/tmp2.svg --svg -t 24 -O 2 --flat" ); 
		# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
		# to be edited or to its conversion into png or jpg to make a movie from
		print "SVG_movie_layer_composition",join(' ', ("--output-format", "SVG", 
        	"--nb-files", 2, 
        	"-o", ${output_file_name_SVGs}, 
        	"-i", "${tmp_dir}/tmp1.svg", "${tmp_dir}/tmp2.svg", 
        	"-c", "#808080", "#000000"  )), "\n";
        if( SVG_movie_layer_composition::MAIN("--output-format", "SVG", 
        	"--nb-files", 2, 
        	"-o", ${output_file_name_SVGs}, 
        	"-i", "${tmp_dir}/tmp1.svg", "${tmp_dir}/tmp2.svg", 
        	"-c", "#808080", "#000000" ) == 0) {
          return 0;
        }
	}
	elsif($nb_layers == 1) {
		# -t n means that the pixel darker than this value are taken
		# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
		# my $indLayer = 0;
		my $indLayer = 1;
		my $interval = $dimming / ($nb_layers + 1);
		$indLayer--;
		# $threshold = 1. - ($indLayer + 1) * $interval;
		my $threshold = ($indLayer + 1) * $interval;
		system( "mkbitmap ${tmp_dir}/tmp.bmp -o ${tmp_dir}/tmp1.pbm -x -t ${threshold} -s 2" ); 
		# go from larger and grighter areas to smaller and darker 
		# the layers should be piled in this order in SVG and colored from brighter to darker
		system( "potrace ${tmp_dir}/tmp1.pbm -o ${tmp_dir}/tmp1.svg --svg -t 24 -O 2 --flat" ); 
		# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
		# to be edited or to its conversion into png or jpg to make a movie from
		print "SVG_movie_layer_composition",join(' ', ("--output-format", "SVG", 
        	"--nb-files", 1, 
        	"-o", ${output_file_name_SVGs}, 
        	"-i", "${tmp_dir}/tmp1.svg", 
        	"-c", "#000000" )), "\n";
        if( SVG_movie_layer_composition::MAIN("--output-format", "SVG", 
        	"--nb-files", 1, 
        	"-o", ${output_file_name_SVGs}, 
        	"-i", "${tmp_dir}/tmp1.svg", 
        	"-c", "#000000" ) == 0) {
          return 0;
        }
	}
	else {
		print "Unprocessed number of layers ${nb_layers}, currently only 1, 2, or 4 layers are accepted\n"; 
		return 0;
	}
	return 1;
}

