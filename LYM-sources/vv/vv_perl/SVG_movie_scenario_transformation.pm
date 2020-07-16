#!/usr/bin/perl 

use strict;

use warnings;

package SVG_movie_scenario_transformation;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_scenario_interpolation_lib;
use SVG_movie_transform;
use SVG_movie_fillAndStroke;


# SVG_movie_scenario_transformation.pl
# reads a transformation scenario and executes it

my $USAGE =
"/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_scenario_transformation.pl -i scenario_file

ARGUMENTS:
   -i: input scenariofile
";

##################################################################
# MAIN SUB
##################################################################
sub MAIN {
  my $full_svg_input_file_name;
  my $full_svg_output_file_name;
  my $tmp_dir;
  my $nb_layers;
  my $transformation_function_string;
  my $nb_scenes;
  my $scene_ID;

  my $FILEin;
  my $line;

  my @var_names;
  my %var_types;
  my %val_init;
  my %val_fin;
  my %val_interp;
  my %interp;

  my $trace_only = 0;

  if( @_ != 2 ) {
      print $USAGE; 
      print "SVG_movie_scenario_transformation.pl: number of arguments: 2 not ", 0+@_, "\n";
      return 0;
  }

  # scenario_file_name
  my $option = $_[0];
  if( $option ne "-i" ) {
      print $USAGE; 
      print"SVG_movie_scenario_transformation.pl: -i expected for argt 1\n";
      return 0;
  }

  # print "Scenario file ", $scenario_file_name,"\n";
  my $scenario_file_name = $_[1];
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
  $nb_scenes = $values_line[1];

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
    $scene_ID = $values_line[1];

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

    # printf("\nNb scenes %d scene ID %s input_file_name %s IN start-end %s %s OUT start-end %s %s\n\n", $nb_scenes, $scene_ID,
    #   $val_init{"svg_input_file_name"}, $val_init{"imageInputIndex"}, 
    #   $val_fin{"imageInputIndex"}, $val_init{"imageOutputIndex"}, 
    #   $val_fin{"imageOutputIndex"});

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

    my $indImageOutput = $val_init{"imageOutputIndex"};
    for(my $indImageInput = $val_init{"imageInputIndex"} ; 
           $indImageInput != $val_fin{"imageInputIndex"} + $pas; 
           $indImageInput += $pas) {
        my $countInput = sprintf "%04d", $indImageInput;
        my $countOutput = sprintf "%04d", $indImageOutput;
        $nb_layers = $val_init{"nb_posterization_layers"};
        $tmp_dir = $val_init{"tmp_directory"};

        my $scene_percentage = 0;
        if($pas != 0 && ($val_fin{"imageInputIndex"} - $val_init{"imageInputIndex"}) != 0 ) {
          $scene_percentage = ($indImageInput - $val_init{"imageInputIndex"})/($val_fin{"imageInputIndex"} - $val_init{"imageInputIndex"});
        }
        #clamping
        $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

        for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
          $val_interp{$var_names[$indVar]} = SVG_scenario_interpolation_lib::interpolated_value_calculation($var_types{$var_names[$indVar]}, $val_init{$var_names[$indVar]}, $val_fin{$var_names[$indVar]}, $interp{$var_names[$indVar]}, $scene_percentage);
        }

        if($trace_only) {
           # printf("Ind image %d Scene percentage %.2f percent_transf1 %.2f fill_opacity_l1 %.2f fill_color_l4 %s fill_color_bg %s\n", 
           #    $indImageInput, $scene_percentage, $val_interp{"percent_transf1"}, $val_interp{"fill_opacity_l1"}, $val_interp{"fill_color_l4"}, $val_interp{"fill_color_bg"});
        }

        my $transf_percentage = $val_interp{"percent_transf1"};
        #clamping
        $transf_percentage = SVG_scenario_interpolation_lib::clamp($transf_percentage);

        $full_svg_input_file_name = $val_init{"svg_input_directory"} . $val_init{"svg_input_file_name"};
        $full_svg_output_file_name = $val_init{"svg_output_directory"} . $val_init{"svg_output_file_name"};
        $transformation_function_string = $val_init{"transf1_ID"};

        if(!$trace_only) {
          # print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_transform.pl --nb-layers ${nb_layers} -o ${tmp_dir}tmp_transformed.svg -i ${full_svg_input_file_name}_${countInput}.svg --transformation ${transformation_function_string} --percent ${transf_percentage} \n";
          
          # print "Transformed SVG file ", ${tmp_dir}."tmp_transformed.svg\n";
          # passes translation arguments from the scenario file: translation vector
          if((exists $val_interp{"x_translation"}) && (exists $val_interp{"y_translation"})) {
            my $interp_x_translation = $val_interp{"x_translation"};
            my $interp_y_translation = $val_interp{"y_translation"};
            if( SVG_movie_transform::MAIN("--nb-layers", ${nb_layers}, 
              "-o", "${tmp_dir}tmp_transformed.svg", 
              "-i", "${full_svg_input_file_name}_${countInput}.svg", 
              "--transformation", ${transformation_function_string}, 
              "--percent", ${transf_percentage}, "--translation", ${interp_x_translation}, ${interp_y_translation}) == 0) {
              return 0;
            }
          }
          # passes rotation arguments from the scenario file: translation vector
          elsif((exists $val_interp{"x_rotation_center"}) && (exists $val_interp{"y_rotation_center"})
             && (exists $val_interp{"rotation_angle"})) {
            my $interp_x_rotation_center = $val_interp{"x_rotation_center"};
            my $interp_y_rotation_center = $val_interp{"y_rotation_center"};
            my $interp_rotation_angle = $val_interp{"rotation_angle"};
            if( SVG_movie_transform::MAIN("--nb-layers", ${nb_layers}, 
              "-o", "${tmp_dir}tmp_transformed.svg", 
              "-i", "${full_svg_input_file_name}_${countInput}.svg", 
              "--transformation", ${transformation_function_string}, "--percent", ${transf_percentage}, 
              "--rotation", ${interp_x_rotation_center}, ${interp_y_rotation_center}, 
              ${interp_rotation_angle}) == 0) {
              return 0;
            }
          }
          # passes scaling arguments from the scenario file: translation vector
          elsif((exists $val_interp{"x_scaling_center"}) && (exists $val_interp{"y_scaling_center"})
             && (exists $val_interp{"scaling_factor"})) {
            my $interp_x_scaling_center = $val_interp{"x_scaling_center"};
            my $interp_y_scaling_center = $val_interp{"y_scaling_center"};
            my $interp_scaling_factor = $val_interp{"scaling_factor"};
            # print "SVG_movie_transform::MAIN(--nb-layers", ${nb_layers}, 
            #   "-o", "${tmp_dir}tmp_transformed.svg", 
            #   "-i", "${full_svg_input_file_name}_${countInput}.svg", 
            #   "--transformation", ${transformation_function_string}, 
            #   "--percent", ${transf_percentage}, 
            #   "--scale", ${interp_x_scaling_center}, ${interp_y_scaling_center}, "\n";
            if( SVG_movie_transform::MAIN("--nb-layers", ${nb_layers}, 
              "-o", "${tmp_dir}tmp_transformed.svg", 
              "-i", "${full_svg_input_file_name}_${countInput}.svg", 
              "--transformation", ${transformation_function_string}, "--percent", ${transf_percentage}, 
              "--scale", ${interp_x_scaling_center}, ${interp_y_scaling_center}, 
              ${interp_scaling_factor}) == 0) {
              return 0;
            }
          }
          # passes Bezier arguments from the scenario file: control points
          elsif((exists $val_interp{"b0_x"}) && (exists $val_interp{"b0_y"})
             && (exists $val_interp{"b1_x"}) && (exists $val_interp{"b1_y"})
             && (exists $val_interp{"b2_x"}) && (exists $val_interp{"b2_y"})
             && (exists $val_interp{"b3_x"}) && (exists $val_interp{"b3_y"})) {
            my $interp_b0_x = $val_interp{"b0_x"};
            my $interp_b0_y = $val_interp{"b0_y"};
            my $interp_b1_x = $val_interp{"b1_x"};
            my $interp_b1_y = $val_interp{"b1_y"};
            my $interp_b2_x = $val_interp{"b2_x"};
            my $interp_b2_y = $val_interp{"b2_y"};
            my $interp_b3_x = $val_interp{"b3_x"};
            my $interp_b3_y = $val_interp{"b3_y"};
            if( SVG_movie_transform::MAIN("--nb-layers", ${nb_layers}, 
              "-o", "${tmp_dir}tmp_transformed.svg", 
              "-i", "${full_svg_input_file_name}.svg", # empty input file for string substitution, does not take frame count in consideration
              "--transformation", ${transformation_function_string}, 
              "--percent", ${transf_percentage}, 
              "--bezier_curve", ${interp_b0_x}, ${interp_b0_y}, ${interp_b1_x}, ${interp_b1_y}, 
              ${interp_b2_x}, ${interp_b2_y}, ${interp_b3_x}, ${interp_b3_y}) == 0) {
              return 0;
            }
          }
          # passes Archimedean spiral arguments from the scenario file: center, radial expansion, angular step 
          # and number of steps
          elsif((exists $val_interp{"spiral_center_x"}) && (exists $val_interp{"spiral_center_y"})
             && (exists $val_interp{"spiral_radius_step"}) && (exists $val_interp{"spiral_angle_step"})
             && (exists $val_interp{"spiral_nb_steps"})) {
            my $interp_spiral_center_x = $val_interp{"spiral_center_x"};
            my $interp_spiral_center_y = $val_interp{"spiral_center_y"};
            my $interp_spiral_radius_step = $val_interp{"spiral_radius_step"};
            my $interp_spiral_angle_step = $val_interp{"spiral_angle_step"};
            my $interp_spiral_nb_steps = $val_interp{"spiral_nb_steps"};
            if( SVG_movie_transform::MAIN("--nb-layers", ${nb_layers}, 
              "-o", "${tmp_dir}tmp_transformed.svg", 
              "-i", "${full_svg_input_file_name}.svg", # empty input file for string substitution, does not take frame count in consideration
              "--transformation", ${transformation_function_string}, 
              "--percent", ${transf_percentage}, 
              "--archimede_spiral", ${interp_spiral_center_x}, ${interp_spiral_center_y}, 
              ${interp_spiral_radius_step}, ${interp_spiral_angle_step}, ${interp_spiral_nb_steps}) == 0) {
              return 0;
            }
          }
          # no additional arguments to the transformation
          else {
            if( SVG_movie_transform::MAIN("--nb-layers", ${nb_layers}, "-o", "${tmp_dir}tmp_transformed.svg", "-i", "${full_svg_input_file_name}_${countInput}.svg", "--transformation", ${transformation_function_string}, "--percent", ${transf_percentage}) == 0) {
              return 0;
            }
          }
        }

        if((exists $val_interp{"fill_color_bg"}) && (exists $val_interp{"fill_color_l1"})
             && (exists $val_interp{"fill_color_l2"}) && (exists $val_interp{"fill_color_l3"})
             && (exists $val_interp{"fill_color_l4"}) && (exists $val_interp{"fill_opacity_l1"})
             && (exists $val_interp{"fill_opacity_l2"}) && (exists $val_interp{"fill_opacity_l3"})
             && (exists $val_interp{"fill_opacity_l4"}) && (exists $val_interp{"stroke_color"}) 
             && (exists $val_interp{"stroke_opacity"}) && (exists $val_interp{"stroke_width"}) ) {
          my $interp_fill_color_bg = $val_interp{"fill_color_bg"};
          my $interp_fill_color_l1 = $val_interp{"fill_color_l1"};
          my $interp_fill_color_l2 = $val_interp{"fill_color_l2"};
          my $interp_fill_color_l3 = $val_interp{"fill_color_l3"};
          my $interp_fill_color_l4 = $val_interp{"fill_color_l4"};
          my $interp_fill_opacity_l1 = $val_interp{"fill_opacity_l1"};
          my $interp_fill_opacity_l2 = $val_interp{"fill_opacity_l2"};
          my $interp_fill_opacity_l3 = $val_interp{"fill_opacity_l3"};
          my $interp_fill_opacity_l4 = $val_interp{"fill_opacity_l4"};
          my $interp_stroke_color = $val_interp{"stroke_color"};
          my $interp_stroke_opacity = $val_interp{"stroke_opacity"};
          my $interp_stroke_width = $val_interp{"stroke_width"};

          if(!$trace_only) {
            # print "Fill and stroke SVG file ", ${full_svg_output_file_name}."_".${countOutput}.".svg"  ,"\n";

            if( SVG_movie_fillAndStroke::MAIN("--nb-layers", ${nb_layers}, 
              "-o", "${full_svg_output_file_name}_${countOutput}.svg", 
              "-i", "${tmp_dir}tmp_transformed.svg", 
              "--style", "$interp_fill_color_bg", 
              "$interp_fill_color_l1", "$interp_fill_color_l2", 
              "$interp_fill_color_l3", "$interp_fill_color_l4", 
              $interp_fill_opacity_l1, $interp_fill_opacity_l2, 
              $interp_fill_opacity_l3, $interp_fill_opacity_l4, 
              "$interp_stroke_color", $interp_stroke_opacity, $interp_stroke_width) == 0) {
                return 0;
              }
          }
        }
        else {
          system("cp ${tmp_dir}tmp_transformed.svg ${full_svg_output_file_name}_${countOutput}.svg");
        }

        # moves forward on output frames
        $indImageOutput++;

        # only one frame if frame ini and frame end are equal
        last if ($pas == 0);
    }

    # /scene line
    $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    if($values_line[0] ne "/scene") {
      print "Expected tag \"/scene\" not found!\n";
      return 0;
    }
  }

  # /scenario line
  $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
  @values_line = split(/,/,$line);
  if($values_line[0] ne "/scenario") {
    print "Expected tag \"/scenario\" not found!\n";
    return 0;
  }
  return 1;
}

