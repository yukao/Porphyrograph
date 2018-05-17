/***********************************************************************
File: alKemi/shaders/LYM_Final_alKemi-FS.cg

LYM alKemi & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // previous pass output
layout (binding = 1) uniform samplerRect fs_lookupTable1; // previous display tracks output

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin;
uniform vec4 uniform_Final_fs_4fv_xy_frameno_cursorSize;
uniform float uniform_Final_fs_height;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  vec2 imCoords = decalCoords;
  float leftWindowWidth = uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin.z;
  float rightWindowVMargin = uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin.w;
  float height = uniform_Final_fs_height;

  // Boris Vian special set-up: top and bottom white stripes for partial display
  // if( imCoords.x < leftWindowWidth && ( int(imCoords.y) == 70 || int(imCoords.y) == 980 )) {
  //   if( uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin.y > 0 ) {
  //     outColor0 = texture(fs_decal, imCoords );
  //   }
  //   else {
  //     outColor0 = vec4(vec3(1,1,1) - texture(fs_decal, imCoords ).rgb , 1);
  //   }
  //   return;
  // }

  if( imCoords.x > leftWindowWidth ) {
    if( imCoords.x < leftWindowWidth + rightWindowVMargin
        || imCoords.x > 2 * leftWindowWidth + rightWindowVMargin ) {
      outColor0 = vec4(0,0,0,1);
      return;
    }
    imCoords.x = imCoords.x - (leftWindowWidth + rightWindowVMargin);
  }

  outColor0 = clamp( texture(fs_decal, imCoords ) + texture(fs_lookupTable1, imCoords ) , 0 , 1 );

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse
  float mouse_x = uniform_Final_fs_4fv_xy_frameno_cursorSize.x;
  float mouse_y = uniform_Final_fs_4fv_xy_frameno_cursorSize.y;
  float frameno = uniform_Final_fs_4fv_xy_frameno_cursorSize.z;
  float cursorSize = (uniform_Final_fs_4fv_xy_frameno_cursorSize.w + 0.5) * 2;
  if( length(vec2((imCoords.x - mouse_x) , 
                  (uniform_Final_fs_height - imCoords.y - mouse_y))) < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  if( uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin.y > 0 ) {
	   outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }
  outColor0.rgb *= uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin.x;
}
