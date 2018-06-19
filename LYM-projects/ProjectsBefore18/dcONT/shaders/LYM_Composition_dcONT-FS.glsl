/***********************************************************************
File: dcONT/shaders/LYM_Composition_dcONT-FS.cg

LYM dcONT & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 150

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// COLOR OUTPUT
out vec4 outColor0;

uniform vec4 glsl_fs_4fv_width_height_trace_scale;
uniform vec4 glsl_fs_4fv_xy_frameno_cursorSize;
uniform vec4 glsl_fs_4fv_dcONTWeight_drawingWeight_blendTransp_pixellization;

/////////////////////////////////////
// COLOR INPUT
// the RED canal contains 1 if it is written
// the GREEN canal of OutGrayLevel contains the current gray level
// the BLUE canal contains a boolean that is set to 2 when a 
//     a region is filled
uniform samplerRect  fs_decal; // ping-pong mouse localColor/gray GOL
// pos FBO

/////////////////////////////////////
// FONT
uniform samplerRect fs_lookupTable_font;
uniform samplerRect fs_lookupTable_message;
uniform float glsl_fs_message_transparency;

void main() {
  float width = glsl_fs_4fv_width_height_trace_scale.x;
  float height = glsl_fs_4fv_width_height_trace_scale.y;
  float trace = glsl_fs_4fv_width_height_trace_scale.z;

  // FX scaling
  vec2 center = vec2(width, height) / 2.0;
  vec2 scaled_decalCoords 
    = (decalCoords - center) * glsl_fs_4fv_width_height_trace_scale.w + center;
	     scaled_decalCoords;
  scaled_decalCoords = floor( scaled_decalCoords );

  // FX pixellization
  float pixellization =glsl_fs_4fv_dcONTWeight_drawingWeight_blendTransp_pixellization.w;
  if( pixellization > 0.2 ) {
    int nbPixels = int(pixellization); 
    scaled_decalCoords = float(nbPixels) * vec2(ivec2(scaled_decalCoords) / nbPixels);
  }

  vec4 localColor_graydcONT
    = texture(fs_decal, scaled_decalCoords);
  float graydcONT = localColor_graydcONT.a;
  if( graydcONT < 0 ) {
    graydcONT = 0;
  }

  ////////////////////////////////////////////////////////////////////
  // drawing / dcONT interpolated combination: should be parameterized
  // drawing and dcONT combination
  // should be reworked
  vec3 combined_color 
    = clamp( glsl_fs_4fv_dcONTWeight_drawingWeight_blendTransp_pixellization.y 
	     * localColor_graydcONT.rgb 
	     +  glsl_fs_4fv_dcONTWeight_drawingWeight_blendTransp_pixellization.x 
	     * vec3(graydcONT,graydcONT,graydcONT) , 0.0 , 1.0 );
  
  // blinking cursor 1 pixel wide under the mouse
  float mouse_x = glsl_fs_4fv_xy_frameno_cursorSize.x;
  float mouse_y = glsl_fs_4fv_xy_frameno_cursorSize.y;
  float frameno = glsl_fs_4fv_xy_frameno_cursorSize.z;
  float cursorSize = (glsl_fs_4fv_xy_frameno_cursorSize.w + 0.5) * 2;
  if( abs(decalCoords.x - mouse_x) < cursorSize 
      && abs(height - decalCoords.y - mouse_y) < cursorSize ) {
    outColor0 = vec4(abs(sin(frameno/10.0)),abs(sin(frameno/10.0)),abs(sin(frameno/10.0)),1);
    return;
  }

  // blend with a saved background in case of clearing up
  vec4 returnC; // return color
  returnC = vec4( combined_color * glsl_fs_4fv_dcONTWeight_drawingWeight_blendTransp_pixellization.z , 1 );

  ////////////////////////////////////
  // MESSAGE
  if( decalCoords.x < 128.0 && decalCoords.y < height && decalCoords.y > height - 14 ) {
    vec2 locDecalCoord = vec2(decalCoords.x ,  height - decalCoords.y);
    vec4 offset = texture(fs_lookupTable_message, vec2(locDecalCoord.x,0) ) * 255.0;
    vec3 fontC = vec3(0,0,0);
    if( locDecalCoord.y > offset.w && locDecalCoord.y < offset.w + offset.z ) {
      fontC = texture(fs_lookupTable_font, vec2(offset.x, locDecalCoord.y - offset.w + offset.y)).rgb;
      // returnC.rgb *= returnC.a;
    }
    //    returnC.rgb = mix( returnC.rgb , fontC , fontC.r );
    if( offset.w > 0 ) { // non nul height
      returnC.rgb = mix( returnC.rgb , fontC , glsl_fs_message_transparency );
    }
    else {
      // keep returnC previously computed
    }
    // returnC = lerp(returnC, returnC, 1.0 - returnC.a);
  }

  outColor0 = returnC;
}
