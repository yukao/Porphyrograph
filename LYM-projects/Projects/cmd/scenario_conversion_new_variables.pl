#!/usr/bin/perl 

use strict;
use warnings;

# scenario_conversion_new_variables.pl
# scenario_conversion_new_variables transform a scenario for a new variable mapping

# usage
# scenario_conversion_new_variables.pl -i <old_scenario> -m <variable_mapping> -o <new_scenario>

# IN
#
#              the old scenario and a mapping between old and new variables + new variables + abandonned variables

# OUT 
#
#              the new scenario for the new variables with values preserved

my $USAGE =
"usage: scenario_conversion_new_variables.pl -i <old_scenario> -m <variable_mapping> -o <new_scenario>
./scenario_conversion_new_variables.pl -o converted_scenarios/LYM_demo-Bezier-scenario-v1.txt -i LYM_demo-Bezier-scenario-v1.txt -m LYM_variable_mapping_to_demo.txt

ARGUMENTS:
   -o: output scenario 
   -i: input scenario
   -m: mapping file: 1st line = list of old IDs / 2nd line = list of new IDs / 3rd line = NEW or corresponding old ID / 4th line = abandonned IDs
   NEW/VERBATIM/TYPE/ID/FUNCTION/GUI/SHADER/CHAR-/CHAR+/STEP-/STEP+/MIN_OR_EQUAL/MAX_OR_EQUAL/initial_value
";

my $numArgs;
$numArgs = $#ARGV + 1;

if( @ARGV != 6 ) {
    print $USAGE; die "number of arguments: 6 not ", 0+@ARGV;
}

# output_scenario
if( $ARGV[0] ne "-o" ) {
    print $USAGE; die "-o expected for argt 1";
}
my $output_scenario_name = $ARGV[1];
# input_scenario
if( $ARGV[2] ne "-i" ) {
    print $USAGE; die "-i expected for argt 3";
}
my $input_scenario_name = $ARGV[3];
# mapping file
if( $ARGV[4] ne "-m" ) {
    print $USAGE; die "-m expected for argt 5";
}
my $mapping_file_name = $ARGV[5];


my $FILEout;
my $FILEin;
my $FILEMapping;
print "Generating file ", $output_scenario_name,"\n";
open(  $FILEout , ">$output_scenario_name" ) 
	|| die "Can't open file $output_scenario_name!";
print "Reading scenario file ", $input_scenario_name,"\n";
open($FILEin , "<$input_scenario_name") 
	|| die "Can't open file $input_scenario_name!";
print "Reading mapping file ", $mapping_file_name,"\n";
open($FILEMapping , "<$mapping_file_name") 
	|| die "Can't open file $mapping_file_name!";

# storing the mapping in hash tables

my $new_line;
$new_line = <$FILEMapping>;
chomp($new_line);
my @Mapping_Old_IDs = (split /\t/, $new_line);
$new_line = <$FILEMapping>;
chomp($new_line);
my @Mapping_New_IDs = (split /\t/, $new_line);
$new_line = <$FILEMapping>;
chomp($new_line);
my @Mapping_NewFromOld_IDs = (split /\t/, $new_line);
$new_line = <$FILEMapping>;
chomp($new_line);
my @Mapping_Pulsing_Modes = (split /\t/, $new_line);
$new_line = <$FILEMapping>;
chomp($new_line);
my @Mapping_Abandoned_IDs = (split /\t/, $new_line);
close($FILEMapping);

#hash table of old ids column in old scenario
my %Old_IDs_column;
(shift @Mapping_Old_IDs);
foreach my $ID (@Mapping_Old_IDs) {
	$Old_IDs_column{$ID} = -1;
}
#hash table of abandonned ids
my %Mapping_Abandoned_IDs;
(shift @Mapping_Abandoned_IDs);
foreach my $ID (@Mapping_Abandoned_IDs) {
	$Mapping_Abandoned_IDs{$ID} = 1;
}
#hash table of new ids origin (from old IDs)
my %Mapping_NewFromOld_IDs;
(shift @Mapping_New_IDs);
(shift @Mapping_NewFromOld_IDs);
for my $i (0..$#Mapping_New_IDs) {
   	my $New_ID = $Mapping_New_IDs[$i];
   	my $FromOld_ID = $Mapping_NewFromOld_IDs[$i];
	$Mapping_NewFromOld_IDs{$New_ID} = $FromOld_ID;
	# print "New $New_ID old $FromOld_ID\n";
}
# inserts ID_pulse besides ID in the new IDs table in case the variable is pulsed
# searches for old_ID_pulse as previous pulsed values, 
# otherwise, creates a new pulsed variable with O values
# possibly replaces previous pulsed values equal to 1 with 0
(shift @Mapping_Pulsing_Modes);
my @Copy_of_Mapping_New_IDs = @Mapping_New_IDs;
my @Copy_of_Mapping_NewFromOld_IDs = @Mapping_NewFromOld_IDs;
my @Copy_of_Mapping_Pulsing_Modes =  @Mapping_Pulsing_Modes;
@Mapping_New_IDs = ();
@Mapping_NewFromOld_IDs = ();
@Mapping_Pulsing_Modes = ();
for my $i_column (0..$#Copy_of_Mapping_New_IDs) {
	my $new_ID_string = $Copy_of_Mapping_New_IDs[$i_column];
	my $old_ID_string = $Copy_of_Mapping_NewFromOld_IDs[$i_column];
	my $pulsed_ID_string = $Copy_of_Mapping_Pulsing_Modes[$i_column];

	push @Mapping_New_IDs, $new_ID_string;
	push @Mapping_NewFromOld_IDs, $old_ID_string;
	push @Mapping_Pulsing_Modes, $pulsed_ID_string;
	# adds pulsed variable
	if( !($pulsed_ID_string eq "*") ) {
		my $pulsed_new_ID_string = $new_ID_string . "_pulse";
		push @Mapping_New_IDs, $pulsed_new_ID_string;
		push @Mapping_Pulsing_Modes, "*";

		# looks for an old pulsed variable
		# first looks for old variable name
		if(!($old_ID_string eq "NEW_float"
			|| $old_ID_string eq "NEW_bool"
			|| $old_ID_string eq "NEW_int")) {
			# builds old pulsed name
			my $pulsed_old_ID_string = $old_ID_string . "_pulse";
			# old pulsed name found in old scenario
	   		if(exists $Old_IDs_column{$pulsed_old_ID_string}) {
				$Mapping_NewFromOld_IDs{$pulsed_new_ID_string} = $pulsed_old_ID_string;
				push @Mapping_NewFromOld_IDs, $pulsed_old_ID_string;
	   		}
			# new pulsed name found in old scenario
	   		elsif(exists $Old_IDs_column{$pulsed_new_ID_string}) {
				$Mapping_NewFromOld_IDs{$pulsed_new_ID_string} = $pulsed_new_ID_string;
				push @Mapping_NewFromOld_IDs, $pulsed_new_ID_string;
	   		}
	   		else {
				$Mapping_NewFromOld_IDs{$pulsed_new_ID_string} = "NEW_float";
				push @Mapping_NewFromOld_IDs, "NEW_float";
	   		}
	   	}
   		else {
			$Mapping_NewFromOld_IDs{$pulsed_new_ID_string} = "NEW_float";
			push @Mapping_NewFromOld_IDs, "NEW_float";
   		}
	}
}

# storing the lines of the old scenario
my $line;
my @scenario_lines = ();
my $line_index = 0;
my @scenario_IDs;
my $scenario_nb_scenes;
my $scenes_first_line_index = 16;
my $scenes_last_line_index = 0;
while($line = <$FILEin>){
	chomp($line);
	$scenario_lines[$line_index] = $line;
	# IDs
	if($line_index == 3) {
		@scenario_IDs = (split /\t/, $line);
		#stores the column index of the scenario IDs
		for my $i (1..$#scenario_IDs) {
		   	my $Old_ID = $scenario_IDs[$i];
		   	if(exists $Old_IDs_column{$Old_ID}) {
		   		$Old_IDs_column{$Old_ID} = $i;
		   	}
		   	elsif(exists $Mapping_Abandoned_IDs{$Old_ID}) {
		   		# nothing to do, the old ID is purposely not listed in the new ones
		   	}
		   	else {
		   		die "Scenario analysis: Old ID in scenario $Old_ID not found in mapping file (whether Old or Abandoned ID)!";
		   	}
		}
	}
	# the scenes range from  $scenes_first_line_index to  $scenes_last_line_index
	# both indices are included
	# in addition there is the opening and closing tag scenario before and after these lines
	elsif($line_index == $scenes_first_line_index) {
		$scenario_nb_scenes = (split /\t/, $line)[1];
		$scenes_last_line_index 
			= $scenes_first_line_index + $scenario_nb_scenes * 7;
	}
	$line_index++;
}
close($FILEin);

# checks whether all the old IDs in the mapping file were found in the initial scenario
foreach my $key (keys %Old_IDs_column) {
   	if($Old_IDs_column{$key} == -1) {
   		die "Old ID in mapping file $key not found in scenario file!";
   	}
}


# using the mapping to generate the new file from the old one
# HEADER LINES
my @floatPrototype = ("0", "ID comment", "float", "ID", "NULL", "ID", "*", "r", "R", "-1", "1", "0", "9999", "", "0", "", "");
my @boolPrototype = ("0", "ID comment", "bool", "ID", "NULL", "ID", "*", "*", "*", "0", "0", "0", "1", "", "0", "", "");
my @intPrototype = ("0", "ID comment", "int", "ID", "NULL", "ID", "*", "*", "0", "-1", "1", "0", "1000", "", "0", "", "");
for my $i_line (0..($scenes_first_line_index - 1)) {
	# the lines with opening and closing tag initial_values are copied
	if($i_line == ($scenes_first_line_index - 1)
		|| $i_line == ($scenes_first_line_index - 3)) {
		print $FILEout $scenario_lines[$i_line], "\n";
	}
	# the lines with STEP- STEP+ MIN_OR_EQUAL MAX_OR_EQUAL are discarded
	# a new line with the possible pulsation of the variable is written
	elsif($i_line == ($scenes_first_line_index - 4)
		|| $i_line == ($scenes_first_line_index - 5)
		|| $i_line == ($scenes_first_line_index - 6)
		|| $i_line == ($scenes_first_line_index - 7)
		|| $i_line == ($scenes_first_line_index - 8)
		|| $i_line == ($scenes_first_line_index - 9)) {
		if($i_line == ($scenes_first_line_index - 4)) {
			# first column
			print $FILEout "PULSE";
			for my $i_column (0..$#Mapping_New_IDs) {
				print $FILEout "\t" , $Mapping_Pulsing_Modes[$i_column];
			}
			print $FILEout "\n";
		}
	}
	# writes the values 
	else {
		# first column
		print $FILEout (split /\t/, $scenario_lines[$i_line])[0];
		if($i_line == 0) {
			for my $i_column (0..$#Mapping_New_IDs) {
				print $FILEout "\t" , $i_column;
			}
		}
		elsif($i_line == 3 || $i_line == 5) {
			for my $i_column (0..$#Mapping_New_IDs) {
				print $FILEout "\t" , $Mapping_New_IDs[$i_column];
			}
		}
		else {
			for my $i_column (0..$#Mapping_New_IDs) {
				my $new_ID = $Mapping_New_IDs[$i_column];
				my $old_ID = $Mapping_NewFromOld_IDs[$i_column];

				if($old_ID eq "NEW_float") {
					# ships video hue arts to Update_fs
					if($i_line == 6 && ($new_ID eq "video_hue")) {
						print $FILEout "\t" , "Update_fs";
					}
					elsif($i_line == 6 && ($new_ID eq "video_value")) {
						print $FILEout "\t" , "Update_fs";
					}
					elsif($i_line == 6 && ($new_ID eq "photo_hue")) {
						print $FILEout "\t" , "Update_fs";
					}
					else {
						print $FILEout "\t" , $floatPrototype[$i_line];
					}
				}
				elsif($old_ID eq "NEW_bool") {
					print $FILEout "\t" , $boolPrototype[$i_line];
				}
				elsif($old_ID eq "NEW_int") {
					print $FILEout "\t" , $intPrototype[$i_line];
				}
				elsif(exists $Old_IDs_column{$old_ID}) {
					my $old_scenario_column = $Old_IDs_column{$old_ID};
					my $cell_string = (split /\t/, $scenario_lines[$i_line])[$old_scenario_column];
					# substitutes new ID in callBack name
					if($i_line == 4 && ($cell_string =~ m/$old_ID/)) {
						$cell_string =~ s/$old_ID/$new_ID/;
					}
					# part_size is used by ParticleSplat or ParticleCurve shader and ParticleAnimation_fs
					if($i_line == 6 && ($new_ID eq "part_size")) {
						$cell_string = "ParticleAnimation_fs";
					}
					# ships part_acc to ParticleAnimation_fs
					if($i_line == 6 && ($new_ID eq "repop_part")) {
						$cell_string = "ParticleAnimation_fs";
					}
					# ships part_acc to ParticleAnimation_fs
					if($i_line == 6 && ($new_ID eq "repop_path")) {
						$cell_string = "ParticleAnimation_fs";
					}
					# ships part_acc to ParticleAnimation_fs
					if($i_line == 6 && ($new_ID eq "part_acc")) {
						$cell_string = "ParticleAnimation_fs";
					}
					# ships part_damp to ParticleAnimation_fs
					if($i_line == 6 && ($new_ID eq "part_damp")) {
						$cell_string = "ParticleAnimation_fs";
					}
					# ships pixel_radius to Update_fs
					if($i_line == 6 && ($new_ID eq "pixel_radius")) {
						$cell_string = "Update_fs";
					}
					# ships active clip arts to Update_fs
					if($i_line == 6 && ($new_ID eq "activeClipArts")) {
						$cell_string = "Update_fs";
					}
					# ships active photoWeight to Update_fs
					if($i_line == 6 && ($new_ID eq "photoWeight")) {
						$cell_string = "Update_fs";
					}
					# ships CADecay to Update_fs
					if($i_line == 6 && ($new_ID eq "CAdecay")) {
						$cell_string = "Update_fs";
					}
					# does not ships CADecay_pulse to Update_fs
					if($i_line == 6 && ($new_ID eq "CAdecay_pulse")) {
						$cell_string = "*";
					}
					# ships partDecay to ParticleAnimation_fs
					if($i_line == 6 && ($new_ID eq "partDecay")) {
						$cell_string = "ParticleAnimation_fs";
					}
					# ships trkDecay_0, 1, 2, 3 to Update_fs
					if($i_line == 6 && ($new_ID eq "trkDecay_0" || $new_ID eq "trkDecay_1" 
						                || $new_ID eq "trkDecay_2" || $new_ID eq "trkDecay_3")) {
						$cell_string = "Update_fs";
					}
					# CA type processed in CPU
					if($i_line == 6 && ($new_ID eq "CA1Type" || $new_ID eq "CA1SubType")) {
						$cell_string = "*";
					}
					print $FILEout "\t" , $cell_string;
				}
			   	elsif(exists $Mapping_Abandoned_IDs{$old_ID}) {
			   		# nothing to do, the old ID is purposely not listed in the new ones
			   	}
				else {
			   		die "New scenario generation: Old ID in mapping $old_ID not found in scenario file, should be removed from mapping!";
				}
			}
		}
		print $FILEout "\n";
	}
}

# SCENE LINES
# prints scenario \t number of scenes
print $FILEout $scenario_lines[$scenes_first_line_index], "\n";
my @scenePrototype = ("0", "0", "s");
my $scene_name;
for my $i_line (($scenes_first_line_index + 1)..$scenes_last_line_index) {
	# the line with opening tag scene, scene name and duration
	if(($i_line - $scenes_first_line_index - 1) % 7 == 0) {
		print $FILEout $scenario_lines[$i_line], "\n";
		$scene_name = (split /\t/, $scenario_lines[$i_line])[1];
	}
	# the lines with closing tags scene
	elsif(($i_line - $scenes_first_line_index - 1) % 7 == 6) {
		print $FILEout $scenario_lines[$i_line], "\n";
	}
	# the line with repeated scene name
	elsif(($i_line - $scenes_first_line_index - 1) % 7 == 1) {
		# first column
		print $FILEout $scene_name;
		for my $i_column (0..$#Mapping_New_IDs) {
			print $FILEout "\t" , $scene_name;
		}
		print $FILEout "\n";
	}
	# the line with variable IDs
	elsif(($i_line - $scenes_first_line_index - 1) % 7 == 2) {
		# first column
		print $FILEout (split /\t/, $scenario_lines[$i_line])[0];
		for my $i_column (0..$#Mapping_New_IDs) {
			print $FILEout "\t" , $Mapping_New_IDs[$i_column];
		}
		print $FILEout "\n";
	}
	# writes the values and interpolation mode (0,0,s for new variables)
	else {
		# first column
		print $FILEout (split /\t/, $scenario_lines[$i_line])[0];
		for my $i_column (0..$#Mapping_New_IDs) {
			my $new_ID = $Mapping_New_IDs[$i_column];
			my $old_ID = $Mapping_NewFromOld_IDs[$i_column];

			if($old_ID eq "NEW_float" || $old_ID eq "NEW_bool" || $old_ID eq "NEW_int") {
				print $FILEout "\t" , $scenePrototype[(($i_line - $scenes_first_line_index - 1) % 7) - 3];
			}
			elsif(exists $Old_IDs_column{$old_ID}) {
				my $old_scenario_column = $Old_IDs_column{$old_ID};
				my $cell_string = (split /\t/, $scenario_lines[$i_line])[$old_scenario_column];
				if(($i_line - $scenes_first_line_index - 1) % 7 == 3
					|| ($i_line - $scenes_first_line_index - 1) % 7 == 4) {
					# integrates decay sign into decay value
					if($old_ID eq "CAdecay" || $old_ID eq "partDecay") {
						if(exists $Old_IDs_column{$old_ID."_sign"}) {
							$old_scenario_column = $Old_IDs_column{$old_ID."_sign"};
							if($old_scenario_column >= 0) {
								$cell_string *= (split /\t/, $scenario_lines[$i_line])[$old_scenario_column];
							}
						}
					}
					# CAdecay_pulse value is made negative
					elsif($old_ID eq "CAdecay_pulse") {
						if(exists $Old_IDs_column{$old_ID}) {
							$cell_string *= -1;
						}
					}
					elsif($old_ID =~ m/trkDecay_([0-9])/) {
						if(exists $Old_IDs_column{"trkDecay_sign_$1"}) {
							$old_scenario_column = $Old_IDs_column{"trkDecay_sign_$1"};
							if($old_scenario_column >= 0) {
								$cell_string *= (split /\t/, $scenario_lines[$i_line])[$old_scenario_column];
							}
						}
					}
					elsif($new_ID eq "track_x_transl_0_pulse"
						 || $new_ID eq "track_x_transl_1_pulse") {
						if(exists $Old_IDs_column{"tracks_Hshift_pulse"}) {
							$old_scenario_column = $Old_IDs_column{"tracks_Hshift_pulse"};
							if($old_scenario_column >= 0) {
								$cell_string = (split /\t/, $scenario_lines[$i_line])[$old_scenario_column];
							}
						}
					}
				}
				print $FILEout "\t" , $cell_string;
			}
		   	elsif(exists $Mapping_Abandoned_IDs{$old_ID}) {
		   		# nothing to do, the old ID is purposely not listed in the new ones
		   	}
			else {
		   		die "New scenario generation: Old ID in scenario $old_ID not found in mapping file (whether Old or Abandoned ID)!";
			}
		}
		print $FILEout "\n";
	}
	# the column of the pulsed variable should be added there
}
print $FILEout $scenario_lines[$scenes_last_line_index + 1], "\n";

# FOOTER LINES
for my $i_line (($scenes_last_line_index + 2)..$#scenario_lines) {
	print $FILEout $scenario_lines[$i_line], "\n";
}
close($FILEout);
