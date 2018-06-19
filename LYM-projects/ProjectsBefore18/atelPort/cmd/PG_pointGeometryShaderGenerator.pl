#!/usr/bin/perl

# ods to conf

# PG_pointGeometryShaderGenerator.pl
# Sept 2017
# (c) Lola and Yukao Meet

# output: point lists for geometry shader
#

use Math::Trig;

open( OUTPUT_GEOMETRYSHADER_FILE , ">../shaders/LYM_ParticleSplat_atelPort-incl.geom" ) 
    || die "File ../shaders/LYM_ParticleSplat_atelPort-incl.geom not opened!";
printf "Writing shader include file ../shaders/LYM_ParticleSplat_atelPort-incl.geom\n";

# STAR
printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 starPoints[13] = \n       {");
for($i = 0 ; $i < 13 ; $i++) {
	if( int($i)%2 == 1 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE "{0., 0.}");
	}
	else {
		printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
			cos((int($i)/2) * pi / 3) ,sin((int($i)/2) * pi / 3) );
	}
	if( $i < 12 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
	if( int($i) % 4 == 3 ) {
		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n";

# TRIANGLE
@Ax = (cos(-(pi/6)),cos(-(pi/6) + 2 * pi / 3),cos(-(pi/6) + 4 * pi / 3));
@Ay = (sin(-(pi/6)),sin(-(pi/6) + 2 * pi / 3),sin(-(pi/6) + 4 * pi / 3));
printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 trianglePoints[13] = \n       {");
for($i = 0 ; $i < 5 ; $i++) {
	$alpha = $i / 4.;
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}, ", 
		(1. - $alpha) * $Ax[0] + $alpha * $Ax[1], 
		(1. - $alpha) * $Ay[0] + $alpha * $Ay[1] );
}
printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
for($i = 1 ; $i < 5 ; $i++) {
	$alpha = $i / 4.;
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}, ", 
		(1. - $alpha) * $Ax[1] + $alpha * $Ax[2], 
		(1. - $alpha) * $Ay[1] + $alpha * $Ay[2] );
}
printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
for($i = 1 ; $i < 5 ; $i++) {
	$alpha = $i / 4.;
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		(1. - $alpha) * $Ax[2] + $alpha * $Ax[0], 
		(1. - $alpha) * $Ay[2] + $alpha * $Ay[0] );
	if( $i < 4 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n";

# CIRCLE
printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 circlePoints[13] = \n       {");
for($i = 0 ; $i < 13 ; $i++) {
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", cos($i * pi / 6) ,sin($i * pi / 6) );
	if( $i < 12 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
	if( int($i) % 4 == 3 ) {
		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n";

# SQUARE
@Ax = (cos((pi/4)), cos((pi/4) + 1 * (pi/2)), cos((pi/4) + 2 * (pi/2)), cos((pi/4) + 3 * (pi/2)));
@Ay = (sin((pi/4)), sin((pi/4) + 1 * (pi/2)), sin((pi/4) + 2 * (pi/2)), sin((pi/4) + 3 * (pi/2)));
printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 squarePoints[13] = \n       {");
for($i = 0 ; $i < 4 ; $i++) {
	$alpha = $i / 3.;
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}, ", 
		(1. - $alpha) * $Ax[0] + $alpha * $Ax[1], 
		(1. - $alpha) * $Ay[0] + $alpha * $Ay[1] );
}
printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
for($i = 1 ; $i < 4 ; $i++) {
	$alpha = $i / 3.;
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}, ", 
		(1. - $alpha) * $Ax[1] + $alpha * $Ax[2], 
		(1. - $alpha) * $Ay[1] + $alpha * $Ay[2] );
}
printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
for($i = 1 ; $i < 4 ; $i++) {
	$alpha = $i / 3.;
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}, ", 
		(1. - $alpha) * $Ax[2] + $alpha * $Ax[3], 
		(1. - $alpha) * $Ay[2] + $alpha * $Ay[3] );
}
printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
for($i = 1 ; $i < 4 ; $i++) {
	$alpha = $i / 3.;
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		(1. - $alpha) * $Ax[3] + $alpha * $Ax[0], 
		(1. - $alpha) * $Ay[3] + $alpha * $Ay[0] );
	if( $i < 3 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n";

close( OUTPUT_GEOMETRYSHADER_FILE ); 

