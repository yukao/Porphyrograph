/***********************************************************************
File: effe/shaders/LYM_TrackDisplay_effe-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// drawing modes
#define VOID_DRAWING     0
#define LINE_DRAWING     1
#define DOT_DRAWING      2

/////////////////////////////////////
// INPUT - OUTPUT PRIMITIVES
layout(lines, invocations = 1)  in;
layout(triangle_strip , max_vertices = 4 ) out;

/////////////////////////////////////
// INPUT - OUTPUT VARIABLES
in VertexAttrib
{
  vec4 color;
  vec3 tangent;
  vec3 radius_Brush_drawingMode;
} vertex[];

out vec4 vertex_color;          // color of the pixel
out vec2 decalCoords;           // coord text
out float brush;                // brush ID

// UNIFORMS
// passed by the C program
uniform mat4 gs_modelMatrix, gs_viewMatrix, gs_projMatrix;

void main() {
  // emission face originale
  vec4 p1 = gl_in[0].gl_Position;
  vec4 p2 = gl_in[1].gl_Position;
  mat4 modelViewProj = gs_projMatrix * gs_viewMatrix * gs_modelMatrix;

  if( p1.w < 0 || p2.w < 0 || (p1.x <= 0 && p1.y <= 0 ) || (p2.x <= 0 && p2.y <= 0) ) {
    // EndPrimitive();
  }
  else {
    vec4 binormal1 
      = normalize(vec4( cross( vertex[0].tangent , vec3(0.0,0.0,1.0) ), 0));
    vec4 binormal2 
      = normalize(vec4( cross( vertex[1].tangent , vec3(0.0,0.0,1.0) ), 0));

    float drawingMode1 = vertex[0].radius_Brush_drawingMode.z;
    float drawingMode2 = vertex[1].radius_Brush_drawingMode.z;

    if( drawingMode1 == VOID_DRAWING ) {
      binormal1 *= 0.0;
    }
    else {
      binormal1 *= vertex[0].radius_Brush_drawingMode.x;
    }
    if( drawingMode2 == VOID_DRAWING ) {
      binormal2 *= 0;
    }
    else {
      binormal2 *= vertex[1].radius_Brush_drawingMode.x;
    }

    if( drawingMode1 == VOID_DRAWING ) {
      EndPrimitive();
    }
    else {
	  vertex_color = vertex[0].color;
	  decalCoords = vec2(0.0,0.0);
	  brush = vertex[0].radius_Brush_drawingMode.y;
	  gl_Position = modelViewProj * (p1 - binormal1);
      EmitVertex();

	  vertex_color = vertex[0].color;
	  decalCoords = vec2(0.0,1.0);
	  brush = vertex[0].radius_Brush_drawingMode.y;
	  gl_Position = modelViewProj * (p1 + binormal1);
      EmitVertex();
    }

    if( drawingMode1 == DOT_DRAWING ) {
      vertex_color = vertex[0].color;
	  decalCoords = vec2(0.0,0.0);
	  brush = vertex[0].radius_Brush_drawingMode.y;
	  gl_Position = modelViewProj * (0.75*p1+0.25*p2 - binormal1);
      EmitVertex();

	  vertex_color = vertex[0].color;
      decalCoords = vec2(0.0,0.0);
	  brush = vertex[0].radius_Brush_drawingMode.y;
	  gl_Position = modelViewProj * (0.75*p1+0.25*p2 + binormal1);
      EmitVertex();

	  vertex_color = vertex[0].color;
      decalCoords = vec2(0.0,1.0);
	  brush = vertex[0].radius_Brush_drawingMode.y;
	  gl_Position = modelViewProj * (0.25*p1+0.75*p2 - binormal2);
      EmitVertex();

	  vertex_color = vertex[0].color;
      decalCoords = vec2(0.0,1.0);
	  brush = vertex[0].radius_Brush_drawingMode.y;
	  gl_Position = modelViewProj * (0.25*p1+0.75*p2 + binormal2);
      EmitVertex();

      EndPrimitive();
    }

    if( drawingMode2 == VOID_DRAWING ) {
      EndPrimitive();
    }
    else {
	  vertex_color = vertex[1].color;
      decalCoords = vec2(1.0,0.0);
	  brush = vertex[1].radius_Brush_drawingMode.y;
	  gl_Position = modelViewProj * (p2 - binormal2);
      EmitVertex();

	  vertex_color = vertex[1].color;
      decalCoords = vec2(1.0,1.0);
      brush = vertex[1].radius_Brush_drawingMode.y;
	  gl_Position = modelViewProj * (p2 + binormal2);
      EmitVertex();

      EndPrimitive();
    }
  }
}
