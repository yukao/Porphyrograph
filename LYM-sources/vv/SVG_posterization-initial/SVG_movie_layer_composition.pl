#!/usr/bin/perl 

use strict;
use warnings;

# SVG_movie_layer_composition_image.pl
# SVG_movie_layer_compositions an image into another image or into a SVG vector graphics

# usage
# SVG_movie_layer_composition_image.pl --output-format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

# IN
#
#              n paths

# OUT 
#
#              coposited and colored image

my $USAGE =
"usage: SVG_movie_layer_composition_image.pl --output-format (SVG|PATHS|JPG|PNG) --nb-files <integer> -o <file_name> -i <file_name>+
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_layer_composition_image.pl --output-format SVG --nb-files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg -c \"#cbcbcb\" \"#9a9a9a\" \"#6b6b6b\" \"#1c1c1c\"

ARGUMENTS:
   --output-format: output format of the composited image (SVG, paths for GPU shipping, image)
   --nb_files: number of input images
   -o: ouput files 
   -i: input files
   -c: input colors
";

my $numArgs;
$numArgs = $#ARGV + 1;

use constant {
    SVG   => 1,
    PNG   => 2,
    JPG  => 3,
    PATHS  => 4,
};
my %file_format = (
   "SVG" => SVG,
   "PNG" => PNG,
   "JPG" => JPG,
   "PATHS" => PATHS,
);

my $argnum;
my $output_format;
my $nb_files;
my $output_file_name;
my @input_file_names = ();
my @path_colors = ();

if( @ARGV < 10 ) {
    print $USAGE; die "minimal number of arguments: 10 not ", 0+@ARGV;
}

# output_format
if( $ARGV[0] ne "--output-format" ) {
    print $USAGE; die "--output-format expected for argt 1";
}
if (!$file_format{$ARGV[1]}) {
    print $USAGE; die $ARGV[1]," does not exist in the hash %file_format\n";
}
$output_format = $file_format{$ARGV[1]};

# nb_files
if( $ARGV[2] ne "--nb-files" ) {
    print $USAGE; die "--nb-files expected for argt 3";
}
if ($ARGV[3] < 1) {
    print $USAGE; die $ARGV[3]," 1 path file should be provided minimally\n";
}
$nb_files = $ARGV[3];
if( @ARGV != (6 + 2 * ($nb_files + 1)) ) {
    print $USAGE; die "Incorrect number of arguments, expected",(6 + 2 * ($nb_files + 1)) ,"\n";
}

# output_file_name
if( $ARGV[4] ne "-o" ) {
    print $USAGE; die "-o expected for argt 5";
}
$output_file_name = $ARGV[5];

# input_file_names
if( $ARGV[6] ne "-i" ) {
    print $USAGE; die "-i expected for argt 7";
}
for(my $i = 0 ; $i < $nb_files ; $i++) {
	$input_file_names[$i] = $ARGV[6 + 1 + $i];
}

# path_colors
if( $ARGV[6 + 1 + $nb_files] ne "-c" ) {
    print $USAGE; die "-c expected for argt ",6 + 1 + $nb_files, " not ", $ARGV[6 + 1 + $nb_files], "\n";
}
for(my $i = 0 ; $i < $nb_files ; $i++) {
	$path_colors[$i] = $ARGV[6 + 1 + $nb_files + 1 + $i];
}

my $FILEout;
my $FILEin;
print "Generating ", $ARGV[1], " file ", $output_file_name,"\n";
open(  $FILEout , ">$output_file_name" ) 
	|| die "Can't open file $output_file_name!";
if($output_format == SVG) {
	print $FILEout "<?xml version=\"1.0\" standalone=\"no\"?>
<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"
 \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">
<svg version=\"1.0\" xmlns=\"http://www.w3.org/2000/svg\"
 width=\"3860.000000pt\" height=\"2180.000000pt\" viewBox=\"0 0 3860.000000 2180.000000\"
 preserveAspectRatio=\"xMidYMid meet\">
<metadata>
Created by potrace 1.13, written by Peter Selinger 2001-2015
</metadata>
";
}
my $line;
for(my $i = 0 ; $i < $nb_files ; $i++) {
	open($FILEin , "<".$input_file_names[$i]) 
		|| die "Can't open file ".$input_file_names[$i]."!";
    while($line = <$FILEin>){
		if( $line =~ /^<g/ ) {
			chomp($line);
			if($line =~ /fill\=\"\#000000\"/) {
				$line =~ s/fill\=\"\#000000\"/style="fill:#$path_colors[$i];"/;
			}
			if($output_format == SVG) {
				if(!($line =~ / $/)) {
					$line = $line . " ";
				}
		    	print $FILEout $line;
		    }
		    while( !($line =~ />/) ) {
		    	$line = <$FILEin>;
				chomp($line);
				if($line =~ /fill\=\"\#000000\"/) {
					$line =~ s/fill\=\"\#000000\"/style="fill:$path_colors[$i];"/;
				}
				if($output_format == SVG) {
			    	print $FILEout $line;
			    }
        		if (eof($FILEin)) { 
        			last;
        		}
		    }
	    	print $FILEout "\n";
		}
		elsif( $line =~ /^<path/ ) {
			if($output_format == PATHS) {
		    	$line =~ s/<path/<path style="fill:$path_colors[$i];"/;
		    }
			if($output_format == PATHS || $output_format == SVG) {
				chomp($line);
		    	print $FILEout $line;
		    }
		    while( !($line =~/\/>/) ) {
		    	$line = <$FILEin>;
				if($output_format == PATHS || $output_format == SVG) {
					chomp($line);
			    	print $FILEout " " , $line;
			    }
        		if (eof($FILEin)) { 
        			last;
        		}
		    }
			if($output_format == PATHS || $output_format == SVG) {
		    	print $FILEout "\n";
		    }
	}
		elsif( $line =~/^<\/g>/ ) {
			if($output_format == SVG) {
		    	print $FILEout $line;
		    }
		}
    }
	close($FILEin);
}
if($output_format == SVG) {
	print  $FILEout "</svg>\n";
}
close($FILEout);
