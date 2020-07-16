#!/usr/bin/perl 

use strict;
use warnings;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_scenario_interpolation_lib;

# SVG_movie_scenario_rasterization.pm
# reads a rasterization scenario and executes it

my $USAGE =
" /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_rasterization/SVG_rasterization/SVG_movie_scenario_rasterization.pm -i scenario_file

ARGUMENTS:
   -i: input scenariofile
";

##################################################################
# MAIN SUB
##################################################################
sub MAIN {
  my $full_svg_input_file_name;
  my $full_png_output_file_name;
  my $tmp_dir;
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

  ##################################################################
  # ARGUMENTS
  ##################################################################
  if( @_ != 2 ) {
      print $USAGE; 
      print "SVG_movie_scenario_rasterization.pm: number of arguments: 2 not ", 0+@_, "\n";
      return 0;
  }

  # scenario_file_name
  my $option = $_[0];
  if( $option ne "-i" ) {
      print $USAGE; 
      print"SVG_movie_scenario_rasterization.pm: -i expected for argt 1\n";
      return 0;
  }

  # print "Scenario file ", $scenario_file_name,"\n";
  my $scenario_file_name = $_[1];
  if( !open(  $FILEin , "<$scenario_file_name" ) ) {
    print "Can't open rasterization scenario file $scenario_file_name!\n";
    return 0;
  }

  $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);
  $line = <$FILEin>; $line=SVG_scenario_interpolation_lib::win_chomp($line);

  #3rd line: variable types
  my $line_types = <$FILEin>; $line_types=SVG_scenario_interpolation_lib::win_chomp($line_types);

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

    printf("\nNb scenes %d scene ID %s input_file_name %s IN start-end %s %s OUT start-end %s %s\n\n", $nb_scenes, $scene_ID,
      $val_init{"svg_input_file_name"}, $val_init{"imageInputIndex"}, 
      $val_fin{"imageInputIndex"}, $val_init{"imageOutputIndex"}, 
      $val_fin{"imageOutputIndex"});

    # VIDEO RASTERIZATION

    # from video to images
    # creates the directories in which the PNG files are saved
    if ( !-d $val_init{"png_output_directory"}   ) {
        if(!mkdir $val_init{"png_output_directory"}) {
          print  "Failed to create path: ".$val_init{"png_output_directory"}."\n";
          return 0;
        }
    }
    $full_svg_input_file_name = $val_init{"svg_input_directory"} . $val_init{"svg_input_file_name"};
    $full_png_output_file_name = $val_init{"png_output_directory"} . $val_init{"png_output_file_name"};

    # count the images from PNG files
    my $FILEcount;
    open($FILEcount, "ls -1 ${full_svg_input_file_name}_*.svg | wc -l|");
    my $nb_files = <$FILEcount>;
    chomp($nb_files);
    close($FILEcount);

    # transformation of the vector video of SVG files into PNG files
    my $indImageOutput = $val_init{"imageOutputIndex"};
    for(my $indImageInput = $val_init{"imageInputIndex"} ; $indImageInput <= $val_fin{"imageInputIndex"}; $indImageInput++) {
        my $countInput = sprintf "%04d", $indImageInput;
        my $countOutput = sprintf "%04d", $indImageOutput;
        $tmp_dir = $val_init{"tmp_directory"};

        my $scene_percentage = 0.0;
        if($val_fin{"imageInputIndex"} != $val_init{"imageInputIndex"}) {
          $scene_percentage = ($indImageInput - $val_init{"imageInputIndex"})
                    /($val_fin{"imageInputIndex"} - $val_init{"imageInputIndex"});
        }
        $scene_percentage = SVG_scenario_interpolation_lib::clamp($scene_percentage);

        for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
          $val_interp{$var_names[$indVar]} = SVG_scenario_interpolation_lib::interpolated_value_calculation($var_types{$var_names[$indVar]}, $val_init{$var_names[$indVar]}, $val_fin{$var_names[$indVar]}, $interp{$var_names[$indVar]}, $scene_percentage);
        }

        # converts the SVG into a bitmap, larger than the final image for a better quality and to crop the border
        # background color should not be visible because a rectangle covering the whole page as been added in the fill and stroke step
        print "inkscape -z -w 3840 -h 2160 --export-area-page -b #FFFFFFFF -e ${tmp_dir}tmp_large.png ${full_svg_input_file_name}_${countInput}.svg","\n";
        system( "inkscape -z -w 3840 -h 2160 --export-area-page -b #FFFFFFFF -e ${tmp_dir}tmp_large.png ${full_svg_input_file_name}_${countInput}.svg" ); 

        # cropping and downscale of the png inmages
        my $crop_offset_x = int($val_interp{"crop_offset_x"});
        my $crop_offset_y = int($val_interp{"crop_offset_y"});
        my $crop_size_x = int($val_interp{"crop_size_x"});
        my $crop_size_y = int($val_interp{"crop_size_y"});
        print "convert ${tmp_dir}tmp_large.png -crop ${crop_size_x}x${crop_size_y}+${crop_offset_x}+${crop_offset_y} -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 ${full_png_output_file_name}_${countOutput}.png\n";
        system("convert ${tmp_dir}tmp_large.png -crop ${crop_size_x}x${crop_size_y}+${crop_offset_x}+${crop_offset_y} -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 ${full_png_output_file_name}_${countOutput}.png");

        $indImageOutput++;
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

MAIN(@ARGV);
