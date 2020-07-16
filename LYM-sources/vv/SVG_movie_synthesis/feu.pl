#!/usr/bin/perl

use strict;
use warnings;

use SVG;
use POSIX;
use Math::Trig;
use List::Util qw(min max);

my $width = 1920;
my $height = 1080;

my $number_of_images = 300; # 1000
my $average_number_of_paths_in_a_flame = 30; # 1000
my $number_of_paths_in_a_flame; # 1000
my $number_of_flames = 100;

my $indPoint;
my @pathsIniCP_x=();
my @pathsIniCP_y=();
my $ind_flame;
my @redIni = ();
my @greenIni = ();
my @blueIni = ();

my @flameHeightIni = ();
my @flameWidthIni = ();
my @flameWidthBaseIni = ();

my @backboneIni_x = ();
my @backboneIni_y = ();
my @curvatureIni = ();

# generates number_of_flames Bezier curves different sizes and orientations and transparent
for($ind_flame = 0; $ind_flame < $number_of_flames; $ind_flame++) {
    # add a path to the group
    # color
    $redIni[$ind_flame] = 180 + int(rand(80) - 40);
    $greenIni[$ind_flame] = 100 + int(rand(80) - 40);
    $blueIni[$ind_flame] = int(rand(40));
    # saturation
    my $maxCol = max(max($redIni[$ind_flame],$greenIni[$ind_flame]),$blueIni[$ind_flame]);
    $redIni[$ind_flame] += 255 - $maxCol;
    $greenIni[$ind_flame] += 255 - $maxCol;
    $blueIni[$ind_flame] += 255 - $maxCol;

    # initial random control points
    # the number of points has to be a multiple of 3 for building cuves
    $flameHeightIni[$ind_flame] = rand( $height );
    $flameWidthBaseIni[$ind_flame] = rand( $width / 20 ) + $width / 20;

    # position of the flame along the width
    $backboneIni_x[$ind_flame][0] = rand( $width );
    $backboneIni_y[$ind_flame][0] = 0.;

    # backbone
    for($indPoint = 0; $indPoint <= 36; $indPoint++) {
    	# angle of rotation along the flame step by step
        $curvatureIni[$ind_flame][$indPoint] = rand(2) - 1.;
        # flame width goes decreasing along the steps
        $flameWidthIni[$ind_flame][$indPoint] = $flameWidthBaseIni[$ind_flame] - $indPoint * ($flameWidthBaseIni[$ind_flame]/36);
        # adds a random factor of width at each step
        $flameWidthIni[$ind_flame][$indPoint] = rand($flameWidthIni[$ind_flame][$indPoint] / 2) + ($flameWidthIni[$ind_flame][$indPoint] / 2);
        # defines the backbone along which the flame is defined
        # a fraction of the growth + a small variation
        if($indPoint > 0) {
	        $backboneIni_y[$ind_flame][$indPoint] = ($flameHeightIni[$ind_flame] / 36) * $indPoint 
	        	+ rand($flameHeightIni[$ind_flame] / 36 / 2) - ($flameHeightIni[$ind_flame] / 36 / 4);
        }
        # position of the next step according to the curvature and the position of the preceding step
	    $backboneIni_x[$ind_flame][$indPoint + 1] 
	        = $backboneIni_x[$ind_flame][$indPoint] + $curvatureIni[$ind_flame][$indPoint] * rand($flameWidthIni[$ind_flame][$indPoint]);
    }

}

# create an SVG object with a size of 200x200 pixels
my $svg = SVG->new(
    width  => $width,
    height => $height,
);
$svg->title()->cdata('multiple paths');
$svg->desc()->cdata('multiple paths');

# use explicit element constructor to generate a group element
my $y = $svg->group(
    id    => 'group_y',
    style => {
        stroke => 'black',
        fill   =>'black',
    },
);

my $imageRank = 0;
my $fileName = sprintf("images/feu%04d.svg", $imageRank); 
my $OutFile;

open($OutFile, '>' , $fileName);

# now render the SVG object, implicitly use svg namespace
print $OutFile $svg->xmlify;
close($OutFile);

# next images for animation
my @pathsCP_x=();
my @pathsCP_y=();

my @backbone_x=();
my @backbone_y=();
my @curvature=();

my @flameWidth=();
my @flameHeight=();

my @red = ();
my @green = ();
my @blue = ();

# render a sequence of images based on variations of the initial one
for($imageRank = 0; $imageRank < $number_of_images; $imageRank++) {
	$fileName = sprintf("images/feu%04d.svg", $imageRank); 

	$svg = SVG->new(
	    width  => $width,
	    height => $height,
	);
	$svg->title()->cdata('multiple paths');
	$svg->desc()->cdata('multiple paths');

	# use explicit element constructor to generate a group element
	$y = $svg->group(
	    id    => 'group_y',
	    style => {
	        stroke => 'black',
	        fill   =>'black',
	    },
	);

	# generates number_of_flames Bezier curves different sizes and orientations and transparent
	for($ind_flame = 0; $ind_flame < $number_of_flames; $ind_flame++) {

	    # backbone variation
	    # if($imageRank == 0) {
    	# 	$flameHeight[$ind_flame] = $flameHeightIni[$ind_flame];
    	# }
	    # else {
    	# 	$flameHeight[$ind_flame] += (rand(1) - 0.5) * $flameHeight[$ind_flame];
    	# }
	  #   for($indPoint = 0; $indPoint < 36; $indPoint++) {
		 #    if($imageRank == 0) {
		 #    	# angle of rotation along the flame step by step
		 #        $curvature[$ind_flame][$indPoint] = $curvatureIni[$ind_flame][$indPoint];
		 #        # flame width goes decreasing along the steps
		 #        $flameWidth[$ind_flame][$indPoint] = $flameWidthIni[$ind_flame][$indPoint];
		 #        # defines the backbone along which the flame is defined
		 #        # a fraction of the growth + a small variation
			#     $backbone_x[$ind_flame][$indPoint] = $backboneIni_x[$ind_flame][$indPoint];
		 #        $backbone_y[$ind_flame][$indPoint] = $backboneIni_y[$ind_flame][$indPoint];
		 #    }
		 #    else {
		 #    	# angle of rotation along the flame step by step
		 #        $curvature[$ind_flame][$indPoint] = $curvature[$ind_flame][$indPoint] + (rand(2) - 1);
		 #        $curvature[$ind_flame][$indPoint] = max(min($curvature[$ind_flame][$indPoint],1.5),-1.5);

		 #        # flame width goes decreasing along the steps
		 #        $flameWidth[$ind_flame][$indPoint] += (rand(0.4) - 0.2) * $flameWidth[$ind_flame][$indPoint];
		 #        $flameWidth[$ind_flame][$indPoint] = max(min($flameWidth[$ind_flame][$indPoint],$flameWidthIni[$ind_flame][$indPoint]*4),
		 #        										$flameWidthIni[$ind_flame][$indPoint]*1);

		 #        # defines the backbone along which the flame is defined
		 #        # a fraction of the growth + a small variation
		 #        # position of the next step according to the curvature and the position of the preceding step
			#     $backbone_x[$ind_flame][$indPoint + 1] 
			#         = $backbone_x[$ind_flame][$indPoint] + $curvature[$ind_flame][$indPoint] * ($flameHeight[$ind_flame] / 36);
		 #        # $backbone_y[$ind_flame][$indPoint] += (rand(0.2) - 0.1) * ($flameHeight[$ind_flame] / 36);
			# }
	  #   }

	    $flameHeight[$ind_flame] = rand( $height );

	    # position of the flame along the width
	    $backbone_x[$ind_flame][0] = rand( $width );
	    $backbone_y[$ind_flame][0] = 0.;
	    for($indPoint = 0; $indPoint <= 36; $indPoint++) {
	    	# angle of rotation along the flame step by step
	        $curvature[$ind_flame][$indPoint] = rand(2) - 1.;
	        # flame width goes decreasing along the steps
	        $flameWidth[$ind_flame][$indPoint] = $flameWidthBaseIni[$ind_flame] - $indPoint * ($flameWidthBaseIni[$ind_flame]/36);
	        # adds a random factor of width at each step
	        $flameWidth[$ind_flame][$indPoint] = rand($flameWidth[$ind_flame][$indPoint] / 2) + ($flameWidth[$ind_flame][$indPoint] / 2);
	        # defines the backbone along which the flame is defined
	        # a fraction of the growth + a small variation
	        if($indPoint > 0) {
		        $backbone_y[$ind_flame][$indPoint] = ($flameHeight[$ind_flame] / 36) * $indPoint 
		        	+ rand($flameHeight[$ind_flame] / 36 / 2) - ($flameHeight[$ind_flame] / 36 / 4);
	        }
	        # position of the next step according to the curvature and the position of the preceding step
		    $backbone_x[$ind_flame][$indPoint + 1] 
		        = $backbone_x[$ind_flame][$indPoint] + $curvature[$ind_flame][$indPoint] * rand($flameWidth[$ind_flame][$indPoint]);
	    }

	    # color
	    if($imageRank == 0) {
		    $red[$ind_flame] = $redIni[$ind_flame];
		    $green[$ind_flame] = $greenIni[$ind_flame];
		    $blue[$ind_flame] = $blueIni[$ind_flame];
		}
	 	#    else {
		#     $red[$ind_flame] = min(max(int($red[$ind_flame]) + int(rand(20) - 10), 0), 255);
		#     $green[$ind_flame] = min(max(int($green[$ind_flame]) + int(rand(20) - 10), 0), 255);
		#     $blue[$ind_flame] = min(max(int($blue[$ind_flame]) + int(rand(20) - 10), 0), 255);
		# }
	    # saturation
	    # my $maxCol = max(max($red[$ind_flame],$green[$ind_flame]),$blue[$ind_flame]);
	    # $red[$ind_flame] += 255 - $maxCol;
	    # $green[$ind_flame] += 255 - $maxCol;
	    # $blue[$ind_flame] += 255 - $maxCol;

	    # a flame is made of several paths
	    $number_of_paths_in_a_flame = rand( $average_number_of_paths_in_a_flame);
		for(my $ind_flame_path = 0; $ind_flame_path < $number_of_paths_in_a_flame; $ind_flame_path++) {
		    # add a path to the group
		    my $curr_path = $y->path(
		        id    => 'spline'.$ind_flame.'.'.$ind_flame_path,
		        style => {
		            'fill-opacity' => 0,
		            'fill'   => "rgb(".$red[$ind_flame].",".$green[$ind_flame].",".$blue[$ind_flame].")",
		            'stroke'   => "rgb(".$red[$ind_flame].",".$green[$ind_flame].",".$blue[$ind_flame].")",
		            'stroke-opacity' => 1,
		            'stroke-width' => 1
		        }
		    );

		    # control points
		    # ascending curve
		    for($indPoint = 0; $indPoint < 36; $indPoint++) {
		        $pathsCP_x[$ind_flame][$indPoint] = $backbone_x[$ind_flame][$indPoint] 
		        			+ $flameWidth[$ind_flame][$indPoint] * rand($ind_flame_path/$number_of_paths_in_a_flame);
		        $pathsCP_y[$ind_flame][$indPoint] = $backbone_y[$ind_flame][$indPoint]
		        			+ rand($flameWidth[$ind_flame][$indPoint]);
		    }

		    # Bezier curve with flat tangent at each control point on the curve, except the first and last one
		    for($indPoint = 1; $indPoint < 36; $indPoint++) {
		        # points that are multiple of 3 + 2 are points on the curve
		        if(int($indPoint) % 3 == 0 && $indPoint != 36) {
		            $pathsCP_x[$ind_flame][$indPoint] = 0.5 * ($pathsCP_x[$ind_flame][$indPoint - 1] + $pathsCP_x[$ind_flame][$indPoint + 1]);
		            $pathsCP_y[$ind_flame][$indPoint] = 0.5 * ($pathsCP_y[$ind_flame][$indPoint - 1] + $pathsCP_y[$ind_flame][$indPoint + 1]);
		        }
		    }

		    # writes the SVG code for the path control points
		    my $pathpoints = "M ";
		    $pathpoints .= $pathsCP_x[$ind_flame][0].",".($height - $pathsCP_y[$ind_flame][0])." ";
		    $pathpoints .= "C ";
		    for($indPoint = 1; $indPoint < 36; $indPoint++) {
		        $pathpoints .= $pathsCP_x[$ind_flame][$indPoint].",".($height - $pathsCP_y[$ind_flame][$indPoint])." ";
		    }

		    $curr_path->attrib("d", $pathpoints);
		}
  		# if($ind_flame == 0 && $imageRank == 999) {
	 	#   for($indPoint = 0; $indPoint <= 36; $indPoint++) {
		# 		print $curvature[$ind_flame][$indPoint] . " / " . $backbone_x[$ind_flame][$indPoint] . " , " . $backbone_y[$ind_flame][$indPoint] . "\n";
		# 	}
		# }
	}

	$y->comment($fileName);

	# now render the SVG object, implicitly use svg namespace
	$fileName = sprintf("images/feu%04d.svg", $imageRank); 

	open($OutFile, '>' , $fileName);
	print $OutFile $svg->xmlify;
	close($OutFile);
}

# render a sequence of images based on variations of the initial one
for($imageRank = 0; $imageRank < $number_of_images; $imageRank++) {
	# turns them into png
	my $bg_color = '#000000';
	$fileName = sprintf("images/feu%04d", $imageRank); 
    print "inkscape -z -w 1920 -h 1080 --export-area-page -b ${bg_color} -e ${fileName}.png ${fileName}.svg","\n";
    system( "inkscape -z -w 1920 -h 1080 --export-area-page -b ${bg_color} -e ${fileName}.png ${fileName}.svg" ); 
}


