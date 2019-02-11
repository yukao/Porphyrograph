/***********************************************************************
File: alKemi/shaders/LYM_Sensor_geneMuse-FS.cg

LYM alKemi & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// Main shader.

/*in vec4 gl_TexCoord[gl_MaxTextureCoords];
*/in vec4 gl_Color;
/*in vec4 gl_FrontColor;
in vec4 gl_BackColor;
in vec4 gl_SecondaryColor;
in vec4 gl_FrontSecondaryColor;
in vec4 gl_BackSecondaryColor;
in float gl_FogFragCoord;
*/
/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  outColor0 = gl_Color;
}
