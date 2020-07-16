#!/usr/bin/perl 

use strict;
use warnings;

# posterize_video.pl
# composition of color paths into a single file

# usage
# posterize_video.pl -o SVG_movies/5187_40s_SVG.mp4 -i ../5187_40s.mp4

# IN
#
#              mp4 video

# OUT 
#
#              mp4 posterized video

my $USAGE =
"usage: posterize_video.pl -o <file_name> -i <file_name>
./posterize_video.pl -o SVG_movies/5187_40s.mp4 -i ../5187_40s.mp4

ARGUMENTS:
   -o: ouput file
   -i: input file
";

my $output_file_name;
my $input_file_name;
my $input_file_name_wo_extension;

if( @ARGV != 4 ) {
    print $USAGE; die"posterize_video.pl: minimal number of arguments: 4 not ", 0+@ARGV;
}

# output_file_name
if( $ARGV[0] ne "-o" ) {
    print $USAGE; die"posterize_video.pl: -o expected for argt 1";
}
$output_file_name = $ARGV[1];

# input_file_name
if( $ARGV[2] ne "-i" ) {
    print $USAGE; die"posterize_video.pl: -i expected for argt 3";
}
$input_file_name = $ARGV[3];
$input_file_name_wo_extension = $input_file_name;
if($input_file_name_wo_extension =~ /\.m.*/) {
	# extension
 	$input_file_name_wo_extension =~ s/\.m.*$//;
 	# subrepertories
	while($input_file_name_wo_extension =~ /\//) {
 		$input_file_name_wo_extension =~ s/^.*\///;
 	}
}

# from video to images
# to uncomment system( "ffmpeg -i $input_file_name -r 24 movie_PNGs/${input_file_name_wo_extension}_%4d.png");

# count the images
my $FILEcount;
open($FILEcount, "ls -1 movie_PNGs/${input_file_name_wo_extension}_*.png | wc -l|");
my $nb_files = <$FILEcount>;
chomp($nb_files);
close($FILEcount);
print "Movie  $input_file_name has generated $nb_files images\n";

# path_colors
for(my $i = 1 ; $i <= $nb_files ; $i++) {
# for(my $i = 1 ; $i <= 10 ; $i++) {
	my $count = sprintf "%04d", $i;
	print "./posterize.pl --output-format PNG_PATHS --nb-layers 2 -o SVG_movie_PNGs_large/${input_file_name_wo_extension}_${count}.png movie_PATHs/${input_file_name_wo_extension}_${count}.path -i movie_PNGs/${input_file_name_wo_extension}_${count}.png\n";
	system("./posterize.pl --output-format PNG_PATHS --nb-layers 2 -o SVG_movie_PNGs_large/${input_file_name_wo_extension}_${count}.png movie_PATHs/${input_file_name_wo_extension}_${count}.path -i movie_PNGs/${input_file_name_wo_extension}_${count}.png");
}

# from images to videos
system( "ffmpeg -i SVG_movie_PNGs_large/${input_file_name_wo_extension}_%04d.png -r 24 -b:v 32M -minrate 20M -maxrate 40M -bufsize 10M  $output_file_name");

# better: resize the images with resize_png and use vegas pro to generate the video 