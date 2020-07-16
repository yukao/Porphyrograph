#!/usr/bin/perl 

use strict;
use warnings;

use Math::Trig;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_scenario_interpolation_lib;
use SVG_movie_scenario_transformation;
use SVG_movie_scenario_layer_crossfading;

# SVG_movie_production_webbing-1.pl
# reads a transformation scenario and executes it

##################################################################
# CONSTANTS
##################################################################
my $NB_FRAMES_MOVIE = 120 * 30; # 2 mminutes video
# $NB_FRAMES_MOVIE = 20 * 30; # 20 sec video
# $NB_FRAMES_MOVIE = 100;
my $DELAY_BETWEEN_CLIP = 1 * 30; # 1 s between the end of a clip and the choice of another one

my $TRACE_ONLY = 0;

my $SVG_PAGE_WIDTH = 38400;
my $SVG_PAGE_HEIGHT = 21600;

my $NUMBER_OF_THREADS_IN_WEB = 36;
my $WEB_RADIUS = 30000;

my $NUMBER_OF_SPIRAL_TURNS_IN_WEB = 10;

##################################################################
# GLOBAL VARIABLES
##################################################################
my @used_web_threads = ();

##################################################################
# SUBROUTINES
##################################################################
sub random_thread_rotation_angle_choice {
  my $frame_out = shift;

  my $ret_angle = -1;
  my $nb_available_keys = 0;
  for(my $i = 0; $i < $NUMBER_OF_THREADS_IN_WEB; $i++) {
    if($used_web_threads[$i] == -1) {
      $nb_available_keys++;
    }
  }
  if(!$nb_available_keys) {
    # printf "No angle available\n";
    return $ret_angle;
  }
  my $new_thread = (int(rand($nb_available_keys)) + 1) % $nb_available_keys;
  my $ind_available_thread = 0;
  for(my $i = 0; $i < $NUMBER_OF_THREADS_IN_WEB; $i++) {
    if($used_web_threads[$i] == -1 && $ind_available_thread == $new_thread) {
      $ret_angle = ($i + 1) * (360/$NUMBER_OF_THREADS_IN_WEB)  
                  + rand((360/$NUMBER_OF_THREADS_IN_WEB)/3) - (0.5*((360/$NUMBER_OF_THREADS_IN_WEB)/3));
      $used_web_threads[$i] = $ret_angle;
      printf "Thread ID%d angle %.2f %d\n", $i, $ret_angle, $frame_out;
      last;
    }
    if($used_web_threads[$i] == -1) {
      $ind_available_thread++;
    }
  }
  # if($ret_angle == -1) {
  #   printf "No angle available\n ";
  # }
  return $ret_angle;
}
sub read_rotation_center {
  my ($loc_clip_dir_name, $loc_clip_name, $loc_initial_frame_in, $key_rank) = @_;
  
  my $full_clip_name = $loc_clip_dir_name . $loc_clip_name . "_log.csv";
  my $FILEin;
  open(  $FILEin , '<:crlf', "<$full_clip_name" ) 
      || die "Can't open file $full_clip_name!";
  # $loc_initial_frame_in starts at number 1
  my $ind_line = 1;
  my $line;
  while($line = <$FILEin>){
    chomp($line);
    if($ind_line == $loc_initial_frame_in) {
      # 38400,21600,35664,8103,41366,4851
      # screen dims   top left    bottom right
      my ($screen_w,$screen_h,$x_min,$y_max,$x_max,$y_min) 
          = split(/,/, $line);
      close($FILEin);
      if($key_rank == 0) {
        return ($x_min,$y_max);
      }
      elsif($key_rank == 1)  {
        return ($x_max,$y_max);
      }
    }
    $ind_line++;
  }
  close($FILEin);
  return(0,0);
}

##################################################################
# MAIN
##################################################################
sub MAIN {
  my $USAGE =
  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_production_webbing-1.pm

  ARGUMENTS:
  ";

  ##################################################################
  # ARGUMENTS
  ##################################################################
  if( @_ != 0 ) {
      print $USAGE; die"SVG_movie_production_webbing-1.pl: number of arguments: 0 not ", 0+@_;
      return 0;
  }

  ##################################################################
  # HASHES FOR CLIP MANAGEMENT
  ##################################################################
  my %performing_clip_sizes = (
     "contour-01" => 67,
     "contour-02" => 108,
     "contour-03" => 266,
     "contour-04" => 145,
     "contour-05" => 44,
     "contour-06" => 142,
     "contour-07" => 100,
     "contour-08" => 110,
     "contour-09" => 67,
     "contour-10" => 108,
     "contour-11" => 266,
     "contour-12" => 145,
     "contour-13" => 44,
     "contour-14" => 142,
     "contour-15" => 100,
     "contour-16" => 110,
  );

  my %clip_probability_weight_performing_dance = (
     "contour-01" => 2/28,
     "contour-02" => 2/28,
     "contour-03" => 1/28,
     "contour-04" => 1/28,
     "contour-05" => 2/28,
     "contour-06" => 2/28,
     "contour-07" => 2/28,
     "contour-08" => 2/28,
     "contour-09" => 2/28,
     "contour-10" => 2/28,
     "contour-11" => 1/28,
     "contour-12" => 1/28,
     "contour-13" => 2/28,
     "contour-14" => 2/28,
     "contour-15" => 2/28,
     "contour-16" => 2/28,
  );

  my %clip_probability_weight_performing_webbing = (
     "contour-01" => 2/64,
     "contour-02" => 2/64,
     "contour-03" => 10/64,
     "contour-04" => 10/64,
     "contour-05" => 2/64,
     "contour-06" => 2/64,
     "contour-07" => 2/64,
     "contour-08" => 2/64,
     "contour-09" => 2/64,
     "contour-10" => 2/64,
     "contour-11" => 10/64,
     "contour-12" => 10/64,
     "contour-13" => 2/64,
     "contour-14" => 2/64,
     "contour-15" => 2/64,
     "contour-16" => 2/64,
  );

  my %clip_probability_weight = (
     "contour-01" => 0,
     "contour-02" => 0,
     "contour-03" => 0,
     "contour-04" => 0,
     "contour-05" => 0,
     "contour-06" => 0,
     "contour-07" => 0,
     "contour-08" => 0,
     "contour-09" => 0,
     "contour-10" => 0,
     "contour-11" => 0,
     "contour-12" => 0,
     "contour-13" => 0,
     "contour-14" => 0,
     "contour-15" => 0,
     "contour-16" => 0,
  );

  # hash of arrays, each array contains
  # ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
  # $clip_contour_color, $clip_contour_width, $clip_noise)
  # the key is the clip name
  my %active_performing_clips;
  # ($clip_name, $holder_clip_size, $thread_clip_size, $clip_first_frame_no, $clip_fill_color, 
  #         $clip_contour_color, $clip_contour_width, $clip_noise)
  # the key is the clip name
  my %active_webbing_threads;
  my %stored_webbing_threads;

  ##################################################################
  # VARIABLES
  ##################################################################
  for(my $i = 0; $i < $NUMBER_OF_THREADS_IN_WEB; $i++) {
    $used_web_threads[$i] = -1;
  }
  my $transformation_scenario_file_name;
  my $crossfading_scenario_file_name;
  my $FILEout;
  my $line;

  my $transformed_clip_dir;
  my $tmp_dir_name;
  $tmp_dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/";
  $transformed_clip_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_transformed_SVGs/contour/";

  ##################################################################
  ##################################################################
  # FOR THE WHOLE MOVIE: FRAME BY FRAME CLIP COMPOSITION 
  # 1: PARAMETER INTERPOLATION
  # 2: MANAGEMENT OF CLIP ACTIVATION
  # 3: TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
  # 4: CROSSFADING OF THE TRANSFORMED CURRENT FRAMES OF THE ACTIVE MOVIES
  ##################################################################
  ##################################################################
  # FOR ALL THE FRAMES IN THE MOVIES
  for(my $current_crossfaded_frame_out = 0; 
      $current_crossfaded_frame_out < $NB_FRAMES_MOVIE ; 
      $current_crossfaded_frame_out++) {

    # number of transformed performing clips
    my $nb_transformed_performing_clips = 0;
    # number of webbing thread algorithm (each thread = 1 ray in web)
    my $nb_webbing_threads = 0;
    # number of spiral steps
    my $nb_archimede_spiral_steps = 0;

    my $fill_color_min = 0;
    # my $contour_color_min = 0;
    my $contour_width_min = 0;
    my $noise_min = 0;

    my $fill_color_var = 0;
    # my $contour_color_var = 0;
    my $contour_width_var = 0;
    my $noise_var = 0;

    my $webbing_clip_weight;

    ##################################################################
    # 1: PARAMETERS INTERPOLATION
    ##################################################################
    my $scene_percentage = 0;
    if($current_crossfaded_frame_out <= int($NB_FRAMES_MOVIE/3)) {
      $scene_percentage = 0;
      if(int($NB_FRAMES_MOVIE/3) > 0) {
        $scene_percentage = ($current_crossfaded_frame_out - 0)/int($NB_FRAMES_MOVIE/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

      $nb_transformed_performing_clips = 1 + int($scene_percentage * 2);
      $nb_webbing_threads =  int($scene_percentage * 2);
      $nb_archimede_spiral_steps = 0;

      $fill_color_min = 0.0;
      $fill_color_var = $scene_percentage * 0.2;

      # $contour_color_min = 0.0;
      # $contour_color_var = $scene_percentage * 0.1;

      $noise_min = 0.0;
      $noise_var = $scene_percentage * 0.02;

      $contour_width_min = 12;
      $contour_width_var = (1 - $scene_percentage) * 24;

      $webbing_clip_weight = $scene_percentage * 0.3;
    }
    elsif($current_crossfaded_frame_out <= 2 * int($NB_FRAMES_MOVIE/3)) {
      $scene_percentage = 0;
      if(int($NB_FRAMES_MOVIE/3) > 0) {
        $scene_percentage = ($current_crossfaded_frame_out - int($NB_FRAMES_MOVIE/3))/int($NB_FRAMES_MOVIE/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

      $nb_transformed_performing_clips = 3 + int($scene_percentage * 7);
      $nb_webbing_threads = 1 + int($scene_percentage * 4);
      $nb_archimede_spiral_steps = 0;

      $fill_color_min = 0.2;
      $fill_color_var = $scene_percentage * 0.5;

      # $contour_color_min = 0.1;
      # $contour_color_var = $scene_percentage * 0.2;

      $noise_min = 0.02;
      $noise_var = $scene_percentage * 0.04;

      $contour_width_min = 4;
      $contour_width_var = (1 - $scene_percentage) * 8;

      $webbing_clip_weight = 0.3 + $scene_percentage * 0.3;
    }
    else {
      $scene_percentage = 0;
      if(int($NB_FRAMES_MOVIE/3) > 0) {
        $scene_percentage = ($current_crossfaded_frame_out - (2 * int($NB_FRAMES_MOVIE/3)))/int($NB_FRAMES_MOVIE/3);
      }
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

      $nb_transformed_performing_clips = 10 + int($scene_percentage * 20);
      $nb_webbing_threads = 2;
      $nb_archimede_spiral_steps = int($scene_percentage * $NUMBER_OF_SPIRAL_TURNS_IN_WEB * $NUMBER_OF_THREADS_IN_WEB);

      $fill_color_min = 0.7;
      $fill_color_var = $scene_percentage * 0.3;

      # $contour_color_min = 0.3;
      # $contour_color_var = $scene_percentage * 0.7;

      $noise_min = 0.06;
      $noise_var = $scene_percentage * 0.06;

      $contour_width_min = 0.0;
      $contour_width_var = (1 -$scene_percentage) * 4;

      $webbing_clip_weight = 0.6 + $scene_percentage * 0.4;
    }

    ##################################################################
    # 2: MANAGEMENT OF CLIP ACTIVATION
    ##################################################################

    ##################################################################
    # RELEASING CLIPS THAT HAVE BEEN PLAYED UNTIL THE END
    my $key;
    foreach $key (sort(keys %active_performing_clips)) {
      my ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
          $clip_contour_color, $clip_contour_width, $clip_noise) = @{ $active_performing_clips{$key} };
      # clip deactivation
      if($current_crossfaded_frame_out - $clip_first_frame_no + 1 > $clip_size) {
        delete($active_performing_clips{$key});
      }
    }
    foreach $key (sort(keys %active_webbing_threads)) {
      my ($b0_x,$b0_y,$b1_x,$b1_y,$b2_x,$b2_y,$b3_x,$b3_y, 
          $B0_x,$B0_y,$B1_x,$B1_y,$B2_x,$B2_y,$B3_x,$B3_y, 
          $clip_size, $clip_first_frame_no, $clip_fill_color, 
          $clip_contour_color, $clip_contour_width, $clip_noise) = @{ $active_webbing_threads{$key} };
      # clip deactivation
      if($current_crossfaded_frame_out - $clip_first_frame_no + 1 > $clip_size) {
        # looks for an available key for the newly stored thread
        my $indKeyNewActiveClip = 1;
        my $keyNewActiveClip = sprintf("key-%02d" , $indKeyNewActiveClip);
        while(exists $stored_webbing_threads{$keyNewActiveClip}) {
          $indKeyNewActiveClip++;
          $keyNewActiveClip = sprintf("key-%02d" , $indKeyNewActiveClip);
        }
        $stored_webbing_threads{$keyNewActiveClip} 
          = [$B0_x,$B0_y,$B1_x,$B1_y,$B2_x,$B2_y,$B3_x,$B3_y, 
             $clip_fill_color, 
             $clip_contour_color, $clip_contour_width, $clip_noise];

        delete($active_webbing_threads{$key});
      }
    }

    ##################################################################
    # ACTIVATING NEW PERFORMING CLIPS UNTIL THE EXPECTED QUANTITY OF ACTIVE CLIPS IS REACHED
    for(my $indNewClipActivation = int(keys %active_performing_clips) ; 
           $indNewClipActivation < $nb_transformed_performing_clips; 
           $indNewClipActivation++) {
      my $fill_color = int(255 * ($fill_color_min + ($fill_color_var * rand(1))));
      my $mix_gray = sprintf("%02X" , ($fill_color < 255? $fill_color:255));
      my $fill_color_string = "#" . $mix_gray . $mix_gray . $mix_gray;
      my $contour_color_string = "#000000";

      my $transf_noise = $noise_min + $noise_var * rand(1);
      my $contour_width = $contour_width_min + $contour_width_var * rand(1);
      # $contour_width = 0;

      # printf "noise %.2f contour %.2f colors F %s C %s\n",  $transf_noise, $contour_width, $fill_color_string, $contour_color_string;
      
      # random choice of a clip in the list
      my ($key, $weight);
      while ( ($key, $weight) = each %clip_probability_weight ) { 
        $clip_probability_weight{$key} 
          = (1 - $webbing_clip_weight) * $clip_probability_weight_performing_dance{$key}
            + $webbing_clip_weight * $clip_probability_weight_performing_webbing{$key}; 
      }
      my $newActivatedClipID = "";
      do {                     # to avoid floating point inaccuracies
          my $rand = rand;
          while ( ($key, $weight) = each %clip_probability_weight ) {
              if (($rand -= $weight) < 0) {
                $newActivatedClipID = $key;
                last;
              }
          }
      } while($newActivatedClipID eq "");

      my $newActivatedClipSize = 0;
      if (exists $performing_clip_sizes{ $newActivatedClipID }) {
         $newActivatedClipSize = $performing_clip_sizes{ $newActivatedClipID };
      }
      else {
        die "Unknown clip ${newActivatedClipID}\n";
      }

      # looks for an available key for the newly active clip
      my $indKeyNewActiveClip = 1;
      my $keyNewActiveClip = sprintf("key-%02d" , $indKeyNewActiveClip);
      while(exists $active_performing_clips{$keyNewActiveClip}) {
        $indKeyNewActiveClip++;
        $keyNewActiveClip = sprintf("key-%02d" , $indKeyNewActiveClip);
      }
      $active_performing_clips{$keyNewActiveClip} 
        = [$newActivatedClipID, $newActivatedClipSize, $current_crossfaded_frame_out, $fill_color_string, 
          $contour_color_string, $contour_width, $transf_noise];
    }

    ##################################################################
    # ACTIVATING NEW WEBBING LINES UNTIL THE EXPECTED QUANTITY OF ACTIVE LINES IS REACHED
    # holder clips last longer than the threading clips that are dropped at some point
    for(my $indNewClipActivation = int(keys %active_webbing_threads) ; 
           $indNewClipActivation < $nb_webbing_threads; 
           $indNewClipActivation++) {
      my $fill_color_string    = "#000000";
      my $contour_color_string = "#000000";
      my $transf_noise = 0;
      my $contour_width = 0;
      
      # looks for an available key for the newly active clip
      my $indKeyNewActiveClip = 1;
      my $keyNewActiveClip = sprintf("key-%02d" , $indKeyNewActiveClip);
      while(exists $active_webbing_threads{$keyNewActiveClip}) {
        $indKeyNewActiveClip++;
        $keyNewActiveClip = sprintf("key-%02d" , $indKeyNewActiveClip);
      }

      # ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
      # $clip_contour_color, $clip_contour_width, $clip_noise)
      my $rotation_angle = random_thread_rotation_angle_choice($current_crossfaded_frame_out);
      # all the radial threads have not yet been drawn
      if($rotation_angle != -1) {
        my $rotation_angle_rad = deg2rad($rotation_angle);
        # lowercase variable: initial thread - UPPERCASE VARIABLE: FINAL THREAD
        # index 0 fix point on center
        # from first to last frame
        my ($b0_x,$b0_y) = ($SVG_PAGE_WIDTH / 2, 
                            $SVG_PAGE_HEIGHT / 2);
        my ($B0_x,$B0_y) = ($b0_x,$b0_y);
        # index 3 moving point from center to outside frame circle 
        my ($b3_x,$b3_y) = ($SVG_PAGE_WIDTH / 2 + 1 * cos($rotation_angle_rad), 
                            $SVG_PAGE_HEIGHT / 2 + 1 * cos($rotation_angle_rad));
        # at last frame
        my ($B3_x,$B3_y) = ($SVG_PAGE_WIDTH / 2 + $WEB_RADIUS * cos($rotation_angle_rad), 
                            $SVG_PAGE_HEIGHT / 2 + $WEB_RADIUS * sin($rotation_angle_rad));

        if($rotation_angle < 0) {$rotation_angle += 360;}
        if($rotation_angle > 360) {$rotation_angle -= 360;}
        # tangent gravity at moving point
        # at first frame
        my $delta_alpha1 = ($rotation_angle < 180? (2 + rand(4)) * (abs($rotation_angle - 90)/90)
                                                 : (2 + rand(4)) * (abs($rotation_angle - 270)/90));
        $delta_alpha1 = (($rotation_angle <= 90) || ($rotation_angle >= 270)? -$delta_alpha1 : $delta_alpha1);
        # at last frame
        my $delta_ALPHA1 = ($rotation_angle < 180? (1.5 + rand(2)) * (abs($rotation_angle - 90)/90)
                                                 : (1.5 + rand(2)) * (abs($rotation_angle - 270)/90));
        $delta_ALPHA1 = (($rotation_angle <= 90) || ($rotation_angle >= 270)? -$delta_ALPHA1 : $delta_ALPHA1);
        $delta_ALPHA1 = 0;
        # tangent gravity at fixed point
        # at first frame
        my $delta_alpha2 = ($rotation_angle < 180? (1 + rand(2)) * (abs($rotation_angle - 90)/90)
                                                 : (1 + rand(2)) * (abs($rotation_angle - 270)/90));
        $delta_alpha2 = (($rotation_angle <= 90) || ($rotation_angle >= 270)? -$delta_alpha2 : $delta_alpha2);
        # at last frame
        my $delta_ALPHA2 = ($rotation_angle < 180? (0.5 + rand(1)) * (abs($rotation_angle - 90)/90)
                                                 : (0.5 + rand(1)) * (abs($rotation_angle - 270)/90));
        $delta_ALPHA2 = (($rotation_angle <= 90) || ($rotation_angle >= 270)? -$delta_ALPHA2 : $delta_ALPHA2);
        $delta_ALPHA2 = 0;
        # radian conversion
        my $delta_alpha1_rad = deg2rad(10 * $delta_alpha1);
        my $delta_ALPHA1_rad = deg2rad(10 * $delta_ALPHA1);
        my $delta_alpha2_rad = deg2rad(10 * $delta_alpha2);
        my $delta_ALPHA2_rad = deg2rad(10 * $delta_ALPHA2);
        # $delta_alpha1_rad = 0;
        # $delta_ALPHA1_rad = 0;
        # $delta_alpha2_rad = 0;
        # $delta_ALPHA2_rad = 0;
        # at first frame
        my $tangent_length = 1 / 4;
        my ($b1_x,$b1_y) = ($b0_x + $tangent_length * cos($rotation_angle_rad + $delta_alpha1_rad), 
                            $b0_y + $tangent_length * sin($rotation_angle_rad + $delta_alpha1_rad));
        my ($b2_x,$b2_y) = ($b3_x - $tangent_length * cos($rotation_angle_rad - $delta_alpha2_rad), 
                            $b3_y - $tangent_length * sin($rotation_angle_rad - $delta_alpha2_rad));
        # at last frame
        $tangent_length = $WEB_RADIUS / 4;
        my ($B1_x,$B1_y) = ($B0_x + $tangent_length * cos($rotation_angle_rad + $delta_ALPHA1_rad), 
                            $B0_y + $tangent_length * sin($rotation_angle_rad + $delta_ALPHA1_rad));
        my ($B2_x,$B2_y) = ($B3_x - $tangent_length * cos($rotation_angle_rad - $delta_ALPHA2_rad), 
                            $B3_y - $tangent_length * sin($rotation_angle_rad - $delta_ALPHA2_rad));
        $active_webbing_threads{$keyNewActiveClip} 
          = [$b0_x,$b0_y,$b1_x,$b1_y,$b2_x,$b2_y,$b3_x,$b3_y, 
             $B0_x,$B0_y,$B1_x,$B1_y,$B2_x,$B2_y,$B3_x,$B3_y, 
             80+int(rand(40)), # angle and duration for reaching center
             $current_crossfaded_frame_out, $fill_color_string, 
             $contour_color_string, $contour_width, $transf_noise];
      }
    }

    ##################################################################
    # 3: TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
    ##################################################################
    my $initial_transformed_frame_out = 1;
    my $final_transformed_frame_out = 1;
    my $indActiveClip = 1;
    if(!$TRACE_ONLY) {
      print "Nb active clips ", 
      ((keys %active_performing_clips)+(keys %active_webbing_threads)+(keys %stored_webbing_threads)+0),"\n";
    }
    foreach $key (sort(keys %active_performing_clips),
                  sort(keys %active_webbing_threads),
                  sort(keys %stored_webbing_threads),
                  "archimede_spiral") {
      my $countActiveClip = sprintf "%03d", $indActiveClip;

      #######################################################
      # collecting current clip parameters
      my ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
          $clip_contour_color, $clip_contour_width, $clip_noise);
      my ($interp_spiral_center_x, $interp_spiral_center_y, $interp_spiral_radius_step,
          $interp_spiral_angle_step, $interp_spiral_nb_steps);
      $clip_name = "";

      my $transformation = "COPY";
      # lowercase variable: initial thread - UPPERCASE VARIABLE: FINAL THREAD
      my ($interp_b0_x,$interp_b0_y,$interp_b1_x,$interp_b1_y,$interp_b2_x,$interp_b2_y,$interp_b3_x,$interp_b3_y)=(0,0,0,0,0,0,0,0);
      my $clip_rotation;
      my $initial_frame_in;
      my $clip_dir_name = "";

      if( $indActiveClip <= ((keys %active_performing_clips)+0)) {
        ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
          $clip_contour_color, $clip_contour_width, $clip_noise) = @{ $active_performing_clips{$key} };

        $clip_dir_name 
          = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/${clip_name}/";
        $initial_frame_in = $current_crossfaded_frame_out - $clip_first_frame_no + 1;
        $transformation = "C_LINES";
      }
      elsif( $indActiveClip <= ((keys %active_performing_clips)
                                + (keys %active_webbing_threads)+0)) {
        $clip_dir_name 
          = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/";
        $clip_name = "Empty_file";
        my ($b0_x,$b0_y,$b1_x,$b1_y,$b2_x,$b2_y,$b3_x,$b3_y, 
            $B0_x,$B0_y,$B1_x,$B1_y,$B2_x,$B2_y,$B3_x,$B3_y);
        ($b0_x,$b0_y,$b1_x,$b1_y,$b2_x,$b2_y,$b3_x,$b3_y, 
         $B0_x,$B0_y,$B1_x,$B1_y,$B2_x,$B2_y,$B3_x,$B3_y, 
         $clip_size, $clip_first_frame_no, $clip_fill_color, 
         $clip_contour_color, $clip_contour_width, $clip_noise) 
            = @{ $active_webbing_threads{$key} };
        $initial_frame_in = $current_crossfaded_frame_out - $clip_first_frame_no + 1;
        $transformation = "BEZIER_CURVE";
        my $alpha = ($clip_size > 0 ? ($initial_frame_in / $clip_size) : 1.0);
        $interp_b0_x = $alpha * $B0_x + (1 - $alpha) * $b0_x;
        $interp_b0_y = $alpha * $B0_y + (1 - $alpha) * $b0_y;
        $interp_b1_x = $alpha * $B1_x + (1 - $alpha) * $b1_x;
        $interp_b1_y = $alpha * $B1_y + (1 - $alpha) * $b1_y;
        $interp_b2_x = $alpha * $B2_x + (1 - $alpha) * $b2_x;
        $interp_b2_y = $alpha * $B2_y + (1 - $alpha) * $b2_y;
        $interp_b3_x = $alpha * $B3_x + (1 - $alpha) * $b3_x;
        $interp_b3_y = $alpha * $B3_y + (1 - $alpha) * $b3_y;
      }
      elsif( $indActiveClip <= ((keys %active_performing_clips)
                                + (keys %active_webbing_threads)
                                + (keys %stored_webbing_threads)
                                + 0) ) {
        $clip_dir_name 
          = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/";
        $clip_name = "Empty_file";
        ($interp_b0_x,$interp_b0_y,$interp_b1_x,$interp_b1_y,$interp_b2_x,$interp_b2_y,$interp_b3_x,$interp_b3_y,
          $clip_fill_color, 
          $clip_contour_color, $clip_contour_width, $clip_noise) = @{ $stored_webbing_threads{$key} };
        $initial_frame_in = 1;
        $transformation = "BEZIER_CURVE";
      }
      elsif( $indActiveClip <= ((keys %active_performing_clips)
                                + (keys %active_webbing_threads)
                                + (keys %stored_webbing_threads)
                                + 1) ) {
        $clip_dir_name 
          = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/";
        $clip_name = "Empty_file";
        $interp_spiral_center_x = $SVG_PAGE_WIDTH/2;
        $interp_spiral_center_y = $SVG_PAGE_HEIGHT/2;
        $interp_spiral_radius_step = ($SVG_PAGE_WIDTH/2) / ($NUMBER_OF_SPIRAL_TURNS_IN_WEB);
        $interp_spiral_angle_step = 360 / $NUMBER_OF_THREADS_IN_WEB;
        $interp_spiral_nb_steps = $nb_archimede_spiral_steps;
        $initial_frame_in = 1;
        $transformation = "C_POLYGONS";
      }

      my $final_frame_in = $initial_frame_in;
      my $transformed_clip_name = "contour-${countActiveClip}";

      #######################################################
      # buiding the transformation scenario file for each added frame
      $transformation_scenario_file_name = $tmp_dir_name . "LYM_Arachnid_scenario_transformations-movie.csv";

      # print "Transformation scenario file ", $transformation_scenario_file_name,"\n";
      open(  $FILEout , ">$transformation_scenario_file_name" ) 
        || die "Can't open file $transformation_scenario_file_name!";

      my @transformation_scenario_string;
      if($transformation eq "BEZIER_CURVE") {
        @transformation_scenario_string = ("RANK,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29",
          "VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width,b0_x,b0_y,b1_x,b1_y,b2_x,b2_y,b3_x,b3_y",
          "TYPE,string,string,string,string,string,int,int,int,string,float,hexcolor,hexcolor,hexcolor,hexcolor,hexcolor,float,float,float,float,hexcolor,float,float,float,float,float,float,float,float,float,float",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width,b0_x,b0_y,b1_x,b1_y,b2_x,b2_y,b3_x,b3_y",
          "scenario,1,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
          "scene,transf_scene,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
          "transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width,b0_x,b0_y,b1_x,b1_y,b2_x,b2_y,b3_x,b3_y",
          "Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_noise_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,0,0,0,0,ZZZ_contour_color_ZZZ,1,ZZZ_contour_width_ZZZ,ZZZ_b0_x_ZZZ,ZZZ_b0_y_ZZZ,ZZZ_b1_x_ZZZ,ZZZ_b1_y_ZZZ,ZZZ_b2_x_ZZZ,ZZZ_b2_y_ZZZ,ZZZ_b3_x_ZZZ,ZZZ_b3_y_ZZZ",
          "Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_noise_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,0,0,0,0,ZZZ_contour_color_ZZZ,1,ZZZ_contour_width_ZZZ,ZZZ_b0_x_ZZZ,ZZZ_b0_y_ZZZ,ZZZ_b1_x_ZZZ,ZZZ_b1_y_ZZZ,ZZZ_b2_x_ZZZ,ZZZ_b2_y_ZZZ,ZZZ_b3_x_ZZZ,ZZZ_b3_y_ZZZ",
          "Interp,s,s,s,s,s,s,s,s,s,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l",
          "/scene,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
          "/scenario,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
      }
      elsif($transformation eq "ARCHIMEDE_SPIRAL" || $transformation eq "C_POLYGONS") {
        @transformation_scenario_string = ("RANK,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26",
          "VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width,spiral_center_x,spiral_center_y,spiral_radius_step,spiral_angle_step,spiral_nb_steps",
          "TYPE,string,string,string,string,string,int,int,int,string,float,hexcolor,hexcolor,hexcolor,hexcolor,hexcolor,float,float,float,float,hexcolor,float,float,float,float,float,float,float",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width,spiral_center_x,spiral_center_y,spiral_radius_step,spiral_angle_step,spiral_nb_steps",
          "scenario,1,,,,,,,,,,,,,,,,,,,,,,,,,,",
          "scene,transf_scene,,,,,,,,,,,,,,,,,,,,,,,,,,",
          "transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width,spiral_center_x,spiral_center_y,spiral_radius_step,spiral_angle_step,spiral_nb_steps",
          "Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_noise_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,0,0,0,0,ZZZ_contour_color_ZZZ,1,ZZZ_contour_width_ZZZ,ZZZ_spiral_center_x_ZZZ,ZZZ_spiral_center_y_ZZZ,ZZZ_spiral_radius_step_ZZZ,ZZZ_spiral_angle_step_ZZZ,ZZZ_spiral_nb_steps_ZZZ",
          "Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_noise_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,0,0,0,0,ZZZ_contour_color_ZZZ,1,ZZZ_contour_width_ZZZ,ZZZ_spiral_center_x_ZZZ,ZZZ_spiral_center_y_ZZZ,ZZZ_spiral_radius_step_ZZZ,ZZZ_spiral_angle_step_ZZZ,ZZZ_spiral_nb_steps_ZZZ",
          "Interp,s,s,s,s,s,s,s,s,s,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l",
          "/scene,,,,,,,,,,,,,,,,,,,,,,,,,,,",
          "/scenario,,,,,,,,,,,,,,,,,,,,,,,,,,,");
          $clip_noise = 0;
          $clip_fill_color = "#00000000";
          $clip_contour_color = "#000000";
          $clip_contour_width = 20;
      }
      else {
        # print "Input frame ", $transformation_scenario_file_name,"\n";
        my $countInput = sprintf "%04d", $initial_frame_in;
        my $full_frame_name = $clip_dir_name . $clip_name."_".$countInput.".svg";
        if(!(-e  $full_frame_name)) {
          die "Can't open frame $full_frame_name\nfrom clip $clip_name at frame no $initial_frame_in!";
        }

        @transformation_scenario_string = ("RANK,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21",
          "VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
          "TYPE,string,string,string,string,string,int,int,int,string,float,hexcolor,hexcolor,hexcolor,hexcolor,hexcolor,float,float,float,float,hexcolor,float,float",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
          "scenario,1,,,,,,,,,,,,,,,,,,,,,",
          "scene,transf_scene,,,,,,,,,,,,,,,,,,,,,",
          "transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
          "Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_noise_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,1,1,1,1,ZZZ_contour_color_ZZZ,1,ZZZ_contour_width_ZZZ",
          "Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_noise_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,1,1,1,1,ZZZ_contour_color_ZZZ,1,ZZZ_contour_width_ZZZ",
          "Interp,s,s,s,s,s,s,s,s,s,l,l,l,l,l,l,l,l,l,l,l,l,l",
          "/scene,,,,,,,,,,,,,,,,,,,,,,",
          "/scenario,,,,,,,,,,,,,,,,,,,,,,");
      }
      for(my $i = 8; $i<=9; $i++) {
        $transformation_scenario_string[$i] =~ s/ZZZ_clip_dir_name_ZZZ/$clip_dir_name/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_clip_name_ZZZ/$clip_name/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_transformed_clip_dir_ZZZ/$transformed_clip_dir/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_transformed_clip_name_ZZZ/$transformed_clip_name/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_tmp_dir_name_ZZZ/$tmp_dir_name/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_transf_noise_ZZZ/$clip_noise/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_fill_color_ZZZ/$clip_fill_color/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_contour_color_ZZZ/$clip_contour_color/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_contour_width_ZZZ/$clip_contour_width/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_initial_frame_in_ZZZ/$initial_frame_in/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_final_frame_in_ZZZ/$final_frame_in/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_initial_transformed_frame_out_ZZZ/$initial_transformed_frame_out/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_final_transformed_frame_out_ZZZ/$final_transformed_frame_out/g;
        $transformation_scenario_string[$i] =~ s/ZZZ_transformation_ZZZ/$transformation/g;
        if($transformation eq "BEZIER_CURVE") {
          $transformation_scenario_string[$i] =~ s/ZZZ_b0_x_ZZZ/$interp_b0_x/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_b0_y_ZZZ/$interp_b0_y/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_b1_x_ZZZ/$interp_b1_x/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_b1_y_ZZZ/$interp_b1_y/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_b2_x_ZZZ/$interp_b2_x/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_b2_y_ZZZ/$interp_b2_y/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_b3_x_ZZZ/$interp_b3_x/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_b3_y_ZZZ/$interp_b3_y/g;
        }
        elsif($transformation eq "ARCHIMEDE_SPIRAL" || $transformation eq "C_POLYGONS") {
          $transformation_scenario_string[$i] =~ s/ZZZ_spiral_center_x_ZZZ/$interp_spiral_center_x/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_spiral_center_y_ZZZ/$interp_spiral_center_y/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_spiral_radius_step_ZZZ/$interp_spiral_radius_step/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_spiral_angle_step_ZZZ/$interp_spiral_angle_step/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_spiral_nb_steps_ZZZ/$interp_spiral_nb_steps/g;
        }
      }

      #######################################################
      # applying the transformation with the current scenario file for each added frame
      foreach (@transformation_scenario_string) {
        print $FILEout "$_\r\n";
      }


      close( $FILEout );

      if(!$TRACE_ONLY) {
        # print  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/transform-movie.sh\n";
        if( SVG_movie_scenario_transformation::MAIN("-i", 
            "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_transformations-movie.csv") == 0) {
          return 0;
        }
      }
      $indActiveClip++;
    }

    ##################################################################
    # 4: CROSSFADING OF THE TRANSFORMED CURRENT FRAMES OF THE ACTIVE MOVIES
    ##################################################################

    #######################################################
    # buiding the crossfading scenario file with the list of added frames
    my @crossfading_scenario_string = (
      "RANK,0,1,2,3,4,5",
      "VERBATIM,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex",
      "TYPE,string,string,string,int,int,int",
      "ID,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex",
      "scenario,1,,,,,",
      "scene,crossfade_scene,,,,,",
      "crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene",
      "ID,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex",
      "Initial,ZZZ_crossfaded_clip_dir_ZZZ,ZZZ_crossfaded_clip_name_ZZZ,PARALLEL,1,ZZZ_nb_composed_files_ZZZ,ZZZ_initial_crossfaded_frame_out_ZZZ",
      "Final,ZZZ_crossfaded_clip_dir_ZZZ,ZZZ_crossfaded_clip_name_ZZZ,PARALLEL,1,ZZZ_nb_composed_files_ZZZ,ZZZ_final_crossfaded_frame_out_ZZZ",
      "Interp,s,s,s,s,s,s",
      "/scene,,,,,,",
      "/scenario,,,,,,");

    my $rank = 6;

    my $crossfaded_clip_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_composed_SVGs/contour/";
    my $crossfaded_clip_name = "contour";

    $crossfading_scenario_file_name = $tmp_dir_name . "/LYM_Arachnid_scenario_layer_crossfading-movie.csv";
    # print "Crossfading scenario file ", $crossfading_scenario_file_name,"\n";
    open(  $FILEout , ">$crossfading_scenario_file_name" ) 
      || die "Can't open file $crossfading_scenario_file_name!";

    # lines 9 and 10 for the values of the output file name and directory, number of composed files and frame out number
    my $nb_active_clips 
      = ((keys %active_performing_clips)+(keys %active_webbing_threads)+(keys %stored_webbing_threads)+1);
    for(my $i = 8; $i<=9; $i++) {
      $crossfading_scenario_string[$i] =~ s/ZZZ_crossfaded_clip_dir_ZZZ/$crossfaded_clip_dir/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_crossfaded_clip_name_ZZZ/$crossfaded_clip_name/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_nb_composed_files_ZZZ/$nb_active_clips/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_initial_crossfaded_frame_out_ZZZ/$current_crossfaded_frame_out/g;
      $crossfading_scenario_string[$i] =~ s/ZZZ_final_crossfaded_frame_out_ZZZ/$current_crossfaded_frame_out/g;
    }

    #######################################################
    # addition of the columns of all the frames
    $indActiveClip = 1;
    foreach $key (sort(keys %active_performing_clips),
                  sort(keys %active_webbing_threads),
                  sort(keys %stored_webbing_threads),
                  "archimede_spiral") {
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

      my $transformed_clip_name;
      $transformed_clip_name = "contour-${countActiveClip}";
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",${transformed_clip_dir},${transformed_clip_name},${initial_transformed_frame_out}";
      $ind_line++;
      $crossfading_scenario_string[$ind_line] = $crossfading_scenario_string[$ind_line].",${transformed_clip_dir},${transformed_clip_name},${final_transformed_frame_out}";
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

    #######################################################
    # applying the transformation with the current scenario file for each added frame
    if(!$TRACE_ONLY) {
      my $count = sprintf("%04d",$current_crossfaded_frame_out);
      print "Crossfaded SVG file ", $crossfaded_clip_dir.$crossfaded_clip_name."_".$count.".svg","\n\n";
      # print  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/crossfade-movie.sh\n";
      if( SVG_movie_scenario_layer_crossfading::MAIN("-i", 
          "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_layer_crossfading-movie.csv") == 0) {
        return 0;
      }
    }
  }
  return 1;
}

MAIN;