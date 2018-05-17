#!/usr/bin/perl

# ods to conf

# PG_pointGeometryShaderGenerator.pl
# Sept 2017
# (c) Lola and Yukao Meet

# output: point lists for geometry shader
#

use Math::Trig;

open( OUTPUT_GEOMETRYSHADER_FILE , ">../shaders/LYM_ParticleSplat_atelPort-incl-v2.geom" ) 
    || die "File ../shaders/LYM_ParticleSplat_atelPort-incl-v2.geom not opened!";
printf "Writing shader include file ../shaders/LYM_ParticleSplat_atelPort-incl-v2.geom\n";

# MOTEUR
printf(OUTPUT_GEOMETRYSHADER_FILE "///////////////////////\n// MOTEUR\n///////////////////////\n");
printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 gearMoteur[16] = \n       {");
for($i = 0 ; $i < 16 ; $i++) {
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		cos(int($i/2) * 2 * pi / 7) * (0.75 + (int($i+2) % 4 < 2? .5 : 0)),
		sin(int($i/2) * 2 * pi / 7) * (0.75 + (int($i+2) % 4 < 2? .5 : 0)) );
	if( $i < 15 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
	if( int($i) % 4 == 3 && $i < 15 ) {
		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n";

printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 antiGearMoteur[16] = \n       {");
for($i = 0 ; $i < 16 ; $i++) {
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		cos($i * 2 * pi / 15) * (0.5 + (int($i + 1) % 4 < 3? 1 : 0)),
		sin($i * 2 * pi / 15) * (0.5 + (int($i + 1) % 4 < 3? 1 : 0)) );
	if( $i < 15 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
	if( int($i) % 4 == 3 && $i < 15 ) {
		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n\n";

# SUSTAIN
printf(OUTPUT_GEOMETRYSHADER_FILE "///////////////////////\n// SUSTAIN\n///////////////////////\n");
printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 circleSustain[16] = \n       {");
for($i = 0 ; $i < 16 ; $i++) {
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		cos($i * pi / 7),
		sin($i * pi / 7));
	if( $i < 15 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
	if( int($i) % 4 == 3 && $i < 15 ) {
		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n";

printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 spikyCircleSustain[16] = \n       {");
for($i = 0 ; $i < 16 ; $i++) {
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		cos($i * pi / 7) * (0.5 + (int($i) % 2)),
		sin($i * pi / 7) * (0.5 + (int($i) % 2)));
	if( $i < 15 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
	if( int($i) % 4 == 3 && $i < 15 ) {
		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n\n";

# DOPPLER
printf(OUTPUT_GEOMETRYSHADER_FILE "///////////////////////\n// DOPPLER\n///////////////////////\n");
printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 lineDoppler[4] = \n       {");
for($i = 0 ; $i < 4 ; $i++) {
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		 int($i) % 2 , int($i + 1) % 2);
	if( $i < 3 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n";

printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 triangleDoppler[4] = \n       {");
for($i = 0 ; $i < 4 ; $i++) {
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		cos($i * 2 * pi / 3),
		sin($i * 2 * pi / 3));
	if( $i < 3 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n\n";

# # GLISSANDO
# printf(OUTPUT_GEOMETRYSHADER_FILE "///////////////////////\n// GLISSANDO\n///////////////////////\n");
# printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 starGlissando[16] = \n       {");
# for($i = 0 ; $i < 16 ; $i++) {
# 	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
# 		cos(int($i/2) * pi / 8) * (int($i + 1) % 2),
# 		sin(int($i/2) * pi / 8) * (int($i + 1) % 2));
# 	if( $i < 15 ) {
# 		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
# 	}
# 	if( int($i) % 4 == 3 && $i < 15 ) {
# 		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
# 	}
# }
# printf OUTPUT_GEOMETRYSHADER_FILE "};\n\n";

# GLISSANDO
printf(OUTPUT_GEOMETRYSHADER_FILE "///////////////////////\n// GLISSANDO\n///////////////////////\n");
printf(OUTPUT_GEOMETRYSHADER_FILE "const vec2 starGlissando[16] = \n       {");
for($i = 0 ; $i < 8 ; $i++) {
	$angle = $i * pi / 16;
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		cos($angle) * (0.2 + 0.3 * $angle),
		sin($angle) * (0.2 + 0.3 * $angle));
	if( $i < 15 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
	if( int($i) % 4 == 3 && $i < 15 ) {
		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
	}
}
for($i = 8 ; $i < 16 ; $i++) {
	$angle = (pi / 2) - (3 * pi / 16) - (($i - 9) * pi / 16);
	printf(OUTPUT_GEOMETRYSHADER_FILE "{%.3f, %.3f}", 
		cos($angle) * (0.2 + 0.5 * $angle),
		sin($angle) * (0.2 + 0.5 * $angle));
	if( $i < 15 ) {
		printf(OUTPUT_GEOMETRYSHADER_FILE ", ");
	}
	if( int($i) % 4 == 3 && $i < 15 ) {
		printf OUTPUT_GEOMETRYSHADER_FILE "\n        ";
	}
}
printf OUTPUT_GEOMETRYSHADER_FILE "};\n\n";


close( OUTPUT_GEOMETRYSHADER_FILE ); 

