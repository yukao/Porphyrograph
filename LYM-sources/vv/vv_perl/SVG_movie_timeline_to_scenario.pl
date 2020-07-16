#!/usr/bin/perl 

use strict;
use warnings;

use POSIX qw/floor/;

# SVG_movie_timeline_to_scenario.pl
# reads a transformation scenario and executes it

my $USAGE =
"/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_timeline_to_scenario.pl -i scenario_file

ARGUMENTS:
   -i: input DaVinci Resolve timeline
   -o: output csv scenario
";

my $full_svg_input_file_name;
my $full_svg_output_file_name;

my $timeline_file_name;
my $scenario_file_name;
my $FILEout;
my $FILEin;
my $line;

##################################################################
# ARGUMENTS
##################################################################
if( @ARGV != 4 ) {
    print $USAGE; die"SVG_movie_timeline_to_scenario.pl: number of arguments: 4 not ", 0+@ARGV;
}

# scenario_file_name
if( $ARGV[0] ne "-i" ) {
    print $USAGE; die"SVG_movie_timeline_to_scenario.pl: -i expected for argt 1";
}
$timeline_file_name = $ARGV[1];

# scenario_file_name
if( $ARGV[2] ne "-o" ) {
    print $USAGE; die"SVG_movie_timeline_to_scenario.pl: -o expected for argt 3";
}
$scenario_file_name = $ARGV[3];

print "Timeline file ", $timeline_file_name,"\n";
open(  $FILEin , '<:crlf', "<$timeline_file_name" ) 
  || die "Can't open file $timeline_file_name!";

print "Scenario file ", $scenario_file_name,"\n";
open(  $FILEout , ">$scenario_file_name" ) 
  || die "Can't open file $scenario_file_name!";

# count the images from PNG files
my $FILEcount;
my $nb_scenes;
open($FILEcount, "cat ${timeline_file_name} | grep \\<clip| grep mp4|wc -l|");
$nb_scenes = <$FILEcount> + 0;
chomp($nb_scenes);
close($FILEcount);

print  $FILEout "RANK,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width
TYPE,string,string,string,string,string,int,int,int,string,float,hexcolor,hexcolor,hexcolor,hexcolor,hexcolor,float,float,float,float,hexcolor,float,float
ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width
scenario,", $nb_scenes , ",,,,,,,,,,,,,,,,,,,,,
";

my $scene_no = 1;
my $scene_ID;
my $imageOutputIndex_Ini = 0;
my $imageOutputIndex_Fin = 0;
my $imageInputIndex_Ini = 0;
my $imageInputIndex_Fin = 0;
while($line = <$FILEin>) {
  if($line =~ /<clip/ && $line =~ /mp4/) {
    $imageOutputIndex_Ini = $imageOutputIndex_Fin + 1;

    # print $line;
    my $offset_string = 0;
    my $offset_seconds_float = 0;
    my $offset_seconds_int = 0;
    my $offset_minutes_int = 0;
    my $offset_second_minutes_int = 0;
    my $offset_frames_second_minutes = 0;
    my $offset_frames = 0;
    if($line =~ /offset=\"([^\"]+)\"/) {
      $offset_string = $1;
      if($offset_string =~ /^([0-9]+)\/([0-9]+)s$/) {
        $offset_seconds_float = $1/$2 - 3600;

        # for verifying the correspondence with the DaVinci storyboard at 24FPS
        $offset_seconds_int = int(floor($offset_seconds_float));
        $offset_frames_second_minutes = int(($offset_seconds_float-$offset_seconds_int) * 24);
        # for verifying the correspondence with the DaVinci storyboard at 24FPS
        # printf "offset %d:%d\n", $offset_seconds_int , $offset_frames_second_minutes;
        $offset_second_minutes_int = $offset_seconds_int % 60;
        $offset_minutes_int = int(($offset_seconds_int - $offset_second_minutes_int)/60);
        # printf "offset %d:%d:%d\n", 
        #    $offset_minutes_int , $offset_second_minutes_int , $offset_frames_second_minutes;

        # for the final video frames that are made at 29.97 FPS
        $offset_frames = int(floor($offset_seconds_float * 29.97));  # for imageOutputIndex
      }
    }
    my $duration_string = 0;
    my $duration_seconds_float = 0;
    my $duration_seconds_int = 0;
    my $duration_minutes_int = 0;
    my $duration_second_minutes_int = 0;
    my $duration_frames_second_minutes = 0;
    my $duration_frames = 0;
    if($line =~ /duration=\"([^\"]+)\"/) {
      $duration_string = $1;
      if($duration_string =~ /^([0-9]+)\/([0-9]+)s$/) {
        $duration_seconds_float = $1/$2;

        # for verifying the correspondence with the DaVinci storyboard at 24FPS
        $duration_seconds_int = int(floor($duration_seconds_float));
        $duration_frames_second_minutes = int(($duration_seconds_float-$duration_seconds_int) * 24);
        # for verifying the correspondence with the DaVinci storyboard at 24FPS
        # printf "duration %d:%d\n", $duration_seconds_int , $duration_frames_second_minutes;
        $duration_second_minutes_int = $duration_seconds_int % 60;
        $duration_minutes_int = int(($duration_seconds_int - $duration_second_minutes_int)/60);
        # printf "duration %d:%d:%d\n", 
        #    $duration_minutes_int , $duration_second_minutes_int , $duration_frames_second_minutes;

        # for the final video frames that are made at 29.97 FPS
        $duration_frames = int(floor($duration_seconds_float * 29.97));  # for imageOutputIndex & imageOutputIndex
     }
    }
    my $start_string = 0;
    my $start_seconds_float = 0;
    my $start_seconds_int = 0;
    my $start_minutes_int = 0;
    my $start_second_minutes_int = 0;
    my $start_frames_second_minutes = 0;
    my $start_frames = 0;
    if($line =~ /start=\"([^\"]+)\"/) {
      $start_string = $1;
      if($start_string =~ /^([0-9]+)\/([0-9]+)s$/) {
        $start_seconds_float = $1/$2;

        # for verifying the correspondence with the DaVinci storyboard at 24FPS
        $start_seconds_int = int(floor($start_seconds_float));
        $start_frames_second_minutes = int(($start_seconds_float-$start_seconds_int) * 24);
        # printf "start %d:%d\n", $start_seconds_int , $start_frames_second_minutes;
        $start_second_minutes_int = $start_seconds_int % 60;
        $start_minutes_int = int(($start_seconds_int - $start_second_minutes_int)/60);
        # printf "start %d:%d:%d\n\n", 
        #    $start_minutes_int , $start_second_minutes_int , $start_frames_second_minutes;

        # for the final video frames that are made at 29.97 FPS
        $start_frames = int(floor($start_seconds_float * 29.97));  # for imageInputIndex
      }
    }
    my $video_name = "";
    my $video_short_name = "";
    if($line =~ /name=\"([^\"]+)\"/) {
      $video_name = $1;
      $video_short_name = $video_name;
      $video_short_name =~ s/_bw\.mp4//;
    }

    # indices for images access
    $imageOutputIndex_Fin = int(floor(($offset_seconds_float + $duration_seconds_float) * 29.97)) - 1;
    $imageInputIndex_Ini = int(floor($start_seconds_float * 29.97)) + 1;
    $imageInputIndex_Fin = $imageInputIndex_Ini + $imageOutputIndex_Fin - $imageOutputIndex_Ini;

    # print $video_name , "\n";
    my $count = sprintf "%03d", $scene_no;
    $scene_ID = "scene_" . $count . "_" . $video_short_name;
    printf $FILEout "scene,%s,,,,,,,,,,,,,,,,,,,,,
%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s
ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width
,/mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_SVGs/%s/,%s,/mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_transformed_SVGs/TawTra/,TawTra,/mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/tmp/,%d,%d,4,VH_RANDOM,0.0,#ffffff,#c0c0c0,#808080,#404040,#000000,1,1,1,1,#000000,1,0
,/mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_SVGs/%s/,%s,/mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_transformed_SVGs/TawTra/,TawTra,/mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/tmp/,%d,%d,4,VH_RANDOM,0.0,#ffffff,#c0c0c0,#808080,#404040,#000000,1,1,1,1,#000000,1,0
,s,s,s,s,s,s,s,s,s,l,l,l,l,l,l,l,l,l,l,l,l,l
/scene,,,,,,,,,,,,,,,,,,,,,,
" , $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $video_short_name, $video_short_name, $imageInputIndex_Ini, $imageOutputIndex_Ini, $video_short_name, $video_short_name, $imageInputIndex_Fin, $imageOutputIndex_Fin; 

    $scene_no++;
  }
}
printf $FILEout "/scenario,,,,,,,,,,,,,,,,,,,,,,
";
close($FILEout);
close($FILEin);
