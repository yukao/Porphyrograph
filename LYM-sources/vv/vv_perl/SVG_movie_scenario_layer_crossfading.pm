#!/usr/bin/perl 

use strict;
use warnings;

package SVG_movie_scenario_layer_crossfading;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_scenario_interpolation_lib;
use SVG_movie_layer_crossfading;

# SVG_movie_scenario_layer_crossfading.pl
# reads a transformation scenario and executes it

my $USAGE =
"/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_scenario_layer_crossfading.pl -i scenario_file

ARGUMENTS:
   -i: input scenariofile
";

##################################################################
# MAIN SUB
##################################################################
sub MAIN {
  my $full_svg_input_file_name;
  my $full_svg_output_file_name;
  my $nb_layers;
  my $nb_composed_files;
  my $composition_mode_string;
  my $nb_scenes;
  my $scene_ID;

  my $scenario_file_name;
  my $FILEin;
  my $line;

  my @var_names;
  my %var_types;
  my %val_init;
  my %val_fin;
  my %val_interp;
  my %interp;

  my $trace_only = 0;

  ##################################################################
  # ARGUMENTS
  ##################################################################
  if( @_ != 2 ) {
      print $USAGE; die"SVG_movie_scenario_layer_crossfading.pl: number of arguments: 2 not ", 0+@_;
  }

  # scenario_file_name
  if( $_[0] ne "-i" ) {
      print $USAGE; die"SVG_movie_scenario_layer_crossfading.pl: -i expected for argt 1";
  }

  # print "Scenario file ", $scenario_file_name,"\n";
  $scenario_file_name = $_[1];
  if( !open(  $FILEin , '<:crlf', "<$scenario_file_name" )  ) {
    if( !open(  $FILEin , "<$scenario_file_name" )  ) {
      print "Can't open file $scenario_file_name!\n";
      return 0;
    }
  }

  $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
  $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);

  #3rd line: variable types
  my $line_types = <$FILEin>; 
  $line_types=SVG_scenario_interpolation_lib::win_chomp($line_types);

  #4th line: variable names
  $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
  @var_names = split(/,/,$line);
  #removes the first one
  shift(@var_names);
  # print"Var names [" , join("-", @var_names),"]\n";

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

    # applies possible transformations on the SVG files
    # creates the directories in which the paths files are saved
    if ( !-d $val_init{"svg_output_directory"}   ) {
        if(mkdir $val_init{"svg_output_directory"} == 0) {
          print "Failed to create path: ".$val_init{"svg_output_directory"}."\n";
          return 0;
        }
    }

    $nb_layers = $val_init{"nb_posterization_layers"};
    $nb_composed_files = $val_init{"nb_composed_files"};

    # printf("\nNb scenes %d scene ID %s input_file_name %s OUT start-end %s %s\n\n", 
    #   $nb_scenes, $scene_ID,
    #   $val_init{"svg_input_file_name_001"}, $val_init{"imageOutputIndex"}, 
    #   $val_fin{"imageOutputIndex"});

    $composition_mode_string = $val_init{"composition_mode"};
    for(my $indInputFile = 1 ; $indInputFile <= $nb_composed_files ; $indInputFile++) {
      my $countInputFile = sprintf "%03d", $indInputFile;
      if(($val_fin{"imageInputIndex_${countInputFile}"} - $val_init{"imageInputIndex_${countInputFile}"}) 
        < ($val_fin{"imageOutputIndex"} - $val_init{"imageOutputIndex"})) {
        print "Not enough input images in input file #${indInputFile}\n";
        return 0;
      }
    }
    
    my $image_rank = 0;
    for(my $indImageOutput = $val_init{"imageOutputIndex"} ; $indImageOutput <= $val_fin{"imageOutputIndex"}; $indImageOutput++) {

      my $scene_percentage = 0;
      if(($val_fin{"imageOutputIndex"} - $val_init{"imageOutputIndex"}) > 0) {
        $scene_percentage = ($indImageOutput - $val_init{"imageOutputIndex"})
              /($val_fin{"imageOutputIndex"} - $val_init{"imageOutputIndex"});
      }
      #clamping
      $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

      for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
        $val_interp{$var_names[$indVar]} = SVG_scenario_interpolation_lib::interpolated_value_calculation($var_types{$var_names[$indVar]}, $val_init{$var_names[$indVar]}, $val_fin{$var_names[$indVar]}, $interp{$var_names[$indVar]}, $scene_percentage);
      }

      if($trace_only) {
         printf("Ind image %d Scene percentage %.2f percent_transf1 %.2f\n", 
            $indImageOutput, $scene_percentage, $val_interp{"percent_transf1"});
      }

      # scans all the files and collects the paths to compose them
      my $string_all_input_files = "";
      for(my $indInputFile = 1 ; $indInputFile <= $nb_composed_files ; $indInputFile++) {
        my $countInputFile = sprintf "%03d", $indInputFile;
        $full_svg_input_file_name = $val_init{"svg_input_directory_${countInputFile}"} . $val_init{"svg_input_file_name_${countInputFile}"};

        my $indImageInput = $image_rank + $val_init{"imageInputIndex_${countInputFile}"};
        if( $indImageInput > $val_fin{"imageInputIndex_${countInputFile}"}) {
          $indImageInput = $val_fin{"imageInputIndex_${countInputFile}"};
        }
        my $countInputImage = sprintf "%04d", $indImageInput;
        $string_all_input_files = $string_all_input_files . "${full_svg_input_file_name}_${countInputImage}.svg ";
      }


      my $countOutputImage = sprintf "%04d", $indImageOutput;
      $full_svg_output_file_name = $val_init{"svg_output_directory"} . $val_init{"svg_output_file_name"} . "_${countOutputImage}.svg";

      if(!$trace_only) {
        # print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_layer_crossfading.pl --composition-mode ${composition_mode_string} --nb-files ${nb_composed_files} --nb-layers ${nb_layers} -o ${full_svg_output_file_name} -i ${string_all_input_files}\n";
        if(SVG_movie_layer_crossfading::MAIN("--composition-mode", ${composition_mode_string},
          "--nb-files", ${nb_composed_files}, "--nb-layers", ${nb_layers}, 
          "-o", ${full_svg_output_file_name}, "-i", split(/ +/,${string_all_input_files})) == 0) {
          return 0;
        }
      }

      # moves forward on output frames
      $image_rank++;
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
