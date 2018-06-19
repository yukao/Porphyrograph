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

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec3 uniform_Final_fs_3fv_transparency_invert_scale;
uniform vec4 uniform_Final_fs_4fv_xy_frameno_cursorSize;
uniform vec2 uniform_Final_fs_2fv_tablet1xy;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  vec2 coords = vec2( (decalCoords.x > 1024 ? decalCoords.x - 1024 : decalCoords.x) , decalCoords.y);

    // FX scaling
  vec2 center = vec2(1024, 768) / 2.0;
  float scale = uniform_Final_fs_3fv_transparency_invert_scale.z;
  vec2 scaled_decalCoords 
    = (coords - center) / scale + center;
  scaled_decalCoords = floor( scaled_decalCoords );

  // FX pixellization
  //int nbPixels = int(scale); 
  // scaled_decalCoords = float(nbPixels) * vec2(ivec2(scaled_decalCoords) / nbPixels);

  outColor0 = texture(fs_decal, scaled_decalCoords );

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse
  vec2 mouse_xy = uniform_Final_fs_4fv_xy_frameno_cursorSize.xy;
  float frameno = uniform_Final_fs_4fv_xy_frameno_cursorSize.z;
  float cursorSize = (uniform_Final_fs_4fv_xy_frameno_cursorSize.w + 0.5) * 2;
  if( length(vec2(scaled_decalCoords.x - mouse_xy.x , 768 - scaled_decalCoords.y - mouse_xy.y)) 
      < cursorSize * scale ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }
  mouse_xy = uniform_Final_fs_2fv_tablet1xy;
  if( length(vec2(scaled_decalCoords.x - mouse_xy.x , 768 - scaled_decalCoords.y - mouse_xy.y)) 
      < cursorSize * scale ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  if( uniform_Final_fs_3fv_transparency_invert_scale.y > 0 ) {
	   outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }
  outColor0.rgb *= uniform_Final_fs_3fv_transparency_invert_scale.x;
}
