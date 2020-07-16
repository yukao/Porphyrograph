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
" /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_posterization/SVG_movie_posterize_video.pl -o SVG_movies/UW1_3_01.mp4 
   -i ../UW1_3.mp4 -ss 00:04:16 -t 00:00:06 -l 2 --fade_in_length 24 --fade_out_length 24 --start_step TRANSFORMATIONS --stop_step IMAGES 
   --transformation V_SCALE --transformation_offset 30 --transformation_length 30 (--input-second-video ../Video-sources/5187/5187_40s.mp4)? 
   --composition-function OVERLAY --composition-file movie_PNGs/5187_SVG_sky.png
   --background_color_fonction BLEND --background_color_offset 30 --background_color_length 40 --background_color_initial 'FFFFFF' --background_color_final '000000'

ARGUMENTS:
   -o: ouput file
   -i: input file
   -ss: initial video start time
   -t: initial video duration
   -l: number of posterization layers
   --fade_in_length: fade in length in seconds
   --fade_out_length: fade out length in seconds
   --start_step: step in the posterization process to restart from
   --stop_step: step in the posterization process to stop at
   --transformation: name of the transformation procedure applied
   --transformation_offset: duration of the transformation offset in seconds (duration before starting interpolation)
   --transformation_length: duration of the transformation interpolation in seconds
   --input-second-paths: for crossfading or other composition of 2 videos, provides the second SVG input
   --background_color_function: name of the background_color procedure applied
   --background_color_offset: duration of the background_color offset in seconds (duration before starting interpolation)
   --background_color_length: duration of the background_color interpolation in seconds
   --background_color_initial: initial value of the background_color (XXXXXX)
   --background_color_final: duration of the background_color (XXXXXX)
";

my $input_file_name;
my $start_time;
my $duration;
my $output_file_name_wo_extension;
my $crossfaded_input_file_name_wo_extension;
my $crossfaded_output_name_wo_extension;
my $crossfaded_input_file_duration;
my $nb_layers;
my $fade_in_length;
my $fade_out_length;
my $start_step_string;
my $start_step;
my $stop_step_string;
my $stop_step;
my $transformation_function_string;
my $transformation_function;
my $transformation_offset;
my $transformation_length;
my $composition_function_string;
my $composition_function;
my $composition_file;
my $background_color_function_string;
my $background_color_function;
my $background_color_offset;
my $background_color_length;
my $background_color_initial;
my $background_color_final;
use constant {
    START   => 1,
    COMPOSITION   => 2,
    PATHS   => 3,
    TRANSFORMATIONS  => 4,
    IMAGES  => 5,
};
my %process_step = (
   "START" => START,
   "PATHS" => PATHS,
   "COMPOSITION" => COMPOSITION,
   "TRANSFORMATIONS" => TRANSFORMATIONS,
   "IMAGES" => IMAGES,
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
    CROSSFADE_ALTERNATE => 9,
    SMOOTHING => 10,
    BLUR => 11,
    FILLANDSTROKE => 12,
    V_SCALE2 => 13,
    V_SCALE3 => 14,
    C_SCALE2 => 15,
    C_SCALE_TO_BLACK => 16,
    C_SCALE_TO_BLACK_FROM_NEG => 17,
    VH_RANDOM_LINE => 18,
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
   "CROSSFADE_ALTERNATE" => CROSSFADE_ALTERNATE,
   "SMOOTHING" => SMOOTHING,
   "BLUR" => BLUR,
   "FILLANDSTROKE" => FILLANDSTROKE,
   "V_SCALE2" => V_SCALE2,
   "V_SCALE3" => V_SCALE3,
   "C_SCALE2" => C_SCALE2,
   "C_SCALE_TO_BLACK" => C_SCALE_TO_BLACK,
   "C_SCALE_TO_BLACK_FROM_NEG" => C_SCALE_TO_BLACK_FROM_NEG,
   "VH_RANDOM_LINE" => VH_RANDOM_LINE,
);
use constant {
    COPY => 1,
    OVERLAY => 2,
    OVERLAY_INVERT => 3,
};
my %composition_function = (
   "COPY" => COPY,
   "OVERLAY" => OVERLAY,
   "OVERLAY_INVERT" => OVERLAY_INVERT,
);
use constant {
    COPY => 1,
    BLEND => 2,
};
my %background_color_function = (
   "COPY" => COPY,
   "BLEND" => BLEND,
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
    my $mix_red = (1 - $local_percentage) * (eval $initial_red) + $local_percentage * (eval $target_red);
    my $mix_green = (1 - $local_percentage) * (eval $initial_green) + $local_percentage * (eval $target_green);
    my $mix_blue = (1 - $local_percentage) * (eval $initial_blue) + $local_percentage * (eval $target_blue);
    $mix_red = sprintf("%02X" , $mix_red);
    $mix_green = sprintf("%02X" , $mix_green);
    $mix_blue = sprintf("%02X" , $mix_blue);
    my $mixed_color = $mix_red . $mix_green . $mix_blue;
    # print "target_color initial_color local_percentage: $target_color, $initial_color, $local_percentage / mixed_color: $mixed_color \n";
    return $mixed_color;
}

if( @ARGV != 38 && @ARGV != 44 ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: number of arguments: 38 or 44 not ", 0+@ARGV;
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

#  fade in length
if( $ARGV[10] ne "--fade_in_length" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --fade_in_length expected for argt 11";
}
$fade_in_length = $ARGV[11];

#  fade out length
if( $ARGV[12] ne "--fade_out_length" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --fade_out_length expected for argt 13";
}
$fade_out_length = $ARGV[13];

#  possible shortcut to avoid initial steps of the algorithm
if( $ARGV[14] ne "--start_step" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --start_step expected for argt 15";
}
$start_step_string = $ARGV[15];
if (!$process_step{$start_step_string}) {
    print $USAGE; die $start_step_string," does not exist in the hash %process_step\n";
}
$start_step = $process_step{$start_step_string};

#  possible shortcut to stop before final steps of the algorithm
if( $ARGV[16] ne "--stop_step" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --stop_step expected for argt 17";
}
$stop_step_string = $ARGV[17];
if (!$process_step{$stop_step_string}) {
    print $USAGE; die $stop_step_string," does not exist in the hash %process_step\n";
}
$stop_step = $process_step{$stop_step_string};

#  SVG path transformation
if( $ARGV[18] ne "--transformation" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --transformation expected for argt 19";
}
$transformation_function_string = $ARGV[19];
if (!$transformation_function{$transformation_function_string}) {
    print $USAGE; die $transformation_function_string," does not exist in the hash %transformation_function\n";
}
$transformation_function = $transformation_function{$transformation_function_string};

#  SVG path transformation offset
if( $ARGV[20] ne "--transformation_offset" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --transformation_offset expected for argt 21";
}
$transformation_offset = $ARGV[21];

#  SVG path transformation length
if( $ARGV[22] ne "--transformation_length" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --transformation_length expected for argt 23";
}
$transformation_length = $ARGV[23];
my $next_param_no = 24;

if($transformation_function == CROSSFADE || $transformation_function == CROSSFADE_FINE 
    || $transformation_function == CROSSFADE_FINE_ALTERNATE || $transformation_function == CROSSFADE_ALTERNATE) {
    # reads the second video input for crossfading
    # crossfaded_input_file_name_wo_extension
    if( $ARGV[$next_param_no] ne "--crossfaded-input-paths" ) {
        print $USAGE; die"SVG_movie_posterize_video.pl: --crossfaded-input-paths expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
    }
    $next_param_no++;
    # selects the first and second file index according to percentage
    $crossfaded_input_file_name_wo_extension = $ARGV[$next_param_no];
    $next_param_no++;

    # duration of the crossfaded file
    if( $ARGV[$next_param_no] ne "--crossfaded-input-paths-length" ) {
        print $USAGE; die"SVG_movie_posterize_video.pl: --crossfaded-input-paths-length expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
    }
    $next_param_no++;
    $crossfaded_input_file_duration = $ARGV[$next_param_no];
    $next_param_no++;

    # new name
    if( $ARGV[$next_param_no] ne "--crossfaded-ouput-paths" ) {
        print $USAGE; die"SVG_movie_posterize_video.pl: --crossfaded-ouput-paths expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
    }
    $next_param_no++;
    $crossfaded_output_name_wo_extension = $ARGV[$next_param_no];
    $next_param_no++;
}

#  PNG image composition
if( $ARGV[$next_param_no] ne "--composition" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --composition expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
}
$next_param_no++;

$composition_function_string = $ARGV[$next_param_no];
if (!$composition_function{$composition_function_string}) {
    print $USAGE; die $composition_function_string," does not exist in the hash %composition_function\n";
}
$composition_function = $composition_function{$composition_function_string};
$next_param_no++;

#  PNG image composition file (eg transparent overlay)
if( $ARGV[$next_param_no] ne "--composition_file" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --composition_file expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
}
$next_param_no++;
$composition_file = $ARGV[$next_param_no];
$next_param_no++;

#  PNG image composition
if( $ARGV[$next_param_no] ne "--background_color_function" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --background_color_function expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
}
$next_param_no++;

$background_color_function_string = $ARGV[$next_param_no];
if (!$background_color_function{$background_color_function_string}) {
    print $USAGE; die $background_color_function_string," does not exist in the hash %background_color_function\n";
}
$background_color_function = $background_color_function{$background_color_function_string};
$next_param_no++;


#  background for SVG to PNG conversion transformation offset
if( $ARGV[$next_param_no] ne "--background_color_offset" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --background_color_offset expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
}
$next_param_no++;
$background_color_offset = $ARGV[$next_param_no];
$next_param_no++;

#  SVG path transformation length
if( $ARGV[$next_param_no] ne "--background_color_length" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --background_color_length expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
}
$next_param_no++;
$background_color_length = $ARGV[$next_param_no];
$next_param_no++;

#  background for SVG to PNG conversion transformation offset
if( $ARGV[$next_param_no] ne "--background_color_initial" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --background_color_initial expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
}
$next_param_no++;
$background_color_initial = $ARGV[$next_param_no];
$next_param_no++;

#  SVG path transformation length
if( $ARGV[$next_param_no] ne "--background_color_final" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --background_color_final expected for argt ", ($next_param_no + 1), " not ", $ARGV[$next_param_no], "\n";
}
$next_param_no++;
$background_color_final = $ARGV[$next_param_no];
$next_param_no++;

# VIDEO POSTERIZATION

if($start_step <= START && $stop_step >= START) {
    # from video to images
    if ( !-d "movie_PNGs/${output_file_name_wo_extension}" ) {
        mkdir "movie_PNGs/${output_file_name_wo_extension}" or die "Failed to create path: SVG_movie_PNGs/${output_file_name_wo_extension}";
    }
    my $duration_option = "";
    # null duration == use the full movie (no -t option in the ffmpeg command)
    if($duration ne "00:00:00") {
        $duration_option = "-t ${duration}"
    }
    print "ffmpeg -i $input_file_name -ss ${start_time} ${duration_option} -r 24 movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_%4d.png\n";
    system( "ffmpeg -i $input_file_name -ss ${start_time} ${duration_option} -r 24 movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_%4d.png");
}

# count the images from PNG files
my $FILEcount;
open($FILEcount, "ls -1 movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_*.png | wc -l|");
my $nb_files = <$FILEcount>;
chomp($nb_files);
close($FILEcount);

# image composition: generally addtion of a mask above the initial video to make a zone such as rock or sky in a uniform color
if($start_step <= COMPOSITION && $stop_step >= COMPOSITION && $composition_function != COPY) {
    # creates the directories in which the paths files are saved
    if ( !-d "movie_composed_PNGs/${output_file_name_wo_extension}" ) {
        mkdir "movie_composed_PNGs/${output_file_name_wo_extension}" or die "Failed to create path: movie_composed_PNGs/${output_file_name_wo_extension}";
    }
    for(my $i = 1 ; $i <= $nb_files ; $i++) {
        my $count = sprintf "%04d", $i;
        # movie_PATHs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.path
        if($composition_function == OVERLAY) {
          print "composite ${composition_file} movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png\n";
          system("composite ${composition_file} movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png");
        }
        elsif($composition_function == OVERLAY_INVERT) {
          print "composite ${composition_file} movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png tmp/overlaid.png\n";
          system("composite ${composition_file} movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png tmp/overlaid.png");
          print "convert tmp/overlaid.png -channel RGB -negate movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png\n";
          system("convert tmp/overlaid.png -channel RGB -negate movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png");
        }
    }
}

# transformation of the video into a vector video of SVG files
if($start_step <= PATHS && $stop_step >= PATHS) {
    # creates the directories in which the paths files are saved
    # if ( !-d "movie_PATHs/${output_file_name_wo_extension}" ) {
    #    mkdir "movie_PATHs/${output_file_name_wo_extension}" or die "Failed to create path: movie_PATHs/${output_file_name_wo_extension}";
    # }
    if ( !-d "movie_SVGs/${output_file_name_wo_extension}" ) {
        mkdir "movie_SVGs/${output_file_name_wo_extension}" or die "Failed to create path: movie_SVGs/${output_file_name_wo_extension}";
    }
    for(my $i = 1 ; $i <= $nb_files ; $i++) {
        my $count = sprintf "%04d", $i;
        my $dimming = 1.0;
        if($fade_in_length > 0 && $count <= ($fade_in_length * 24)) {
            $dimming = ($count - 1)/($fade_in_length * 24);
        }
        elsif($fade_out_length > 0 && $count >=  ($nb_files - ($fade_out_length * 24))) {
            $dimming = ($nb_files - $count)/($fade_out_length * 24);
        }
        #clamping
        if($dimming > 1.0) {
            $dimming = 1.0;
        }
        elsif($dimming < 0.0) {
            $dimming = 0.0;
        }
        # movie_PATHs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.path
        if($composition_function != COPY) {
            print "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_posterize.pl --nb-layers ${nb_layers} --dimming ${dimming} -o movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png\n";
            system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_posterize.pl --nb-layers ${nb_layers} --dimming ${dimming} -o movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png");
        }
        else {
            print "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_posterize.pl --nb-layers ${nb_layers} --dimming ${dimming} -o movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png\n";
            system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_posterize.pl --nb-layers ${nb_layers} --dimming ${dimming} -o movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png");
        }
    }
    # auxiliary video for crossfading is supposed to be already rendered
    if(defined $crossfaded_input_file_name_wo_extension) {
        # creates the directories in which the paths files are saved
        if ( !-e "movie_SVGs/${crossfaded_input_file_name_wo_extension}" ) {
            die "Expected posterized file for crossfading not found: movie_SVGs/${crossfaded_input_file_name_wo_extension}";
        }
    }
}

# re-counts the images from SVG files
open($FILEcount, "ls -1 movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_*.svg | wc -l|");
$nb_files = <$FILEcount>;
chomp($nb_files);
close($FILEcount);
my $second_nb_files = 0;
if(defined $crossfaded_input_file_name_wo_extension) {
    my $secondFILEcount;
    open($secondFILEcount, "ls -1 movie_SVGs/${crossfaded_input_file_name_wo_extension}/${crossfaded_input_file_name_wo_extension}_*.svg | wc -l|");
    $second_nb_files = <$secondFILEcount>;
    chomp($second_nb_files);
    close($secondFILEcount);
    print "Movie $output_file_name_wo_extension and $crossfaded_input_file_name_wo_extension have generated $nb_files and $second_nb_files images\n";
}
else  {
    print "Movie $output_file_name_wo_extension has generated $nb_files images\n";
}

# applies possible transformations on the SVG files
if($start_step <= TRANSFORMATIONS && $stop_step >= TRANSFORMATIONS && $transformation_function != COPY) {
    if($transformation_function == CROSSFADE
        || $transformation_function == CROSSFADE_FINE
        || $transformation_function == CROSSFADE_FINE_ALTERNATE
        || $transformation_function == CROSSFADE_ALTERNATE) {
        # creates the directories in which the paths files are saved
        if ( !-d "movie_SVGs/${crossfaded_output_name_wo_extension}" ) {
            mkdir "movie_SVGs/${crossfaded_output_name_wo_extension}" or die "Failed to create path: movie_SVGs/${crossfaded_output_name_wo_extension}";
        }
        printf "begin offset $transformation_offset (%d), length $transformation_length , end  %d (%d), second index %d\n" , 24 * ($transformation_offset) + 1 , 
                  ($transformation_offset + $transformation_length), 24 * ($transformation_offset + $transformation_length), 24 * ($transformation_offset) + 1 - 24 * ($transformation_offset);
        for(my $i = 1 ; $i <= 24 * ($transformation_offset) ; $i++) {
            my $count = sprintf "%04d", $i;
            print "cp movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg movie_SVGs/${crossfaded_output_name_wo_extension}/${crossfaded_output_name_wo_extension}_${count}.svg\n";
            system("cp movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg movie_SVGs/${crossfaded_output_name_wo_extension}/${crossfaded_output_name_wo_extension}_${count}.svg");
        }
        for(my $i = 24 * ($transformation_offset) + 1 ; $i <= 24 * ($transformation_offset + $transformation_length) ; $i++) {
            my $index_file = $i;
            my $second_index_file = $i - 24 * ($transformation_offset);         
            my $count = sprintf "%04d", $index_file;
            my $second_count = sprintf "%04d", $second_index_file;
            # auxiliary video for crossfading 
            my $additional_arguments = "";
            if(defined $crossfaded_input_file_name_wo_extension) {
                $additional_arguments 
                    = "--input-second-SVGs movie_SVGs/${crossfaded_input_file_name_wo_extension}/${crossfaded_input_file_name_wo_extension}_${second_count}.svg";
            }
            if($index_file <= $nb_files && $second_index_file <= $second_nb_files) 
            {
                my $percentage = ($i - 24 * ($transformation_offset))/(24 * $transformation_length) ;
                print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_SVGs/${crossfaded_output_name_wo_extension}/${crossfaded_output_name_wo_extension}_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}\n";
                system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_SVGs/${crossfaded_output_name_wo_extension}/${crossfaded_output_name_wo_extension}_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}");
            }
        }
        for(my $i = 24 * ($transformation_offset + $transformation_length) + 1 ; $i <= 24 * ($transformation_offset) + $second_nb_files ; $i++) {
            my $second_index_file = $i - 24 * ($transformation_offset);
            my $second_count = sprintf "%04d", $second_index_file;
            my $count = sprintf "%04d", $i;
            print "cp movie_SVGs/${crossfaded_input_file_name_wo_extension}/${crossfaded_input_file_name_wo_extension}_${second_count}.svg movie_SVGs/${crossfaded_output_name_wo_extension}/${crossfaded_output_name_wo_extension}_${count}.svg\n";
            system("cp movie_SVGs/${crossfaded_input_file_name_wo_extension}/${crossfaded_input_file_name_wo_extension}_${second_count}.svg movie_SVGs/${crossfaded_output_name_wo_extension}/${crossfaded_output_name_wo_extension}_${count}.svg");
        }
    }
    else {
        # creates the directories in which the paths files are saved
        if ( !-d "movie_transformed_SVGs/${output_file_name_wo_extension}" ) {
            mkdir "movie_transformed_SVGs/${output_file_name_wo_extension}" or die "Failed to create path: movie_transformed_SVGs/${output_file_name_wo_extension}";
        }
        for(my $i = 1 ; $i <= $nb_files ; $i++) {
            my $count = sprintf "%04d", $i;
            my $percentage = 1.0;
            my $additional_arguments = "";
            if($i <= 24 * $transformation_offset) {
                $percentage = 1.0;
                if($transformation_function == C_SCALE2) {
                    print "cp movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg\n";
                    system("cp movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg");
                }
                else {
                    print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}\n";
                    system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}");
                }
            }
            elsif($i >= (24 * $transformation_offset)
                && $i <= (24 * ($transformation_offset + $transformation_length))) {
                $percentage = 1.0 - (($i -24.0 * $transformation_offset) / (24.0 * $transformation_length));
                #clamping
                if($percentage > 1.0) {
                    $percentage = 1.0;
                }
                elsif($percentage < 0.0) {
                    $percentage = 0.0;
                }
                print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}\n";
                system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}");
            }
            else {
                $percentage = 0.0;
                print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}\n";
                system("/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers ${nb_layers} -o movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg -i movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg --transformation ${transformation_function_string} --percent ${percentage} ${additional_arguments}");
            }
        }
    }
}

# transforms vector inmages back into bitmaps
if($start_step <= IMAGES && $stop_step >= IMAGES) {
    # from paths to large PNG images
    # resize the PNG images with resize_png and crop and then use DAVinci Resolve to generate the video 
    if ( !-d "SVG_movie_PNGs/${output_file_name_wo_extension}" ) {
        mkdir "SVG_movie_PNGs/${output_file_name_wo_extension}" or die "Failed to create path: SVG_movie_PNGs/${output_file_name_wo_extension}";
    }
    for(my $i = 1 ; $i <= $nb_files ; $i++) {
        my $count = sprintf "%04d", $i;
        # SVG to image conversion
        my $bg_color = '#FFFFFF';

        if($background_color_function == BLEND) {
            my $percentage = 1.0;
            if($i <= 24 * $background_color_offset) {
                $percentage = 1.0;
            }
            elsif($i >= 24 * $background_color_offset
                && $i <= 24 * ($background_color_offset + $background_color_length)) {
                $percentage = 1.0 - (($i -24.0 * $background_color_offset) / (24.0 * $background_color_length));
            }
            else {
                $percentage = 0.0;
            }
            #clamping
            if($percentage > 1.0) {
                $percentage = 1.0;
            }
            elsif($percentage < 0.0) {
                $percentage = 0.0;
            }

            # background color calculation as an interpolation between white and red
            # for the second half of the interpolation (color), the first half of the interpolation
            # is about stroke width (should be remade)
            if($percentage >= 1) {
                $bg_color = '#' . $background_color_initial;
            }
            elsif($percentage >= 0.0) {
                $bg_color = mix_color($background_color_initial, $background_color_final, $percentage);
                $bg_color = '#' . ${bg_color};
            }
            else {
                $bg_color = '#' . $background_color_final;
            }
        }
        # converts the SVG into a bitmap, larger than the final image for a better quality and to crop the border
        if($transformation_function != COPY) {
            print "inkscape -z -w 3860 -h 2180 --export-area-page -b ${bg_color} -e tmp/tmp_large.png movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg","\n";
            system( "inkscape -z -w 3860 -h 2180 --export-area-page -b ${bg_color} -e tmp/tmp_large.png movie_transformed_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg" ); 
        }
        else {
            print "inkscape -z -w 3860 -h 2180 --export-area-page -b ${bg_color} -e tmp/tmp_large.png movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg","\n";
            system( "inkscape -z -w 3860 -h 2180 --export-area-page -b ${bg_color} -e tmp/tmp_large.png movie_SVGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.svg" ); 
        }
        # cropping and downscale of the png inmages
        print "convert tmp/tmp_large.png -crop 3800x2138+10+10 -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 SVG_movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png\n";
        system("convert tmp/tmp_large.png -crop 3800x2138+10+10 -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 SVG_movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png");
    }
}
