#!/usr/bin/perl 

use strict;
use warnings;

package SVG_movie_layer_crossfading;

# SVG_movie_layer_crossfading.pl
# SVG_movie_layer_compositions an image into another image or into a SVG vector graphics

# usage
# SVG_movie_layer_crossfading.pl --output-format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

# IN
#
#              n paths

# OUT 
#
#              coposited and colored image

my $USAGE =
"usage: SVG_movie_layer_crossfading.pl --output-format (SVG|PATHS|JPG|PNG) --nb-files <integer> -o <file_name> -i <file_name>+
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_layer_crossfading.pl --output-format SVG --nb-files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg -c \"#cbcbcb\" \"#9a9a9a\" \"#6b6b6b\" \"#1c1c1c\"

ARGUMENTS:
   --composition-mode: composition mode of the composited image (SEQUENCE or PARALLEL)
   --nb-files: number of input SVG files
   --nb-layers: number of layers in the SVG files
   -o: ouput file 
   -i: input file
";

##################################################################
# MAIN SUB
##################################################################
sub MAIN {
	my $numArgs;
	$numArgs = @_ + 1;

	use constant {
	    SEQUENCE   => 1,
	    PARALLEL   => 2,
	};
	my %composition_mode_list = (
	   "SEQUENCE" => SEQUENCE,
	   "PARALLEL" => PARALLEL,
	);

	my $argnum;
	my $nb_files;
	my $nb_layers;
	my $composition_mode;
	my $output_file_name;
	my @input_file_names = ();

	if( @_ < 6 ) {
	    print $USAGE; 
	    print "minimal number of arguments: 6 not ", 0+@_, "\n";
		return 0;
	}

	# composition_mode
	if( $_[0] ne "--composition-mode" ) {
	    print $USAGE; 
	    print "--composition-mode expected for argt 1\n";
		return 0;
	}
	if (!$composition_mode_list{$_[1]}) {
	    print $USAGE; 
	    print $_[1]," does not exist in the hash %composition_mode_list\n";
		return 0;
	}
	$composition_mode = $composition_mode_list{$_[1]};

	# nb_files
	if( $_[2] ne "--nb-files" ) {
	    print $USAGE; 
	    print "--nb-files expected for argt 3\n";
		return 0;
	}
	if ($_[3] < 1) {
	    print $USAGE; 
	    print $_[3]," 1 path file should be provided minimally\n";
		return 0;
	}
	$nb_files = $_[3];
	if( @_ != (8 + ($nb_files + 1)) ) {
	    print $USAGE; 
	    print "Incorrect number of arguments, for ", $nb_files, " files expected ",(8 + ($nb_files + 1)) ," arguments not ", (@_+0) ,"\n";
		return 0;
	}

	# nb_layers
	if( $_[4] ne "--nb-layers" ) {
	    print $USAGE; 
	    print "--nb-layers expected for argt 5\n";
		return 0;
	}
	if ($_[5] < 1) {
	    print $USAGE; 
	    print $_[5]," 1 layer should be provided minimally\n";
		return 0;
	}
	$nb_layers = $_[5];

	# output_file_name
	if( $_[6] ne "-o" ) {
	    print $USAGE; 
	    print "-o expected for argt 7\n";
		return 0;
	}
	$output_file_name = $_[7];

	# input_file_names
	if( $_[8] ne "-i" ) {
	    print $USAGE; 
	    print "-i expected for argt 9\n";
		return 0;
	}
	for(my $i = 0 ; $i < $nb_files ; $i++) {
		$input_file_names[$i] = $_[8 + 1 + $i];
	}

	my $FILEout;
	my $FILEin;
	if(open(  $FILEout , ">$output_file_name" ) == 0) {
		print "Can't open file $output_file_name!\n";
		return 0;
	}
	# radial gradient is for the beam lines of the transformation C_LINES
	print $FILEout "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>
	<svg
	   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"
	   xmlns:cc=\"http://creativecommons.org/ns#\"
	   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"
	   xmlns:svg=\"http://www.w3.org/2000/svg\"
	   xmlns=\"http://www.w3.org/2000/svg\"
	   xmlns:xlink=\"http://www.w3.org/1999/xlink\"
	   id=\"svg284\"
	   preserveAspectRatio=\"xMidYMid meet\"
	   viewBox=\"0 0 3860.000000 2180.000000\"
	   height=\"2180.000000pt\"
	   width=\"3860.000000pt\"
	   version=\"1.0\">
	  <defs
	     id=\"defs124\">
	    <linearGradient
	       id=\"linearGradient001\">
	      <stop
	         id=\"stop951\"
	         offset=\"0\"
	         style=\"stop-color:#000000;stop-opacity:0.15\" />
	      <stop
	         id=\"stop953\"
	         offset=\"1\"
	         style=\"stop-color:#000000;stop-opacity:1\" />
	    </linearGradient>
	    <radialGradient
	       gradientUnits=\"userSpaceOnUse\"
	       gradientTransform=\"matrix(1.3700451,-0.02534341,0.01423842,0.76971741,-7276.1383,2027.7884)\"
	       r=\"14030.874\"
	       fy=\"12028.837\"
	       fx=\"19200\"
	       cy=\"12028.837\"
	       cx=\"19200\"
	       id=\"radialGradient001\"
	       xlink:href=\"#linearGradient001\" />
	  </defs>
	  <metadata id=\"metadata7\">
	Created by potrace 1.13, written by Peter Selinger 2001-2015
	</metadata>
	";
	my $line;
	my $nb_groups = 0;
	my @parallel_lines = ();
	my $sequenced_lines = "";
	# lines produced by C_LINES transformation
	my $sequenced_beam_lines = "";
	my $is_beam_group = 0;
	for(my $i = 0 ; $i < $nb_files ; $i++) {
		# print "input file $i: ", $input_file_names[$i] , "\n";
		if(!open($FILEin , '<:crlf', "<".$input_file_names[$i])) {
			if(!open($FILEin , "<".$input_file_names[$i])) {
				print "Can't open file ".$input_file_names[$i]."!\n";
				return 0;
			}
		}

		# composition in sequence: the image paths are piled: first the paths of image 1 from gray
		# to black then the paths of image 2, etc.
		# the underlying images can fill the blank spaces of the upper ones
		if( $composition_mode == SEQUENCE) {
		    while($line = <$FILEin>){
				if( $line =~ /<g/ ) {
					my $out_line = "";
					$is_beam_group = 0;
					chomp($line);
					if(!($line =~ / $/)) {
						$line = $line . " ";
					}
					# lines produced by C_LINES transformation have a radial gradient
					if($line =~ /radialGradient/) {
						$is_beam_group = 1;
					}
					$out_line = $out_line . $line;
				    while( !($line =~ />/) ) {
				    	$line = <$FILEin>;
						chomp($line);
						# lines produced by C_LINES transformation have a radial gradient
						if($line =~ /radialGradient/) {
							$is_beam_group = 1;
						}
						$out_line = $out_line . $line;
		        		if (eof($FILEin)) { 
		        			last;
		        		}
				    }
					if($is_beam_group) {
						$sequenced_beam_lines = $sequenced_beam_lines . $out_line . "\n";
					}
					else {
						$sequenced_lines = $sequenced_lines . $out_line . "\n";
					}
				}
				elsif( $line =~ /<path/ ) {
					my $out_line = "";
					chomp($line);
					$out_line = $out_line . $line;
				    while( !($line =~/\/>/) ) {
				    	$line = <$FILEin>;
						chomp($line);
						$out_line = $out_line . " " . $line;
		        		if (eof($FILEin)) { 
		        			last;
		        		}
				    }
					if($is_beam_group) {
						$sequenced_beam_lines = $sequenced_beam_lines . $out_line . "\n";
					}
					else {
						$sequenced_lines = $sequenced_lines . $out_line . "\n";
					}
				}
				elsif( $line =~/<\/g>/ ) {
					if($is_beam_group) {
						$sequenced_beam_lines = $sequenced_beam_lines . $line;
					}
					else {
						$sequenced_lines = $sequenced_lines . $line;
					}
				}
		    }
			close($FILEin);
		}
		# the composition mode in parallel leads to a different result: 
		# each color layer is combined between all the files.
		elsif( $composition_mode == PARALLEL) {
			my $ind_group = 0;
		    while($line = <$FILEin>){
				if( $line =~ /<g/ ) {
					my $out_line = "";
					$is_beam_group = 0;
					# string initialization
	  				$parallel_lines[$ind_group]  = "" unless defined $parallel_lines[$ind_group]; 
	  				chomp($line);
					if(!($line =~ / $/)) {
						$line = $line . " ";
					}
					# lines produced by C_LINES transformation have a radial gradient
					if($line =~ /radialGradient/) {
						$is_beam_group = 1;
					}
			    	# print $FILEout $line;
					$out_line = $out_line . $line;
				    while( !($line =~ />/) ) {
				    	$line = <$FILEin>;
						chomp($line);
						# lines produced by C_LINES transformation have a radial gradient
						if($line =~ /radialGradient/) {
							$is_beam_group = 1;
						}
					    # print $FILEout $line;
						$out_line = $out_line . $line;
		        		if (eof($FILEin)) { 
		        			last;
		        		}
				    }
					if($is_beam_group) {
						$sequenced_beam_lines = $sequenced_beam_lines . $out_line . "\n";
					}
					else {
						$parallel_lines[$ind_group] = $parallel_lines[$ind_group] . $out_line . "\n";
					}
				    # print $FILEout "\n";
				}
				elsif( $line =~ /<path/ ) {
					my $out_line = "";
					chomp($line);
			    	# print $FILEout $line;
					$out_line = $out_line . $line;
				    while( !($line =~/\/>/) ) {
				    	$line = <$FILEin>;
						chomp($line);
						$out_line = $out_line . " " . $line;
				    	# print $FILEout " " , $line;
		        		if (eof($FILEin)) { 
		        			last;
		        		}
				    }
					if($is_beam_group) {
						$sequenced_beam_lines = $sequenced_beam_lines . $out_line . "\n";
					}
					else {
						$parallel_lines[$ind_group] = $parallel_lines[$ind_group] . $out_line . "\n";
					}
				    # print $FILEout "\n";
				}
				elsif( $line =~/<\/g>/ ) {
					if($is_beam_group) {
						$sequenced_beam_lines = $sequenced_beam_lines . $line;
					}
					else {
						$parallel_lines[$ind_group] = $parallel_lines[$ind_group] . $line;
					}
			    	# print $FILEout $line;
					$ind_group++;
				}
		    }
			close($FILEin);
			if( $nb_groups < $ind_group ) {
				$nb_groups = $ind_group;
			}
		}
	}
	# lines produced by C_LINES transformation are displayed first so that the characters are on top of them
	print $FILEout $sequenced_beam_lines;
	if( $composition_mode == PARALLEL) {
		for(my $ind_group = 0 ; $ind_group < $nb_groups ; $ind_group++) {
			print $FILEout $parallel_lines[$ind_group];
		}
	}
	elsif( $composition_mode == SEQUENCE) {
		print $FILEout $sequenced_lines;
	}
	print  $FILEout "</svg>\n";
	close($FILEout);

	return 1;
}