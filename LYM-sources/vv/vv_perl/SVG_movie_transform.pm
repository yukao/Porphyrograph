#!/usr/bin/perl 

use strict;
use warnings;

use POSIX;

use Math::Vector::Real;
use Math::Trig;
use List::Util qw[min max];

package SVG_movie_transform;

# SVG_movie_transform.pm
# paths transformations in a SVG file

# usage
# SVG_movie_transform.pm --format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

my $USAGE =
"usage: SVG_movie_transform.pm --nb-layers <integer> -o <file_name> -i <file_name> --transform <single_transf>
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pm --nb-layers 4 -o mer.svg -i mer.svg --transformation COPY --percent 0.7 --translation

ARGUMENTS:
   --nb_layers: number of layers for posterization
   -o: ouput files 
   -i: input files
   --transformation: transformation function
   --percent: percentage of transformation
optional
   --translation: x, y coordinates of translation
";

##################################################################
# MAIN SUB
##################################################################
my $nb_layers;
my $percent;
my $FILE_log;
sub MAIN {
	my $numArgs;
	$numArgs = @_ + 1;

	my $argnum;
	my $output_format;
	my $output_file_name;
	my $input_file_name;
	# my $second_input_file_name;
	my $single_transf;
	my $transf_string;
	my @single_transf_strings;
	my @single_transfs;

	use constant {
	    COPY => 1,
	    V_SCALE => 2,
	    V_SCALE2 => 3,
	    V_SCALE3 => 4,
	    C_SCALE => 5,
	    C_SCALE2 => 6,
	    VH_RANDOM => 7,
	    V_FLATTEN => 8,
	    SMOOTHING => 9,
	    BLUR => 10,
	    REFRAME_TOP_LEFT => 11,
	    COPY_TOP_LEFT => 12,
	    C_LINES => 13,
	    TRANSLATE => 14,
	    ROTATE => 15,
	   	SCALE => 16,
	   	H_SYMMETRY => 17,
	   	V_SYMMETRY => 18,
	   	BEZIER_CURVE => 19,
	   	ARCHIMEDE_SPIRAL => 20,
	   	C_POLYGONS => 21,

	    SVG_PAGE_WIDTH => 38400,
		SVG_PAGE_HEIGHT => 21600,
	};
	my %single_transf_hash = (
	   "COPY" => COPY,
	   "V_SCALE" => V_SCALE,
	   "V_SCALE2" => V_SCALE2,
	   "V_SCALE3" => V_SCALE3,
	   "C_SCALE" => C_SCALE,
	   "C_SCALE2" => C_SCALE2,
	   "VH_RANDOM" => VH_RANDOM,
	   "V_FLATTEN" => V_FLATTEN,
	   "SMOOTHING" => SMOOTHING,
	   "BLUR" => BLUR,
	   "REFRAME_TOP_LEFT" => REFRAME_TOP_LEFT,
	   "COPY_TOP_LEFT" => COPY_TOP_LEFT,
	   "C_LINES" => C_LINES,
	   "TRANSLATE" => TRANSLATE,
	   "ROTATE" => ROTATE,
	   "SCALE" => SCALE,
	   "H_SYMMETRY" => H_SYMMETRY,
	   "V_SYMMETRY" => V_SYMMETRY,
	   "BEZIER_CURVE" => BEZIER_CURVE,
	   "ARCHIMEDE_SPIRAL" => ARCHIMEDE_SPIRAL,
	   "C_POLYGONS" => C_POLYGONS,
	);

	##################################################################
	# ARGUMENTS OF THE COMMAND
	##################################################################

	if( 0+@_ != 10 && 0+@_ != 13 && 0+@_ != 14 && 0+@_ != 19 && 0+@_ != 16) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; print"SVG_movie_transform.pm: expected number of arguments: 10, 13, 14 16 or 19 not ", 0+@_, "\n" ;
	    return 0;
	}

	# nb_layers
	if( $_[0] ne "--nb-layers" ) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; print"SVG_movie_transform.pm: --nb-layers expected for argt 1\n";
	    return 0;
	}
	if ($_[1] < 1) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; print "SVG_movie_transform.pm: ", $_[1]," 1 layer should be produced minimally\n";
	    return 0;
	}
	$nb_layers = $_[1];

	# output_file_name_PATHs
	if( $_[2] ne "-o" ) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; print"SVG_movie_transform.pm: -o expected for argt 3\n";
	    return 0;
	}
	$output_file_name = $_[3];

	# input_file_names
	if( $_[4] ne "-i" ) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; print"SVG_movie_transform.pm: -i expected for argt 5\n";
	    return 0;
	}
	$input_file_name = $_[5];

	#  SVG path transformation
	if( $_[6] ne "--transformation" ) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; print"SVG_movie_transform.pm: --transformation expected for argt 7\n";
	    return 0;
	}
	$transf_string = $_[7];
	@single_transf_strings = split(/\//, $transf_string);
	my $single_transf_string;
	foreach $single_transf_string (@single_transf_strings) {
		if (!$single_transf_hash{$single_transf_string}) {
		    print "Arguments: ", join(' ',@_), "\n";
		    print $USAGE; print $single_transf_string," does not exist in the hash %start_step\n";
		    return 0;
		}
		push @single_transfs, $single_transf_hash{$single_transf_string};
	}

	#  SVG path transformation percentage
	if( $_[8] ne "--percent" ) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; print"SVG_movie_transform.pm: --percent expected for argt 9\n";
	    return 0;
	}
	$percent = $_[9];
	$percent = ($percent < 0? 0: $percent);
	$percent = ($percent > 1? 1: $percent);

	my $x_absolute_translation = 0;
	my $y_absolute_translation = 0;
	my $x_absolute_rotation_center = 0;
	my $y_absolute_rotation_center = 0;
	my $rotation_angle = 0;
	my $x_absolute_scaling_center = 0;
	my $y_absolute_scaling_center = 0;
	my $scaling_factor = 0;
	my ($b0_x,$b0_y,$b1_x,$b1_y,$b2_x,$b2_y,$b3_x,$b3_y);
	my $spiral_center_x;
	my $spiral_center_y;
	my $spiral_radius_step;
	my $spiral_angle_step;
	my $spiral_nb_steps;
	# SVG path translation
	if(0+@_ == 13 && $single_transfs[0] == TRANSLATE) {
		if($_[10] eq "--translation") {
			$x_absolute_translation = $_[11];
			$y_absolute_translation = $_[12];
		}
		else {
		    print "Arguments: ", join(' ',@_), "\n";
		    print $USAGE; print"SVG_movie_transform.pm: --translation expected for argt 11\n";
		    return 0;
		}
	}
	# SVG path rotation
	elsif(0+@_ == 14 && $single_transfs[0] == ROTATE) {
		if($_[10] eq "--rotation") {
			$x_absolute_rotation_center = $_[11];
			$y_absolute_rotation_center = $_[12];
			$rotation_angle = $_[13];
		}
		else {
		    print "Arguments: ", join(' ',@_), "\n";
		    print $USAGE; print"SVG_movie_transform.pm: --rotation expected for argt 11\n";
		    return 0;
		}
	}
	# SVG path scaling
	elsif(0+@_ == 14 && $single_transfs[0] == SCALE) {
		if($_[10] eq "--scale") {
			$x_absolute_scaling_center = $_[11];
			$y_absolute_scaling_center = $_[12];
			$scaling_factor = $_[13];
		}
		else {
		    print "Arguments: ", join(' ',@_), "\n";
		    print $USAGE; print"SVG_movie_transform.pm: --rotation expected for argt 11\n";
		    return 0;
		}
	}
	# SVG path control points
	elsif(0+@_ == 19 && $single_transfs[0] == BEZIER_CURVE) {
		if($_[10] eq "--bezier_curve") {
			$b0_x = $_[11];
			$b0_y = $_[12];
			$b1_x = $_[13];
			$b1_y = $_[14];
			$b2_x = $_[15];
			$b2_y = $_[16];
			$b3_x = $_[17];
			$b3_y = $_[18];
		}
		else {
		    print "Arguments: ", join(' ',@_), "\n";
		    print $USAGE; print"SVG_movie_transform.pm: --bezier_curve expected for argt 11\n";
		    return 0;
		}
	}
	# spiral parameters
	elsif(0+@_ == 16 && $single_transfs[0] == ARCHIMEDE_SPIRAL) {
		if($_[10] eq "--archimede_spiral") {
			$spiral_center_x = $_[11];
			$spiral_center_y = $_[12];
			$spiral_radius_step = $_[13];
			$spiral_angle_step = $_[14];
			$spiral_nb_steps = $_[15];
		}
		else {
		    print "Arguments: ", join(' ',@_), "\n";
		    print $USAGE; print"SVG_movie_transform.pm: --archimede_spiral expected for argt 11\n";
		    return 0;
		}
	}
	# SVG path control points
	elsif(0+@_ != 10) {
	    print "Arguments: ", join(' ',@_), "\n";
	    print $USAGE; print"SVG_movie_transform.pm: 10 arguments expected for transformation " . $single_transf_strings[0] . "²\n";
	    return 0;
	}

	##################################################################
	##################################################################
	# MAIN PROGRAM
	##################################################################
	##################################################################

	my $FILEout;
	my $FILEin;
	# my $second_FILEin;
	if( open(  $FILEout , ">$output_file_name" ) == 0) {
		print "Can't open output file $output_file_name for transformation $transf_string!\n";
	    return 0;
	}
	if( open($FILEin , "<".$input_file_name) == 0) {
		print "Can't open input file $input_file_name for transformation $transf_string!\n";
	    return 0;
	}

	my $line;
	my $path_data_string;
	my @path_data;
	my $path_data_size;
	# my $second_path_data_string;
	# my @second_path_data;
	# my $second_path_data_size;
	##################################################################
	# STORES THE LINES IN AN ARRAY
	##################################################################
	my @Unormalized_SVG_file_lines = ();
	while($line = <$FILEin>){
		chomp($line);
		push @Unormalized_SVG_file_lines , $line;
	}

	##################################################################
	# NORMALIZES THE SVG FILE BY HAVING TAGS ON A SINGLE LINE
	##################################################################
	my @SVG_file_lines = ();
	my $ind_line = 0;
	while($ind_line < (@Unormalized_SVG_file_lines + 0)) {
		$line = $Unormalized_SVG_file_lines[$ind_line++];
		if( $line =~ /<path/ ) {
			$line =~ s/^[\s]+<path/<path/;
			while(!($line =~/\/>/)) {
				$line = $line . $Unormalized_SVG_file_lines[$ind_line++];
			}
			# transforms , into space
			$line =~ s/\,/ /g;
			# only keeps the path data
			$line =~ s/<path.*\sd="([^\"]*)\".*\/>/<path d=\"$1\" \/>/g;
		}
		elsif( $line =~ /<g/ ) {
			$line =~ s/^[\s]+<g/<g/;
			while(!($line =~/>/)) {
				$line = $line . $Unormalized_SVG_file_lines[$ind_line++];
			}
		}
		push @SVG_file_lines , $line;
	}

	##################################################################
	# MAKES ABSOLUTE COORDINATES
	##################################################################
	my $path_no = 0;
	my @absoluteCoords_SVG_file_lines = ();
	foreach $line (@SVG_file_lines) {
		##################################################################
		# reads the path and transform it into absolute coordinates
		##################################################################
		if( $line =~ /^<path/ ) {		
			if($line =~ /d\=\"(.*)\"/) {
				##################################################################
				# transforms relative coordinates of base video into absolute ones
				##################################################################
				$path_data_string = $1;
				# adds space after move or curve and before z
				$path_data_string =~ s/([MmCcLlHhVv])/$1 /g;
				$path_data_string =~ s/([Zz])/ $1 /g;
				$line = relative_to_absolute_coordinates($path_data_string);
			}
		}
		push @absoluteCoords_SVG_file_lines , $line;
		$path_no++;
	}

	##################################################################
	# ITERATES OVER ALL THE TRANSFORMATIONS FOR EACH PATH
	##################################################################
	my @transformed_SVG_file_lines = ();
	my $nb_transformations = 0;
	my $path_beam_data_string;
	foreach $single_transf (@single_transfs) {
		# copies the output of the preceding transformation
		if($nb_transformations > 0) {
			@absoluteCoords_SVG_file_lines = @transformed_SVG_file_lines;
		}
		if($single_transf == COPY_TOP_LEFT) {
			my $log_file_name = $input_file_name;
			$log_file_name =~ s/_[0-9]+\.svg/_log.csv/;
			if( open( $FILE_log , ">>$log_file_name" ) == 0) {
				print "Can't open log file $log_file_name!\n";
			    return 0;
			}
		}

		# processes the current transformation
		$path_no = 0;
		@transformed_SVG_file_lines = ();
		foreach $line (@absoluteCoords_SVG_file_lines) {
			##################################################################
			# appplies the transformation on the path
			##################################################################
			if( $line =~ /^<path/ ) {		
				if($line =~ /d\=\"(.*)\"/) {
					$path_data_string = $1;

					##################################################################
					# COPY TRANSFORMATION
					##################################################################
					if($single_transf == COPY) {
						# nothing to be done
						$path_data_string = $line;
					}
					##################################################################
					# COPY TRANSFORMATION AND STORES TOP LEFT
					##################################################################
					elsif($single_transf == COPY_TOP_LEFT) {
						# nothing to be done
						$path_data_string = $line;
						COPY_TOP_LEFT_transformation();
					}
					##################################################################
					# VERTICAL SCALING (WITH AS MANY LEVELS AS LAYERS)
					##################################################################
					elsif($single_transf == V_SCALE) {
						$path_data_string = V_SCALE_transformation($path_data_string, $path_no);
					}
					##################################################################
					# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS)
					##################################################################
					elsif($single_transf == V_SCALE2) {
						$path_data_string = V_SCALE2_transformation($path_data_string, $path_no);
					}
					##################################################################
					# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
					# similar to V_SCALE2 but with double length for the first part wrt the second part
					##################################################################
					elsif($single_transf == V_SCALE3) {
						$path_data_string = V_SCALE3_transformation($path_data_string, $path_no);
					}
					##################################################################
					# CENTRAL SCALING (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
					##################################################################
					elsif($single_transf == C_SCALE) {
						$path_data_string = C_SCALE_transformation($path_data_string, $path_no);
					}
					##################################################################
					# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
					##################################################################
					elsif($single_transf == C_SCALE2) {
						$path_data_string = C_SCALE2_transformation($path_data_string, $path_no);
					}
					##################################################################
					# ALL DIRECTIONS RANDOMNESS
					##################################################################
					elsif($single_transf == VH_RANDOM) {
						$path_data_string = VH_RANDOM_transformation($path_data_string, $path_no);
					}
					##################################################################
					# VERTICAL FLATNESS
					##################################################################
					elsif($single_transf == V_FLATTEN) {
						$path_data_string = V_FLATTEN_transformation($path_data_string, $path_no);
					}
					##################################################################
					# SMOOTHING AND/OR SIMPLIFICATION OF THE BEZIER CURVES
					##################################################################
					elsif($single_transf == SMOOTHING
						|| $single_transf == BLUR) {
						$path_data_string = V_FLATTEN_or_BLUR_transformation($path_data_string, $path_no, $single_transf);
					}
					##################################################################
					# PLACES THE TOP LEFT POINT OF THE DRAWING AT THE TOP LEFT POINT OF THE PAGE
					##################################################################
					elsif($single_transf == REFRAME_TOP_LEFT) {
						$path_data_string = REFRAME_TOP_LEFT_transformation($path_data_string, $path_no);
					}
					##################################################################
					# ADDS RAYS BETWEEN THE POINTS AND THE CENTER OF THE IMAGE
					##################################################################
					elsif($single_transf == C_LINES) {
						($path_data_string, $path_beam_data_string)
							= C_LINES_transformation($path_data_string, $path_no);
						# print "************ Beam path $path_beam_data_string\n";
					}
					##################################################################
					# TRANSLATES THE FULL IMAGE
					##################################################################
					elsif($single_transf == TRANSLATE) {
						$path_data_string = TRANSLATE_transformation($path_data_string, $path_no, 
							$x_absolute_translation, $y_absolute_translation);
					}
					##################################################################
					# ROTATES THE FULL IMAGE
					##################################################################
					elsif($single_transf == ROTATE) {
						$path_data_string = ROTATE_transformation($path_data_string, $path_no,  
							$x_absolute_rotation_center, $y_absolute_rotation_center, $rotation_angle);
					}
					##################################################################
					# SCALES THE FULL IMAGE
					##################################################################
					elsif($single_transf == SCALE) {
						$path_data_string = SCALE_transformation($path_data_string, $path_no,  
							$x_absolute_scaling_center, $y_absolute_scaling_center, $scaling_factor);
					}
					##################################################################
					# SYMMETRIES
					##################################################################
					elsif($single_transf == H_SYMMETRY) {
						$path_data_string = SYMMETRY_transformation($path_data_string, $path_no,  
							SVG_PAGE_WIDTH/2, -1);
					}
					elsif($single_transf == V_SYMMETRY) {
						$path_data_string = SYMMETRY_transformation($path_data_string, $path_no,  
							-1, SVG_PAGE_HEIGHT/2);
					}
					##################################################################
					# WEB LINE
					##################################################################
					elsif($single_transf == BEZIER_CURVE) {
						$path_data_string = "<path d=\"M $b0_x $b0_y C $b1_x $b1_y $b2_x $b2_y $b3_x $b3_y\" id=\"path116\" style=\"stroke:#000000; stroke-width:20;stroke-opacity:1\"/>";
					}
					##################################################################
					# SPIRAL
					##################################################################
					elsif($single_transf == ARCHIMEDE_SPIRAL) {
						my $x = $spiral_center_x;
						my $y = $spiral_center_y;
						$path_data_string = "<path d=\"M $spiral_center_x $spiral_center_y ";
						for(my $i = 0; $i < $spiral_nb_steps; $i++){
							my $angle = $i * $spiral_angle_step;
							$x = $spiral_center_x + ($spiral_radius_step * ($angle/360)) * cos(Math::Trig::deg2rad($angle));
							$y = $spiral_center_y + ($spiral_radius_step * ($angle/360)) * sin(Math::Trig::deg2rad($angle));
							$path_data_string = $path_data_string . "L $x $y ";
						}
						$path_data_string = $path_data_string . "\" id=\"path116\" style=\"stroke:#000000; stroke-width:20;stroke-opacity:1\"/>";
					}
					##################################################################
					# CONCENTRIC POLYGONES
					##################################################################
					elsif($single_transf == C_POLYGONS) {
						my $x = $spiral_center_x;
						my $y = $spiral_center_y;
						my $nb_steps_per_polygon = int(360/$spiral_angle_step);
						$path_data_string = "<path d=\"M $spiral_center_x $spiral_center_y ";
						for(my $i = 0; $i < $spiral_nb_steps; $i++){
							my $angle = $i * $spiral_angle_step;
							$x = $spiral_center_x + ($spiral_radius_step * (int(POSIX::floor($angle/360))+1)) * cos(Math::Trig::deg2rad($angle));
							$y = $spiral_center_y + ($spiral_radius_step * (int(POSIX::floor($angle/360))+1)) * sin(Math::Trig::deg2rad($angle));
							$path_data_string = $path_data_string . "L $x $y ";
							# closes the polygon
							if($i % $nb_steps_per_polygon == $nb_steps_per_polygon - 1) {
								$x = $spiral_center_x + ($spiral_radius_step * (int(POSIX::floor($angle/360))+1)) * cos(Math::Trig::deg2rad($angle+$spiral_angle_step));
								$y = $spiral_center_y + ($spiral_radius_step * (int(POSIX::floor($angle/360))+1)) * sin(Math::Trig::deg2rad($angle+$spiral_angle_step));
								$path_data_string = $path_data_string . "L $x $y ";
							}
						}
						$path_data_string = $path_data_string . "\" id=\"path116\" style=\"stroke:#000000; stroke-width:20;stroke-opacity:1\"/>";
					}
					##################################################################
					# UNKNOWN TRANSFORMATION
					##################################################################
					else {
						print "Unknown transformation ", $single_transf, "\n";
					    return 0;
					}
					# prints the modified path
					push @transformed_SVG_file_lines , $path_data_string;
				}
				else {
					print "Path data not found in path ", $line, "\n";
				    return 0;
				}
				$path_no++;
			}
			##########################################################################
			# non path line reproduced
			##########################################################################
			else {
				##################################################################
				# stores the lines produced by C_LINES transformation
				##################################################################
				if( $line =~ /<\/svg>/ && defined $path_beam_data_string) {
					push @transformed_SVG_file_lines , "<g transform=\"translate(0.000000,2160.000000) scale(0.100000,-0.100000)\" style=\"stroke:url(#radialGradient001);stroke-opacity:1\">";
					push @transformed_SVG_file_lines , $path_beam_data_string;
					push @transformed_SVG_file_lines , "</g>";
				}

				# reproducing line
				push @transformed_SVG_file_lines , $line;
			}
		}

		if($single_transf == COPY_TOP_LEFT) {
			close( $FILE_log );
		}
		$nb_transformations++;
	}

	##########################################################################
	# COPIES THE LINES IN THE OUTPUT FILE
	##########################################################################
	foreach $line (@transformed_SVG_file_lines) {
		print $FILEout $line, "\n";
	}

	close($FILEin);
	close($FILEout);
	return 1;
}

##################################################################
# TRANSFORMS RELATIVE COORDINATES OF A SVG PATH INTO ABSOLUTE ONES
##################################################################
my $min_abs_X;
my $min_abs_Y;
my $max_abs_X;
my $max_abs_Y;
my $absolute_current_point_X;
my $absolute_current_point_Y;
sub minMaxUpdate {
	$min_abs_X = List::Util::min($min_abs_X, $absolute_current_point_X);
	$min_abs_Y = List::Util::min($min_abs_Y, $absolute_current_point_Y);
	$max_abs_X = List::Util::max($max_abs_X, $absolute_current_point_X);
	$max_abs_Y = List::Util::max($max_abs_Y, $absolute_current_point_Y);
}
sub relative_to_absolute_coordinates {
	my $local_path_data_string = shift;

	# print $local_path_data_string, "\n";
	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	my $index = 0;
	$min_abs_X = 1000000;
	$min_abs_Y = 1000000;
	$max_abs_X = -1000000;
	$max_abs_Y = -1000000;
	$absolute_current_point_X = 0;
	$absolute_current_point_Y = 0;

	while( $index < $local_path_data_size) {
		if($local_path_data[$index] eq "M") {
			$local_path_data[$index] = "M";
			$index++;
			# processes the next moveto and possible following lineto
			do {
				$absolute_current_point_X = $local_path_data[$index];
				$absolute_current_point_Y = $local_path_data[$index + 1];
				minMaxUpdate();
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MmCcZzLlHhVv]/));
		}
		elsif($local_path_data[$index] eq "m") {
			# the first move even if relative (m), has absolute coordinates as first coordinates
			# possibly following moves have relative coordinates
			my $first_move = 0;
			if($index == 0) {
				$first_move = 1;
			}

			$local_path_data[$index] = "M";
			$index++;
			# processes the next moveto and possible following lineto
			my $first_loop = 1;
			do {
				if($first_move && $first_loop) {
					$absolute_current_point_X = $local_path_data[$index];
					$absolute_current_point_Y = $local_path_data[$index + 1];
				}
				else {
					$local_path_data[$index] += $absolute_current_point_X;
					$local_path_data[$index + 1] += $absolute_current_point_Y;
					$absolute_current_point_X = $local_path_data[$index];
					$absolute_current_point_Y = $local_path_data[$index + 1];
				}
				minMaxUpdate();
				$index += 2;

				$first_loop = 0;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MmCcZzLlHhVv]/));
		}
		elsif($local_path_data[$index] eq "L") {
			$index++;
			# processes the next lineto
			do {
				$absolute_current_point_X = $local_path_data[$index];
				$absolute_current_point_Y = $local_path_data[$index + 1];
				minMaxUpdate();
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MmCcZzLlHhVv]/));
		}
		elsif($local_path_data[$index] eq "l") {
			$local_path_data[$index] = "L";
			$index++;
			# processes the next lineto
			do {
				$local_path_data[$index] += $absolute_current_point_X;
				$local_path_data[$index + 1] += $absolute_current_point_Y;
				$absolute_current_point_X = $local_path_data[$index];
				$absolute_current_point_Y = $local_path_data[$index + 1];
				minMaxUpdate();
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MmCcZzLlHhVv]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			do {
				$absolute_current_point_X = $local_path_data[$index + 4];
				$absolute_current_point_Y = $local_path_data[$index + 5];
				minMaxUpdate();
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MmCcZzLlHhVv]/));
		}
		elsif($local_path_data[$index] eq "c") {
			$local_path_data[$index] = "C";
			$index++;
			# processes the next curve segments
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					$local_path_data[$i] += $absolute_current_point_X;
					$local_path_data[$i + 1] += $absolute_current_point_Y;
				}
				$absolute_current_point_X = $local_path_data[$index + 4];
				$absolute_current_point_Y = $local_path_data[$index + 5];
				minMaxUpdate();
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MmCcZzLlHhVv]/));
		}
		# horizontal line h/H x and vertical line v/V y, are transformed into a curve 
		# prec_point middle middle next_point
		elsif($local_path_data[$index] eq "H" || $local_path_data[$index] eq "h"
			|| $local_path_data[$index] eq "V" || $local_path_data[$index] eq "v") {
			my $SVG_path_tag = $local_path_data[$index];
			# definition of the replacing line segment
			$local_path_data[$index] = "L";
			$index++;
			do {
				# coordinates of new point
				if($SVG_path_tag eq "H" || $SVG_path_tag eq "h") {
					if($SVG_path_tag eq "h") {
						$absolute_current_point_X = $absolute_current_point_X + $local_path_data[$index];
					}
					else {  # if($SVG_path_tag eq "H")
						$absolute_current_point_X = $local_path_data[$index];
					}
				}
				else { # if($SVG_path_tag eq "V" || $SVG_path_tag eq "v")
					if($SVG_path_tag eq "v") {
						$absolute_current_point_Y = $absolute_current_point_Y + $local_path_data[$index];
					}
					else {  # if($SVG_path_tag eq "V")
						$absolute_current_point_Y = $local_path_data[$index];
					}
				}

				# h delta -> L absolute_current_point_X absolute_current_point_Y 
				# -> insertion of 1 value inside the array of the path
				for(my $i = $local_path_data_size ; $i >= $index + 2 ; $i--) {
					$local_path_data[$i] = $local_path_data[$i - 1];
				}
				$local_path_data_size += 1;

				# coordinates of the line segment end point
				$local_path_data[$index] = $absolute_current_point_X;
				$local_path_data[$index + 1] = $absolute_current_point_Y;
				minMaxUpdate();
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MmCcZzLlHhVv]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "relative_to_absolute_coordinates Unexpected sign at index $index: ",$local_path_data[$index],"\n";
		    return 0;
		}
	}
	# printf("Image min ($min_abs_X,$min_abs_Y) max ($max_abs_X,$max_abs_Y)\n");
	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# TRANSFORMATION FUNCTIONS
##################################################################

##################################################################
# VERTICAL SCALING (WITH AS MANY LEVELS AS LAYERS)
sub V_SCALE_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# horizontal base line according to the number of colors
	my $y_reference = (SVG_PAGE_HEIGHT / ($nb_layers - 1)) * $local_path_no;
	# scales the path vertically according to the number of colors
	my $index = 0;
	while($index < $local_path_data_size) {
		if(!($local_path_data[$index] =~ /^[MmCcZzLl]/)) {
			$local_path_data[$index + 1] 
				= $y_reference 
				+ (1 - $percent) * ($local_path_data[$index + 1] - $y_reference);
			$index += 2;
		} 
		else {
			$index++;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS)
sub V_SCALE2_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# arbirary height of the initially visible part
	my $h_ini = SVG_PAGE_HEIGHT/3;
	# height of the whole image
	my $h_final = SVG_PAGE_HEIGHT;
	# scaling of the layers to the initially visible part
	my $initial_scale = $h_ini/$h_final;
	# scaling of the layers to the initially visible part
	my $mid_scale = 1.0/$nb_layers;
	# scaling of the layers to the full image
	my $final_scale = 1;
	# initial translation: no translation, only scaling down to the visible part
	my $y_initial_translation = 0;
	# middle translation: scaling is kept down to the visible part but layers are brought towards the top
	# according to the color rank
	my $y_mid_translation = ($h_final / $nb_layers) * $local_path_no;
	# final translation is no translation again but the image has the final size
	my $y_final_translation = 0;
	# 2 steps: first translation towards the top according to layer rank and scaling according to the layer so as to make the layers visible
	# second scaling towards 1 and translation towards 0 
	# percent varies from 1 to 0
	my $index = 0;
	my $local_percent = 0;
	my $local_translation = 0;
	my $local_scale = 0;
	# first part, percent from 1 to 0.5, 1-percant from 0 to 0.5, local_percent from 0 to 1
	if((1 - $percent) < 0.5) {
		$local_percent = (1 - $percent) * 2;
		$local_translation = (1.0 - $local_percent) * $y_initial_translation
			+ $local_percent * $y_mid_translation;
		$local_scale = (1.0 - $local_percent) * $initial_scale
			+ $local_percent * $mid_scale;
	}
	# second part, percent from 0.5 to 0. 1-percent from 0.5 to 1, Local percent from 0 to 1
	else {
		$local_percent = (1 - $percent) * 2 - 1.0;
		$local_translation = (1.0 - $local_percent) * $y_mid_translation
			+ $local_percent * $y_final_translation;
		$local_scale = (1.0 - $local_percent) * $mid_scale
			+ $local_percent * $final_scale;
	}
	while($index < $local_path_data_size) {
		if(!($local_path_data[$index] =~ /^[MmCcZzLl]/)) {
			$local_path_data[$index + 1] 
				= $local_translation + $local_scale * $local_path_data[$index + 1];
			$index += 2;
		} 
		else {
			$index++;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
# similar to V_SCALE2 but with double length for the first part wrt the second part
sub V_SCALE3_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# arbirary height of the initially visible part
	my $h_ini = SVG_PAGE_HEIGHT/3;
	# height of the whole image
	my $h_final = SVG_PAGE_HEIGHT;
	# scaling of the layers to the initially visible part
	my $initial_scale = $h_ini/$h_final;
	# scaling of the layers to the initially visible part
	my $mid_scale = 1.0/$nb_layers;
	# scaling of the layers to the full image
	my $final_scale = 1;
	# initial translation: no translation, only scaling down to the visible part
	my $y_initial_translation = 0;
	# middle translation: scaling is kept down to the visible part but layers are brought towards the top
	# according to the color rank
	my $y_mid_translation = ($h_final / $nb_layers) * $local_path_no;
	# final translation is no translation again but the image has the final size
	my $y_final_translation = 0;
	# 2 steps: first translation towards the top according to layer rank and scaling according to the layer so as to make the layers visible
	# second scaling towards 1 and translation towards 0 
	# percent varies from 1 to 0
	my $index = 0;
	my $local_percent = 0;
	my $local_translation = 0;
	my $local_scale = 0;
	# first part, percent from 1 to 0.33 (1-percent varies from 0 to 0.66)
	# local_percent varies from 0 to 1
	if((1 - $percent) < 0.66) {
		$local_percent = (1 - $percent) / 0.66; # from 0 to 0.66/0.66
		$local_translation = (1.0 - $local_percent) * $y_initial_translation
			+ $local_percent * $y_mid_translation;
		$local_scale = (1.0 - $local_percent) * $initial_scale
			+ $local_percent * $mid_scale;
	}
	# second part, percent from 0.33 to 0. (1-percent varies from 0.66 to 1.)
	# local_percent varies from 0 to 1
	else {
		$local_percent = 1 - ($percent / (1 - 0.66)); # from 1 - (1 - 0.66)/(1 - 0.66) = 0 to 1 - (0/(1 - 0.66)) = 1
		$local_translation = (1.0 - $local_percent) * $y_mid_translation
			+ $local_percent * $y_final_translation;
		$local_scale = (1.0 - $local_percent) * $mid_scale
			+ $local_percent * $final_scale;
	}
	while($index < $local_path_data_size) {
		if(!($local_path_data[$index] =~ /^[MmCcZzLl]/)) {
			$local_path_data[$index + 1] 
				= $local_translation + $local_scale * $local_path_data[$index + 1];
			$index += 2;
		} 
		else {
			$index++;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# CENTRAL SCALING (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
sub C_SCALE_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# screen center
	my $x_reference = SVG_PAGE_WIDTH / 2;
	my $y_reference = SVG_PAGE_HEIGHT / 2;
	# scales the path centrally =
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# processes the next moveto and lineto
			do {
				$local_path_data[$index] 
					= $x_reference 
					+ (1 - $percent) * ($local_path_data[$index] - $x_reference);
				$local_path_data[$index + 1] 
					= $y_reference 
					+ (1 - $percent) * ($local_path_data[$index + 1] - $y_reference);
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
					my $weight = $control_point_rank / 3.0;
					$local_path_data[$i] 
						= $x_reference 
						+ (1 - $percent * $weight) * ($local_path_data[$i] - $x_reference);
					$local_path_data[$i + 1] 
						= $y_reference 
						+ (1 - $percent * $weight) * ($local_path_data[$i + 1] - $y_reference);
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "V_SCALE_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";
		    return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
sub C_SCALE2_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# screen center
	my $x_reference = SVG_PAGE_WIDTH / 2;
	my $y_reference = SVG_PAGE_HEIGHT / 2;
	# scales the path centrally =
	my $index = 0;
	# percentage depends on the color
	# my $color_percentage = ($percent / ($nb_layers - 1)) * $local_path_no;
	my $color_percentage = $percent * $nb_layers;
	my $path_from_black = $nb_layers - 1 - $local_path_no;
	if($color_percentage >= $path_from_black && $color_percentage <= $path_from_black + 1) {
		$color_percentage = $color_percentage - $path_from_black;
	}
	elsif($color_percentage > $path_from_black + 1) {
		$color_percentage = 1;
	}
	elsif($color_percentage < $path_from_black) {
		$color_percentage = 0;
	}
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# processes the next moveto and lineto
			do {
				$local_path_data[$index] 
					= $x_reference 
					+ $color_percentage * ($local_path_data[$index] - $x_reference);
				$local_path_data[$index + 1] 
					= $y_reference 
					+ $color_percentage * ($local_path_data[$index + 1] - $y_reference);
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
					# quicker scaling for the control points
					my $exponential_percentage = $color_percentage;
					if($control_point_rank == 1 || $control_point_rank == 2) {
						$exponential_percentage = $color_percentage ** 3;
					}
					$local_path_data[$i] 
						= $x_reference 
						+ $exponential_percentage * ($local_path_data[$i] - $x_reference);
					$local_path_data[$i + 1] 
						= $y_reference 
						+ $exponential_percentage * ($local_path_data[$i + 1] - $y_reference);
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "C_SCALE2_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
		    return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# ALL DIRECTIONS RANDOMNESS
sub VH_RANDOM_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# adds all directions randomness with weight on the control point rank
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# processes the next moveto and lineto
			do {
				$local_path_data[$index] 
					+= $percent * rand(3000);
				$local_path_data[$index + 1] 
					+= $percent * rand(3000);
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
					my $weight = $control_point_rank / 3.0;
					$local_path_data[$i] 
						+= $percent * $weight * rand(3000);
					$local_path_data[$i + 1] 
						+= $percent * $weight * rand(3000);
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "VH_RANDOM_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
	    	return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# VERTICAL FLATNESS
sub V_FLATTEN_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# scales the path centrally according to the number of colors
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			my $y_reference;
			# processes the next moveto and lineto
			do {
				if(defined $y_reference) {
					$local_path_data[$index + 1] = $y_reference 
					+ (1 - $percent) * ($local_path_data[$index + 1] - $y_reference);
				}
				else {
					$y_reference = $local_path_data[$index + 1];
				}
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
			my $y_reference;
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
					my $weight = $control_point_rank / 3.0;
					if(defined $y_reference) {
						$local_path_data[$i + 1] = $y_reference 
						+ (1 - $percent * $weight) * ($local_path_data[$i + 1] - $y_reference);
					}
					else {
						$y_reference = $local_path_data[$i + 1];
					}
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "V_FLATTEN_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
		    return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# SMOOTHING AND/OR SIMPLIFICATION OF THE BEZIER CURVES
sub V_FLATTEN_or_BLUR_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;
	my $local_single_transf = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	my $index = 0;
	my $smooting_percentage = $percent;

	# simplifies the curve by removing segments according to percentage
	if($local_single_transf == BLUR){
		# a percentage of 1.0 gives a blank image
		if($percent > 0.9) {
			$percent = 0.9;
		}
		my $index_simplified = 0;
		my @simplified_path_data = ();
		my $preserved_multiple_curve_segment;
		for( my $ind = 1 ; $ind <= 20 ; $ind++) {
			if($percent < $ind * 0.05) {
				$preserved_multiple_curve_segment = $ind + 1;
				$smooting_percentage = 1 - 20 * ($percent - ($ind - 1) * 0.05);
				last;
			}
		}
		while($index < $local_path_data_size) {
			if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
				$simplified_path_data[$index_simplified] = $local_path_data[$index];
				$index_simplified++;
				$index++;
				# copies the next moveto and lineto
				do {
					$simplified_path_data[$index_simplified] = $local_path_data[$index];
					$index_simplified++;
					$index++;
					$simplified_path_data[$index_simplified] = $local_path_data[$index];
					$index_simplified++;
					$index++;
				}
				while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
			}
			elsif($local_path_data[$index] eq "C") {
				$simplified_path_data[$index_simplified] = $local_path_data[$index];
				$index_simplified++;
				$index++;
				my $curve_rank = 0;
				do {
					# copies the next curve segments
					# if it has the rank to be preserved (the n-1th if every nth is preserved)
					# or if it is the last one
					if( ((int($curve_rank) % int($preserved_multiple_curve_segment))
						== (int($preserved_multiple_curve_segment) - 1))
						|| (($index + 6 < $local_path_data_size)
							&& ($local_path_data[$index + 6] =~ /^[MCLZz]/)) ) {
						for(my $ind = 0; $ind < 6 ; $ind++ ) {
							$simplified_path_data[$index_simplified] = $local_path_data[$index];
							$index_simplified++;
							$index++;
						}
					}
					# discards the next curve segments
					else {
						$index += 6;
					}
					$curve_rank++;
				}
				while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
			}
			elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
				$simplified_path_data[$index_simplified] = $local_path_data[$index];
				$index_simplified++;
				$index++;
			}
			else {
				print "Line: ", $local_path_data_string, "\n";
				print "V_FLATTEN_or_BLUR_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
			    return 0;
			}
		}
		@local_path_data = @simplified_path_data;
		$local_path_data_size = @local_path_data + 0;
	}

	# smoothes the curve with symmetrical control points around a final control point
	$index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# passes the next moveto and lineto, no smoothing takes lace here
			do {
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			my $initial_polycurve_index = $index;
			# processes the preceding curve segments (and requires two previous curve segments)
			do {
				if(($index - $initial_polycurve_index) >= 12){
					# the vectors joining the last control points of 2 neighboring segments
					my $vec_In_1In = V($local_path_data[$index] - $local_path_data[$index - 6], $local_path_data[$index + 1] - $local_path_data[$index - 6 + 1]);
					my $vec_In_1In_2 = V($local_path_data[$index - 12] - $local_path_data[$index - 6], $local_path_data[$index - 12 + 1] - $local_path_data[$index - 6 + 1]);
					# and their norms
					my $norm_In_1In = abs($vec_In_1In);
					my $norm_In_1In_2 = abs($vec_In_1In_2);
					# the central last control point
					my $point_In = V($local_path_data[$index - 6], $local_path_data[$index - 6 + 1]);
					# if the 3 last control points are distinct
					if($norm_In_1In * $norm_In_1In_2 != 0) {
						# calculates the normal vector at the central point by summing the normalized two vectors
						my $vec_normal = ($vec_In_1In / $norm_In_1In) + ($vec_In_1In_2 / $norm_In_1In_2);
						# normalizes it
						my $norm_normal = abs($vec_normal);
						if($norm_normal != 0) {
							$vec_normal = $vec_normal / $norm_normal;
							# calculates the tangent by giving it the average third length of the distance between the last control points
							my $vec_tang_In = V($vec_normal->[1], -$vec_normal->[0]) * ($norm_In_1In + $norm_In_1In_2) / 6.0;
							# aligns the tangent with the vector In-1 In-2
							if($vec_tang_In * $vec_In_1In_2 < 0) {
								$vec_tang_In = -$vec_tang_In;
							}
							# calculates the new control points around the central point In-1
							# the control point before In-1
							$local_path_data[$index - 6 - 2] = (1 - $smooting_percentage) * $local_path_data[$index - 6 - 2] 
														+ $smooting_percentage * ($point_In->[0] + $vec_tang_In->[0]);
							$local_path_data[$index - 6 - 2 + 1] = (1 - $smooting_percentage) * $local_path_data[$index - 6 - 2 + 1] 
														+ $smooting_percentage * ($point_In->[1] + $vec_tang_In->[1]);
							# the control point after In-1
							$local_path_data[$index - 6 + 2] = (1 - $smooting_percentage) * $local_path_data[$index - 6 + 2] 
														+ $smooting_percentage * ($point_In->[0] - $vec_tang_In->[0]);
							$local_path_data[$index - 6 + 2 + 1] = (1 - $smooting_percentage) * $local_path_data[$index - 6 + 2 + 1] 
														+ $smooting_percentage * ($point_In->[1] - $vec_tang_In->[1]);
						}
					}
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "V_FLATTEN_or_BLUR_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
		    return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# PLACES THE TOP LEFT POINT OF THE DRAWING AT THE TOP LEFT POINT OF THE PAGE
sub REFRAME_TOP_LEFT_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# top left and bottom right of the drawing have been stored by 
	# relative_to_absolute_coordinates
	# $min_abs_X;
	# $min_abs_Y;
	# $max_abs_X;
	# $max_abs_Y;

	# translates the drawing to the top left
	# SVG_PAGE_WIDTH = 38400,
	# SVG_PAGE_HEIGHT = 21600,
	my ($x_transl, $y_transl) = (-$min_abs_X, SVG_PAGE_HEIGHT-$max_abs_Y); 
	# print "\n************ Translation $x_transl $y_transl\n\n";
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# processes the next moveto and lineto
			do {
				$local_path_data[$index] += $x_transl;
				$local_path_data[$index + 1] += $y_transl;
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					$local_path_data[$i] += $x_transl;
					$local_path_data[$i + 1] += $y_transl;
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "REFRAME_TOP_LEFT_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
	    	return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# TRNASLATION TRANSFORMATION
sub TRANSLATE_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;
	my $x_transl = shift;
	my $y_transl = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# print "\n************ Translation $x_transl $y_transl\n\n";
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# processes the next moveto and lineto
			do {
				$local_path_data[$index] += $x_transl;
				$local_path_data[$index + 1] += $y_transl;
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					$local_path_data[$i] += $x_transl;
					$local_path_data[$i + 1] += $y_transl;
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "TRANSLATE_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
		    return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# ROTATION TRANSFORMATION
sub apply_rotation {
	my $x = shift;
	my $y = shift;
	my $x_center = shift;
	my $y_center = shift;
	my $angle = shift;
	my $x_ret;
	my $y_ret;
	$x_ret = $x_center + cos($angle) * ($x - $x_center) - sin($angle) * ($y - $y_center);
	$y_ret = $y_center + sin($angle) * ($x - $x_center) + cos($angle) * ($y - $y_center);
	return($x_ret, $y_ret);
}
sub ROTATE_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;
	my $x_center = shift;
	my $y_center = shift;
	my $angle = Math::Trig::deg2rad(shift);

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# print "\n************ Translation $x_transl $y_transl\n\n";
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# processes the next moveto and lineto
			do {
				($local_path_data[$index] , $local_path_data[$index + 1])
				= apply_rotation($local_path_data[$index] , $local_path_data[$index + 1],
						$x_center, $y_center, $angle);
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					($local_path_data[$i] , $local_path_data[$i + 1])
					= apply_rotation($local_path_data[$i] , $local_path_data[$i + 1],
						$x_center, $y_center, $angle);
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "ROTATE_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
	    	return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# SCALE TRANSFORMATION
sub apply_scaling {
	my $x = shift;
	my $y = shift;
	my $x_center = shift;
	my $y_center = shift;
	my $factor = shift;
	my $x_ret;
	my $y_ret;
	$x_ret = $x_center + $factor * ($x - $x_center);
	$y_ret = $y_center + $factor * ($y - $y_center);
	return($x_ret, $y_ret);
}
sub SCALE_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;
	my $x_center = shift;
	my $y_center = shift;
	my $factor = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# print "\n************ Translation $x_transl $y_transl\n\n";
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# processes the next moveto and lineto
			do {
				($local_path_data[$index] , $local_path_data[$index + 1])
				= apply_scaling($local_path_data[$index] , $local_path_data[$index + 1],
						$x_center, $y_center, $factor);
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					($local_path_data[$i] , $local_path_data[$i + 1])
					= apply_scaling($local_path_data[$i] , $local_path_data[$i + 1],
						$x_center, $y_center, $factor);
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "SCALE_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
	    	return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# SYMMETRY TRANSFORMATION
sub SYMMETRY_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;
	my $x_center = shift;
	my $y_center = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# print "\n************ Translation $x_transl $y_transl\n\n";
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "M" || $local_path_data[$index] eq "L") {
			$index++;
			# processes the next moveto and lineto
			do {
				if($x_center >= 0) {
					$local_path_data[$index] = 2 * $x_center - $local_path_data[$index];
				}
				if($y_center >= 0) {
					$local_path_data[$index + 1] = 2 * $y_center - $local_path_data[$index + 1];
				}
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					if($x_center >= 0) {
						$local_path_data[$i] = 2 * $x_center - $local_path_data[$i];
					}
					if($y_center >= 0) {
						$local_path_data[$i + 1] = 2 * $y_center - $local_path_data[$i + 1];
					}
				}
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "SYMMETRY_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";;
	    	return 0;
		}
	}

	return "<path d=\"" . join(' ', @local_path_data) . "\"/>";
}

##################################################################
# COPY TRANSFORMATION AND STORES TOP LEFT
# copies the coordinates of the top left point of the drawing
sub COPY_TOP_LEFT_transformation {
	# translates the drawing to the top left
	# SVG_PAGE_WIDTH = 38400,
	# SVG_PAGE_HEIGHT = 21600,
	print $FILE_log SVG_PAGE_WIDTH,",",SVG_PAGE_HEIGHT,",",int($min_abs_X),",",int($max_abs_Y),",",int($max_abs_X),",",int($min_abs_Y),"\r\n";
}

##################################################################
# ADDS LINE RAYS BETWEEN THE POINTS AND THE CENTER OF THE IMAGE
sub C_LINES_transformation {
	my $local_path_data_string = shift;
	my $local_path_no = shift;

	my @local_path_data = split(/ +/ , $local_path_data_string);
	my $local_path_data_size = @local_path_data + 0;

	# coordinates of the curve points from which start the lines
	my @line_starting_points = ();
	# scales the path centrally =
	my $index = 0;
	while($index < $local_path_data_size) {
		if($local_path_data[$index] eq "L") {
			$index++;
			# processes the next lineto: pushes the end point(s)
			do {
				push( @line_starting_points , ($local_path_data[$index], $local_path_data[$index + 1]));
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "M") {
			$index++;
			# processes the next moveto: nothing to be done
			do {
				$index += 2;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "C") {
			$index++;
			# processes the next curve segments
			# pushes only the end point(s), not the control points
			do {
				push( @line_starting_points , ($local_path_data[$index + 4], $local_path_data[$index + 5]));
				$index += 6;
			}
			while($index < $local_path_data_size && !($local_path_data[$index] =~ /^[MCLZz]/));
		}
		elsif($local_path_data[$index] eq "Z" || $local_path_data[$index] eq "z") {
			$index++;
		}
		else {
			print "Line: ", $local_path_data_string, "\n";
			print "C_LINES_transformation Unexpected sign at index $index: ",$local_path_data[$index],"\n";
	    	return 0;
		}
	}

	# screen center
	my $x_reference = SVG_PAGE_WIDTH / 2;
	my $y_reference = SVG_PAGE_HEIGHT / 2;
	# points selected on curves as candidates for lines
	my $number_of_starting_points = int((@line_starting_points+0)/2);
	# the number of lines increases with percentage
	my $number_of_lines = int($number_of_starting_points * $percent);
	# how many points between two connected points to the center
	my $intervals_between_lines = 100000;
	if($number_of_lines > 0 ) {
		$intervals_between_lines = int($number_of_starting_points/$number_of_lines);
	}
	if($intervals_between_lines < 1 ) {
		$intervals_between_lines = 1;
	}
	my $ind_point = 1;
	my  @local_beam_path_data = ();
	$index = 0;
	while(@line_starting_points) {
	    my $line_X = shift @line_starting_points;
	    my $line_Y = shift @line_starting_points;
	    if(($ind_point % $intervals_between_lines) == 0) {
		    $local_beam_path_data[$index++] = "M";
		    $local_beam_path_data[$index++] = $x_reference;
		    $local_beam_path_data[$index++] = $y_reference;
		    $local_beam_path_data[$index++] = "L";
		    $local_beam_path_data[$index++] = $line_X;
		    $local_beam_path_data[$index++] = $line_Y;
		}
	    $ind_point++;
	}
    $local_beam_path_data[$index++] = "Z";
	return ("<path d=\"" . join(' ', @local_path_data) . "\"/>", 
		"<path d=\"" . join(' ', @local_beam_path_data) . "\" id=\"path116\" style=\"stroke:url(#radialGradient001);stroke-opacity:1\"/>");
}

1;
