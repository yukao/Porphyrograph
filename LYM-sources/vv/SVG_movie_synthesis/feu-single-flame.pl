#!/usr/bin/perl

use strict;
use warnings;

use SVG;
use POSIX;
use Math::Trig;
use List::Util qw(min max);

my $width = 1920;
my $height = 1080;

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

my $indPoint;
my @pathsIniCP_x=();
my @pathsIniCP_y=();
my $nbFlames = 100;
my $indPath;
my @redIni = ();
my @greenIni = ();
my @blueIni = ();

my @flameHeightIni = ();
my @flameWidthIni = ();
my @flameWidthBaseIni = ();

my @backboneIni_x = ();
my @backboneIni_y = ();
my @curvatureIni = ();

my $imageRank = 0;
my $fileName = sprintf("images/feu%04d.svg", $imageRank); 
my $OutFile;

open($OutFile, '>' , $fileName);

# generates nbFlames Bezier curves different sizes and orientations and transparent
for($indPath = 0; $indPath < $nbFlames; $indPath++) {
    # add a path to the group
    # color
    $redIni[$indPath] = 180 + int(rand(80) - 40);
    $greenIni[$indPath] = 100 + int(rand(80) - 40);
    $blueIni[$indPath] = int(rand(40));
    # saturation
    my $maxCol = max(max($redIni[$indPath],$greenIni[$indPath]),$blueIni[$indPath]);
    $redIni[$indPath] += 255 - $maxCol;
    $greenIni[$indPath] += 255 - $maxCol;
    $blueIni[$indPath] += 255 - $maxCol;
    my $curr_path = $y->path(
        id    => 'pline'.$indPath,
        style => {
            'fill-opacity' => 1.0,
            'fill'   => "rgb(".$redIni[$indPath].",".$greenIni[$indPath].",".$blueIni[$indPath].")",
            'stroke'   => "rgb(".$redIni[$indPath].",".$greenIni[$indPath].",".$blueIni[$indPath].")",
            'stroke-opacity' => 0.3,
            'stroke-width' => 0.0
        }
    );

    # initial random control points
    # the number of points has to be a multiple of 3 for building cuves
    $flameHeightIni[$indPath] = rand( $height );
    $flameWidthBaseIni[$indPath] = rand( $width / 20 ) + $width / 20;

    # position of the flame along the width
    $backboneIni_x[$indPath][0] = rand( $width );
    $backboneIni_y[$indPath][0] = 0.;

    # backbone
    for($indPoint = 0; $indPoint <= 36; $indPoint++) {
    	# angle of rotation along the flame step by step
        $curvatureIni[$indPath][$indPoint] = rand(2) - 1.;
        # flame width goes decreasing along the steps
        $flameWidthIni[$indPath][$indPoint] = $flameWidthBaseIni[$indPath] - $indPoint * ($flameWidthBaseIni[$indPath]/36);
        # adds a random factor of width at each step
        $flameWidthIni[$indPath][$indPoint] = rand($flameWidthIni[$indPath][$indPoint] / 2) + ($flameWidthIni[$indPath][$indPoint] / 2);
        # defines the backbone along which the flame is defined
        # a fraction of the growth + a small variation
        if($indPoint > 0) {
	        $backboneIni_y[$indPath][$indPoint] = ($flameHeightIni[$indPath] / 36) * $indPoint 
	        	+ rand($flameHeightIni[$indPath] / 36 / 2) - ($flameHeightIni[$indPath] / 36 / 4);
        }
        # position of the next step according to the curvature and the position of the preceding step
	    $backboneIni_x[$indPath][$indPoint + 1] 
	        = $backboneIni_x[$indPath][$indPoint] + $curvatureIni[$indPath][$indPoint] * rand($flameWidthIni[$indPath][$indPoint]);
    }

    # ascending curve
    for($indPoint = 0; $indPoint < 36; $indPoint++) {
        $pathsIniCP_x[$indPath][$indPoint] = $backboneIni_x[$indPath][$indPoint] + $flameWidthIni[$indPath][$indPoint];
        $pathsIniCP_y[$indPath][$indPoint] = $backboneIni_y[$indPath][$indPoint];
    }
    # midpoint
    $indPoint = 36;
    $pathsIniCP_x[$indPath][$indPoint] = $backboneIni_x[$indPath][$indPoint];
    $pathsIniCP_y[$indPath][$indPoint] = $backboneIni_y[$indPath][$indPoint];
    # descending curve
    for($indPoint = 36 - 1; $indPoint >= 0; $indPoint--) {
        $pathsIniCP_x[$indPath][36 * 2 - $indPoint] = $backboneIni_x[$indPath][$indPoint] - $flameWidthIni[$indPath][$indPoint];
        $pathsIniCP_y[$indPath][36 * 2 - $indPoint] = $backboneIni_y[$indPath][$indPoint];
    }
    # Bezier curve with flat tangent at each control point on the curve, except the first and last one
    for($indPoint = 1; $indPoint < 2 * 36; $indPoint++) {
        # points that are multiple of 3 + 2 are points on the curve
        if(int($indPoint) % 3 == 0) {
            $pathsIniCP_x[$indPath][$indPoint] = 0.5 * ($pathsIniCP_x[$indPath][$indPoint - 1] + $pathsIniCP_x[$indPath][$indPoint + 1]);
            $pathsIniCP_y[$indPath][$indPoint] = 0.5 * ($pathsIniCP_y[$indPath][$indPoint - 1] + $pathsIniCP_y[$indPath][$indPoint + 1]);
        }
    }
    #except the middle point that is a cusp (singularity)
    $indPoint = 36;
    $pathsIniCP_x[$indPath][$indPoint - 1] = $pathsIniCP_x[$indPath][$indPoint + 1];
    $pathsIniCP_y[$indPath][$indPoint - 1] = $pathsIniCP_y[$indPath][$indPoint + 1];

    # writes the SVG code for the path control points
    my $pathpoints = "M ";
    $pathpoints .= $pathsIniCP_x[$indPath][0].",".($height - $pathsIniCP_y[$indPath][0])." ";
    $pathpoints .= "C ";
    for($indPoint = 1; $indPoint <= 36 * 2; $indPoint++) {
        $pathpoints .= $pathsIniCP_x[$indPath][$indPoint].",".($height - $pathsIniCP_y[$indPath][$indPoint])." ";
    }

    $curr_path->attrib("d", $pathpoints);
}

$y->comment($fileName);

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
for($imageRank = 1; $imageRank < 1000; $imageRank++) {
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

	# generates nbFlames Bezier curves different sizes and orientations and transparent
	for($indPath = 0; $indPath < $nbFlames; $indPath++) {

	    # add a path to the group
	    # color
	    if($imageRank == 1) {
		    $red[$indPath] = $redIni[$indPath];
		    $green[$indPath] = $greenIni[$indPath];
		    $blue[$indPath] = $blueIni[$indPath];
		}
	 	#    else {
		#     $red[$indPath] = min(max(int($red[$indPath]) + int(rand(20) - 10), 0), 255);
		#     $green[$indPath] = min(max(int($green[$indPath]) + int(rand(20) - 10), 0), 255);
		#     $blue[$indPath] = min(max(int($blue[$indPath]) + int(rand(20) - 10), 0), 255);
		# }
	    # saturation
	    # my $maxCol = max(max($red[$indPath],$green[$indPath]),$blue[$indPath]);
	    # $red[$indPath] += 255 - $maxCol;
	    # $green[$indPath] += 255 - $maxCol;
	    # $blue[$indPath] += 255 - $maxCol;
	    my $curr_path = $y->path(
	        id    => 'pline'.$indPath,
	        style => {
	            'fill-opacity' => 1.0,
	            'fill'   => "rgb(".$red[$indPath].",".$green[$indPath].",".$blue[$indPath].")",
	            'stroke'   => "rgb(".$red[$indPath].",".$green[$indPath].",".$blue[$indPath].")",
	            'stroke-opacity' => 0.3,
	            'stroke-width' => 0.0
	        }
	    );

	    # backbone variation
	    if($imageRank == 1) {
    		$flameHeight[$indPath] = $flameHeightIni[$indPath];
    	}
	    else {
    		$flameHeight[$indPath] += (rand(0.2) - 0.1) * $flameHeight[$indPath];
    	}
	    for($indPoint = 0; $indPoint <= 36; $indPoint++) {
		    if($imageRank == 1) {
		    	# angle of rotation along the flame step by step
		        $curvature[$indPath][$indPoint] = $curvatureIni[$indPath][$indPoint];
		        # flame width goes decreasing along the steps
		        $flameWidth[$indPath][$indPoint] = $flameWidthIni[$indPath][$indPoint];
		        # defines the backbone along which the flame is defined
		        # a fraction of the growth + a small variation
			    $backbone_x[$indPath][$indPoint] = $backboneIni_x[$indPath][$indPoint];
		        $backbone_y[$indPath][$indPoint] = $backboneIni_y[$indPath][$indPoint];
		    }
		    else {
		    	# angle of rotation along the flame step by step
		        $curvature[$indPath][$indPoint] = $curvature[$indPath][$indPoint] + (rand(1) - 0.5);
		        $curvature[$indPath][$indPoint] = max(min($curvature[$indPath][$indPoint],1.5),-1.5);

		        # flame width goes decreasing along the steps
		        $flameWidth[$indPath][$indPoint] += (rand(0.2) - 0.1) * $flameWidth[$indPath][$indPoint];
		        $flameWidth[$indPath][$indPoint] = max(min($flameWidth[$indPath][$indPoint],$flameWidthIni[$indPath][$indPoint]*2),
		        										$flameWidthIni[$indPath][$indPoint]*0.5);

		        # defines the backbone along which the flame is defined
		        # a fraction of the growth + a small variation
		        # position of the next step according to the curvature and the position of the preceding step
			    $backbone_x[$indPath][$indPoint + 1] 
			        = $backbone_x[$indPath][$indPoint] + $curvature[$indPath][$indPoint] * ($flameHeight[$indPath] / 36);
		        # $backbone_y[$indPath][$indPoint] += (rand(0.2) - 0.1) * ($flameHeight[$indPath] / 36);
			}
	    }

	    # control points
	    # ascending curve
	    for($indPoint = 0; $indPoint < 36; $indPoint++) {
	        $pathsCP_x[$indPath][$indPoint] = $backbone_x[$indPath][$indPoint] + $flameWidth[$indPath][$indPoint];
	        $pathsCP_y[$indPath][$indPoint] = $backbone_y[$indPath][$indPoint];
	    }
	    # midpoint
	    $indPoint = 36;
	    $pathsCP_x[$indPath][$indPoint] = $backbone_x[$indPath][$indPoint];
	    $pathsCP_y[$indPath][$indPoint] = $backbone_y[$indPath][$indPoint];
	    # descending curve
	    for($indPoint = 36 - 1; $indPoint >= 0; $indPoint--) {
	        $pathsCP_x[$indPath][36 * 2 - $indPoint] = $backbone_x[$indPath][$indPoint] - $flameWidth[$indPath][$indPoint];
	        $pathsCP_y[$indPath][36 * 2 - $indPoint] = $backbone_y[$indPath][$indPoint];
	    }
	    # Bezier curve with flat tangent at each control point on the curve, except the first and last one
	    for($indPoint = 1; $indPoint < 2 * 36; $indPoint++) {
	        # points that are multiple of 3 + 2 are points on the curve
	        if(int($indPoint) % 3 == 0 && $indPoint != 36) {
	            $pathsCP_x[$indPath][$indPoint] = 0.5 * ($pathsCP_x[$indPath][$indPoint - 1] + $pathsCP_x[$indPath][$indPoint + 1]);
	            $pathsCP_y[$indPath][$indPoint] = 0.5 * ($pathsCP_y[$indPath][$indPoint - 1] + $pathsCP_y[$indPath][$indPoint + 1]);
	        }
	    }
	    #except the middle point that is a cusp (singularity)
	    $indPoint = 36;
	    $pathsCP_x[$indPath][$indPoint - 1] = $pathsCP_x[$indPath][$indPoint + 1];
	    $pathsCP_y[$indPath][$indPoint - 1] = $pathsCP_y[$indPath][$indPoint + 1];

	    # writes the SVG code for the path control points
	    my $pathpoints = "M ";
	    $pathpoints .= $pathsCP_x[$indPath][0].",".($height - $pathsCP_y[$indPath][0])." ";
	    $pathpoints .= "C ";
	    for($indPoint = 1; $indPoint <= 36 * 2; $indPoint++) {
	        $pathpoints .= $pathsCP_x[$indPath][$indPoint].",".($height - $pathsCP_y[$indPath][$indPoint])." ";
	    }

	    $curr_path->attrib("d", $pathpoints);

  # 	    if($indPath == 0 && $imageRank == 999) {
	 #    	for($indPoint = 0; $indPoint <= 36; $indPoint++) {
		# 		print $curvature[$indPath][$indPoint] . " / " . $backbone_x[$indPath][$indPoint] . " , " . $backbone_y[$indPath][$indPoint] . "\n";
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
for($imageRank = 0; $imageRank < 1000; $imageRank++) {
	# turns them into png
	my $bg_color = '#000000';
	$fileName = sprintf("images/feu%04d", $imageRank); 
    print "inkscape -z -w 1920 -h 1080 --export-area-page -b ${bg_color} -e ${fileName}.png ${fileName}.svg","\n";
    system( "inkscape -z -w 1920 -h 1080 --export-area-page -b ${bg_color} -e ${fileName}.png ${fileName}.svg" ); 
}


