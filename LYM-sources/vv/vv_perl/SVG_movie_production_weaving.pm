#!/usr/bin/perl 

use strict;
use warnings;

use Math::Trig;

package SVG_movie_production_weaving;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_scenario_interpolation_lib;
use SVG_movie_scenario_transformation;
use SVG_movie_scenario_layer_crossfading;

# SVG_movie_production_weaving-1.pl
# reads a transformation scenario and executes it

##################################################################
# CONSTANTS
##################################################################
my $NB_FRAMES_MOVIE = 120 * 30; # 2 mminutes video
# $NB_FRAMES_MOVIE = 20 * 30; # 20 sec video
$NB_FRAMES_MOVIE = 10;
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
for(my $i = 0; $i < $NUMBER_OF_THREADS_IN_WEB; $i++) {
  $used_web_threads[$i] = -1;
}

##################################################################
# HASHES FOR CLIP management
##################################################################
my %performing_clip_sizes = (
   "silhouette-01" => 66,
   "silhouette-02" => 108,
   "silhouette-03" => 265,
   "silhouette-04" => 144,
   "silhouette-05" => 43,
   "silhouette-06" => 141,
   "silhouette-07" => 99,
   "silhouette-08" => 110,
   "silhouette-09" => 67,
   "silhouette-10" => 108,
   "silhouette-11" => 266,
   "silhouette-12" => 145,
   "silhouette-13" => 44,
   "silhouette-14" => 142,
   "silhouette-15" => 100,
   "silhouette-16" => 110,
);

my %clip_probability_weight_performing_dance = (
   "silhouette-01" => 2/28,
   "silhouette-02" => 2/28,
   "silhouette-03" => 1/28,
   "silhouette-04" => 1/28,
   "silhouette-05" => 2/28,
   "silhouette-06" => 2/28,
   "silhouette-07" => 2/28,
   "silhouette-08" => 2/28,
   "silhouette-09" => 2/28,
   "silhouette-10" => 2/28,
   "silhouette-11" => 1/28,
   "silhouette-12" => 1/28,
   "silhouette-13" => 2/28,
   "silhouette-14" => 2/28,
   "silhouette-15" => 2/28,
   "silhouette-16" => 2/28,
);

my %clip_probability_weight_performing_weaving = (
   "silhouette-01" => 2/64,
   "silhouette-02" => 2/64,
   "silhouette-03" => 10/64,
   "silhouette-04" => 10/64,
   "silhouette-05" => 2/64,
   "silhouette-06" => 2/64,
   "silhouette-07" => 2/64,
   "silhouette-08" => 2/64,
   "silhouette-09" => 2/64,
   "silhouette-10" => 2/64,
   "silhouette-11" => 10/64,
   "silhouette-12" => 10/64,
   "silhouette-13" => 2/64,
   "silhouette-14" => 2/64,
   "silhouette-15" => 2/64,
   "silhouette-16" => 2/64,
);

my %clip_probability_weight = (
   "silhouette-01" => 0,
   "silhouette-02" => 0,
   "silhouette-03" => 0,
   "silhouette-04" => 0,
   "silhouette-05" => 0,
   "silhouette-06" => 0,
   "silhouette-07" => 0,
   "silhouette-08" => 0,
   "silhouette-09" => 0,
   "silhouette-10" => 0,
   "silhouette-11" => 0,
   "silhouette-12" => 0,
   "silhouette-13" => 0,
   "silhouette-14" => 0,
   "silhouette-15" => 0,
   "silhouette-16" => 0,
);

# hash of arrays, each array contains
# ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
# $clip_silhouette_color, $clip_stroke_width, $clip_scaling_factor,
# $clip_scaling_center_x,$clip_scaling_center_y)
# the key is the clip name
my %active_performing_clips;

my $transformation_scenario_file_name;
my $crossfading_scenario_file_name;
my $FILEout;
my $line;

my $transformed_clip_dir;
my $tmp_dir_name;
$tmp_dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/";
$transformed_clip_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_transformed_SVGs/silhouette/";

# SCENARIO VARIABLES
my @var_names;
my %var_types;
my %val_init;
my %val_fin;
my %val_interp;
my %interp;


##################################################################
# MAIN
##################################################################
sub MAIN {
  my $USAGE =
  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_production_weaving-2.pm -i scenario_file

ARGUMENTS:
   -i: input scenariofile
";

  ##################################################################
  # ARGUMENTS
  ##################################################################
  if( @_ != 2 ) {
      print $USAGE; die"SVG_movie_production_weaving-2.pm: number of arguments: 2 not ", 0+@_;
      return 0;
  }

  # scenario_file_name
  my $option = $_[0];
  if( $option ne "-i" ) {
      print $USAGE; 
      print"SVG_movie_production_weaving-2.pm: -i expected for argt 1\n";
      return 0;
  }

  # print "Scenario file ", $scenario_file_name,"\n";
  my $scenario_file_name = $_[1];
  my $FILEin;
  if( !open(  $FILEin , '<:crlf', "<$scenario_file_name" )  ) {
    if( !open(  $FILEin , "<$scenario_file_name" )  ) {
      print "Can't open file $scenario_file_name!\n";
      return 0;
    }
  }

  $line = <$FILEin>; # print "line1 ", $line; 
  $line=SVG_scenario_interpolation_lib::win_chomp($line);
  $line = <$FILEin>; # print "line2 ", $line; 
  $line=SVG_scenario_interpolation_lib::win_chomp($line);

  #3rd line: variable types
  my $line_types = <$FILEin>; $line_types=SVG_scenario_interpolation_lib::win_chomp($line_types);

  #4th line: variable names
  $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
  @var_names = split(/,/,$line);
  #removes the first one
  shift(@var_names);

  #finishes the types now that the variable strings are known
  my @values_line = split(/,/,$line_types);
  #removes the first one
  shift(@values_line);
  for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
    $var_types{$var_names[$indVar]} = $values_line[$indVar];
  }

  # scenario line
  $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
  @values_line = split(/,/,$line);
  if($values_line[0] ne "scenario") {
    print "Expected tag \"scenario\" not found!\n";
    return 0;
  }
  my $nb_scenes = $values_line[1];
  print "nb scenes ",$nb_scenes,"\n";


  ##################################################################
  # READING AND EXECUTING A SCENE
  ##################################################################
  for(my $ind_scene = 0; $ind_scene < $nb_scenes; $ind_scene++) {
    # scene line
    $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    if($values_line[0] ne "scene") {
      print "Expected tag \"scene\" not found!\n";
      return 0;
    }
    my $scene_ID = $values_line[1];

    # scene ID line
    $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
    # var IDs line
    $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);

    # variable initial value
    $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
      $val_init{$var_names[$indVar]} = $values_line[$indVar];
    }

    # variable final value
    $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
      $val_fin{$var_names[$indVar]} = $values_line[$indVar];
    }

    # variable interpolation value
    $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
      $interp{$var_names[$indVar]} = $values_line[$indVar];
    }

    my $pas = 1;
    if($val_init{"imageInputIndex"} <= $val_fin{"imageInputIndex"}) {
      $pas = 1;
    }
    elsif($val_init{"imageInputIndex"} > $val_fin{"imageInputIndex"}) {
      $pas = -1;
    }
    else {
      $pas = 0;
    }

    # applies possible transformations on the SVG files
    # creates the directories in which the paths files are saved
    if ( !-d $val_init{"svg_output_directory"}   ) {
        if(!mkdir $val_init{"svg_output_directory"}) {
          print  "Failed to create path: ".$val_init{"svg_output_directory"}."\n";
          return 0;
        }
    }

    ##################################################################
    # SCANS ALL THE FRAMES IN THE MOVIE
    ##################################################################
    my $current_crossfaded_frame_out = $val_init{"imageOutputIndex"};
    my $nb_layers = $val_init{"nb_posterization_layers"};
    my $tmp_dir = $val_init{"tmp_directory"};
    for(my $indImageInput = $val_init{"imageInputIndex"} ; 
         $indImageInput != $val_fin{"imageInputIndex"} + $pas; 
         $indImageInput += $pas) {
      my $countInput = sprintf "%04d", $indImageInput;
      my $countOutput = sprintf "%04d", $current_crossfaded_frame_out;

      my $scene_percentage = 0;
      if($pas != 0 && ($val_fin{"imageInputIndex"} - $val_init{"imageInputIndex"}) != 0 ) {
        $scene_percentage = ($indImageInput - $val_init{"imageInputIndex"})/($val_fin{"imageInputIndex"} - $val_init{"imageInputIndex"});
      }
      #clamping
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);
      # interpolating all the variables
      for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
        # print "var name [",$var_names[$indVar],"]\n";
        $val_interp{$var_names[$indVar]} = SVG_scenario_interpolation_lib::interpolated_value_calculation($var_types{$var_names[$indVar]}, $val_init{$var_names[$indVar]}, $val_fin{$var_names[$indVar]}, $interp{$var_names[$indVar]}, $scene_percentage);
      }

      ##################################################################
      ##################################################################
      # FOR THE WHOLE MOVIE: FRAME BY FRAME CLIP COMPOSITION 
      # 1: PARAMETER INTERPOLATION
      # 2: MANAGEMENT OF CLIP ACTIVATION
      # 3: TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
      # 4: CROSSFADING OF THE TRANSFORMED CURRENT FRAMES OF THE ACTIVE MOVIES
      # 5: TRANSFORMATIONS OF THE CROSSFADED FRAMES
      ##################################################################
      ##################################################################
      # FOR ALL THE FRAMES IN THE MOVIES

      # number of transformed performing clips
      my $nb_transformed_performing_clips = 0;

      my $fill_color_min = 0;
      my $stroke_color_min = 0;
      my $stroke_width_min = 0;
      my $scale_min = 0;

      my $fill_color_max = 0;
      my $stroke_color_max = 0;
      my $stroke_width_max = 0;
      my $scale_max = 0;

      my $weaving_clip_weight;

      ##################################################################
      # 1: PARAMETERS INTERPOLATION
      ##################################################################
      # FILL COLOR management
      $fill_color_min = $val_interp{"fill_color_min"};
      $fill_color_max = $val_interp{"fill_color_max"};
      # STROKE COLOR management
      $stroke_color_min = $val_interp{"stroke_color_min"};
      $stroke_color_max = $val_interp{"stroke_color_max"};
      # SCALING management
      $scale_min = $val_interp{"scale_min"};
      $scale_max = $val_interp{"scale_max"};

      # STROKE WIDTH management
      $stroke_width_min = $val_interp{"stroke_width_min"};
      $stroke_width_max = $val_interp{"stroke_width_max"};

      # CLIP WEIGHT management
      $weaving_clip_weight = $val_interp{"weaving_clip_weight"};

      # NUMBER OF ACTIVE CLIPS management
      $nb_transformed_performing_clips = $val_interp{"nb_active_clips"};

      ##################################################################
      # 2: management OF CLIP ACTIVATION
      ##################################################################

      ##################################################################
      # RELEASING CLIPS THAT HAVE BEEN PLAYED UNTIL THE END
      my $key;
      foreach $key (sort(keys %active_performing_clips)) {
        my ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
            $clip_silhouette_color, $clip_stroke_width, 
            $clip_scaling_factor, $clip_scaling_center_x, $clip_scaling_center_y) 
        = @{ $active_performing_clips{$key} };
        # clip deactivation
        if($current_crossfaded_frame_out - $clip_first_frame_no + 1 > $clip_size) {
          delete($active_performing_clips{$key});
        }
      }

      ##################################################################
      # ACTIVATING NEW PERFORMING CLIPS UNTIL THE EXPECTED QUANTITY OF ACTIVE CLIPS IS REACHED
      # new clip activation
      # print "current expected number of clips " , $nb_transformed_performing_clips , "\n";
      for(my $indNewClipActivation = int(keys %active_performing_clips) ; 
             $indNewClipActivation < $nb_transformed_performing_clips; 
             $indNewClipActivation++) {
        # strke and fill parameters selection
        my $fill_color = '#' . SVG_scenario_interpolation_lib::mix_color($fill_color_min, $fill_color_max, rand(1));
        my $stroke_color = '#' . SVG_scenario_interpolation_lib::mix_color($stroke_color_min, $stroke_color_min, rand(1));
        my $randVal = rand(1);
        my $stroke_width = (1 - $randVal) * $stroke_width_min + $randVal * $stroke_width_max;
        $randVal = rand(1);
        my $scaling_factor = (1 - $randVal) * $scale_min + $randVal * $scale_max;

        printf "percent %.2f silhouette %.2f colors F %s C %s\n",  $scaling_factor, $stroke_width, $fill_color, $stroke_color;
        
        # random choice of a clip in the list
        my ($key, $weight);
        while ( ($key, $weight) = each %clip_probability_weight ) { 
          $clip_probability_weight{$key} 
            = (1 - $weaving_clip_weight) * $clip_probability_weight_performing_dance{$key}
              + $weaving_clip_weight * $clip_probability_weight_performing_weaving{$key}; 
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
          = [$newActivatedClipID, $newActivatedClipSize, $current_crossfaded_frame_out, 
            $fill_color, $stroke_color, $stroke_width, 
            $scaling_factor, $SVG_PAGE_WIDTH / 2, 0];
      }      # new clip activation

      ##################################################################
      # 3: TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
      ##################################################################
      my $initial_transformed_frame_out = 1;
      my $final_transformed_frame_out = 1;
      my $indActiveClip = 1;
      if(!$TRACE_ONLY) {
        print "Nb active clips ", ((keys %active_performing_clips)+0),"\n";
      }
      # loop on the active clips to apply a transformation
      foreach $key (sort(keys %active_performing_clips)) {
        my $countActiveClip = sprintf "%03d", $indActiveClip;

        #######################################################
        # collecting current clip parameters
        my ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
            $clip_silhouette_color, $clip_stroke_width, $clip_scaling_factor,
            $clip_scaling_center_x, $clip_scaling_center_y);
        $clip_name = "";

        my $transformation = "COPY";
        # INITIAL TRANSFORMATION management
        if(exists $val_init{"transfIni_ID"})  {
            $transformation = $val_init{"transfIni_ID"};
        }
        else {
          print("SVG_production_movie scenarion file $scenario_file_name missing val_init variable\n");
          return 0;
        }

        #######################################################
        # variables for the transformation scenario file
        my $transformation_function_string = $val_init{"transf1_ID"};
        my $initial_frame_in;
        my $clip_dir_name = $val_init{"svg_input_directory"};

        if( $indActiveClip <= ((keys %active_performing_clips)+0)) {
          ($clip_name, $clip_size, $clip_first_frame_no, $clip_fill_color, 
            $clip_silhouette_color, $clip_stroke_width, 
            $clip_scaling_factor, $clip_scaling_center_x, $clip_scaling_center_y) 
          = @{ $active_performing_clips{$key} };

          $clip_dir_name = $clip_dir_name  . "${clip_name}/";
          $initial_frame_in = $current_crossfaded_frame_out - $clip_first_frame_no + 1;
        }

        my $final_frame_in = $initial_frame_in;
        my $transformed_clip_name = $val_init{"svg_input_file_name"}."-${countActiveClip}";

        #######################################################
        # buiding the transformation scenario file for each added frame
        $transformation_scenario_file_name = $tmp_dir_name . "LYM_Arachnid_scenario_transformations-single_clip.csv";

        # print "Transformation scenario file ", $transformation_scenario_file_name,"\n";
        open(  $FILEout , ">$transformation_scenario_file_name" ) 
          || die "Can't open file $transformation_scenario_file_name!";

        my @transformation_scenario_string;
        # print "Input frame ", $transformation_scenario_file_name,"\n";
        my $countInput = sprintf "%04d", $initial_frame_in;
        my $full_frame_name = $clip_dir_name . $clip_name."_".$countInput.".svg";
        if(!(-e  $full_frame_name)) {
          die "Can't open frame $full_frame_name\nfrom clip $clip_name at frame no $initial_frame_in!";
        }

        @transformation_scenario_string = ("RANK,0,1,2,3,4,5,6,7,8,9,,,,10,11,12,13,14,15,16,17,18,19,20,21",
          "VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,x_scaling_center,y_scaling_center,scaling_factor,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
          "TYPE,string,string,string,string,string,int,int,int,string,float,float,float,float,hexcolor,hexcolor,hexcolor,hexcolor,hexcolor,float,float,float,float,hexcolor,float,float",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,x_scaling_center,y_scaling_center,scaling_factor,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
          "scenario,1,,,,,,,,,,,,,,,,,,,,,,,,",
          "scene,transf_scene,,,,,,,,,,,,,,,,,,,,,,,,",
          "transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,x_scaling_center,y_scaling_center,scaling_factor,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",
          "Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_percent_ZZZ,ZZZ_transf_x_scaling_center_ZZZ,ZZZ_transf_y_scaling_center_ZZZ,ZZZ_transf_scaling_factor_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,1,1,1,1,ZZZ_silhouette_color_ZZZ,1,ZZZ_stroke_width_ZZZ",
          "Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_percent_ZZZ,ZZZ_transf_x_scaling_center_ZZZ,ZZZ_transf_y_scaling_center_ZZZ,ZZZ_transf_scaling_factor_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,1,1,1,1,ZZZ_silhouette_color_ZZZ,1,ZZZ_stroke_width_ZZZ",
          "Interp,s,s,s,s,s,s,s,s,s,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l",
          "/scene,,,,,,,,,,,,,,,,,,,,,,,,,",
          "/scenario,,,,,,,,,,,,,,,,,,,,,,,,,");

        # replaces the names of the variables in the string containing the scenario table (lines 8 and 9 that have the variable values)
        for(my $i = 8; $i<=9; $i++) {
          $transformation_scenario_string[$i] =~ s/ZZZ_clip_dir_name_ZZZ/$clip_dir_name/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_clip_name_ZZZ/$clip_name/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_transformed_clip_dir_ZZZ/$transformed_clip_dir/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_transformed_clip_name_ZZZ/$transformed_clip_name/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_tmp_dir_name_ZZZ/$tmp_dir_name/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_transf_percent_ZZZ/1.0/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_fill_color_ZZZ/$clip_fill_color/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_silhouette_color_ZZZ/$clip_silhouette_color/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_stroke_width_ZZZ/$clip_stroke_width/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_initial_frame_in_ZZZ/$initial_frame_in/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_final_frame_in_ZZZ/$final_frame_in/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_initial_transformed_frame_out_ZZZ/$initial_transformed_frame_out/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_final_transformed_frame_out_ZZZ/$final_transformed_frame_out/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_transformation_ZZZ/$transformation/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_transf_x_scaling_center_ZZZ/$clip_scaling_center_x/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_transf_y_scaling_center_ZZZ/$clip_scaling_center_y/g;
          $transformation_scenario_string[$i] =~ s/ZZZ_transf_scaling_factor_ZZZ/$clip_scaling_factor/g;
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
              "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_transformations-single_clip.csv") == 0) {
            return 0;
          }
        }
        $indActiveClip++;
      }  # loop on the active clips to apply a transformation

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

      my $crossfaded_clip_dir = $tmp_dir_name;
      my $crossfaded_clip_name = "crossfaded-" . $val_init{"svg_input_file_name"};

      $crossfading_scenario_file_name = $tmp_dir_name . "/LYM_Arachnid_scenario_layer_crossfading-movie.csv";
      # print "Crossfading scenario file ", $crossfading_scenario_file_name,"\n";
      open(  $FILEout , ">$crossfading_scenario_file_name" ) 
        || die "Can't open file $crossfading_scenario_file_name!";

      # lines 9 and 10 for the values of the output file name and directory, number of composed files and frame out number
      my $nb_active_clips = ((keys %active_performing_clips)+0);
      for(my $i = 8; $i<=9; $i++) {
        $crossfading_scenario_string[$i] =~ s/ZZZ_crossfaded_clip_dir_ZZZ/$crossfaded_clip_dir/g;
        $crossfading_scenario_string[$i] =~ s/ZZZ_crossfaded_clip_name_ZZZ/$crossfaded_clip_name/g;
        $crossfading_scenario_string[$i] =~ s/ZZZ_nb_composed_files_ZZZ/$nb_active_clips/g;
        $crossfading_scenario_string[$i] =~ s/ZZZ_initial_crossfaded_frame_out_ZZZ/1/g;
        $crossfading_scenario_string[$i] =~ s/ZZZ_final_crossfaded_frame_out_ZZZ/1/g;
      }

      #######################################################
      # addition of the columns of all the frames
      $indActiveClip = 1;
      # loop on the active clips to add them to the scenario file
      foreach $key (sort(keys %active_performing_clips)) {
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
        $transformed_clip_name = $val_init{"svg_input_file_name"}."-${countActiveClip}";
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
      } # loop on the active clips to add them to the scenario file

      foreach (@crossfading_scenario_string) {
        print $FILEout "$_\r\n";
      }

      close( $FILEout );

      #######################################################
      # applying the transformation with the current scenario file for each added frame
      if(!$TRACE_ONLY) {
        # print  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/crossfade-movie.sh\n";
        if( SVG_movie_scenario_layer_crossfading::MAIN("-i", 
            "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_layer_crossfading-movie.csv") == 0) {
          return 0;
        }
      }

      ##################################################################
      # 5: FINAL TRANSFORMATIONS OF THE CROSSFADED FRAMES
      ##################################################################

      my $end_transformation = $val_init{"transfFin_ID"};

      # lowercase variable: initial thread - UPPERCASE VARIABLE: FINAL THREAD
      my $clip_dir_name = $tmp_dir_name;
      my $clip_name = "crossfaded-".$val_init{"svg_input_file_name"};

      my $end_transformed_clip_dir = $val_init{"svg_output_directory"};
      my $countOutpput = sprintf "%04d", $current_crossfaded_frame_out;
      my $end_transformed_clip_name = $val_init{"svg_output_file_name"};

      my $initial_frame_in = 1;
      my $final_frame_in = 1;
      my $initial_end_transform_frame_out = $current_crossfaded_frame_out;
      my $final_end_transform_frame_out = $current_crossfaded_frame_out;

      #######################################################
      # buiding the end_transformation scenario file for the cross-faded frame
      my $end_transformation_scenario_file_name = $tmp_dir_name . "LYM_Arachnid_scenario_end_transformations-movie.csv";

      # print "Transformation scenario file ", $end_transformation_scenario_file_name,"\n";
      open(  $FILEout , ">$end_transformation_scenario_file_name" ) 
        || die "Can't open file $end_transformation_scenario_file_name!";

      my @end_transformation_scenario_string;
      @end_transformation_scenario_string = (
          "RANK,0,1,2,3,4,5,6,7,8,9",
          "VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1",
          "TYPE,string,string,string,string,string,int,int,int,string,float",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1",
          "scenario,1,,,,,,,,,",
          "scene,transf_scene,,,,,,,,,",
          "transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene",
          "ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1",
          "Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_end_transformation_ZZZ,ZZZ_transf_percent_ZZZ",
          "Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_end_transformation_ZZZ,ZZZ_transf_percent_ZZZ",
          "Interp,s,s,s,s,s,s,s,s,s,l",
          "/scene,,,,,,,,,,",
          "/scenario,,,,,,,,,,");
      for(my $i = 8; $i<=9; $i++) {
        $end_transformation_scenario_string[$i] =~ s/ZZZ_clip_dir_name_ZZZ/$clip_dir_name/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_clip_name_ZZZ/$clip_name/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_transformed_clip_dir_ZZZ/$end_transformed_clip_dir/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_transformed_clip_name_ZZZ/$end_transformed_clip_name/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_tmp_dir_name_ZZZ/$tmp_dir_name/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_transf_percent_ZZZ/$scene_percentage/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_initial_frame_in_ZZZ/$initial_frame_in/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_final_frame_in_ZZZ/$final_frame_in/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_initial_transformed_frame_out_ZZZ/$initial_end_transform_frame_out/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_final_transformed_frame_out_ZZZ/$final_end_transform_frame_out/g;
        $end_transformation_scenario_string[$i] =~ s/ZZZ_end_transformation_ZZZ/$end_transformation/g;
      }
      #######################################################
      # applying the end_transformation with the current scenario file for each added frame
      foreach (@end_transformation_scenario_string) {
        print $FILEout "$_\r\n";
      }
      close( $FILEout );

      if(!$TRACE_ONLY) {
        # print  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/transform-movie.sh\n";
        my $count = sprintf("%04d",$initial_end_transform_frame_out);
        print "Output SVG file ", $end_transformed_clip_dir.$end_transformed_clip_name."_".$count.".svg","\n\n";
        if( SVG_movie_scenario_transformation::MAIN("-i", 
            "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_end_transformations-movie.csv") == 0) {
          return 0;
        }
      }
      $current_crossfaded_frame_out++;
    } #frames in the movie

    # /scene line
    $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    if($values_line[0] ne "/scene") {
      print "Expected tag \"/scene\" not found!\n";
      return 0;
    }
  } # scenes
  return 1;
}
MAIN(@ARGV);
1;