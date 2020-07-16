#!/usr/bin/perl

use strict;
use warnings;
use Math::Trig;
use List::Util qw(min max);

use SVG;
use POSIX;

my $width = 1920;
my $height = 1080;

# create an SVG object with a size of 200x200 pixels
my $svg = SVG->new(
    width  => $width,
    height => $height,
);
$svg->desc()->cdata('text graphics');
$svg->title()->cdata('text graphics');

# use explicit element constructor to generate a group element
my $y = $svg->group(
    id => 'group_y',
    style => {
        stroke => 'black',
        'fill-opacity' => 0
    },
);

my $indPath;
my $nb_rays = 1000;
my $number_of_images = 300;

my $paths_x_0 = $width/2;
my $paths_y_0 = $height/2;
my @paths_x_L = ();
my @paths_y_L = ();
my @paths_x_R = ();
my @paths_y_R = ();
my @paths_x_1 = ();
my @paths_y_1 = ();

my @angleLIni = ();
my @angleRIni = ();
my @angle1Ini = ();
my @radiusIni = ();
my $deltaAngle = 2 * pi / $nb_rays;

my $fileName; 
my $OutFile; 
my $curr_path;

for($indPath = 0; $indPath < $nb_rays; $indPath++) {
    # add a path to the group
    # a 10-pointsaw-tooth pattern drawn with a path definition
    $curr_path = $y->path(
        id    => 'pline'.$indPath,
        style => {
            'fill-opacity' => 0,
            'fill'   => 'green',
            'stroke' => 'rgb(250,123,23)'
        }
    );

    my $angle_start_final_interpolation = int($nb_rays * 0.99);
    my $ind_remaining_paths;
    my $nb_remaining_paths_tot;
    my $alpha;
    # for the first path, defines the angles of the 3 control points (the first is always in the center)
    # and the length of the ray
    if($indPath == 0) {
	    $angleLIni[$indPath] = rand(($nb_rays / 5) * $deltaAngle) - 0.5 * ($nb_rays / 5) * $deltaAngle;
	    $angleRIni[$indPath] = rand(($nb_rays / 5) * $deltaAngle) - 0.5 * ($nb_rays / 5) * $deltaAngle;
	    $angle1Ini[$indPath] = rand(($nb_rays / 5) * $deltaAngle) - 0.5 * ($nb_rays / 5) * $deltaAngle;
	    $radiusIni[$indPath] = $height * 0.4 + rand($height * 0.1);
    }
    # for the next paths except the last ones, defines the control points and radius as variants of the preceding path
    # plus an incremenation of the angle to distribute the arrays around the sun
    elsif($indPath <= $angle_start_final_interpolation) {
	    $angleLIni[$indPath] = $angleLIni[$indPath - 1] + $deltaAngle + rand($deltaAngle) - 0.5 * $deltaAngle;
	    $angleRIni[$indPath] = $angleRIni[$indPath - 1] + $deltaAngle + rand($deltaAngle) - 0.5 * $deltaAngle;
	    $angle1Ini[$indPath] = $angle1Ini[$indPath - 1] + $deltaAngle + rand($deltaAngle) - 0.5 * $deltaAngle;
	    $radiusIni[$indPath] = $radiusIni[$indPath - 1] + rand($height * 0.008) - 0.004 * $height;
    }
    # for the last paths interpolates to reach smoothly the first path
    else {
    	$ind_remaining_paths = $indPath - $angle_start_final_interpolation;
    	$nb_remaining_paths_tot = $nb_rays - $angle_start_final_interpolation - 1;
    	$alpha = 1.0 - $ind_remaining_paths / $nb_remaining_paths_tot;
	    $angleLIni[$indPath] = $alpha * $angleLIni[$angle_start_final_interpolation] + (1.0 - $alpha) * ($angleLIni[0] + 2 * pi);
	    $angleRIni[$indPath] = $alpha * $angleRIni[$angle_start_final_interpolation] + (1.0 - $alpha) * ($angleRIni[0] + 2 * pi);
	    $angle1Ini[$indPath] = $alpha * $angle1Ini[$angle_start_final_interpolation] + (1.0 - $alpha) * ($angle1Ini[0] + 2 * pi);
	    $radiusIni[$indPath] = $alpha * $radiusIni[$angle_start_final_interpolation] + (1.0 - $alpha) * ($radiusIni[0] + 2 * pi);
    }

    # calculates the coordinates of the control points
	$paths_x_0 = $width / 2;
	$paths_y_0 = $height / 2;
	$paths_x_L[$indPath] = $width / 2 + ($radiusIni[$indPath] / 3) * cos($angleLIni[$indPath]);
	$paths_y_L[$indPath] = $height / 2 + ($radiusIni[$indPath] / 3) * sin($angleLIni[$indPath]);
	$paths_x_R[$indPath] = $width / 2 + (2 * $radiusIni[$indPath] / 3) * cos($angleRIni[$indPath]);
	$paths_y_R[$indPath] = $height / 2 + (2 * $radiusIni[$indPath] / 3) * sin($angleRIni[$indPath]);
	$paths_x_1[$indPath] = $width / 2 + $radiusIni[$indPath] * cos($angle1Ini[$indPath]);
	$paths_y_1[$indPath] = $height / 2 + $radiusIni[$indPath] * sin($angle1Ini[$indPath]);

	# and writes the path inside the svg file
    my $pathpoints = "M ";
    $pathpoints .= $paths_x_0.",".$paths_y_0." ";

    $pathpoints .= "C ";
    $pathpoints .= $paths_x_L[$indPath].",".$paths_y_L[$indPath]." ";
    $pathpoints .= $paths_x_R[$indPath].",".$paths_y_R[$indPath]." ";
    $pathpoints .= $paths_x_1[$indPath].",".$paths_y_1[$indPath]." ";

    $curr_path->attrib("d", $pathpoints);

}

# adds a circle on top of that
my $circleFilling = 'rgb(245,245,50)';
$y->circle( cx => ($width / 2), cy => ($height / 2), r => ($height * 0.2), 
			id => 'sun_circle',
		    style => {
		        'fill-opacity' => 1,
		        'fill'       => $circleFilling,
        		'stroke-width'   => 0.0,
        		'stroke-opacity' => 0.5,
                'stroke'       => 'rgb(0,0,0)'
		    } );

$fileName = sprintf("images/soleil%04d.svg", 0); 

$svg->comment($fileName);

open($OutFile, '>' , $fileName);
print $OutFile $svg->xmlify;
close($OutFile);

my @angleL = ();
my @angleR = ();
my @angle1 = ();
my @radius = ();

# render a sequence of images based on variations of the initial one
for(my $imageRank = 1; $imageRank < $number_of_images; $imageRank++) {
	$fileName = sprintf("images/soleil%04d.svg", $imageRank); 
	
	# create an SVG object with a size of 200x200 pixels
	$svg = SVG->new(
	    width  => $width,
	    height => $height,
	);
	$svg->desc()->cdata('text graphics');
	$svg->title()->cdata('text graphics');

	# use explicit element constructor to generate a group element
	$y = $svg->group(
	    id => 'group_y',
	    style => {
	        stroke => 'black',
	        'fill-opacity' => 0
	    },
	);

	for($indPath = 0; $indPath < $nb_rays; $indPath++) {
	    # add a path to the group
	    # a 10-pointsaw-tooth pattern drawn with a path definition
	    $curr_path = $y->path(
	        id    => 'pline'.$indPath,
	        style => {
	            'fill-opacity' => 0,
	            'fill'   => 'green',
	            'stroke' => 'rgb(250,123,23)'
	        }
	    );

	    my $angle_start_final_interpolation = int($nb_rays * 0.99);
	    my $ind_remaining_paths;
	    my $nb_remaining_paths_tot;
	    my $alpha;
	    # copies the positions of the control points for the first image
	    if($imageRank == 1) {
		    $angleL[$imageRank][$indPath] = $angleLIni[$indPath];
		    $angleR[$imageRank][$indPath] = $angleRIni[$indPath];
		    $angle1[$imageRank][$indPath] = $angle1Ini[$indPath];
		    $radius[$imageRank][$indPath] = $radiusIni[$indPath];
	    }
	    # then varies from the positions of the control points in the preceding image
	    # but does not allow too large variations from the initial position 
	    # so that the shapes does not degenerate
	    elsif($indPath <= $angle_start_final_interpolation) {
		    $angleR[$imageRank][$indPath] = $angleR[$imageRank - 1][$indPath] + rand(0.1 * $deltaAngle) - 0.05 * $deltaAngle;
		    $angleR[$imageRank][$indPath] = min(max($angleR[$imageRank][$indPath], $angleR[1][$indPath] - 0.5 * $deltaAngle),
		    									$angleR[1][$indPath] + 0.5 * $deltaAngle);
		    $angle1[$imageRank][$indPath] = $angle1[$imageRank - 1][$indPath] + rand(0.1 * $deltaAngle) - 0.05 * $deltaAngle;
		    $angle1[$imageRank][$indPath] = min(max($angle1[$imageRank][$indPath], $angle1[1][$indPath] - 0.5 * $deltaAngle),
		    									$angle1[1][$indPath] + 0.5 * $deltaAngle);
		    $radius[$imageRank][$indPath] = $radius[$imageRank - 1][$indPath] + rand($height * 0.0008) - 0.0004 * $height;
		    $radius[$imageRank][$indPath] = min(max($radius[$imageRank][$indPath], $radius[1][$indPath] - 0.004 * $height),
		    									$radius[1][$indPath] + 0.004 * $height);
		    # for the first path, defines the angles of the 3 control points (the first is always in the center)
		    # and the length of the ray
		    if($indPath == 0) {
			    $angleL[$imageRank][$indPath] = $angleL[$imageRank - 1][$indPath] + rand(0.5 * $deltaAngle) - 0.25 * $deltaAngle;
			    # $angleL[$imageRank][$indPath] = min(max($angleL[$imageRank][$indPath], $angleL[1][$indPath] - 1.5 * $deltaAngle),
			    # 									$angleL[1][$indPath] + 1.5 * $deltaAngle);
			    $angleR[$imageRank][$indPath] = $angleR[$imageRank - 1][$indPath] + rand(0.5 * $deltaAngle) - 0.25 * $deltaAngle;
			    $angle1[$imageRank][$indPath] = $angle1[$imageRank - 1][$indPath] + rand(0.5 * $deltaAngle) - 0.25 * $deltaAngle;
			    $radius[$imageRank][$indPath] = $radius[$imageRank][$indPath] + rand($height * 0.01) - $height * 0.005;
		    }
		    # for the next paths except the last ones, defines the control points and radius as variants of the preceding path
		    # plus an incremenation of the angle to distribute the arrays around the sun
		    else {
			    $angleL[$imageRank][$indPath] = $angleL[$imageRank][$indPath - 1] + $deltaAngle + rand($deltaAngle) - 0.5 * $deltaAngle;
			    $angleR[$imageRank][$indPath] = $angleR[$imageRank][$indPath - 1] + $deltaAngle + rand($deltaAngle) - 0.5 * $deltaAngle;
			    $angle1[$imageRank][$indPath] = $angle1[$imageRank][$indPath - 1] + $deltaAngle + rand($deltaAngle) - 0.5 * $deltaAngle;
			    $radius[$imageRank][$indPath] = $radius[$imageRank][$indPath - 1] + rand($height * 0.008) - 0.004 * $height;
		    }
	    }
	    # for the last paths interpolates to reach smoothly the first path
	    else {
	    	$ind_remaining_paths = $indPath - $angle_start_final_interpolation;
	    	$nb_remaining_paths_tot = $nb_rays - $angle_start_final_interpolation - 1;
	    	$alpha = 1.0 - $ind_remaining_paths / $nb_remaining_paths_tot;
		    $angleL[$imageRank][$indPath] = $alpha * $angleL[$imageRank][$angle_start_final_interpolation] + (1.0 - $alpha) * ($angleL[$imageRank][0] + 2 * pi);
		    $angleR[$imageRank][$indPath] = $alpha * $angleR[$imageRank][$angle_start_final_interpolation] + (1.0 - $alpha) * ($angleR[$imageRank][0] + 2 * pi);
		    $angle1[$imageRank][$indPath] = $alpha * $angle1[$imageRank][$angle_start_final_interpolation] + (1.0 - $alpha) * ($angle1[$imageRank][0] + 2 * pi);
		    $radius[$imageRank][$indPath] = $alpha * $radius[$imageRank][$angle_start_final_interpolation] + (1.0 - $alpha) * ($radius[$imageRank][0] + 2 * pi);
	    }

	    # calculates the coordinates of the control points
		$paths_x_0 = $width / 2;
		$paths_y_0 = $height / 2;
		$paths_x_L[$indPath] = $width / 2 + ($radius[$imageRank][$indPath] / 3) * cos($angleL[$imageRank][$indPath]);
		$paths_y_L[$indPath] = $height / 2 + ($radius[$imageRank][$indPath] / 3) * sin($angleL[$imageRank][$indPath]);
		$paths_x_R[$indPath] = $width / 2 + (2 * $radius[$imageRank][$indPath] / 3) * cos($angleR[$imageRank][$indPath]);
		$paths_y_R[$indPath] = $height / 2 + (2 * $radius[$imageRank][$indPath] / 3) * sin($angleR[$imageRank][$indPath]);
		$paths_x_1[$indPath] = $width / 2 + $radius[$imageRank][$indPath] * cos($angle1[$imageRank][$indPath]);
		$paths_y_1[$indPath] = $height / 2 + $radius[$imageRank][$indPath] * sin($angle1[$imageRank][$indPath]);

		# and writes the path inside the svg file
	    my $pathpoints = "M ";
	    $pathpoints .= $paths_x_0.",".$paths_y_0." ";

	    $pathpoints .= "C ";
	    $pathpoints .= $paths_x_L[$indPath].",".$paths_y_L[$indPath]." ";
	    $pathpoints .= $paths_x_R[$indPath].",".$paths_y_R[$indPath]." ";
	    $pathpoints .= $paths_x_1[$indPath].",".$paths_y_1[$indPath]." ";

	    $curr_path->attrib("d", $pathpoints);
	}

	$y->circle( cx => ($width / 2), cy => ($height / 2), r => ($height * 0.2), 
				id => 'sun_circle',
			    style => {
			        'fill-opacity' => 1,
			        'fill'       => $circleFilling,
	        		'stroke-width'   => 0.0,
	        		'stroke-opacity' => 0.5,
	                'stroke'       => 'rgb(0,0,0)'
			    } );

	open($OutFile, '>' , $fileName);
	print $OutFile $svg->xmlify;
	close($OutFile);
}

# render a sequence of images based on variations of the initial one
for(my $imageRank = 0; $imageRank < $number_of_images ; $imageRank++) {
	# turns them into png
	my $bg_color = '#000000';
	$fileName = sprintf("images/soleil%04d", $imageRank); 
    print "inkscape -z -w 1920 -h 1080 --export-area-page -b ${bg_color} -e ${fileName}.png ${fileName}.svg","\n";
    system( "inkscape -z -w 1920 -h 1080 --export-area-page -b ${bg_color} -e ${fileName}.png ${fileName}.svg" ); 
}

