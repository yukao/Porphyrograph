#!/usr/bin/perl 

use strict;
use warnings;

# SVG_movie_transform.pl
# composition of color paths into a single file

# usage
# SVG_movie_transform.pl --format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

# IN
#
#              n paths

# OUT 
#
#              coposited and colored image

my $USAGE =
"usage: SVG_movie_transform.pl --nb-layers <integer> -o <file_name> -i <file_name> --transform <transformation_function>
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pl --nb-layers 4 -o mer.svg -i mer.svg --transformation COPY --percent 0.7 --input-second-SVGs mer_aux.svg

ARGUMENTS:
   --nb_layers: number of layers for posterization
   -o: ouput files 
   -i: input files
   --transformation: transformation function
   --percent: percentage of transformation
   --input-second-SVGs: svg files of an auxiliary video for crossfading
";

my $numArgs;
$numArgs = $#ARGV + 1;

my $argnum;
my $output_format;
my $nb_layers;
my $output_file_name;
my $input_file_name;
my $second_input_file_name;
my $transformation_function;
my $transformation_function_string;
my $percent;

use constant {
    COPY => 1,
    V_SCALE => 2,
    C_SCALE => 3,
    VH_RANDOM => 4,
    V_FLATTEN => 5,
    CROSSFADE => 6,
    CROSSFADE_FINE => 7,
    CROSSFADE_FINE_ALTERNATE => 8,
    CROSSFADE_ALTERNATE => 9,
    SMOOTHING => 10,
    BLUR => 11,
    FILLANDSTROKE => 12,
    V_SCALE2 => 13,
    V_SCALE3 => 14,
    C_SCALE2 => 15,
    C_SCALE_TO_BLACK => 16,
    C_SCALE_TO_BLACK_FROM_NEG => 17,
    VH_RANDOM_LINE => 18,
};
my %transformation_function = (
   "COPY" => COPY,
   "V_SCALE" => V_SCALE,
   "C_SCALE" => C_SCALE,
   "VH_RANDOM" => VH_RANDOM,
   "V_FLATTEN" => V_FLATTEN,
   "CROSSFADE" => CROSSFADE,
   "CROSSFADE_FINE" => CROSSFADE_FINE,
   "CROSSFADE_FINE_ALTERNATE" => CROSSFADE_FINE_ALTERNATE,
   "CROSSFADE_ALTERNATE" => CROSSFADE_ALTERNATE,
   "SMOOTHING" => SMOOTHING,
   "BLUR" => BLUR,
   "FILLANDSTROKE" => FILLANDSTROKE,
   "V_SCALE2" => V_SCALE2,
   "V_SCALE3" => V_SCALE3,
   "C_SCALE2" => C_SCALE2,
   "C_SCALE_TO_BLACK" => C_SCALE_TO_BLACK,
   "C_SCALE_TO_BLACK_FROM_NEG" => C_SCALE_TO_BLACK_FROM_NEG,
   "VH_RANDOM_LINE" => VH_RANDOM_LINE,
);

use POSIX;

use Math::Vector::Real;
use List::Util qw[min max];

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
	$min_abs_X = min($min_abs_X, $absolute_current_point_X);
	$min_abs_Y = min($min_abs_Y, $absolute_current_point_Y);
	$max_abs_X = max($max_abs_X, $absolute_current_point_X);
	$max_abs_Y = max($max_abs_Y, $absolute_current_point_Y);
}
sub relative_to_absolute_coordinates {
	my $path_data_size_local = shift;
	my $path_data_array_ref = shift;

	my $index = 0;
	$min_abs_X = 1000000;
	$min_abs_Y = 1000000;
	$max_abs_X = -1000000;
	$max_abs_Y = -1000000;
	$absolute_current_point_X = 0;
	$absolute_current_point_Y = 0;

	while( $index < $path_data_size_local) {
		if(${$path_data_array_ref}[$index] eq "M" || (${$path_data_array_ref}[$index] eq "m" && $index == 0)) {
			$index++;
			# processes the next moveto and possible following lineto
			do {
				$absolute_current_point_X = ${$path_data_array_ref}[$index];
				$absolute_current_point_Y = ${$path_data_array_ref}[$index + 1];
				minMaxUpdate();
				$index += 2;
			}
			while($index < $path_data_size_local && !(${$path_data_array_ref}[$index] =~ /^[MmCcZzLl]/));
		}
		elsif(${$path_data_array_ref}[$index] eq "m" && $index > 0) {
			${$path_data_array_ref}[$index] = "M";
			$index++;
			# processes the next moveto and possible following lineto
			do {
				${$path_data_array_ref}[$index] += $absolute_current_point_X;
				${$path_data_array_ref}[$index + 1] += $absolute_current_point_Y;
				$absolute_current_point_X = ${$path_data_array_ref}[$index];
				$absolute_current_point_Y = ${$path_data_array_ref}[$index + 1];
				minMaxUpdate();
				$index += 2;
			}
			while($index < $path_data_size_local && !(${$path_data_array_ref}[$index] =~ /^[MmCcZzLl]/));
		}
		elsif(${$path_data_array_ref}[$index] eq "L") {
			$index++;
			# processes the next lineto
			do {
				$absolute_current_point_X = ${$path_data_array_ref}[$index];
				$absolute_current_point_Y = ${$path_data_array_ref}[$index + 1];
				minMaxUpdate();
				$index += 2;
			}
			while($index < $path_data_size_local && !(${$path_data_array_ref}[$index] =~ /^[MmCcZzLl]/));
		}
		elsif(${$path_data_array_ref}[$index] eq "l") {
			${$path_data_array_ref}[$index] = "L";
			$index++;
			# processes the next lineto
			do {
				${$path_data_array_ref}[$index] += $absolute_current_point_X;
				${$path_data_array_ref}[$index + 1] += $absolute_current_point_Y;
				$absolute_current_point_X = ${$path_data_array_ref}[$index];
				$absolute_current_point_Y = ${$path_data_array_ref}[$index + 1];
				minMaxUpdate();
				$index += 2;
			}
			while($index < $path_data_size_local && !(${$path_data_array_ref}[$index] =~ /^[MmCcZzLl]/));
		}
		elsif(${$path_data_array_ref}[$index] eq "C") {
			$index++;
			# processes the next curve segments
			do {
				$absolute_current_point_X = ${$path_data_array_ref}[$index + 4];
				$absolute_current_point_Y = ${$path_data_array_ref}[$index + 5];
				minMaxUpdate();
				$index += 6;
			}
			while($index < $path_data_size_local && !(${$path_data_array_ref}[$index] =~ /^[MmCcZzLl]/));
		}
		elsif(${$path_data_array_ref}[$index] eq "c") {
			${$path_data_array_ref}[$index] = "C";
			$index++;
			# processes the next curve segments
			do {
				for(my $i = $index ; $i < $index + 6 ; $i +=2) {
					${$path_data_array_ref}[$i] += $absolute_current_point_X;
					${$path_data_array_ref}[$i + 1] += $absolute_current_point_Y;
				}
				$absolute_current_point_X = ${$path_data_array_ref}[$index + 4];
				$absolute_current_point_Y = ${$path_data_array_ref}[$index + 5];
				minMaxUpdate();
				$index += 6;
			}
			while($index < $path_data_size_local && !(${$path_data_array_ref}[$index] =~ /^[MmCcZzLl]/));
		}
		elsif(${$path_data_array_ref}[$index] eq "Z" || ${$path_data_array_ref}[$index] eq "z") {
			$index++;
		}
		else {
			print "Unexpected sign at index $index: [", ${$path_data_array_ref}[$index], "]\n";
		}
	}
	# printf("Image min ($min_abs_X,$min_abs_Y) max ($max_abs_X,$max_abs_Y)\n");
}

##################################################################
# COLOR MIXTURE
##################################################################
sub mix_color {
	my ($target_color, $initial_color, $local_percentage) = @_;
	my $target_red = '0x' . substr $target_color , 0 , 2;
	my $target_green = '0x' . substr $target_color , 2 , 2;
	my $target_blue = '0x' . substr $target_color , 4 , 2;
	my $initial_red = '0x' . substr $initial_color , 0 , 2;
	my $initial_green = '0x' . substr $initial_color , 2 , 2;
	my $initial_blue = '0x' . substr $initial_color , 4 , 2;
	my $mix_red = (1 - $local_percentage) * (eval $target_red) + $local_percentage * (eval $initial_red);
	my $mix_green = (1 - $local_percentage) * (eval $target_green) + $local_percentage * (eval $initial_green);
	my $mix_blue = (1 - $local_percentage) * (eval $target_blue) + $local_percentage * (eval $initial_blue);
	$mix_red = sprintf("%02X" , $mix_red);
	$mix_green = sprintf("%02X" , $mix_green);
	$mix_blue = sprintf("%02X" , $mix_blue);
	my $mixed_color = $mix_red . $mix_green . $mix_blue;
	# print "target_color initial_color local_percentage: $target_color, $initial_color, $local_percentage / mixed_color: $mixed_color \n";
	return $mixed_color;
}

if( 0+@ARGV != 10 && 0+@ARGV != 12) {
    print "Arguments: ", join(' ',@ARGV), "\n";
    print $USAGE; die"SVG_movie_transform.pl: expected number of arguments: 10 or 12 not ", 0+@ARGV, "\n" ;
}

# nb_layers
if( $ARGV[0] ne "--nb-layers" ) {
    print $USAGE; die"SVG_movie_transform.pl: --nb-layers expected for argt 1";
}
if ($ARGV[1] < 1) {
    print $USAGE; die "SVG_movie_transform.pl: ", $ARGV[1]," 1 layer should be produced minimally\n";
}
$nb_layers = $ARGV[1];

# output_file_name_PATHs
if( $ARGV[2] ne "-o" ) {
    print $USAGE; die"SVG_movie_transform.pl: -o expected for argt 3";
}
$output_file_name = $ARGV[3];

# input_file_names
if( $ARGV[4] ne "-i" ) {
    print $USAGE; die"SVG_movie_transform.pl: -i expected for argt 5\n";
}
$input_file_name = $ARGV[5];

#  SVG path transformation
if( $ARGV[6] ne "--transformation" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --transformation expected for argt 7";
}
$transformation_function_string = $ARGV[7];
if (!$transformation_function{$transformation_function_string}) {
    print $USAGE; die $transformation_function_string," does not exist in the hash %start_step\n";
}
$transformation_function = $transformation_function{$transformation_function_string};

#  SVG path transformation percentage
if( $ARGV[8] ne "--percent" ) {
    print $USAGE; die"SVG_movie_posterize_video.pl: --percent expected for argt 9";
}
$percent = $ARGV[9];
$percent = ($percent < 0? 0: $percent);
$percent = ($percent > 1? 1: $percent);

if($transformation_function == CROSSFADE || $transformation_function == CROSSFADE_FINE
	 || $transformation_function == CROSSFADE_FINE_ALTERNATE || $transformation_function == CROSSFADE_ALTERNATE) {
	if( $ARGV[10] ne "--input-second-SVGs" ) {
	    print $USAGE; die"SVG_movie_transform.pl: --input-second-SVGs expected for argt 11\n";
	}
	$second_input_file_name = $ARGV[11];
}

my $FILEout;
my $FILEin;
my $second_FILEin;
print "Transformed path generation file ", $output_file_name,"\n";
open(  $FILEout , ">$output_file_name" ) 
	|| die "Can't open file $output_file_name!";
open($FILEin , "<".$input_file_name) 
	|| die "Can't open file ".$input_file_name."!";
# auxiliary video SVG if there is one
if(defined $second_input_file_name) {
	open($second_FILEin , "<".$second_input_file_name) 
		|| die "Can't open file ".$second_input_file_name."!";
}

my $line;
my $path_no = 0;
my $path_data_string;
my @path_data;
my $path_data_size;
my $second_path_data_string;
my @second_path_data;
my $second_path_data_size;
while($line = <$FILEin>){
	chomp($line);
	##################################################################
	# changes fill and stroke
	##################################################################
	if( $line =~ /^<g / ) {
		if($transformation_function == FILLANDSTROKE) {
			# typical group line
			# <g transform="translate(0.000000,2160.000000) scale(0.100000,-0.100000)" style="fill:#c0c0c0;" stroke="none">
			# first part: from black image to white strokes by making the strokes thicker
			# second part: white strokes with filling going from black to the target color (including the background color in the posterior rendering phase)
			# third part: black strokes becoming thiner until they disappear
			# fourth part: from greyish to reddish rendering
			my $color = "ffffff";
			my $max_stroke_width = 20;
			if($line =~ /fill:#([0-9a-fA-F]+);/) {
				$color = $1;
			}
			if($percent >= 0.75) {
				my $local_percentage = ($percent - 0.75) * 4;
				my $stroke_width = (1 - $local_percentage) * $max_stroke_width;
				$line =~ s/stroke="none"/stroke="white" stroke-width="${stroke_width}"/;
				$line =~ s/fill:#([0-9a-fA-F]+);/fill:black;/;
			}
			elsif($percent >= 0.5) {
				my $local_percentage = ($percent - 0.5) * 4;
				$line =~ s/stroke="none"/stroke="black" stroke-width="${max_stroke_width}"/;
				$color = mix_color($color, '000000', $local_percentage);
				$line =~ s/fill:#([0-9a-fA-F]+);/fill:#${color};/;
			}
			elsif($percent >= 0.25) {
				my $local_percentage = ($percent - 0.25) * 4;
				my $stroke_width = $local_percentage * $max_stroke_width;
				$line =~ s/stroke="none"/stroke="black" stroke-width="${stroke_width}"/;
			}
			else {
				my $local_percentage = $percent * 4 / 2.0 + 0.5;
				$color = mix_color('FF0000', $color, $local_percentage);
				$line =~ s/fill:#([0-9a-fA-F]+);/fill:#${color};/;
			}
		}
		elsif($transformation_function == VH_RANDOM_LINE) {
				$line =~ s/stroke="none"/stroke="white" stroke-width="12"/;
				$line =~ s/fill:#([0-9a-fA-F]+);/fill:black;/;
		}
		else {
			# nothing to do
		}
		# prints the modified group
		print $FILEout "${line}\n";
	}
	##################################################################
	# reads the path and transform it into absolute coordinates
	# then appplies the transformation on it
	##################################################################
	elsif( $line =~ /^<path/ ) {		
		if($line =~ /d\=\"(.*)\"/) {
			##################################################################
			# transforms relative coordinates of base video into absolute ones
			##################################################################
			$path_data_string = $1;
			# adds space after move or curve and before z
			$path_data_string =~ s/([MmCcLl])/$1 /g;
			$path_data_string =~ s/([Zz])/ $1/g;
			# print $path_data, "\n";
			@path_data = split(/ +/ , $path_data_string);
			$path_data_size = @path_data + 0;
			relative_to_absolute_coordinates($path_data_size, \@path_data);

			# does the same for the auxiliary video SVG if there is one
			if(defined $second_input_file_name) {
				my $line_aux;
				while($line_aux = <$second_FILEin>){
					chomp($line_aux);
					if( $line_aux =~ /^<path/ ) {
						# reads the path and transform it into absolute coordinates
						if($line_aux =~ /d\=\"(.*)\"/) {
							##################################################################
							# transforms relative coordinates of base video into absolute ones
							##################################################################
							$second_path_data_string = $1;
							# adds space after move or curve and before z
							$second_path_data_string =~ s/([MmCcLl])/$1 /g;
							$second_path_data_string =~ s/([Zz])/ $1/g;
							# print $path_data, "\n";
							@second_path_data = split(/ +/ , $second_path_data_string);
							$second_path_data_size = @second_path_data + 0;
							relative_to_absolute_coordinates($second_path_data_size, \@second_path_data);
						}
						last;
					}
				}
			}

			##################################################################
			# COPY TRANSFORMATION
			##################################################################
			if($transformation_function == COPY 
			   || $transformation_function == FILLANDSTROKE) {
				# nothing to be done
			}
			##################################################################
			# VERTICAL SCALING (WITH AS MANY LEVELS AS LAYERS)
			##################################################################
			elsif($transformation_function == V_SCALE) {
				# horizontal base line according to the number of colors
				my $y_reference = (21600 / ($nb_layers - 1)) * $path_no;
				# scales the path vertically according to the number of colors
				my $index = 0;
				while($index < $path_data_size) {
					if(!($path_data[$index] =~ /^[MmCcZzLl]/)) {
						$path_data[$index + 1] 
							= $y_reference 
							+ (1 - $percent) * ($path_data[$index + 1] - $y_reference);
						$index += 2;
					} 
					else {
						$index++;
					}
				}
			}
			##################################################################
			# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS)
			##################################################################
			elsif($transformation_function == V_SCALE2) {
				# arbirary height of the initially visible part
				my $h_ini = 21600/3;
				# height of the whole image
				my $h_final = 21600;
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
				my $y_mid_translation = ($h_final / $nb_layers) * $path_no;
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
				while($index < $path_data_size) {
					if(!($path_data[$index] =~ /^[MmCcZzLl]/)) {
						$path_data[$index + 1] 
							= $local_translation + $local_scale * $path_data[$index + 1];
						$index += 2;
					} 
					else {
						$index++;
					}
				}
			}
			##################################################################
			# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
			# similar to V_SCALE2 but with double length for the first part wrt the second part
			##################################################################
			elsif($transformation_function == V_SCALE3) {
				# arbirary height of the initially visible part
				my $h_ini = 21600/3;
				# height of the whole image
				my $h_final = 21600;
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
				my $y_mid_translation = ($h_final / $nb_layers) * $path_no;
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
				while($index < $path_data_size) {
					if(!($path_data[$index] =~ /^[MmCcZzLl]/)) {
						$path_data[$index + 1] 
							= $local_translation + $local_scale * $path_data[$index + 1];
						$index += 2;
					} 
					else {
						$index++;
					}
				}
			}
			##################################################################
			# CENTRAL SCALING (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
			##################################################################
			elsif($transformation_function == C_SCALE) {
				# screen center
				my $x_reference = 38400 / 2;
				my $y_reference = 21600 / 2;
				# scales the path centrally =
				my $index = 0;
				while($index < $path_data_size) {
					if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
						$index++;
						# processes the next moveto and lineto
						do {
							$path_data[$index] 
								= $x_reference 
								+ (1 - $percent) * ($path_data[$index] - $x_reference);
							$path_data[$index + 1] 
								= $y_reference 
								+ (1 - $percent) * ($path_data[$index + 1] - $y_reference);
							$index += 2;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "C") {
						$index++;
						# processes the next curve segments
						# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
						do {
							for(my $i = $index ; $i < $index + 6 ; $i +=2) {
								my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
								my $weight = $control_point_rank / 3.0;
								$path_data[$i] 
									= $x_reference 
									+ (1 - $percent * $weight) * ($path_data[$i] - $x_reference);
								$path_data[$i + 1] 
									= $y_reference 
									+ (1 - $percent * $weight) * ($path_data[$i + 1] - $y_reference);
							}
							$index += 6;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
						$index++;
					}
					else {
						print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
					}
				}
			}
			##################################################################
			# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
			##################################################################
			elsif($transformation_function == C_SCALE2) {
				# screen center
				my $x_reference = 38400 / 2;
				my $y_reference = 21600 / 2;
				# scales the path centrally =
				my $index = 0;
				# percentage depends on the color
				# my $color_percentage = ($percent / ($nb_layers - 1)) * $path_no;
				my $color_percentage = $percent * $nb_layers;
				my $path_from_black = $nb_layers - 1 - $path_no;
				if($color_percentage >= $path_from_black && $color_percentage <= $path_from_black + 1) {
					$color_percentage = $color_percentage - $path_from_black;
				}
				elsif($color_percentage > $path_from_black + 1) {
					$color_percentage = 1;
				}
				elsif($color_percentage < $path_from_black) {
					$color_percentage = 0;
				}
				while($index < $path_data_size) {
					if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
						$index++;
						# processes the next moveto and lineto
						do {
							$path_data[$index] 
								= $x_reference 
								+ $color_percentage * ($path_data[$index] - $x_reference);
							$path_data[$index + 1] 
								= $y_reference 
								+ $color_percentage * ($path_data[$index + 1] - $y_reference);
							$index += 2;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "C") {
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
								$path_data[$i] 
									= $x_reference 
									+ $exponential_percentage * ($path_data[$i] - $x_reference);
								$path_data[$i + 1] 
									= $y_reference 
									+ $exponential_percentage * ($path_data[$i + 1] - $y_reference);
							}
							$index += 6;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
						$index++;
					}
					else {
						print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
					}
				}
			}
			##################################################################
			# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
			# similar to C_SCALE2 but the three layers all together
			# and chooses a central point among the stones to scale to black
			##################################################################
			elsif($transformation_function == C_SCALE_TO_BLACK) {
				# screen center
				my $x_reference = 38400 / 2;
				my $y_reference = 21600 / 4;
				# scales the path centrally =
				my $index = 0;
				# percentage depends on the color
				while($index < $path_data_size) {
					if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
						$index++;
						# processes the next moveto and lineto
						do {
							$path_data[$index] 
								= $x_reference 
								+ $percent * ($path_data[$index] - $x_reference);
							$path_data[$index + 1] 
								= $y_reference 
								+ $percent * ($path_data[$index + 1] - $y_reference);
							$index += 2;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "C") {
						$index++;
						# processes the next curve segments
						do {
							for(my $i = $index ; $i < $index + 6 ; $i +=2) {
								my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
								# quicker scaling for the control points
								my $exponential_percentage = 0;
								my $scale = 1 + (1-$percent) * 3;
								if($control_point_rank == 0) {
									$exponential_percentage = (1-$percent);
								}
								elsif($control_point_rank == 1) {
									$exponential_percentage = -(1-$percent) ** 3;
								}
								elsif($control_point_rank == 2) {
									$exponential_percentage = -(1-$percent) ** 3;
								}
								elsif($control_point_rank == 3) {
									$exponential_percentage = (1-$percent);
								}
								$path_data[$i] 
									= $path_data[$i] 
									+ $exponential_percentage * ($path_data[$i] - $x_reference);
								$path_data[$i + 1] 
									= $path_data[$i + 1] 
									+ $exponential_percentage * ($path_data[$i + 1] - $y_reference);
								$path_data[$i] *= $scale;
								$path_data[$i+1] *= $scale;
							}
							$index += 6;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
						$index++;
					}
					else {
						print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
					}
				}
			}
			##################################################################
			# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
			# similar to C_SCALE_TO_BLACK but chooses a point in the sea to scale to white
			##################################################################
			elsif($transformation_function == C_SCALE_TO_BLACK_FROM_NEG) {
				# screen center
				my $x_reference = 38400 / 2;
				my $y_reference = 21600 / 2;
				# scales the path centrally =
				my $index = 0;
				# $percent varies from 1 to 0
				# (1 - $percent) varies from 0 to 1
				# ((1 - $percent) + 1) varies from 1 to 2
				my $local_percentage = ((1 - $percent) + 1); # varies from 1 to 2
				while($index < $path_data_size) {
					if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
						$index++;
						# processes the next moveto and lineto
						do {
							my $scale = $local_percentage ** 3;
							$path_data[$index] 
								= $x_reference 
								+ $scale * ($path_data[$index] - $x_reference);
							$path_data[$index + 1] 
								= $y_reference 
								+ $scale * ($path_data[$index + 1] - $y_reference);
							$index += 2;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "C") {
						$index++;
						# processes the next curve segments
						do {
							for(my $i = $index ; $i < $index + 6 ; $i +=2) {
								my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
								# quicker scaling for the control points
								my $scale = 1;
								if($control_point_rank == 0) {
									$scale = $local_percentage ** 3;
								}
								elsif($control_point_rank == 1) {
									$scale = $local_percentage ** 6;
								}
								elsif($control_point_rank == 2) {
									$scale = $local_percentage ** 6;
								}
								elsif($control_point_rank == 3) {
									$scale = $local_percentage ** 3;
								}
								$path_data[$i] 
									= $x_reference 
									+ $scale * ($path_data[$i] - $x_reference);
								$path_data[$i + 1] 
									= $y_reference 
									+ $scale * ($path_data[$i + 1] - $y_reference);
							}
							$index += 6;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
						$index++;
					}
					else {
						print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
					}
				}
			}
			##################################################################
			# ALL DIRECTIONS RANDOMNESS
			##################################################################
			elsif($transformation_function == VH_RANDOM) {
				# adds all directions randomness with weight on the control point rank
				my $index = 0;
				while($index < $path_data_size) {
					if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
						$index++;
						# processes the next moveto and lineto
						do {
							$path_data[$index] 
								+= $percent * rand(3000);
							$path_data[$index + 1] 
								+= $percent * rand(3000);
							$index += 2;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "C") {
						$index++;
						# processes the next curve segments
						# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
						do {
							for(my $i = $index ; $i < $index + 6 ; $i +=2) {
								my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
								my $weight = $control_point_rank / 3.0;
								$path_data[$i] 
									+= $percent * $weight * rand(3000);
								$path_data[$i + 1] 
									+= $percent * $weight * rand(3000);
							}
							$index += 6;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
						$index++;
					}
					else {
						print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
					}
				}
			}
			##################################################################
			# ALL DIRECTIONS RANDOMNESS FOR WHITE LINE CONTOURS
			##################################################################
			elsif($transformation_function == VH_RANDOM_LINE) {
				# adds all directions randomness with weight on the control point rank
				my $index = 0;
				while($index < $path_data_size) {
					if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
						$index++;
						# processes the next moveto and lineto
						do {
							$path_data[$index] 
								+= $percent * rand(3000);
							$path_data[$index + 1] 
								+= $percent * rand(3000);
							$index += 2;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "C") {
						$index++;
						# processes the next curve segments
						# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
						do {
							for(my $i = $index ; $i < $index + 6 ; $i +=2) {
								my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
								my $weight = $control_point_rank / 3.0;
								$path_data[$i] 
									+= $percent * $weight * rand(3000);
								$path_data[$i + 1] 
									+= $percent * $weight * rand(3000);
							}
							$index += 6;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
						$index++;
					}
					else {
						print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
					}
				}
			}
			##################################################################
			# VERTICAL FLATNESS
			##################################################################
			elsif($transformation_function == V_FLATTEN) {
				# scales the path centrally according to the number of colors
				my $index = 0;
				while($index < $path_data_size) {
					if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
						$index++;
						my $y_reference;
						# processes the next moveto and lineto
						do {
							if(defined $y_reference) {
								$path_data[$index + 1] = $y_reference 
								+ (1 - $percent) * ($path_data[$index + 1] - $y_reference);
							}
							else {
								$y_reference = $path_data[$index + 1];
							}
							$index += 2;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "C") {
						$index++;
						# processes the next curve segments
						# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
						my $y_reference;
						do {
							for(my $i = $index ; $i < $index + 6 ; $i +=2) {
								my $control_point_rank = int(POSIX::floor(($i - $index)/2)) + 1;
								my $weight = $control_point_rank / 3.0;
								if(defined $y_reference) {
									$path_data[$i + 1] = $y_reference 
									+ (1 - $percent * $weight) * ($path_data[$i + 1] - $y_reference);
								}
								else {
									$y_reference = $path_data[$i + 1];
								}
							}
							$index += 6;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
						$index++;
					}
					else {
						print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
					}
				}
			}
			##################################################################
			# CROSSFADE BETWEEN TWO VIDEOS
			##################################################################
			elsif($transformation_function == CROSSFADE
				|| $transformation_function == CROSSFADE_FINE
				|| $transformation_function == CROSSFADE_FINE_ALTERNATE
				|| $transformation_function == CROSSFADE_ALTERNATE) {
				# crossfades:
				# 0..25% fade in first video
				# 25..50% crossfades by adding progressively second video
				# 50..75% crossfades by removing progressively first video
				# 75..100% fades out second video
				my $percent_video_1 = 0.;
				my $percent_video_2 = 0.;
				if($percent < .25) {
					$percent_video_1 = $percent * 4;
				}
				elsif($percent < .5) {
					$percent_video_1 = 1.0;
					$percent_video_2 = ($percent - .25) * 4;
				}
				elsif($percent < .75) {
					$percent_video_1 = 1.0 - ($percent - .5) * 4;
					$percent_video_2 = 1.0;
				}
				else {
					$percent_video_1 = 0.;
					$percent_video_2 = 1.0 - ($percent - .75) * 4;
				}
				my $index = 0;
				my $index_composite = 0;
				my @composite_path_data = ();
				my $max_index_video_1 = int(POSIX::floor($percent_video_1 * $path_data_size));
				my $max_index_video_2 = int(POSIX::floor($percent_video_2 * $second_path_data_size));

				# in this crossfading mode, the curves are taken alternatively from each video to
				# ensure a maximal mixture of both videos
				# the mixture is fine because we possibly stop the copy inside a curve if we have
				# reached the number of items corresponding to the current crossfading percentage
				if($transformation_function == CROSSFADE_FINE_ALTERNATE) {
					$index = 0;
					my $second_index = 0;
					my $fist_video_choice = 1;
					my $current_point_X = -1;
					my $current_point_Y = -1;
					my $second_current_point_X = -1;
					my $second_current_point_Y = -1;

					# copies the number of items
					while(($index < $path_data_size && $index <  $max_index_video_1)
							|| ($second_index < $second_path_data_size && $second_index <  $max_index_video_2)) {
						# print "index size stopsize $index $path_data_size $max_index_video_1 / index size stopsize $second_index $second_path_data_size $max_index_video_2\n";

						# takes a segment from the first video if it is not finished or if the second one is finished
						if(($index < $path_data_size && $index <  $max_index_video_1)
							&& (($fist_video_choice == 1) 
								|| !($second_index < $second_path_data_size && $second_index <  $max_index_video_2))) {
							if($current_point_X != -1 && $current_point_Y != -1) {
								$composite_path_data[$index_composite] = "M";
								$index_composite++;
								$composite_path_data[$index_composite] = $current_point_X;
								$index_composite++;
								$composite_path_data[$index_composite] = $current_point_Y;
								$index_composite++;
							}
							if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
								# processes the next moveto and lineto
								do {
									for(my $ind = 0; $ind < 2 ; $ind++ ) {
										$composite_path_data[$index_composite] = $path_data[$index];
										$index_composite++;
										$index++;
									}
								}
								while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
								$current_point_X = $path_data[$index -2];
								$current_point_Y = $path_data[$index -1];
							}
							elsif($path_data[$index] eq "C") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
								# processes the next curve segments
								# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
								do {
									for(my $ind = 0; $ind < 6 ; $ind++ ) {
										$composite_path_data[$index_composite] = $path_data[$index];
										$index_composite++;
										$index++;
									}
								}
								while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
								$current_point_X = $path_data[$index -2];
								$current_point_Y = $path_data[$index -1];
							}
							elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
							}
							$fist_video_choice = 0;
						}
						# takes a segment from the second video
						elsif(($second_index < $second_path_data_size && $second_index <  $max_index_video_2)
							&& (($fist_video_choice == 0) 
								|| !($index < $path_data_size && $index <  $max_index_video_1))) {
							if($second_current_point_X != -1 && $second_current_point_Y != -1) {
								$composite_path_data[$index_composite] = "M";
								$index_composite++;
								$composite_path_data[$index_composite] = $second_current_point_X;
								$index_composite++;
								$composite_path_data[$index_composite] = $second_current_point_Y;
								$index_composite++;
							}
							if($second_path_data[$second_index] eq "M" || $second_path_data[$second_index] eq "L") {
								$composite_path_data[$index_composite] = $second_path_data[$second_index];
								$index_composite++;
								$second_index++;
								# processes the next moveto and lineto
								do {
									for(my $ind = 0; $ind < 2 ; $ind++ ) {
										$composite_path_data[$index_composite] = $second_path_data[$second_index];
										$index_composite++;
										$second_index++;
									}
								}
								while($second_index < $second_path_data_size && !($second_path_data[$second_index] =~ /^[MCLZz]/));
								$second_current_point_X = $second_path_data[$second_index -2];
								$second_current_point_Y = $second_path_data[$second_index -1];
							}
							elsif($second_path_data[$second_index] eq "C") {
								$composite_path_data[$index_composite] = $second_path_data[$second_index];
								$index_composite++;
								$second_index++;
								# processes the next curve segments
								# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
								do {
									for(my $ind = 0; $ind < 6 ; $ind++ ) {
										$composite_path_data[$index_composite] = $second_path_data[$second_index];
										$index_composite++;
										$second_index++;
									}
								}
								while($second_index < $second_path_data_size && !($second_path_data[$second_index] =~ /^[MCLZz]/));
								$second_current_point_X = $second_path_data[$second_index -2];
								$second_current_point_Y = $second_path_data[$second_index -1];
							}
							elsif($second_path_data[$second_index] eq "Z" || $second_path_data[$second_index] eq "z") {
								$composite_path_data[$index_composite] = $second_path_data[$second_index];
								$index_composite++;
								$second_index++;
							}
							$fist_video_choice = 1;
						}
						# the two videos are over
						else {
							last;
						}
					}
					# if necessary ends up with a z
					if($index_composite >= 1) {
						if(!($composite_path_data[$index_composite - 1] =~ /^[Zz]/)) {
							$composite_path_data[$index_composite] = "Z";
							$index_composite++;
						}
					}
				}
				# in this crossfading mode, the curves are taken alternatively from each video to
				# ensure a maximal mixture of both videos
				elsif($transformation_function == CROSSFADE_ALTERNATE) {
					$index = 0;
					my $second_index = 0;
					my $fist_video_choice = 1;
					my $current_point_X = -1;
					my $current_point_Y = -1;
					my $second_current_point_X = -1;
					my $second_current_point_Y = -1;

					# copies the number of items
					while(($index < $path_data_size) || ($second_index < $second_path_data_size)) {
						# print "index size stopsize $index $path_data_size $max_index_video_1 / index size stopsize $second_index $second_path_data_size $max_index_video_2\n";

						# takes a segment from the first video if it is not finished or if the second one is finished
						if(($index < $path_data_size) 
							&& (($fist_video_choice == 1) || !($second_index < $second_path_data_size))) {
							if($current_point_X != -1 && $current_point_Y != -1) {
								$composite_path_data[$index_composite] = "M";
								$index_composite++;
								$composite_path_data[$index_composite] = $current_point_X;
								$index_composite++;
								$composite_path_data[$index_composite] = $current_point_Y;
								$index_composite++;
							}
							if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
								# processes the next moveto and lineto
								do {
									for(my $ind = 0; $ind < 2 ; $ind++ ) {
										$composite_path_data[$index_composite] = $path_data[$index];
										$index_composite++;
										$index++;
									}
								}
								while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
								$current_point_X = $path_data[$index -2];
								$current_point_Y = $path_data[$index -1];
							}
							elsif($path_data[$index] eq "C") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
								# processes the next curve segments
								# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
								do {
									for(my $ind = 0; $ind < 6 ; $ind++ ) {
										$composite_path_data[$index_composite] = $path_data[$index];
										$index_composite++;
										$index++;
									}
								}
								while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
								$current_point_X = $path_data[$index -2];
								$current_point_Y = $path_data[$index -1];
							}
							elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
							}
							$fist_video_choice = 0;
						}
						# takes a segment from the second video
						elsif(($second_index < $second_path_data_size)
							&& (($fist_video_choice == 0) || !($index < $path_data_size))) {
							if($second_current_point_X != -1 && $second_current_point_Y != -1) {
								$composite_path_data[$index_composite] = "M";
								$index_composite++;
								$composite_path_data[$index_composite] = $second_current_point_X;
								$index_composite++;
								$composite_path_data[$index_composite] = $second_current_point_Y;
								$index_composite++;
							}
							if($second_path_data[$second_index] eq "M" || $second_path_data[$second_index] eq "L") {
								$composite_path_data[$index_composite] = $second_path_data[$second_index];
								$index_composite++;
								$second_index++;
								# processes the next moveto and lineto
								do {
									for(my $ind = 0; $ind < 2 ; $ind++ ) {
										$composite_path_data[$index_composite] = $second_path_data[$second_index];
										$index_composite++;
										$second_index++;
									}
								}
								while($second_index < $second_path_data_size && !($second_path_data[$second_index] =~ /^[MCLZz]/));
								$second_current_point_X = $second_path_data[$second_index -2];
								$second_current_point_Y = $second_path_data[$second_index -1];
							}
							elsif($second_path_data[$second_index] eq "C") {
								$composite_path_data[$index_composite] = $second_path_data[$second_index];
								$index_composite++;
								$second_index++;
								# processes the next curve segments
								# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
								do {
									for(my $ind = 0; $ind < 6 ; $ind++ ) {
										$composite_path_data[$index_composite] = $second_path_data[$second_index];
										$index_composite++;
										$second_index++;
									}
								}
								while($second_index < $second_path_data_size && !($second_path_data[$second_index] =~ /^[MCLZz]/));
								$second_current_point_X = $second_path_data[$second_index -2];
								$second_current_point_Y = $second_path_data[$second_index -1];
							}
							elsif($second_path_data[$second_index] eq "Z" || $second_path_data[$second_index] eq "z") {
								$composite_path_data[$index_composite] = $second_path_data[$second_index];
								$index_composite++;
								$second_index++;
							}
							$fist_video_choice = 1;
						}
						# the two videos are over
						else {
							last;
						}
					}
					# if necessary ends up with a z
					if($index_composite >= 1) {
						if(!($composite_path_data[$index_composite - 1] =~ /^[Zz]/)) {
							$composite_path_data[$index_composite] = "Z";
							$index_composite++;
						}
					}
				}
				# in these crossfading modes, the curves are taken first from the first video and
				# second from the second one (contrary to the alternate mode)
				# the mixture is fine if we possibly stop the copy inside a curve if we have
				# reached the number of items corresponding to the current crossfading percentage
				# in the CROSSFADE mode, we only copy full primitives (curves or lines) from each video
				else {
					######################################
					# copies first video
					if($transformation_function == CROSSFADE) {
						# copies the number of items
						while($index < $path_data_size) {
							$composite_path_data[$index_composite] = $path_data[$index];
							$index_composite++;
							$index++;
						}
						# and then goes to the next non numerical value
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/)) {
							$composite_path_data[$index_composite] = $path_data[$index];
							$index_composite++;
							$index++;
						}
					}
					elsif($transformation_function == CROSSFADE_FINE) {
						# copies the number of items
						while($index < $path_data_size && $index <  $max_index_video_1) {
							if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
								# processes the next moveto and lineto
								do {
									for(my $ind = 0; $ind < 2 ; $ind++ ) {
										$composite_path_data[$index_composite] = $path_data[$index];
										$index_composite++;
										$index++;
									}
								}
								while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
							}
							elsif($path_data[$index] eq "C") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
								# processes the next curve segments
								# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
								do {
									for(my $ind = 0; $ind < 6 ; $ind++ ) {
										$composite_path_data[$index_composite] = $path_data[$index];
										$index_composite++;
										$index++;
									}
								}
								while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
							}
							elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
								$composite_path_data[$index_composite] = $path_data[$index];
								$index_composite++;
								$index++;
							}
						}
					}
					# if necessary ends up with a z
					if($index_composite >= 1) {
						if(!($composite_path_data[$index_composite - 1] =~ /^[Zz]/)) {
							$composite_path_data[$index_composite] = "Z";
							$index_composite++;
						}
					}

					######################################
					# copies second video
					$index = 0;
					if($transformation_function == CROSSFADE) {
						# copies the number of items
						while($index < $second_path_data_size) {
							$composite_path_data[$index_composite] = $second_path_data[$index];
							$index_composite++;
							$index++;
						}
						# and then goes to the next non numerical value
						while($index < $second_path_data_size && !($second_path_data[$index] =~ /^[MCLZz]/)) {
							$composite_path_data[$index_composite] = $second_path_data[$index];
							$index_composite++;
							$index++;
						}
					}
					elsif($transformation_function == CROSSFADE_FINE) {
						# copies the number of items
						while($index < $second_path_data_size && $index <  $max_index_video_2) {
							if($second_path_data[$index] eq "M" || $second_path_data[$index] eq "L") {
								$composite_path_data[$index_composite] = $second_path_data[$index];
								$index_composite++;
								$index++;
								# processes the next moveto and lineto
								do {
									for(my $ind = 0; $ind < 2 ; $ind++ ) {
										$composite_path_data[$index_composite] = $second_path_data[$index];
										$index_composite++;
										$index++;
									}
								}
								while($index < $second_path_data_size && !($second_path_data[$index] =~ /^[MCLZz]/));
							}
							elsif($second_path_data[$index] eq "C") {
								$composite_path_data[$index_composite] = $second_path_data[$index];
								$index_composite++;
								$index++;
								# processes the next curve segments
								# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
								do {
									for(my $ind = 0; $ind < 6 ; $ind++ ) {
										$composite_path_data[$index_composite] = $second_path_data[$index];
										$index_composite++;
										$index++;
									}
								}
								while($index < $second_path_data_size && !($second_path_data[$index] =~ /^[MCLZz]/));
							}
							elsif($second_path_data[$index] eq "Z" || $second_path_data[$index] eq "z") {
								$composite_path_data[$index_composite] = $second_path_data[$index];
								$index_composite++;
								$index++;
							}
						}
					}
					# if necessary ends up with a z
					if($index_composite >= 1) {
						if(!($composite_path_data[$index_composite - 1] =~ /^[Zz]/)) {
							$composite_path_data[$index_composite] = "Z";
							$index_composite++;
						}
					}
				}

				# copies the new path into the old one which will be used for output
				@path_data = @composite_path_data;
			}
			##################################################################
			# SMOOTHING AND/OR SIMPLIFICATION OF THE BEZIER CURVES
			##################################################################
			elsif($transformation_function == SMOOTHING
				|| $transformation_function == BLUR) {
				my $index = 0;
				my $smooting_percentage = $percent;

				# simplifies the curve by removing segments according to percentage
				if($transformation_function == BLUR){
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
					while($index < $path_data_size) {
						if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
							$simplified_path_data[$index_simplified] = $path_data[$index];
							$index_simplified++;
							$index++;
							# copies the next moveto and lineto
							do {
								$simplified_path_data[$index_simplified] = $path_data[$index];
								$index_simplified++;
								$index++;
								$simplified_path_data[$index_simplified] = $path_data[$index];
								$index_simplified++;
								$index++;
							}
							while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
						}
						elsif($path_data[$index] eq "C") {
							$simplified_path_data[$index_simplified] = $path_data[$index];
							$index_simplified++;
							$index++;
							my $curve_rank = 0;
							do {
								# copies the next curve segments
								# if it has the rank to be preserved (the n-1th if every nth is preserved)
								# or if it is the last one
								if( ((int($curve_rank) % int($preserved_multiple_curve_segment))
									== (int($preserved_multiple_curve_segment) - 1))
									|| (($index + 6 < $path_data_size)
										&& ($path_data[$index + 6] =~ /^[MCLZz]/)) ) {
									for(my $ind = 0; $ind < 6 ; $ind++ ) {
										$simplified_path_data[$index_simplified] = $path_data[$index];
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
							while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
						}
						elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
							$simplified_path_data[$index_simplified] = $path_data[$index];
							$index_simplified++;
							$index++;
						}
						else {
							print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
						}
					}
					@path_data = @simplified_path_data;
					$path_data_size = @path_data + 0;
				}

				# smoothes the curve with symmetrical control points around a final control point
				$index = 0;
				while($index < $path_data_size) {
					if($path_data[$index] eq "M" || $path_data[$index] eq "L") {
						$index++;
						# passes the next moveto and lineto, no smoothing takes lace here
						do {
							$index += 2;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "C") {
						$index++;
						my $initial_polycurve_index = $index;
						# processes the preceding curve segments (and requires two previous curve segments)
						do {
							if(($index - $initial_polycurve_index) >= 12){
								# the vectors joining the last control points of 2 neighboring segments
								my $vec_In_1In = V($path_data[$index] - $path_data[$index - 6], $path_data[$index + 1] - $path_data[$index - 6 + 1]);
								my $vec_In_1In_2 = V($path_data[$index - 12] - $path_data[$index - 6], $path_data[$index - 12 + 1] - $path_data[$index - 6 + 1]);
								# and their norms
								my $norm_In_1In = abs($vec_In_1In);
								my $norm_In_1In_2 = abs($vec_In_1In_2);
								# the central last control point
								my $point_In = V($path_data[$index - 6], $path_data[$index - 6 + 1]);
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
										$path_data[$index - 6 - 2] = (1 - $smooting_percentage) * $path_data[$index - 6 - 2] 
																	+ $smooting_percentage * ($point_In->[0] + $vec_tang_In->[0]);
										$path_data[$index - 6 - 2 + 1] = (1 - $smooting_percentage) * $path_data[$index - 6 - 2 + 1] 
																	+ $smooting_percentage * ($point_In->[1] + $vec_tang_In->[1]);
										# the control point after In-1
										$path_data[$index - 6 + 2] = (1 - $smooting_percentage) * $path_data[$index - 6 + 2] 
																	+ $smooting_percentage * ($point_In->[0] - $vec_tang_In->[0]);
										$path_data[$index - 6 + 2 + 1] = (1 - $smooting_percentage) * $path_data[$index - 6 + 2 + 1] 
																	+ $smooting_percentage * ($point_In->[1] - $vec_tang_In->[1]);
									}
								}
							}
							$index += 6;
						}
						while($index < $path_data_size && !($path_data[$index] =~ /^[MCLZz]/));
					}
					elsif($path_data[$index] eq "Z" || $path_data[$index] eq "z") {
						$index++;
					}
					else {
						print "Unexpected sign at index $index: [", $path_data[$index], "]\n";
					}
				}
			}
			# prints the modified path
			print $FILEout "<path d=\"", join(' ', @path_data), "\"/>\n";
		}
		else {
			die "Path data not found in path ", $line, "\n";
		}
		$path_no++;
	}
	##########################################################################
	# non path and non group line reprinted
	##########################################################################
	else {
		print $FILEout $line, "\n";
	}
}

close($FILEin);
close($FILEout);
