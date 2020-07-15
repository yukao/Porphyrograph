#!/usr/bin/perl 
use strict;
use warnings;
sub win_chomp {
    my $loc_chain = shift;
    $loc_chain =~ s/\r?\n$//;
    return $loc_chain;
}
sub MAIN {
  print "file ",$ARGV[0], ", arguments ", (@ARGV + 0), "\n";
  my $FILEin; 
  my $FILEout;
  # print "Scenario file ", $scenario_file_name,"\n";
  my $scenario_in_file_name = $ARGV[0];
  if( !open(  $FILEin , '<:crlf', "<$scenario_in_file_name" )  ) {
    if( !open(  $FILEin , "<$scenario_in_file_name" )  ) {
      print "Can't open file $scenario_in_file_name!\n";
      return 0;
    }
  }

  # print "Scenario file ", $scenario_file_name,"\n";
  my $scenario_out_file_name = $ARGV[1];
  if( !open(  $FILEout , ">$scenario_out_file_name" )  ) {
      print "Can't open file $scenario_out_file_name!\n";
      return 0;
  }

  my $line1 = <$FILEin>; # print "line1 ", $line; 
  $line1=win_chomp($line1);
  my @line1a = split(/,/,$line1);
  my $line2 = <$FILEin>; # print "line2 ", $line; 
  $line2=win_chomp($line2);
  my @line2a = split(/,/,$line2);

  #3rd line: variable types
  my $line_types = <$FILEin>; 
  $line_types=win_chomp($line_types);
  my  @line3a = split(/,/,$line_types);

  #4th line: variable names
  my $line4 = <$FILEin>; 
  $line4=win_chomp($line4);
  my @var_names = split(/,/,$line4);
  my $indRepop = 0;
  my $var;
  foreach $var (@var_names) {
    if($var eq "repop_color") {
      last;
    }
    $indRepop++;
  }

  my @new_vars = qw(repop_colorBG repop_colorBG_pulse repop_greyBG repop_greyBG_pulse repop_colorPart repop_colorPart_pulse repop_greyPart repop_greyPart_pulse repop_colorCA repop_colorCA_pulse  repop_greyCA repop_greyCA_pulse);
  my @new_types = qw(float float float float float float float float float float float float);
  my @new_callbacks = qw(NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL );
  my @new_shaders = qw(* * * * * * * * * * * * );
  my @new_pulses = qw(pulsed_special * pulsed_special * pulsed_special * pulsed_special * pulsed_special * pulsed_special * );

  splice @line2a, $indRepop, 4, @new_vars; 
  splice @line3a, $indRepop, 4, @new_types; 
  splice @var_names, $indRepop, 4, @new_vars; 
  my $ind = 0;
  @line1a = ();
  foreach $var (@var_names) {
        $line1a[$ind] = $ind - 1;
        $ind++;
  }
  $line1a[0] = qw(RANK);

  print $FILEout join( ',' , @line1a) , "\r\n";
  print $FILEout join( ',' , @line2a) , "\r\n";
  print $FILEout join( ',' , @line3a) , "\r\n";
  print $FILEout join( ',' , @var_names) , "\r\n";

  # callbacks
  $line1 = <$FILEin>; 
  $line1=win_chomp($line1);
  @line1a = split(/,/,$line1);
  splice @line1a, $indRepop, 4, @new_callbacks; 
  print $FILEout join( ',' , @line1a) , "\r\n";

  # GUI
  $line1 = <$FILEin>; 
  $line1=win_chomp($line1);
  @line1a = split(/,/,$line1);
  splice @line1a, $indRepop, 4, @new_vars; 
  print $FILEout join( ',' , @line1a) , "\r\n";

  # shaders
  $line1 = <$FILEin>; 
  $line1=win_chomp($line1);
  @line1a = split(/,/,$line1);
  splice @line1a, $indRepop, 4, @new_shaders; 
  print $FILEout join( ',' , @line1a) , "\r\n";

  # pulse
  $line1 = <$FILEin>; 
  $line1=win_chomp($line1);
  @line1a = split(/,/,$line1);
  splice @line1a, $indRepop, 4, @new_pulses; 
  print $FILEout join( ',' , @line1a) , "\r\n";

  # initial_values
  $line1 = <$FILEin>; 
  print $FILEout $line1;

  # values
  $line1 = <$FILEin>; 
  $line1=win_chomp($line1);
  @line1a = split(/,/,$line1);
  my @new_values = ($line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3], $line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3], $line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3]);
  splice @line1a, $indRepop, 4, @new_values; 
  print $FILEout join( ',' , @line1a) , "\r\n";

  # /initial_values
  $line1 = <$FILEin>; 
  print $FILEout $line1;

  # scenario
  $line1 = <$FILEin>; 
  print $FILEout $line1;
  $line1=win_chomp($line1);
  @line1a = split(/,/,$line1);
  my $nb_scenes = $line1a[1];

  ##################################################################
  # READING AND EXECUTING A SCENE
  ##################################################################
  for(my $ind_scene = 0; $ind_scene < $nb_scenes; $ind_scene++) {
    # scene line
    $line1 = <$FILEin>; 
    print $FILEout $line1;
    $line1=win_chomp($line1);
    @line1a = split(/,/,$line1);
    if($line1a[0] ne "scene") {
      print "Expected tag \"scene\" not found!\n";
      return 0;
    }
    my $scene_ID = $line1a[1];
    my @scene_IDs = ($scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID, $scene_ID );

    # scene ID line
    $line1 = <$FILEin>; 
    $line1=win_chomp($line1);
    @line1a = split(/,/,$line1);
    splice @line1a, $indRepop, 4, @scene_IDs; 
    print $FILEout join( ',' , @line1a) , "\r\n";

    # var IDs line
    $line1 = <$FILEin>; 
    $line1=win_chomp($line1);
    @line1a = split(/,/,$line1);
    splice @line1a, $indRepop, 4, @new_vars; 
    print $FILEout join( ',' , @line1a) , "\r\n";

    # variable initial value
    $line1 = <$FILEin>; 
    $line1=win_chomp($line1);
    @line1a = split(/,/,$line1);
    @new_values = ($line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3], $line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3], $line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3]);
    splice @line1a, $indRepop, 4, @new_values; 
    print $FILEout join( ',' , @line1a) , "\r\n";

    # variable final value
    $line1 = <$FILEin>; 
    $line1=win_chomp($line1);
    @line1a = split(/,/,$line1);
    @new_values = ($line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3], $line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3], $line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3]);
    splice @line1a, $indRepop, 4, @new_values; 
    print $FILEout join( ',' , @line1a) , "\r\n";

    # variable interpolation value
    $line1 = <$FILEin>; 
    $line1=win_chomp($line1);
    @line1a = split(/,/,$line1);
    @new_values = ($line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3], $line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3], $line1a[$indRepop], $line1a[$indRepop+1], $line1a[$indRepop+2], $line1a[$indRepop+3]);
    splice @line1a, $indRepop, 4, @new_values; 
    print $FILEout join( ',' , @line1a) , "\r\n";

    # /scenes
    $line1 = <$FILEin>; 
    print $FILEout $line1;
  }
  while($line1 = <$FILEin>) {
    print $FILEout $line1;
  }
  close($FILEout);
  close($FILEin);
}
MAIN(@_);