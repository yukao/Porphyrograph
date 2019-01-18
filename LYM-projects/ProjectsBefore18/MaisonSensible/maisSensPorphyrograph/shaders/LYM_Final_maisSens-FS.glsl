/***********************************************************************
File: effe/shaders/LYM_Final_maisSens-FS.cg

LYM maisSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // Final pass output 

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh;
uniform vec2 uniform_Final_fs_2fv_double_window_proj2Reversed;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {

  float width = uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh.z;
  float height = uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh.w;
  vec2 texCoords = decalCoords;

  // single screen rendering
  if( uniform_Final_fs_2fv_double_window_proj2Reversed.x == 0 ) {
    texCoords.x = texCoords.x * 2;
    outColor0 = texture(fs_decal, texCoords );
  }

  // reversed projector is not taken in consideration for single projector so far
  else {
    // right projector reversed
    if( uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh.y != 0 
        && texCoords.x >= width / 2 )  {
      texCoords.x = 3 * width / 2 - texCoords.x;
      outColor0 = texture(fs_decal, vec2( texCoords.x , height-texCoords.y ));
    }
    // left projector reversed
    else if( uniform_Final_fs_2fv_double_window_proj2Reversed.y != 0 
             && texCoords.x < width / 2 )  {
      texCoords.x = width / 2 - texCoords.x;
      outColor0 = texture(fs_decal, vec2( texCoords.x , height-texCoords.y ));
    }
    else {
      outColor0 = texture(fs_decal, texCoords );
    }
  }

  // if( uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh.y != 0 )  {
  //   if( uniform_Final_fs_2fv_double_window_proj2Reversed != 0 ) {
  //       if( texCoords.x > width / 2 )  {
  //           texCoords.x = (width / 2 - (texCoords.x - width/2)) + width / 2;
  //       }
  //       else  {
  //           texCoords.x = width / 2 - texCoords.x;
  //       }
  //   }
  // }

  // // double screen rendering
  // if( uniform_Final_fs_2fv_double_window_proj2Reversed == 0 ) {
  //   texCoords.x = texCoords.x * 2;
  // }

  // outColor0 = vec4(0,0,0,1);
  // if( uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh.y == 0 ) {
  //   outColor0 = texture(fs_decal, texCoords );
  // }
  // else {
  //   outColor0 = texture(fs_decal, vec2( texCoords.x , height-texCoords.y ));
  // }

  // outputs the center of the screen for calibration purpose
  // of the image so that the points are visible
  float center = 3 * width/4;    // right center
  float center2 = width/4;       // left center
  float centerY = 0;             // right center
  float center2Y = 0;            // left center
  if(uniform_Final_fs_2fv_double_window_proj2Reversed.x == 0) {
    center = width/2;
    center2 = width/2;
  }
  if(uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh.y != 0) { // right reversed
    centerY = height;
  }
  if(uniform_Final_fs_2fv_double_window_proj2Reversed.y != 0) { // left reversed
    center2Y = height;
  }

  if( uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh.x != 0
    && ((abs(texCoords.x-center) < 10 && abs(centerY-texCoords.y) < 10)
        || (abs(texCoords.x-center2) < 10 && abs(center2Y-texCoords.y) < 10)) ) {
    outColor0 = vec4(1.0,1.0,0.0,1.0);
  }
}

