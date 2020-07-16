#!/usr/bin/perl 

use strict;
use warnings;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_scenario_interpolation_lib;
use SVG_movie_scenario_transformation;
use SVG_movie_scenario_layer_crossfading;

# SVG_movie_production_loop.pm
# reads a transformation scenario and executes it

##################################################################
# MAIN
##################################################################
sub MAIN {
  my $USAGE =
  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_production_loop.pm

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

  my $scenario_dir_name;
  my $transformation_scenario_file_name;
  my $crossfading_scenario_file_name;
  my $FILEout;
  my $line;

  my $trace_only = 0;

  ##################################################################
  # HASHES FOR CLIP MANAGEMENT
  ##################################################################
  my %transformation_function = (
     "contour_1" => 89,
     "contour_2" => 102,
     "contour_3" => 119,
     "contour_4" => 89,
     "contour_5" => 89,
     "contour_6" => 89,
     "contour_7" => 89,
     "contour_8" => 89,
  );

  ##################################################################
  # READING AND EXECUTING A SCENE: 
  # TRANSFORMING CLIPS AND ADDING THEM TO THE CURRENT MOVIE
  ##################################################################
  my $transformed_movie_dir;
  my $tmp_dir_name;
  $tmp_dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/";
  $transformed_movie_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_transformed_SVGs/contour/";

  my $NB_FRAMES_VIDEO_CLIP = 180; #TMP 3
  my $NB_OCCURRENCES_VIDEO_CLIP = 100; #TMP 3
  $NB_OCCURRENCES_VIDEO_CLIP = 1;

  my $initial_transformed_frame_out = 1;
  my $final_transformed_frame_out = $NB_FRAMES_VIDEO_CLIP;

  my $initial_crossfaded_frame_out = 1;
  my $final_crossfaded_frame_out =  $NB_FRAMES_VIDEO_CLIP;

  #TMP for(my $ind_scene = 0; $ind_scene < 1; $ind_scene++) {
  for(my $ind_scene = 0; $ind_scene < $NB_OCCURRENCES_VIDEO_CLIP; $ind_scene++) {

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
    if($ind_scene <= int($NB_OCCURRENCES_VIDEO_CLIP/3)) {
      $nb_transformed_inputs = 1;
      if(int($NB_OCCURRENCES_VIDEO_CLIP/3) > 0) {
        $scene_percentage = ($ind_scene - 0)/int($NB_OCCURRENCES_VIDEO_CLIP/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

      $fill_color_min = 0.0;
      $contour_color_min = 0.0;
      $noise_min = 0.0;
      $contour_width_min = 0.0;

      $fill_color_var = $scene_percentage * 0.1;
      $contour_color_var = $scene_percentage * 0.1;
      $noise_var = $scene_percentage * 0.1;
      $contour_width_var = $scene_percentage * 1;
    }
    if($ind_scene <= 2 * int($NB_OCCURRENCES_VIDEO_CLIP/3)) {
      $nb_transformed_inputs = 2;
      if(int($NB_OCCURRENCES_VIDEO_CLIP/3) > 0) {
        $scene_percentage = ($ind_scene - int($NB_OCCURRENCES_VIDEO_CLIP/3))/int($NB_OCCURRENCES_VIDEO_CLIP/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

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
      $nb_transformed_inputs = 3;
      if(int($NB_OCCURRENCES_VIDEO_CLIP/3) > 0) {
        $scene_percentage = ($ind_scene - (2 * int($NB_OCCURRENCES_VIDEO_CLIP/3)))/int($NB_OCCURRENCES_VIDEO_CLIP/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

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
    # TRANSFORMING CLIPS
    ##################################################################
    for(my $indImageInput = 0 ; $indImageInput < $nb_transformed_inputs * 2; $indImageInput++) {
      my $countImageInput = sprintf "%03d", $indImageInput;

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
      
      my $movie_name;
      my $movie_dir_name;
      if (0 == $indImageInput % 2) {
        $movie_dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/contour-DG/";
        $movie_name = "contour-DG";
      } else {
        $movie_dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/contour-GD/";
        $movie_name = "contour-GD";
      }
      my $initial_frame_in = 120 + int(rand(60));
      my $final_frame_in = $initial_frame_in + $NB_FRAMES_VIDEO_CLIP - 1;

      my $transformed_movie_name;
      $transformed_movie_name = "contour_${countImageInput}";

      $transformation_scenario_file_name = $tmp_dir_name . "/LYM_Arachnid_scenario_transformations-movie.csv";

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
        $transformation_scenario_string[$i] =~ s/ZZZ_transf_noise_ZZZ/$transf_noise/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_fill_color_ZZZ/$fill_color_string/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_contour_color_ZZZ/$contour_color_string/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_contour_width_ZZZ/$contour_width/g;
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
    }

    ##################################################################
    # ADDING THEM TO THE CURRENT MOVIE
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
    my $ind_layer = 1;
    my $layer_count = sprintf "%03d", $ind_layer;

    my $crossfaded_movie_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_composed_SVGs/contour/";
    my $crossfaded_movie_name = "contour";
    my $nb_composed_files = 0;

    $crossfading_scenario_file_name = $tmp_dir_name . "/LYM_Arachnid_scenario_layer_crossfading-movie.csv";
    # print "Crossfading scenario file ", $crossfading_scenario_file_name,"\n";
    if( open(  $FILEout , ">$crossfading_scenario_file_name" ) == 0) {
      print "Can't open file $crossfading_scenario_file_name!\n";
      return 0;
    }

    if($ind_scene > 0) {
      $nb_composed_files = $nb_transformed_inputs * 2 + 1;
    }
    else {
      $nb_composed_files = $nb_transformed_inputs * 2;
    }
    for(my $i = 8; $i<=9; $i++) {
      $crossfading_scenario_string[$i] =~ s/ZZZ_crossfaded_movie_dir_ZZZ/$crossfaded_movie_dir/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_crossfaded_movie_name_ZZZ/$crossfaded_movie_name/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_nb_composed_files_ZZZ/$nb_composed_files/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_initial_crossfaded_frame_out_ZZZ/$initial_crossfaded_frame_out/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_final_crossfaded_frame_out_ZZZ/$final_crossfaded_frame_out/g;
    }

    # previous frames of the cumulated movie_SVGs
    if($ind_scene > 0) {
      my $ind_line = 0;
      for(my $i = 0; $i<3; $i++) {
        $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",".$rank;
        $rank++;
      }
      $ind_line++;
      $layer_count = sprintf "%03d", $ind_layer;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${layer_count},svg_input_file_name_${layer_count},imageInputIndex_${layer_count}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",string,string,int";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${layer_count},svg_input_file_name_${layer_count},imageInputIndex_${layer_count}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",crossfade_scene,crossfade_scene,crossfade_scene";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${layer_count},svg_input_file_name_${layer_count},imageInputIndex_${layer_count}";
      $ind_line++;

      my $previous_scene_initial_crossfaded_frame_out = $initial_crossfaded_frame_out - $NB_FRAMES_VIDEO_CLIP;
      my $previous_scene_final_crossfaded_frame_out = $final_crossfaded_frame_out - $NB_FRAMES_VIDEO_CLIP;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",${crossfaded_movie_dir},${crossfaded_movie_name},${previous_scene_initial_crossfaded_frame_out}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",${crossfaded_movie_dir},${crossfaded_movie_name},${previous_scene_final_crossfaded_frame_out}";
      $ind_line++;

      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",s,s,s";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;

      $ind_layer++;
    }
    
    # addition of the new frames
    for(my $indImageInput = 0 ; $indImageInput < $nb_transformed_inputs * 2; $indImageInput++) {
      my $countImageInput = sprintf "%03d", $indImageInput;

      my $ind_line = 0;
      for(my $i = 0; $i<3; $i++) {
        $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",".$rank;
        $rank++;
      }
      $ind_line++;
      $layer_count = sprintf "%03d", $ind_layer;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${layer_count},svg_input_file_name_${layer_count},imageInputIndex_${layer_count}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",string,string,int";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${layer_count},svg_input_file_name_${layer_count},imageInputIndex_${layer_count}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",,,";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",crossfade_scene,crossfade_scene,crossfade_scene";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",svg_input_directory_${layer_count},svg_input_file_name_${layer_count},imageInputIndex_${layer_count}";
      $ind_line++;

      my $transformed_movie_name;
      $transformed_movie_name = "contour_${countImageInput}";
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

      $ind_layer++;
    }

    $initial_crossfaded_frame_out += $NB_FRAMES_VIDEO_CLIP;
    $final_crossfaded_frame_out += $NB_FRAMES_VIDEO_CLIP;

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