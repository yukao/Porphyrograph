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
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // LYM logo

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Final_fs_4fv_transparency_scale_leftwidth_rightVMargin;
uniform vec4 uniform_Final_fs_4fv_xy_frameno_cursorSize;
uniform vec4 uniform_Final_fs_4fv_tablet1xy_height_is_onlyLeft_screen;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  vec2 imCoords = decalCoords;
  float leftWindowWidth = uniform_Final_fs_4fv_transparency_scale_leftwidth_rightVMargin.z;
  float rightWindowVMargin = uniform_Final_fs_4fv_transparency_scale_leftwidth_rightVMargin.w;
  float height = uniform_Final_fs_4fv_tablet1xy_height_is_onlyLeft_screen.z;

  if( imCoords.x > leftWindowWidth ) {
    if(uniform_Final_fs_4fv_tablet1xy_height_is_onlyLeft_screen.w != 0.f) {
      outColor0 = vec4(0,0,0,1);
      return;
    }
    if( imCoords.x < leftWindowWidth + rightWindowVMargin
        || imCoords.x > 2 * leftWindowWidth + rightWindowVMargin ) {
      outColor0 = vec4(0,0,0,1);
      return;
    }
    imCoords.x = imCoords.x - (leftWindowWidth + rightWindowVMargin);
  }
  
  //   // FX scaling
  // vec2 center = vec2(leftWindowWidth, height) / 2.0;
  // float scale = uniform_Final_fs_4fv_transparency_scale_leftwidth_rightVMargin.y;
  // vec2 scaled_decalCoords 
  //   = (coords - center) / scale + center;
  // scaled_decalCoords = floor( scaled_decalCoords );

  // FX pixellization
  //int nbPixels = int(scale); 
  // scaled_decalCoords = float(nbPixels) * vec2(ivec2(scaled_decalCoords) / nbPixels);

  outColor0 = texture(fs_decal, imCoords );

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse
  vec2 mouse_xy = uniform_Final_fs_4fv_xy_frameno_cursorSize.xy;
  float frameno = uniform_Final_fs_4fv_xy_frameno_cursorSize.z;
  float cursorSize = (uniform_Final_fs_4fv_xy_frameno_cursorSize.w + 0.5) * 2;
  if( length(vec2(decalCoords.x - mouse_xy.x , height - decalCoords.y - mouse_xy.y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }
  mouse_xy = uniform_Final_fs_4fv_tablet1xy_height_is_onlyLeft_screen.xy;
  if( length(vec2(decalCoords.x - mouse_xy.x , height - decalCoords.y - mouse_xy.y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  outColor0.rgb = mix( texture(fs_lookupTable1, imCoords ).rgb , outColor0.rgb , 
		       uniform_Final_fs_4fv_transparency_scale_leftwidth_rightVMargin.x );
}
