#!/usr/bin/perl 

use strict;
use warnings;

# posterize.pl
# composition of color paths into a single file

# usage
# posterize.pl --format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

# IN
#
#              n paths

# OUT 
#
#              coposited and colored image

my $USAGE =
"usage: posterize.pl --output-format (SVG|PATHS|JPG|PNG) --nb-layers <integer> -o <file_name> -i <file_name>
./posterize.pl --output-format SVG --nb-layers 4 -o mer.svg -i mer.jpg

ARGUMENTS:
   --output-format: output format of the composited image (SVG, paths for GPU shipping, image)
   --nb_files: number of input images
   -o: ouput files 
   -i: input files
   -c: input colors
";

my $numArgs;
$numArgs = $#ARGV + 1;

use constant {
    SVG   => 1,
    PNG   => 2,
    JPG  => 3,
    PATHS  => 4,
    PNG_PATHS  => 5,
};
my %file_format = (
   "SVG" => SVG,
   "PNG" => PNG,
   "JPG" => JPG,
   "PATHS" => PATHS,
   "PNG_PATHS" => PNG_PATHS,
);

my $argnum;
my $output_format;
my $nb_layers;
my $output_file_name;
my $output_file_nameAux;
my $input_file_name;
my $input_file_extension;

if( 0+@ARGV != 8 && 0+@ARGV != 9 ) {
    print "Arguments: ", join(' ',@ARGV), "\n";
    print $USAGE; die"posterize.pl: expected number of arguments: 8 or 9 not ", 0+@ARGV, "\n" ;
}

# output_format
if( $ARGV[0] ne "--output-format" ) {
    print $USAGE; die"posterize.pl: --output-format expected for argt 1";
}
if (!$file_format{$ARGV[1]}) {
    print $USAGE; die "posterize.pl: ", $ARGV[1]," does not exist in the hash %file_format\n";
}
$output_format = $file_format{$ARGV[1]};

# nb_layers
if( $ARGV[2] ne "--nb-layers" ) {
    print $USAGE; die"posterize.pl: --nb-layers expected for argt 3";
}
if ($ARGV[3] < 1) {
    print $USAGE; die "posterize.pl: ", $ARGV[3]," 1 layer should be produced minimally\n";
}
$nb_layers = $ARGV[3];

# output_file_name
if( $ARGV[4] ne "-o" ) {
    print $USAGE; die"posterize.pl: -o expected for argt 5";
}
$output_file_name = $ARGV[5];
my $indArg = 6;
if($output_format == PNG_PATHS) {
	$output_file_nameAux = $ARGV[6];
	$indArg++;
}

# input_file_names
if( $ARGV[$indArg] ne "-i" ) {
    print $USAGE; die"posterize.pl: -i expected for argt ", ($indArg+1), "\n";
}
$indArg++;
$input_file_name = $ARGV[$indArg];
if($input_file_name =~ /\.jpg/) {
	$input_file_name =~ s/\.jpg//;
 	$input_file_extension = ".jpg";
}
elsif($input_file_name =~ /\.png/) {
	$input_file_name =~ s/\.png//;
 	$input_file_extension = ".png";
}

system( "convert $input_file_name$input_file_extension tmp/tmp.bmp" ); 
print "convert $input_file_name$input_file_extension tmp/tmp.bmp","\n";

################################################
# PATHS GENERATION
if($nb_layers == 4) {
	# -t n means that the pixel darker than this value are taken
	# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp1.pbm -x -t 0.8 -s 2" ); 
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp2.pbm -x -t 0.6 -s 2" ); 
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp3.pbm -x -t 0.4 -s 2" ); 
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp4.pbm -x -t 0.2 -s 2" ); 
	# go from larger and grighter areas to smaller and darker 
	# the layers should be piled in this order in SVG and colored from brighter to darker
	system( "potrace ./tmp/tmp1.pbm -o ./tmp/tmp1.svg --svg -t 24 -O 2 --flat" ); 
	system( "potrace ./tmp/tmp2.pbm -o ./tmp/tmp2.svg --svg -t 24 -O 2 --flat" ); 
	system( "potrace ./tmp/tmp3.pbm -o ./tmp/tmp3.svg --svg -t 24 -O 2 --flat" ); 
	system( "potrace ./tmp/tmp4.pbm -o ./tmp/tmp4.svg --svg -t 24 -O 2 --flat" ); 
	# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
	# to be edited or to its conversion into png or jpg to make a movie from
	if($output_format == SVG) {
		system( "./layer_composition.pl --output-format SVG --nb-files 4 -o ${output_file_name} -i ./tmp/tmp1.svg ./tmp/tmp2.svg ./tmp/tmp3.svg ./tmp/tmp4.svg -c \"#c0c0c0\" \"#808080\" \"#404040\" \"#000000\"" ); 
	}
	elsif($output_format == PATHS) {
	  	system( "./layer_composition.pl --output-format PATHS --nb-files 4 -o ${output_file_name} -i ./tmp/tmp1.svg ./tmp/tmp2.svg ./tmp/tmp3.svg ./tmp/tmp4.svg -c \"#c0c0c0\" \"#808080\" \"#404040\" \"#000000\"" ); 
	}
	elsif($output_format == PNG_PATHS) {
	  	system( "./layer_composition.pl --output-format PATHS --nb-files 4 -o ${output_file_nameAux} -i ./tmp/tmp1.svg ./tmp/tmp2.svg ./tmp/tmp3.svg ./tmp/tmp4.svg -c \"#c0c0c0\" \"#808080\" \"#404040\" \"#000000\"" ); 
	}
}
elsif($nb_layers == 2) {
	# -t n means that the pixel darker than this value are taken
	# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp1.pbm -x -t 0.2 -s 2" ); 
	system( "mkbitmap ./tmp/tmp.bmp -o ./tmp/tmp2.pbm -x -t 0.1 -s 2" ); 
	# go from larger and grighter areas to smaller and darker 
	# the layers should be piled in this order in SVG and colored from brighter to darker
	system( "potrace ./tmp/tmp1.pbm -o ./tmp/tmp1.svg --svg -t 24 -O 2 --flat" ); 
	system( "potrace ./tmp/tmp2.pbm -o ./tmp/tmp2.svg --svg -t 24 -O 2 --flat" ); 
	# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
	# to be edited or to its conversion into png or jpg to make a movie from
	if($output_format == SVG) {
		system( "./layer_composition.pl --output-format SVG --nb-files 2 -o ${output_file_name} -i ./tmp/tmp1.svg ./tmp/tmp2.svg -c \"#808080\" \"#000000\"" ); 
	}
	elsif($output_format == PATHS) {
	  	system( "./layer_composition.pl --output-format PATHS --nb-files 2 -o ${output_file_name} -i ./tmp/tmp1.svg ./tmp/tmp2.svg -c \"#808080\" \"#000000\"" ); 
	}
	elsif($output_format == PNG_PATHS) {
	  	system( "./layer_composition.pl --output-format PATHS --nb-files 2 -o ${output_file_nameAux} -i ./tmp/tmp1.svg ./tmp/tmp2.svg -c \"#808080\" \"#000000\"" ); 
	}
}

################################################
# IMAGE FROM PATHS GENERATION
if($output_format == JPG || $output_format == PNG || $output_format == PNG_PATHS) {
	# SVG image generation
	if($nb_layers == 4) {
		system( "./layer_composition.pl --output-format SVG --nb-files 4 -o ./tmp/tmp.svg -i ./tmp/tmp1.svg ./tmp/tmp2.svg ./tmp/tmp3.svg ./tmp/tmp4.svg -c \"#c0c0c0\" \"#808080\" \"#404040\" \"#000000\"" ); 
	}
	elsif($nb_layers == 2) {
		system( "./layer_composition.pl --output-format SVG --nb-files 2 -o ./tmp/tmp.svg -i ./tmp/tmp1.svg ./tmp/tmp2.svg -c \"#808080\" \"#000000\"" ); 
	}
	# SVG to image conversion
	if($output_format == JPG) {
		system( "convert ./tmp/tmp.svg -background white -flatten ${output_file_name}" ); 
		print "convert ./tmp/tmp.svg -background white -flatten ${output_file_name}","\n";
	}
	# the size of the PNG files was not correct (varying around 3844 and 2162)
	# should be either made more accurate in the co-crop 3840x2160+1+1 -resize 1920x1080vert command or should be followed
	# by a crop, resize  and transformation from grayscale to color for Vegas Pro
	#   system("convert SVG_movie_PNGs_large/${input_dir_name}/${input_dir_name}_40s_${count}.png -crop 3840x2160+1+1 -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 SVG_movie_PNGs/${output_dir_name}/${output_dir_name}_40s_${count}.png");
	elsif($output_format == PNG) {
		system( "convert ./tmp/tmp.svg -background white -flatten ${output_file_name}" ); 
		print "convert ./tmp/tmp.svg -background white -flatten ${output_file_name}","\n";
	}
	elsif($output_format == PNG_PATHS) {
		system( "convert ./tmp/tmp.svg -background white -flatten ${output_file_name}" ); 
		print "convert ./tmp/tmp.svg -background white -flatten ${output_file_name}","\n";
	}
}
