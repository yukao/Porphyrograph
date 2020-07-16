#!/usr/bin/perl 

use strict;
use warnings;

# resize_png.pl
# crops and resize pngs to 

# usage
# resize_png.pl -o  -i  

# IN
#
#              mp4 video

# OUT 
#
#              mp4 posterized video

my $USAGE =
"usage: resize_png.pl -o <dir_name> -i <dir_name>
./resize_png.pl -o 5187 -i 5187

ARGUMENTS:
   -o: ouput dir_name
   -i: input dir_name
";

my $output_dir_name;
my $input_dir_name;

if( @ARGV != 4 ) {
    print $USAGE; die"resize_png.pl: number of arguments: 4 not ", 0+@ARGV;
}

# output_file_name
if( $ARGV[0] ne "-o" ) {
    print $USAGE; die"resize_png.pl: -o expected for argt 1";
}
$output_dir_name = $ARGV[1];

# input_file_name
if( $ARGV[2] ne "-i" ) {
    print $USAGE; die"resize_png.pl: -i expected for argt 3";
}
$input_dir_name = $ARGV[3];

# count the images
my $FILEcount;
open($FILEcount, "ls -1 SVG_movie_PNGs_large/${input_dir_name}/${input_dir_name}_*.png | wc -l|");
my $nb_files = <$FILEcount>;
chomp($nb_files);
close($FILEcount);
print "Dir  ${input_dir_name} contains $nb_files images\n";

# path_colors
for(my $i = 1 ; $i <= $nb_files ; $i++) {
	my $count = sprintf "%04d", $i;
	print "convert SVG_movie_PNGs_large/${input_dir_name}/${input_dir_name}_${count}.png -crop 3840x2160+1+1 -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 SVG_movie_PNGs/${output_dir_name}/${output_dir_name}_${count}.png\n";
	system("convert SVG_movie_PNGs_large/${input_dir_name}/${input_dir_name}_${count}.png -crop 3840x2160+1+1 -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 SVG_movie_PNGs/${output_dir_name}/${output_dir_name}_${count}.png");
}
