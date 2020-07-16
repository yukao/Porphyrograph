#!/usr/bin/perl 

use strict;
use warnings;

# SVG_movie_scenario_transformation_precheck.pl
# reads a transformation scenario and checks that all the expected input frames are actually found (to avoid missing frames in the transformation phase)

my $USAGE =
"/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_scenario_transformation.pl -i scenario_file

ARGUMENTS:
   -i: input scenariofile
";

my $full_svg_input_file_name;
my $transformation_function_string;
my $nb_scenes;
my $nb_vars;

my $scenario_file_name;
my $FILEin;
my $line;

my @var_names;
my %var_types;
my %val_init;
my %val_fin;

##################################################################
# MAIN SUB
##################################################################
sub MAIN {
  ##################################################################
  # ARGUMENTS
  ##################################################################
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
  if( !open(  $FILEin , '<:crlf', "<$scenario_file_name" ) ) {
    print "Can't open file $scenario_file_name!\n";
    return 0;
  }


  $line = <$FILEin>; chomp($line);
  $line = <$FILEin>; chomp($line);

  #3rd line: variable types
  my $line_types = <$FILEin>; chomp($line_types);

  #4th line: variable names
  $line = <$FILEin>; chomp($line);
  @var_names = split(/,/,$line);
  #removes the first one
  shift(@var_names);
  # print"Var names [" , join("-", @var_names),"]\n";
  $nb_vars = @var_names + 0;
  printf "Number of variables: %d\n", $nb_vars;

  #finishes the types now that the variable strings are known
  my @values_line = split(/,/,$line_types);
  #removes the first one
  shift(@values_line);
  for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
    $var_types{$var_names[$indVar]} = $values_line[$indVar];
  }

  # scenario line
  $line = <$FILEin>; chomp($line);
  @values_line = split(/,/,$line);
  if($values_line[0] ne "scenario") {
    print "Expected tag \"scenario\" not found!\n";
    return 0;
  }
  $nb_scenes = $values_line[1];

  # READING AND EXECUTING A SCENE

  for(my $ind_scene = 0; $ind_scene < $nb_scenes; $ind_scene++) {

    # scene line
    $line = <$FILEin>; chomp($line);
    @values_line = split(/,/,$line);
    if($values_line[0] ne "scene") {
      print "Expected tag \"scene\" not found!\n";
      return 0;
    }

    # scene ID line
    $line = <$FILEin>; chomp($line);
    # var IDs line
    $line = <$FILEin>; chomp($line);

    # variable initial value
    $line = <$FILEin>; chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
      $val_init{$var_names[$indVar]} = $values_line[$indVar];
    }
    if(@values_line + 0 != $nb_vars) {
      print $line;
      printf "Incorrect number of initial values: %d instead of %d in scene %d\n", (@values_line + 0),  $nb_vars, ($ind_scene + 1);
    }

    # variable final value
    $line = <$FILEin>; chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
      $val_fin{$var_names[$indVar]} = $values_line[$indVar];
    }
    if(@values_line + 0 != $nb_vars) {
      print $line;
      printf "Incorrect number of final values: %d instead of %d in scene %d\n", (@values_line + 0),  $nb_vars, ($ind_scene + 1);
    }

    # variable interpolation value
    $line = <$FILEin>; chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    if(@values_line + 0 != $nb_vars) {
      print $line;
      printf "Incorrect number of interpolation values: %d instead of %d in scene %d\n", (@values_line + 0),  $nb_vars, ($ind_scene + 1);
    }

    my $pas = 1;
    if($val_init{"imageInputIndex"} < $val_fin{"imageInputIndex"}) {
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
    for(my $indImageInput = $val_init{"imageInputIndex"} ; $indImageInput != $val_fin{"imageInputIndex"} + $pas; $indImageInput += $pas) {
        my $countInput = sprintf "%04d", $indImageInput;
        $full_svg_input_file_name = $val_init{"svg_input_directory"} . $val_init{"svg_input_file_name"};

        # checks that ${full_svg_input_file_name}_${countInput}.svg exists
        if ( !-r "${full_svg_input_file_name}_${countInput}.svg"   ) {
             my $scene_no = $ind_scene + 1;
            print "Failed to find frame: ${full_svg_input_file_name}_${countInput}.svg in scene ${scene_no} (image output #$indImageOutput)\n";
        }

        # moves forward on output frames
        $indImageOutput++;

       # only one frame if frame ini and frame end are equal
        last if ($pas == 0);
    }

    # /scene line
    $line = <$FILEin>; chomp($line);
    @values_line = split(/,/,$line);
    if($values_line[0] ne "/scene") {
      print "Expected tag \"/scene\" not found!\n";
      return 0;
    }
  }

  # /scenario line
  $line = <$FILEin>; chomp($line);
  @values_line = split(/,/,$line);
  if($values_line[0] ne "/scenario") {
    print "Expected tag \"/scenario\" not found!\n";
    return 0;
  }

  print "*** Pretest passed successfully! ****\n";
  return 1;
}

MAIN(@ARGV);
