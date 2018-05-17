/***********************************************************************
File: effe/shaders/LYM_TrackDisplay_effe-VP.cg

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_TEXTURE_WIDTH 500

/////////////////////////////////////
// INPUT
layout(location = 0) in vec3 position;                     // espace objet

// UNIFORMS
// passed by the C program
uniform vec4 uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius;

// TEXTURES
layout (binding = 0) uniform sampler1D vp_lookupTable1;  // position
layout (binding = 1) uniform sampler1D vp_lookupTable2;  // color
layout (binding = 2) uniform sampler1D vp_lookupTable3;  // (radius, brush ID and rendering mode)

// layout (binding = 3) uniform samplerRect vp_lookupTable4;  // noise

/////////////////////////////////////
// OUTPUT
out VertexAttrib
{
  vec4 color;
  vec3 tangent;
  vec3 radius_Brush_drawingMode;
} vertex;

void main() {
  int frameNo = int(uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius.y);
  
  float vertexRank = position.x;
  float width = uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius.x;

  if( vertexRank >= width ) {
	vertex.color = vec4(0.0,0.0,0.0,1.0);
	vertex.tangent = vec3(0.0,0.0,0.0);
	vertex.radius_Brush_drawingMode = vec3(0.0,0.0,0.0);
	gl_Position = vec4(-1000.0,-1000.0,-1000.0,-1.0);
	return;
  }

  //////////////////////////////////////////////////
  // the texture contains three data arrays in sequence
  // each array has the length of the line
  // 1 array for the positions
  // 1 array for the color
  // 1 array for the radius, brush ID and drawing mode
    
  ////////////////////////////////////////////////
  // POSITION_array
  // the pixel rank of the drawn array
  /* int noiseRank = int(vertexRank + uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius.y); */

  /* float noiseAmplitude = 10.0 * uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius.z; */
  
  // current noise and position
  float texCoords_cur = vertexRank / width;
  /* vec2 texCoordsNoise_cur = vec2( noiseRank % int(width) , noiseRank / int(width) ); */
  /* vec3 noise  */
  /*     = noiseAmplitude  */
  /*     * (texture( vp_lookupTable4, texCoordsNoise_cur ).rgb - vec3(0.5,0.5,0.5)); */
  vec4 cur_position 
    = texture( vp_lookupTable1, texCoords_cur );// + vec4( noise , 0 );

  // previous noise and position
  float texCoords_prev = (vertexRank > 0 ? (vertexRank - 1) / int(width): vertexRank);
  /* vec2 texCoordsNoise_prev = vec2( (noiseRank - 1) % int(width) , (noiseRank - 1) / int(width) ); */
  /* noise  */
  /*     = noiseAmplitude  */
  /*     * (texture( vp_lookupTable4, texCoordsNoise_prev ).rgb - vec3(0.5,0.5,0.5)); */
  vec4 prev_position 
    = texture( vp_lookupTable1, texCoords_prev ); // + vec4( noise , 0 );

  // next noise and position
  float texCoords_next = (vertexRank + 1) / int(width);
  /* vec2 texCoordsNoise_next = vec2( (noiseRank + 1) % int(width) , (noiseRank + 1) / int(width) ); */
  /* noise  */
  /*     = noiseAmplitude  */
  /*     * (texture( vp_lookupTable4, texCoordsNoise_next ).rgb - vec3(0.5,0.5,0.5)); */
  vec4 next_position 
    = texture( vp_lookupTable1, texCoords_next ); // + vec4( noise , 0 );
    
  // computes tangent from 3 steps and transforms it into the view space
  vec3 previous2current = normalize(cur_position.xyz - prev_position.xyz);
  vec3 current2next = normalize(next_position.xyz - cur_position.xyz);
  vertex.tangent = (previous2current + current2next);
  vertex.tangent = normalize( vertex.tangent );
    
  // cur_position output
  gl_Position = vec4(cur_position.xyz,1.0);
      
  ////////////////////////////////////////////////
  // COLOR array
  // reads and transmits color 
  vertex.color = texture( vp_lookupTable2, texCoords_cur );
    
  ////////////////////////////////////////////////
  // RADIUS, BRUSH ID and DRAWING MODE array
  // reads and transmits radius, brush ID and drawing mode 
  // drawing mode is not used for the moment
  vertex.radius_Brush_drawingMode 
      = vec3( texture( vp_lookupTable3, texCoords_cur ).xyz );
    
  // global radius scaling
  vertex.radius_Brush_drawingMode.x 
      += uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius.w;
}
