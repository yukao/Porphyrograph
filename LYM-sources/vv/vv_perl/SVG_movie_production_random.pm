#!/usr/bin/perl 

use strict;
use warnings;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_scenario_interpolation_lib;
use SVG_movie_scenario_transformation;
use SVG_movie_scenario_layer_crossfading;

# SVG_movie_production_loop.pl
# reads a transformation scenario and executes it

##################################################################
# MAIN
##################################################################
sub MAIN {
  my $USAGE =
  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_production_random.pm

  ARGUMENTS:
  ";

  ##################################################################
  # ARGUMENTS
  ##################################################################
  if( @_ != 0 ) {
      print $USAGE; 
      print"SVG_movie_production_random.pm: number of arguments: 0 not ", 0+@_, "\n";
      return 0;
  }

  my $transformation_scenario_file_name;
  my $crossfading_scenario_file_name;
  my $FILEout;
  my $line;

  my $trace_only = 0;

  ##################################################################
  # HASHES FOR CLIP MANAGEMENT
  ##################################################################
  my %recorded_clip_sizes = (
     "contour-01" => 125,
     "contour-02" => 125,
     "contour-03" => 150,
     "contour-04" => 150,
     "contour-05" => 95,
     "contour-06" => 95,
     "contour-07" => 87,
     "contour-08" => 87,
     "contour-09" => 87,
     "contour-10" => 87,
     "contour-11" => 121,
     "contour-12" => 121,
     "contour-13" => 137,
     "contour-14" => 137,
     "contour-15" => 67,
     "contour-16" => 67,
     "contour-17" => 110,
     "contour-18" => 109,
  );

  # hash of arrays, each arra contains
  # ($clip_string, $clip_size, $clip_first_frame_no, $clip_fill_color, 
  # $clip_contour_color, $clip_contour_width, $clip_noise)
  my %active_clips;

  my $nb_recorded_clips = int(keys %recorded_clip_sizes);
  my $nb_active_clips = int(keys %active_clips);

  ##################################################################
  # FRAME BY FRAME CLIP MANAGEMENT
  ##################################################################
  my $transformed_movie_dir;
  my $tmp_dir_name;
  $tmp_dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/";
  $transformed_movie_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_transformed_SVGs/contour/";

  my $NB_FRAMES_MOVIE = 120 * 30; # 2 mminutes video
  $NB_FRAMES_MOVIE = 1; # 2 mminutes video
  my $DELAY_BETWEEN_CLIP = 1 * 30; # 1 s between the end of a clip and the choice of another one

  #TMP for(my $ind_scene = 0; $ind_scene < 1; $ind_scene++) {
  for(my $current_crossfaded_frame_out = 0; 
      $current_crossfaded_frame_out < $NB_FRAMES_MOVIE ; 
      $current_crossfaded_frame_out++) {

    # number of transformed inputs
    my $nb_transformed_inputs = 0;

    my $fill_color_min = 0;
    my $contour_width_min = 0;
    my $contour_color_min = 0;
    my $noise_min = 0;

    my $fill_color_var = 0;
    my $contour_width_var = 0;
    my $contour_color_var = 0;
    my $noise_var = 0;

    my $scene_percentage = 0;
    ##################################################################
    # CURRENT PARAMETERS
    ##################################################################
    if($current_crossfaded_frame_out <= int($NB_FRAMES_MOVIE/3)) {
      $scene_percentage = 0;
      if(int($NB_FRAMES_MOVIE/3) > 0) {
        $scene_percentage = ($current_crossfaded_frame_out - 0)/int($NB_FRAMES_MOVIE/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

      $nb_transformed_inputs = 1 + int($scene_percentage * 2);

      $fill_color_min = 0.0;
      $contour_color_min = 0.0;
      $noise_min = 0.0;
      $contour_width_min = 0.0;

      $fill_color_var = $scene_percentage * 0.1;
      $contour_color_var = $scene_percentage * 0.1;
      $noise_var = $scene_percentage * 0.1;
      $contour_width_var = $scene_percentage * 1;
    }
    elsif($current_crossfaded_frame_out <= 2 * int($NB_FRAMES_MOVIE/3)) {
      $scene_percentage = 0;
      if(int($NB_FRAMES_MOVIE/3) > 0) {
        $scene_percentage = ($current_crossfaded_frame_out - int($NB_FRAMES_MOVIE/3))/int($NB_FRAMES_MOVIE/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

      $nb_transformed_inputs = 3 + int($scene_percentage * 7);

      $fill_color_min = $scene_percentage * 0.1;
      $contour_color_min = $scene_percentage * 0.1;
      $noise_min = $scene_percentage * 0.1;
      $contour_width_min = $scene_percentage * 1;

      $fill_color_var = $scene_percentage * 0.2;
      $contour_color_var = $scene_percentage * 0.2;
      $noise_var = $scene_percentage * 0.2;
      $contour_width_var = $scene_percentage * 2;
    }
    else {
      $scene_percentage = 0;
      if(int($NB_FRAMES_MOVIE/3) > 0) {
        $scene_percentage = ($current_crossfaded_frame_out - (2 * int($NB_FRAMES_MOVIE/3)))/int($NB_FRAMES_MOVIE/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

      $nb_transformed_inputs = 10 + int($scene_percentage * 20);

      $fill_color_min = $scene_percentage * 0.3;
      $contour_color_min = $scene_percentage * 0.3;
      $noise_min = $scene_percentage * 0.3;
      $contour_width_min = $scene_percentage * 3;

      $fill_color_var = $scene_percentage * 0.7;
      $contour_color_var = $scene_percentage * 0.7;
      $noise_var = $scene_percentage * 0.7;
      $contour_width_var = $scene_percentage * 7;
    }

    ##################################################################
    # MANAGEMENT OF CLIP ACTIVATION
    ##################################################################

    # RELEASING CLIPS THAT HAVE BEEN PLAYED UNTIL THE END
    my $key;
    foreach $key (sort(keys %active_clips)) {
      # do whatever you want with $key and $value here ...
      my ($clip_string, $clip_size, $clip_first_frame_no, $clip_fill_color, 
          $clip_contour_color, $clip_contour_width, $clip_noise) = @{ $active_clips{$key} };
      # clip deactivation
      if($current_crossfaded_frame_out - $clip_first_frame_no + 1 > $clip_size) {
        delete($active_clips{$key});
      }
    }

    # ACTIVATING NEW CLIPS UNTIL THE EXPECTED QUANTITY OF ACTIVE CLIPS IS REACHED
    # $nb_active_clips = int(keys %active_clips);
    for(my $indNewClipActivation = $nb_active_clips ; $indNewClipActivation < $nb_transformed_inputs; 
           $indNewClipActivation++) {
      my $fill_color = int(255 * ($fill_color_min + ($fill_color_var * rand(1))));
      my $mix_red = sprintf("%02X" , ($fill_color < 255? $fill_color:255));
      $fill_color = int(255 * ($fill_color_min + ($fill_color_var * rand(1))));
      my $mix_green = sprintf("%02X" , ($fill_color < 255? $fill_color:255));
      $fill_color = int(255 * ($fill_color_min + ($fill_color_var * rand(1))));
      my $mix_blue = sprintf("%02X" , ($fill_color < 255? $fill_color:255));
      my $fill_color_string = "#" . $mix_red . $mix_green . $mix_blue;

      my $contour_color = int(255 * ($contour_color_min + ($contour_color_var * rand(1))));
      $mix_red = sprintf("%02X" , ($contour_color < 255? $contour_color:255));
      $contour_color = int(255 * ($contour_color_min + ($contour_color_var * rand(1))));
      $mix_green = sprintf("%02X" , ($contour_color < 255? $contour_color:255));
      $contour_color = int(255 * ($contour_color_min + ($contour_color_var * rand(1))));
      $mix_blue = sprintf("%02X" , ($contour_color < 255? $contour_color:255));
      my $contour_color_string = "#" . $mix_red . $mix_green . $mix_blue;

      my $transf_noise = $noise_min + $noise_var * rand(1);
      my $contour_width = $contour_width_min + $contour_width_var * rand(1);

      # printf "noise %.2f contour %.2f colors F %s C %s\n",  $transf_noise, $contour_width, $fill_color_string, $contour_color_string;
      
      # random choice of a clip in the list
      my $indNewActivatedClip = int(rand(1) * $nb_recorded_clips) % $nb_recorded_clips + 1;
      my $newActivatedClipID = sprintf("contour-%02d" , $indNewActivatedClip);
      my $newActivatedClipSize = 0;
      if (exists $recorded_clip_sizes{ $newActivatedClipID }) {
         $newActivatedClipSize = $recorded_clip_sizes{ $newActivatedClipID };
      }
      else {
        print "Unknown clip ${newActivatedClipID}\n";
        return 0;
      }

      # looks for an available key for the newly active clip
      my $indKeyNewActiveClip = 1;
      my $keyNewActiveClip = sprintf("key-%02d" , $indKeyNewActiveClip);
      # hash of arrays, each arra contains
      # ($clip_string, $clip_size, $clip_first_frame_no, $clip_fill_color, 
      # $clip_contour_color, $clip_contour_width, $clip_noise)
      # my %active_clips;
      while(exists $active_clips{$keyNewActiveClip}) {
        $indKeyNewActiveClip++;
        $keyNewActiveClip = sprintf("key-%02d" , $indKeyNewActiveClip);
      }
      # ($clip_string, $clip_size, $clip_first_frame_no, $clip_fill_color, 
      # $clip_contour_color, $clip_contour_width, $clip_noise)
      $active_clips{$keyNewActiveClip} 
        = [$newActivatedClipID, $newActivatedClipSize, $current_crossfaded_frame_out, $fill_color_string, 
          $contour_color_string, $contour_width, $transf_noise];
    }

    ##################################################################
    # TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
    ##################################################################
    my $initial_transformed_frame_out = 1;
    my $final_transformed_frame_out = 1;

    my $indActiveClip = 1;
    print "Nb active clips ", ((keys %active_clips)+0),"\n";
    foreach $key (sort(keys %active_clips)) {
      my $countActiveClip = sprintf "%03d", $indActiveClip;

      # current clip parameters
      my ($clip_string, $clip_size, $clip_first_frame_no, $clip_fill_color, 
          $clip_contour_color, $clip_contour_width, $clip_noise) = @{ $active_clips{$key} };

      my $movie_name = $clip_string;
      my $movie_dir_name 
      = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/${clip_string}/";
      my $initial_frame_in = $current_crossfaded_frame_out - $clip_first_frame_no + 1;
      my $final_frame_in = $initial_frame_in;

      my $transformed_movie_name;
      $transformed_movie_name = "contour-${countActiveClip}";

      $transformation_scenario_file_name = $tmp_dir_name . "LYM_Arachnid_scenario_transformations-movie.csv";

      # print "Transformation scenario file ", $transformation_scenario_file_name,"\n";
      if( open(  $FILEout , ">$transformation_scenario_file_name" ) == 0 ) {
        print "Can't open file $transformation_scenario_file_name\n";
        return 0;
      }

      my @transformation_scenario_string = ("RANK,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21",
        "VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
        "TYPE,string,string,string,string,string,int,int,int,string,float,hexcolor,hexcolor,hexcolor,hexcolor,hexcolor,float,float,float,float,hexcolor,float,float",
        "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
        "scenario,1,,,,,,,,,,,,,,,,,,,,,",
        "scene,transf_scene,,,,,,,,,,,,,,,,,,,,,",
        "transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene",
        "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
        ",ZZZ_movie_dir_name_ZZZ,ZZZ_movie_name_ZZZ,ZZZ_transformed_movie_dir_ZZZ,ZZZ_transformed_movie_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,VH_RANDOM,ZZZ_transf_noise_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,1,1,1,1,ZZZ_contour_color_ZZZ,1,ZZZ_contour_width_ZZZ",
        ",ZZZ_movie_dir_name_ZZZ,ZZZ_movie_name_ZZZ,ZZZ_transformed_movie_dir_ZZZ,ZZZ_transformed_movie_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,VH_RANDOM,ZZZ_transf_noise_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,1,1,1,1,ZZZ_contour_color_ZZZ,1,ZZZ_contour_width_ZZZ",
        ",s,s,s,s,s,s,s,s,s,l,l,l,l,l,l,l,l,l,l,l,l,l",
        "/scene,,,,,,,,,,,,,,,,,,,,,,",
        "/scenario,,,,,,,,,,,,,,,,,,,,,,");
      for(my $i = 8; $i<=9; $i++) {
        $transformation_scenario_string[$i] =~ s/ZZZ_movie_dir_name_ZZZ/$movie_dir_name/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_movie_name_ZZZ/$movie_name/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_transformed_movie_dir_ZZZ/$transformed_movie_dir/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_transformed_movie_name_ZZZ/$transformed_movie_name/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_tmp_dir_name_ZZZ/$tmp_dir_name/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_transf_noise_ZZZ/$clip_noise/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_fill_color_ZZZ/$clip_fill_color/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_contour_color_ZZZ/$clip_contour_color/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_contour_width_ZZZ/$clip_contour_width/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_initial_frame_in_ZZZ/$initial_frame_in/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_final_frame_in_ZZZ/$final_frame_in/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_initial_transformed_frame_out_ZZZ/$initial_transformed_frame_out/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_final_transformed_frame_out_ZZZ/$final_transformed_frame_out/g;
      }

      foreach (@transformation_scenario_string) {
        print $FILEout "$_\r\n";
      }


      close( $FILEout );

      if(!$trace_only) {
        print  "SVG transformation\n";
        if( SVG_movie_scenario_transformation::MAIN("-i", 
            "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_transformations-movie.csv") == 0) {
          return 0;
        }
      }
      $indActiveClip++;
    }

    ##################################################################
    # CROSSFADING OF THE TRANSFORMED CURRENT FRAMES OF THE ACTIVE MOVIES
    ##################################################################
    my @crossfading_scenario_string = (
      "RANK,0,1,2,3,4,5",
      "VERBATIM,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex",
      "TYPE,string,string,string,int,int,int",
      "ID,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex",
      "scenario,1,,,,,",
      "scene,crossfade_scene,,,,,",
      "crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene",
      "ID,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex",
      ",ZZZ_crossfaded_movie_dir_ZZZ,ZZZ_crossfaded_movie_name_ZZZ,SEQUENCE,1,ZZZ_nb_composed_files_ZZZ,ZZZ_initial_crossfaded_frame_out_ZZZ",
      ",ZZZ_crossfaded_movie_dir_ZZZ,ZZZ_crossfaded_movie_name_ZZZ,SEQUENCE,1,ZZZ_nb_composed_files_ZZZ,ZZZ_final_crossfaded_frame_out_ZZZ",
      ",s,s,s,s,s,s",
      "/scene,,,,,,",
      "/scenario,,,,,,");

    my $rank = 6;

    my $crossfaded_movie_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_composed_SVGs/contour/";
    my $crossfaded_movie_name = "contour";

    $crossfading_scenario_file_name = $tmp_dir_name . "/LYM_Arachnid_scenario_layer_crossfading-movie.csv";
    # print "Crossfading scenario file ", $crossfading_scenario_file_name,"\n";
    if( open(  $FILEout , ">$crossfading_scenario_file_name" ) == 0) {
      print "Can't open file $crossfading_scenario_file_name!\n";
      return 0;
    }

    $nb_active_clips = int(keys %active_clips);
    for(my $i = 8; $i<=9; $i++) {
      $crossfading_scenario_string[$i] =~ s/ZZZ_crossfaded_movie_dir_ZZZ/$crossfaded_movie_dir/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_crossfaded_movie_name_ZZZ/$crossfaded_movie_name/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_nb_composed_files_ZZZ/$nb_active_clips/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_initial_crossfaded_frame_out_ZZZ/$current_crossfaded_frame_out/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_final_crossfaded_frame_out_ZZZ/$current_crossfaded_frame_out/g;
    }

    # addition of the new frames
    $indActiveClip = 1;
    foreach my $key (sort(keys %active_clips)) {
      my $countActiveClip = sprintf "%03d", $indActiveClip;

      my $ind_line = 0;
      for(my $i = 0; $i<3; $i++) {
        $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",".$rank;
        $rank++;
      }
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${countActiveClip},svg_input_file_name_${countActiveClip},imageInputIndex_${countActiveClip}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",string,string,int";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${countActiveClip},svg_input_file_name_${countActiveClip},imageInputIndex_${countActiveClip}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",crossfade_scene,crossfade_scene,crossfade_scene";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${countActiveClip},svg_input_file_name_${countActiveClip},imageInputIndex_${countActiveClip}";
      $ind_line++;

      my $transformed_movie_name;
      $transformed_movie_name = "contour-${countActiveClip}";
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",${transformed_movie_dir},${transformed_movie_name},${initial_transformed_frame_out}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",${transformed_movie_dir},${transformed_movie_name},${final_transformed_frame_out}";
      $ind_line++;

      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",s,s,s";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;

      $indActiveClip++;
    }

    foreach (@crossfading_scenario_string) {
      print $FILEout "$_\r\n";
    }

    close( $FILEout );

    if(!$trace_only) {
      print  "SVG cross-fading\n";
      if( SVG_movie_scenario_layer_crossfading::MAIN("-i", 
          "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_layer_crossfading-movie.csv") == 0) {
        return 0;
      }
    }
  }
  return 1;
}

MAIN;