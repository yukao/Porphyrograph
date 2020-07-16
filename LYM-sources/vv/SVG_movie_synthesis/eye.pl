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
my $number_of_iris_rays = 1000;

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
my $fileName; 
my $OutFile;

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
	$fileName = sprintf("images/eye%04d.svg", $imageRank); 

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

	# add a path to the group
	my $curr_path = $y->path(
	    id    => 'spline',
	    style => {
	        'fill-opacity' => 1,
	        'fill'   => "#CCCCCC",
	        'stroke'   => "#FFFFFF",
	        'stroke-opacity' => 1,
	        'stroke-width' => 1
	    }
	);
    # writes the SVG code for the path control points
    my $pathpoints = "M 56.827282,614.25428  C 345.32119,322.89925 758.08576,175.79729 1119.0233,180.07199 c 306.889,23.87435 668.3679,112.37976 742.3743,408.98423 5.0807,20.47407 -11.1104,60.99721 -58.1495,54.2728 -47.0393,-6.72442 -72.6313,-90.95047 -110.484,-94.97737 -154.7838,-7.24413 -168.1795,225.08839 -362.4647,319.82173 C 879.55931,1182.8711 383.47646,508.61066 56.827282,614.25428 Z";
    $curr_path->attrib("d", $pathpoints);

    my $eye_centerx = 1059.9045;
    my $eye_centery = 539.62921;
    my $iris_radius = 356.0376;
    my $pupil_radius = 181.23233;

	# add a circle to the group
	my $curr_circle = $y->circle(
	    id    => "iris",
	    r    =>  "$iris_radius",
	    cy    => "$eye_centery",
	    cx    => "$eye_centerx",
	    style => {
	        'fill-opacity' => 1,
	        'fill'   => "#0284fd",
	        'stroke'   => "#000000",
	        'stroke-opacity' => 1,
	        'stroke-width' => 1
	    }
	);
	
	# add a circle to the group
	$curr_circle = $y->circle(
	    id    => "pupil",
	    r    =>  "$pupil_radius",
	    cy    => "$eye_centery",
	    cx    => "$eye_centerx",
	    style => {
	        'fill-opacity' => 1,
	        'fill'   => "#070707",
	        'stroke'   => "#000000",
	        'stroke-opacity' => 1,
	        'stroke-width' => 1
	    }
	);

	# adds the iris rays
	my $angular_step = ((2*pi)/$number_of_iris_rays) ;
	for(my $ind_iris = 0 ; $ind_iris < $number_of_iris_rays ; $ind_iris++) {
		my $angle = $ind_iris * $angular_step + (rand(2) - 1) * $angular_step;
		my $length = $pupil_radius + rand($iris_radius - $pupil_radius);
		my $x1 = $eye_centerx + $iris_radius * cos($angle);
		my $y1 = $eye_centery + $iris_radius * sin($angle);
		my $x2 = $eye_centerx + $length * cos($angle);
		my $y2 = $eye_centery + $length * sin($angle);
		my $blue = int(rand(100) + 155);
		my $green = int(rand(100) + 75);
		my $red = int(rand(100));
		# add a line to the group
		my $curr_line = $y->line(
		    id    => "line".$ind_iris,
		    x1    => "$x1",
		    y1    => "$y1",
		    x2    => "$x2",
		    y2    => "$y2",
		    style => {
		        'fill-opacity' => 0,
		        'fill'   => "none",
		        'stroke'   => "rgb($red,$green,$blue)",
		        'stroke-opacity' => 1,
		        'stroke-width' => 2
		    }
		);
	}
	
	$y->comment($fileName);

	# now render the SVG object, implicitly use svg namespace
	$fileName = sprintf("images/eye%04d.svg", $imageRank); 

	open($OutFile, '>' , $fileName);
	print $OutFile $svg->xmlify;
	close($OutFile);
}

# render a sequence of images based on variations of the initial one
for($imageRank = 0; $imageRank < $number_of_images; $imageRank++) {
	# turns them into png
	my $bg_color = '#000000';
	$fileName = sprintf("images/eye%04d", $imageRank); 
    print "inkscape -z -w 1920 -h 1080 --export-area-page -b ${bg_color} -e ${fileName}.png ${fileName}.svg","\n";
    system( "inkscape -z -w 1920 -h 1080 --export-area-page -b ${bg_color} -e ${fileName}.png ${fileName}.svg" ); 
}


