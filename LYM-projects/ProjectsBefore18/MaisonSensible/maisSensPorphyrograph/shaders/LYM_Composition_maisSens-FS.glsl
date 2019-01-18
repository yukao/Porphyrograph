/***********************************************************************
File: alKemi/shaders/LYM_Composition_alK-FS.glsl

LYM alK & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 330

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec3 uniform_Composition_fs_3fv_width_height_CAweight;
uniform vec3 uniform_Composition_fs_3fv_trackweight;
uniform vec4 uniform_Composition_fs_4fv_xy_frameno_cursorSize;
uniform vec4 uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void;

/////////////////////////////////////
// INPUT
uniform samplerRect fs_decal; // 3-cycle ping-pong localColor step n + 1 (FBO attachment 1)
// pos FBO
uniform samplerRect fs_lookupTable1;  // 3-cycle ping-pong CA step n + 1 (FBO attachment 3)
uniform samplerRect fs_lookupTable2;  // 3-cycle ping-pong track 1 step n + 1 (FBO attachment 4)
uniform samplerRect fs_lookupTable3;  // 3-cycle ping-pong track 2 step n + 1 (FBO attachment 5)
uniform samplerRect fs_lookupTable4;  // preceding snapshot
// FONT
uniform samplerRect fs_lookupTable_font;
uniform samplerRect fs_lookupTable_message;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  float height = uniform_Composition_fs_3fv_width_height_CAweight.y;

  vec4 bg_color = texture(fs_decal, decalCoords);
  vec4 CA_color = texture(fs_lookupTable1, decalCoords);
  vec4 track1_color = texture(fs_lookupTable2, decalCoords);
  vec4 track2_color = texture(fs_lookupTable3, decalCoords);
  vec3 localColor
    = vec3(bg_color.rgb) * uniform_Composition_fs_3fv_trackweight.x
    +vec3(track1_color.rgb) * uniform_Composition_fs_3fv_trackweight.y
    +vec3(track2_color.rgb) * uniform_Composition_fs_3fv_trackweight.z;

  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }

  ////////////////////////////////////////////////////////////////////
  // drawing / GOL interpolated combination: should be parameterized
  // drawing and GOL combination
  // should be reworked
  vec3 combined_color 
    = clamp( localColor 
	     +  uniform_Composition_fs_3fv_width_height_CAweight.z
	     * CA_color.rgb , 0.0 , 1.0 );
  
  ////////////////////////////////////////////////////////////////////
  // accumulation
  vec4 previous_snapshot_color 
       = texture(fs_lookupTable4, decalCoords);
  float echo = uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void.y;
  float echoNeg = uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void.z;
  if( echo + echoNeg > 0.0 ) {
    combined_color 
    	= clamp( combined_color  * echo
    		 + (1.0  - echoNeg) * previous_snapshot_color.rgb , 
    		 0.0 , 1.0 );
  }

  // outColor0 = vec4(0.0, 0.0, 0.0, 1.0 );  

     
  ////////////////////////////////////
  // MESSAGE
 //  if( decalCoords.x < 128.0 && decalCoords.y < height && decalCoords.y > height - 14 
	// 	&& uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void.x > 0 ) {
 //    vec2 locDecalCoord = vec2(decalCoords.x ,  height - decalCoords.y);
 //    vec4 offset = texture(fs_lookupTable_message, vec2(locDecalCoord.x,0) ) * 255.0;
 //    vec3 fontC = vec3(0,0,0);
	//   vec3 messageColor = vec3(0,0,0);
 //    if( locDecalCoord.y > offset.w && locDecalCoord.y < offset.w + offset.z ) {
 //      fontC = texture(fs_lookupTable_font, vec2(offset.x, locDecalCoord.y - offset.w + offset.y)).rgb;
 //    }

 //    if( offset.w > 0 ) { // non nul height
 //      messageColor = fontC;
	// }
	// else  {
	//   messageColor = vec3(0,0,0);
	// }
	// combined_color = mix( combined_color ,  messageColor , 
	//    uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void.x );
 //  }

  outColor0 = vec4(combined_color , 1.0);
  // outColor0 = outColor0*0.001 + texture(fs_decal, decalCoords);
  // outColor0 = outColor0*0.001 + vec4(1,1,0,1);

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse
  float mouse_x = uniform_Composition_fs_4fv_xy_frameno_cursorSize.x;
  float mouse_y = uniform_Composition_fs_4fv_xy_frameno_cursorSize.y;
  float frameno = uniform_Composition_fs_4fv_xy_frameno_cursorSize.z;
  float cursorSize = (uniform_Composition_fs_4fv_xy_frameno_cursorSize.w + 0.5) * 2;
  if( abs(decalCoords.x - mouse_x) < cursorSize 
      && abs(height - decalCoords.y - mouse_y) < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

}
