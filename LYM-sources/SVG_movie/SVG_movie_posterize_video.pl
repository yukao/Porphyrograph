#!/usr/bin/perl 

use strict;
use warnings;

# SVG_movie_posterize_video.pl
# composition of color paths into a single file

# usage
# SVG_movie_posterize_video.pl -o SVG_movies/5187_40s_SVG.mp4 -i ../5187_40s.mp4

# IN
#
#              mp4 video

# OUT 
#
#              mp4 posterized video

my $USAGE =
"usage: SVG_movie_posterize_video.pl -o <file_name> -i <file_name> -ss <start-time> -t <duration> -l <nb_layers> --start_step <start_step> -- transform <transformation_function>
 /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_posterize_video.pl -o SVG_movies/UW1_3_01.mp4 -i ../UW1_3.mp4 -ss 00:04:16 -t 00:00:06 -l 2 --start_step TRANSFORMATIONS --transformation V_SCALE --transformation_length 30 (--input-second-video ../Video-sources/5187/5187_40s.mp4)?

ARGUMENTS:
   -o: ouput file
   -i: input file
   -ss: initial video start time
   -t: initial video duration
   -l: number of posterization layers
   --start_step: step in the posterization process to restart from
   -- transformation: name of the transformation procedure applied
   -- transformation_length: duration of the transformation interpolation in seconds
    --input-second-video: for crossfading or other composition of 2 videos, provides the second video input
";

my $input_file_name;
my $input_second_file_name;
my $start_time;
my $duration;
my $output_file_name_wo_extension;
my $input_second_file_name_wo_extension;
my $nb_layers;
my $start_step_string;
my $start_step;
my $transformation_function_string;
my $transformation_function;
my $transformation_length;
use constant {
    START   => 1,
    PATHS   => 2,
    TRANSFORMATIONS  => 3,
    IMAGES  => 4,
    IMAGES_RESIZED  => 5,
};
my %start_step = (
   "START" => START,
   "PATHS" => PATHS,
   "TRANSFORMATIONS" => TRANSFORMATIONS,
   "IMAGES" => IMAGES,
   "IMAGES_RESIZED" => IMAGES_RESIZED,
);
use constant {
    COPY => 1,
    V_SCALE => 2,
    C_SCALE => 3,
    VH_RANDOM => 4,
    V_FLATTEN => 5,
    CROSSFADE => 6,
    CROSSFADE_FINE => 7,
    CROSSFADE_FINE_ALTERNATE => 8,
    SMOOTHING => 9,
    BLUR => 10,
    FILLANDSTROKE => 11,
};
my %transformation_function = (
   "COPY" => COPY,
   "V_SCALE" => V_SCALE,
   "C_SCALE" => C_SCALE,
   "VH_RANDOM" => VH_RANDOM,
   "V_FLATTEN" => V_FLATTEN,
   "CROSSFADE" => CROSSFADE,
   "CROSSFADE_FINE" => CROSSFADE_FINE,
   "CROSSFADE_FINE_ALTERNATE" => CROSSFADE_FINE_ALTERNATE,
   "SMOOTHING" => SMOOTHING,
   "BLUR" => BLUR,
   "FILLANDSTROKE" => FILLANDSTROKE,
);

##################################################################
# COLOR MIXTURE
##################################################################
sub mix_color {
    my ($target_color, $initial_color, $local_percentage) = @_;
    my $target_red = '0x' . substr $target_color , 0 , 2;
    my $target_green = '0x' . substr $target_color , 2 , 2;
    my $target_blue = '0x' . substr $target_color , 4 , 2;
    my $initial_red = '0x' . substr $initial_color , 0 , 2;
    my $initial_green = '0x' . substr $initial_color , 2 , 2;
    my $initial_blue = '0x' . substr $initial_color , 4 , 2;
    my $mix_red = (1 - $local_percentage) * (eval $target_red) + $local_percentage * (eval $initial_red);
    my $mix_green = (1 - $local_percentage) * (eval $target_green) + $local_percentage * (eval $initial_green);
    my $mix_blue = (1 - $local_percentage) * (eval $target_blue) + $local_percentage * (eval $initial_blue);
    $mix_red = sprintf("%02X" , $mix_red);
    $mix_green = sprintf("%02X" , $mix_green);
    $mix_blue = sprintf("%02X" , $mix_blue);
    my $mixed_color = $mix_red . $mix_green . $mix_blue;
    # print "target_color initial_color local_percentage: $target_color, $initial_color, $local_percentage / mixed_color: $mixed_color \n";
    return $mixed_color;
}

if( @ARGV != 16 && @ARGV != 18 ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: minimal number of arguments: 16 or 18 not ", 0+@ARGV;
}

# output_file_name
if( $ARGV[0] ne "-o" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: -o expected for argt 1";
}
$output_file_name_wo_extension = $ARGV[1];

# input_file_name
if( $ARGV[2] ne "-i" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: -i expected for argt 3";
}
$input_file_name = $ARGV[3];

# start time
if( $ARGV[4] ne "-ss" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: -ss expected for argt 5";
}
$start_time = $ARGV[5];

# duration
if( $ARGV[6] ne "-t" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: -t expected for argt 7";
}
$duration = $ARGV[7];

#  number of layers
if( $ARGV[8] ne "-l" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: -l expected for argt 9";
}
$nb_layers = $ARGV[9];
if( $nb_layers != 2 && $nb_layers != 4 ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: only 2 or 4 layers currently";
}

#  possible shortcut to avoid initial steps of the algorithm
if( $ARGV[10] ne "--start_step" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --start_step expected for argt 11";
}
$start_step_string = $ARGV[11];
if (!$start_step{$start_step_string}) {
    print $USAGE; die $start_step_string," does not exist in the hash %start_step\n";
}
$start_step = $start_step{$start_step_string};

#  SVG path transformation
if( $ARGV[12] ne "--transformation" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --transformation expected for argt 13";
}
$transformation_function_string = $ARGV[13];
if (!$transformation_function{$transformation_function_string}) {
    print $USAGE; die $transformation_function_string," does not exist in the hash %start_step\n";
}
$transformation_function = $transformation_function{$transformation_function_string};

#  SVG path transformation length
if( $ARGV[14] ne "--transformation_length" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --transformation_length expected for argt 15";
}
$transformation_length = $ARGV[15];

if($transformation_function == CROSSFADE || $transformation_function == CROSSFADE_FINE 
    || $transformation_function == CROSSFADE_FINE_ALTERNATE) {
    # reads the second video input for crossfading
    # input_second_file_name
    if( $ARGV[16] ne "--input-second-video" ) {
        print $USAGE; die"SVG_movie_posterize_video.pl: --input-second-video expected for argt 17";
    }
    $input_second_file_name = $ARGV[17];
    $input_second_file_name_wo_extension = $input_second_file_name;
    $input_second_file_name_wo_extension =~ s/(.*)\..*/$1/;
    while($input_second_file_name_wo_extension =~ /\//) {
        $input_second_file_name_wo_extension =~ s/.*\/(.*)/$1/;
    }
    $input_second_file_name_wo_extension .= "_SVG"
}

# VIDEO POSTERIZATION

if($start_step <= START) {
    # from video to images
    if ( !-d "movie_PNGs/${output_file_name_wo_extension}" ) {
        mkdir "movie_PNGs/${output_file_name_wo_extension}" or die "Failed to create path: SVG_movie_PNGs/${output_file_name_wo_extension}";
    }
    print "ffmpeg -i $input_file_name -ss ${start_time} -t ${duration} -r 24 movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_%4d.png\n";
    system( "ffmpeg -i $input_file_name -ss ${start_time} -t ${duration} -r 24 movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_%4d.png");
    # auxiliary video for crossfading 
    if(defined $input_second_file_name) {
        if ( !-d "movie_PNGs/${input_second_file_name_wo_extension}" ) {
            mkdir "movie_PNGs/${input_second_file_name_wo_extension}" or die "Failed to create path: SVG_movie_PNGs/${input_second_file_name_wo_extension}";
        }
        print "ffmpeg -i $input_second_file_name -ss ${start_time} -t ${duration} -r 24 movie_PNGs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_%4d.png\n";
        system( "ffmpeg -i $input_second_file_name -ss ${start_time} -t ${duration} -r 24 movie_PNGs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_%4d.png");
    }
}

# count the images
my $FILEcount;
open($FILEcount, "ls -1 movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_*.png | wc -l|");
my $nb_files = <$FILEcount>;
chomp($nb_files);
close($FILEcount);
# auxiliary video for crossfading 
if(defined $input_second_file_name) {
    my $secondFILEcount;
    open($secondFILEcount, "ls -1 movie_PNGs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_*.png | wc -l|");
    my $second_nb_files = <$secondFILEcount>;
    chomp($second_nb_files);
    close($secondFILEcount);
    $nb_files = ($nb_files < $second_nb_files? $nb_files : $second_nb_files);
    print "Movie $input_file_name and $input_second_file_name have generated $nb_files images\n";
}
else {
    print "Movie $input_file_name has generated $nb_files images\n";
}

if($start_step <= PATHS) {
    # creates the directories in which the paths files are saved
    if ( !-d "movie_PATHs/${output_file_name_wo_extension}" ) {
        mkdir "movie_PATHs/${output_file_name_wo_extension}" or die "Failed to create path: movie_PATHs/${output_file_name_wo_extension}";
    }
    if ( !-d "movie_SVGs/${output_file_name_wo_extension}" ) {
        mkdir "movie_SVGs/${output_file_name_wo_extension}" or die "Failed to create path: movie_SVGs/${output_file_name_wo_extension}";
    }
    for(my $i = 1 ; $i <= $nb_files ; $i++) {
        my $count = sprintf "%04d", $i;
        print "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_posterize.pl --nb-layers ${nb_layers} -o movie_PATHs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.path movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png\n";
        system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_posterize.pl --nb-layers ${nb_layers} -o movie_PATHs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.path movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png");
    }
    # auxiliary video for crossfading 
    if(defined $input_second_file_name) {
        # creates the directories in which the paths files are saved
        if ( !-d "movie_PATHs/${input_second_file_name_wo_extension}" ) {
            mkdir "movie_PATHs/${input_second_file_name_wo_extension}" or die "Failed to create path: movie_PATHs/${input_second_file_name_wo_extension}";
        }
        if ( !-d "movie_SVGs/${input_second_file_name_wo_extension}" ) {
            mkdir "movie_SVGs/${input_second_file_name_wo_extension}" or die "Failed to create path: movie_SVGs/${input_second_file_name_wo_extension}";
        }
        for(my $i = 1 ; $i <= $nb_files ; $i++) {
            my $count = sprintf "%04d", $i;
            print "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_posterize.pl --nb-layers ${nb_layers} -o movie_PATHs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_${count}.path movie_SVGs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_${count}.svg -i movie_PNGs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_${count}.png\n";
            system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_posterize.pl --nb-layers ${nb_layers} -o movie_PATHs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_${count}.path movie_SVGs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_${count}.svg -i movie_PNGs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_${count}.png");
        }
    }
}

if($start_step <= TRANSFORMATIONS) {
    # creates the directories in which the paths files are saved
    if ( !-d "movie_transformed_SVGs/${output_file_name_wo_extension}" ) {
        mkdir "movie_transformed_SVGs/${output_file_name_wo_extension}" or die "Failed to create path: movie_transformed_SVGs/${output_file_name_wo_extension}";
    }
    for(my $i = 1 ; $i <= $nb_files ; $i++) {
        my $count = sprintf "%04d", $i;
        my $percentage = 1.0 - ($i / (24 * $transformation_length));
        # auxiliary video for crossfading 
        my $additional_arguments = "";
        if(defined $input_second_file_name) {
            $additional_arguments 
                = "--input-second-SVGs movie_SVGs/${input_second_file_name_wo_extension}/${input_second_file_name_wo_extension}_${count}.svg";
        }
        print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_transformed_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}\n";
        system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_transformed_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}");
    }
}

if($start_step <= IMAGES) {
    # from paths to large PNG images
    if ( !-d "SVG_movie_PNGs_large/${output_file_name_wo_extension}" ) {
        mkdir "SVG_movie_PNGs_large/${output_file_name_wo_extension}" or die "Failed to create path: SVG_movie_PNGs_large/${output_file_name_wo_extension}";
    }
    for(my $i = 1 ; $i <= $nb_files ; $i++) {
        my $count = sprintf "%04d", $i;
        my $percentage = 1.0 - ($i / (24 * $transformation_length));
        # SVG to image conversion
        my $bg_color = '#FFFFFF';
        if($transformation_function == FILLANDSTROKE) {
            if($percentage >= 0.75) {
                $bg_color = '#000000';
            }
            elsif($percentage >= 0.5) {
                my $local_percentage = ($percentage - 0.5) * 4;
                $bg_color = mix_color('FFFFFF', '000000', $local_percentage);
                $bg_color = '#' . ${bg_color};
            }
            elsif($percentage >= 0.25) {
            }
            else {
                my $local_percentage = $percentage * 4 / 2.0 + 0.5;
                $bg_color = mix_color('FF0000', 'FFFFFF', $local_percentage);
                $bg_color = '#' . ${bg_color};
            }
        }

        print "inkscape -z -w 3860 -h 2180 --export-area-page -b ${bg_color} -e SVG_movie_PNGs_large/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_transformed_${count}.svg","\n";
        system( "inkscape -z -w 3860 -h 2180 --export-area-page -b ${bg_color} -e SVG_movie_PNGs_large/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_transformed_${count}.svg" ); 
    }
}

if($start_step <= IMAGES_RESIZED) {
    # resize the PNG images with resize_png and crop and then use vegas pro to generate the video 
    if ( !-d "SVG_movie_PNGs/${output_file_name_wo_extension}" ) {
        mkdir "SVG_movie_PNGs/${output_file_name_wo_extension}" or die "Failed to create path: SVG_movie_PNGs/${output_file_name_wo_extension}";
    }
    # image rescale
    for(my $i = 1 ; $i <= $nb_files ; $i++) {
        my $count = sprintf "%04d", $i;
        print "convert SVG_movie_PNGs_large/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png -crop 3800x2138+10+10 -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 SVG_movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png\n";
        system("convert SVG_movie_PNGs_large/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png -crop 3800x2138+10+10 -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 SVG_movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png");
    }
}
