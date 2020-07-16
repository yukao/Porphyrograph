#!/usr/bin/perl 

use strict;
use warnings;

use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/';
use SVG_scenario_interpolation_lib;
use SVG_movie_posterize;

# SVG_movie_scenario_posterization.pm
# reads a posterization scenario and executes it

my $USAGE =
" /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_posterization/SVG_movie_scenario_posterization.pm -i scenario_file

ARGUMENTS:
   -i: input scenariofile
";

##################################################################
# MAIN SUB
##################################################################
sub MAIN {
  my $full_input_file_name;
  my $full_png_output_file_name;
  my $full_svg_output_file_name;
  my $tmp_dir;
  my $start_time;
  my $end_time;
  my $fps;
  my $nb_layers;
  my $nb_scenes;
  my $scene_ID;

  my $FILEin;
  my $line;

  my @var_names;
  my %var_types;
  my %val_init;
  my %val_fin;
  my %interp;

  ##################################################################
  # ARGUMENTS
  ##################################################################
  if( @_ != 2 ) {
      print $USAGE; 
      print "SVG_movie_scenario_posterization.pm: number of arguments: 2 not ", 0+@_, "\n";
      return 0;
  }

  # scenario_file_name
  my $option = $_[0];
  if( $option ne "-i" ) {
      print $USAGE; 
      print"SVG_movie_scenario_posterization.pm: -i expected for argt 1\n";
      return 0;
  }

  # print "Scenario file ", $scenario_file_name,"\n";
  if( !open(  $FILEin , '<:crlf', "<$scenario_file_name" )  ) {
    if( !open(  $FILEin , "<$scenario_file_name" )  ) {
      print "Can't open file $scenario_file_name!\n";
      return 0;
    }
  }

  $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
  $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);

  #3rd line: variable types
  my $line_types = <$FILEin>; $line_types = SVG_scenario_interpolation_lib::win_chomp($line_types);

  #4th line: variable names
  $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
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
  $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
  @values_line = split(/,/,$line);
  if($values_line[0] ne "scenario") {
    print "Expected tag \"scenario\" not found!\n";
    return 0;
  }
  $nb_scenes = $values_line[1];

  # READING AND EXECUTING A SCENE

  for(my $ind_scene = 0; $ind_scene < $nb_scenes; $ind_scene++) {

    # scene line
    $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    if($values_line[0] ne "scene") {
      print "Expected tag \"scene\" not found!\n";
      return 0;
    }
    $scene_ID = $values_line[1];

    # scene ID line
    $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
    # var IDs line
    $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);

    # variable initial value
    $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
      $val_init{$var_names[$indVar]} = $values_line[$indVar];
    }

    # variable final value
    $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
      $val_fin{$var_names[$indVar]} = $values_line[$indVar];
    }

    # variable interpolation value
    $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    shift(@values_line);
    for(my $indVar = 0; $indVar < scalar @var_names; $indVar++) {
      $interp{$var_names[$indVar]} = $values_line[$indVar];
    }

    printf("\nNb scenes %d scene ID %s input_file_name %s start-end %s %s\n\n", $nb_scenes, $scene_ID,
      $val_init{"input_file_name"}, $val_init{"position"}, $val_fin{"position"});

    # VIDEO POSTERIZATION

    # from video to images
    # creates the directories in which the PNG files are saved
    if ( !-d $val_init{"png_output_directory"}   ) {
        if(!mkdir $val_init{"png_output_directory"}) {
          print  "Failed to create path: ".$val_init{"png_output_directory"}."\n";
          return 0;
        }
    }
    $full_input_file_name = $val_init{"input_directory"} . $val_init{"input_file_name"};
    $full_png_output_file_name = $val_init{"png_output_directory"} . $val_init{"png_output_file_name"};
    $start_time = $val_init{"position"};
    $end_time = $val_fin{"position"};
    $fps = $val_init{"fps"};
    print   "ffmpeg -i ${full_input_file_name} -ss ${start_time} -to ${end_time} -r ${fps} ${full_png_output_file_name}_%4d.png\n";
    system( "ffmpeg -i ${full_input_file_name} -ss ${start_time} -to ${end_time} -r ${fps} ${full_png_output_file_name}_%4d.png");

    # count the images from PNG files
    my $FILEcount;
    open($FILEcount, "ls -1 ${full_png_output_file_name}_*.png | wc -l|");
    my $nb_files = <$FILEcount>;
    $nb_files = SVG_scenario_interpolation_lib::win_chomp($nb_files);
    close($FILEcount);

    # image composition: generally addtion of a mask above the initial video to make a zone such as rock or sky in a uniform color
    # creates the directories in which the paths files are saved
    # if ( !-d "movie_composed_PNGs/${output_file_name_wo_extension}" ) {
    #     mkdir "movie_composed_PNGs/${output_file_name_wo_extension}" or die "Failed to create path: movie_composed_PNGs/${output_file_name_wo_extension}";
    # }
    # for(my $i = 1 ; $i <= $nb_files ; $i++) {
    #     my $count = sprintf "%04d", $i;
    #     # movie_PATHs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.path
    #     if($composition_function == OVERLAY) {
    #       print "composite ${composition_file} movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png\n";
    #       system("composite ${composition_file} movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png");
    #     }
    #     elsif($composition_function == OVERLAY_INVERT) {
    #       print "composite ${composition_file} movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png tmp/overlaid.png\n";
    #       system("composite ${composition_file} movie_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_${count}.png tmp/overlaid.png");
    #       print "convert tmp/overlaid.png -channel RGB -negate movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png\n";
    #       system("convert tmp/overlaid.png -channel RGB -negate movie_composed_PNGs/${output_file_name_wo_extension}/${output_file_name_wo_extension}_composed_${count}.png");
    #     }
    # }

    # transformation of the video into a vector video of SVG files
    # creates the directories in which the paths SVG files are saved
    if ( !-d $val_init{"svg_output_directory"}   ) {
        if(!mkdir $val_init{"svg_output_directory"}) {
          print  "Failed to create path: ".$val_init{"svg_output_directory"}."\n";
          return 0;
        }
    }
    for(my $indImage = 1 ; $indImage <= $nb_files ; $indImage++) {
        my $count = sprintf "%04d", $indImage;
        $nb_layers = $val_init{"nb_posterization_layers"};
        $tmp_dir = $val_init{"tmp_directory"};

        my $dimming = 1.0;
        if($val_init{"fade_in_length"} > 0 && $count <= ($val_init{"fade_in_length"} * $val_init{"fps"})) {
            $dimming = ($count - 1)/($val_init{"fade_in_length"} * $val_init{"fps"});
        }
        elsif($val_init{"fade_out_length"} > 0 && $count >=  ($nb_files - ($val_init{"fade_out_length"} * $val_init{"fps"}))) {
            $dimming = ($nb_files - $count)/($val_init{"fade_out_length"} * $val_init{"fps"});
        }
        #clamping
        if($dimming > 1.0) {
            $dimming = 1.0;
        }
        elsif($dimming < 0.0) {
            $dimming = 0.0;
        }
        $full_svg_output_file_name = $val_init{"svg_output_directory"} . $val_init{"svg_output_file_name"};
        print "SVG_movie_posterize --nb-layers ${nb_layers} --dimming ${dimming} -o ${full_svg_output_file_name}_${count}.svg -i ${full_png_output_file_name}_${count}.png --tmp ${tmp_dir}\n";
        if( SVG_movie_posterize::MAIN("--nb-layers", ${nb_layers}, 
          "--dimming", ${dimming}, 
          "-o", "${full_svg_output_file_name}_${count}.svg", 
          "-i", "${full_png_output_file_name}_${count}.png", 
          "--tmp", ${tmp_dir}) == 0) {
          return 0;
        }
    }

    # /scene line
    $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
    @values_line = split(/,/,$line);
    if($values_line[0] ne "/scene") {
      print "Expected tag \"/scene\" not found!\n";
      return 0;
    }
  }

  # /scenario line
  $line = <$FILEin>; $line = SVG_scenario_interpolation_lib::win_chomp($line);
  @values_line = split(/,/,$line);
  if($values_line[0] ne "/scenario") {
    print "Expected tag \"/scenario\" not found!\n";
    return 0;
  }
  return 1;
}

MAIN(@ARGV);
