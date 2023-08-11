/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-FS.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text
in vec3 positionOut; // position
in vec3 normalOut; // normale
in vec4 glpositionOut; // position
in vec4 gl_FragCoord; // position

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_Mesh_texture_fs_decal; // previous pass output

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// UNIFORMS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// passed by the C program
uniform vec4 uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene;
uniform vec4 uniform_Mesh_fs_4fv_textureFrontier;
uniform vec4 uniform_Mesh_fs_4fv_textureFrontier_width;
uniform vec4 uniform_Mesh_fs_4fv_textureScaleTransl;

void main() {
  vec3 light = vec3(-1,-1,-0.5);
  vec3 normedNormal = normalOut / length(normalOut);

  vec2 scaledTranslatedCoords = (decalCoords * uniform_Mesh_fs_4fv_textureScaleTransl.xy 
                   + uniform_Mesh_fs_4fv_textureScaleTransl.zw);
  vec2 locCoords = scaledTranslatedCoords * vec2(1920,2160);

  float textureFrontier_wmin 
  = uniform_Mesh_fs_4fv_textureFrontier.x; // 100
  float textureFrontier_wmax 
  = uniform_Mesh_fs_4fv_textureFrontier.y; // 1770
  float textureFrontier_wmin_width 
  = uniform_Mesh_fs_4fv_textureFrontier_width.x; // 50
  float textureFrontier_wmax_width 
  = uniform_Mesh_fs_4fv_textureFrontier_width.y; // 10

  float textureFrontier_hmin 
  = uniform_Mesh_fs_4fv_textureFrontier.z; // 100
  float textureFrontier_hmax 
  = uniform_Mesh_fs_4fv_textureFrontier.w; // 1770
  float textureFrontier_hmin_width 
  = uniform_Mesh_fs_4fv_textureFrontier_width.z; // 50
  float textureFrontier_hmax_width 
  = uniform_Mesh_fs_4fv_textureFrontier_width.w; // 10

  int currentScene = int(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene.w);

  vec3 borderColor = vec3(-1);

  // black screen on the second projector which also points towards the ground
  if((currentScene <= 5 || (currentScene >= 11 && currentScene <= 14)) && gl_FragCoord.y <= 1080) {
    outColor0 = vec4(0,0,0,1);
    return;
  }
  // black bottom
  if(gl_FragCoord.y < 10) {
    outColor0 = vec4(0,0,0,1);
    return;
  }
  // black bottom
/*  if(currentScene >= 5 && gl_FragCoord.y < 750) {
    outColor0 = vec4(0,0,0,1);
    return;
  }
*/
  // remove top
  if(positionOut.z > 10) {
    outColor0 = vec4(0,0,0,1);
    return;
  }
  locCoords.x = int(locCoords.x) % 1920;
  locCoords.y = int(locCoords.y) % 2160;

  vec4 meshColor = vec4(vec3(0), 1);

/*  if(all(lessThanEqual(scaledTranslatedCoords,vec2(1))) 
    && all(greaterThanEqual(scaledTranslatedCoords,vec2(0)))) {
*/    meshColor = texture(fs_Mesh_texture_fs_decal, locCoords) ;
  /*}
*/
  light /= length(light);
  float gray = clamp( dot(normedNormal, light) , 0, 1);

  // outputs a small point for calibration of the position of the videoprojector 
  // after moving from ground to wall position
  vec2 uv = gl_FragCoord.xy;
  if(currentScene == 5 && uv.x > 1640 && uv.x <= 1650 && uv.y > 1715 && uv.y <= 1720) {
    // pointeur pour la position du VP
    outColor0 = vec4(1.0,1.0,1.0,1.0);
    // outColor0.rgb  *= transparency;
    return;
  }

  // outputs the center of the screen for calibration purpose
  // of the image so that the points are visible
  if( uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene.x > 0
    && abs(glpositionOut.x/glpositionOut.w) < 0.02 && glpositionOut.y/glpositionOut.w < -0.98 ) {
    outColor0 = vec4(1.0,1.0,0.0,1.0);
    // outColor0.rgb  *= transparency;
    return;
  }

  // line display
  if(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene.y == 0) {
    //outColor0 = vec4( 0.1 * vec3(gray) + 0.9 * meshColor.rgb, 1);
      outColor0 = meshColor;
  }
  else {
      outColor0 = vec4(1.0,1.0,1.0,1.0);
      return;
  }

  // removes the top
  if(positionOut.y < 3 && positionOut.z > 5)  {
      outColor0 = vec4(0,0,0,1);
      return;
  }

  // removes the left side column in the first scenes (projection on the ground)
  if(currentScene <= 5 && positionOut.z > 0.001)  {
      outColor0 = vec4(0,0,0,1);
      return;
  }

  // uniform dark blue background
  /*
  if( uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene.z > 0 ) {
    // left is green
    if( true ) {
      outColor0 = vec4(0.0,0.2,0.0,1.0);
      return;
    }
    // right is blue
    else {
      outColor0 = vec4(0.0,0.0,0.2,1.0);
      return;
    }
  }
  */

  // mesh frontier management
  // side border interpolations
  if(all(lessThanEqual(locCoords,vec2(textureFrontier_wmax + textureFrontier_wmax_width,
                                      textureFrontier_hmax + textureFrontier_hmax_width))) 
      && all(greaterThanEqual(locCoords,vec2(textureFrontier_wmin - textureFrontier_wmin_width,
                                             textureFrontier_hmin - textureFrontier_hmin_width)))) {
    if( locCoords.x > textureFrontier_wmax  ) {
      if( locCoords.x < textureFrontier_wmax + textureFrontier_wmax_width ) {
        borderColor = mix( outColor0.rgb , vec3(0.0 , 0.0 , 0.0 ) , 
          (locCoords.x - textureFrontier_wmax)/ textureFrontier_wmax_width );
      }
      else {
        borderColor = vec3(0.0 , 0.0 , 0.0);
      }
    }
    if( locCoords.x < textureFrontier_wmin  ) {
      if( locCoords.x > textureFrontier_wmin - textureFrontier_wmin_width ) {
        borderColor = mix( outColor0.rgb , vec3(0.0 , 0.0 , 0.0 ) , 
          (textureFrontier_wmin - locCoords.x) / textureFrontier_wmin_width );
      }
      else {
        borderColor = vec3(0.0 , 0.0 , 0.0);
      }
    }

    if( borderColor.r >= 0 ) {
      outColor0.rgb = borderColor;
    }

    // top/bottom border interpolations
    if( locCoords.y > textureFrontier_hmax  ) {
      if( locCoords.y < textureFrontier_hmax + textureFrontier_hmax_width ) {
        borderColor = mix( outColor0.rgb , vec3(0.0 , 0.0 , 0.0 ) , 
          (locCoords.y - textureFrontier_hmax)/ textureFrontier_hmax_width );
      }
      else {
        borderColor = vec3(0.0 , 0.0 , 0.0);
      }
    }
    if( locCoords.y < textureFrontier_hmin  ) {
      if( locCoords.y > textureFrontier_hmin - textureFrontier_hmin_width ) {
        borderColor = mix( outColor0.rgb , vec3(0.0 , 0.0 , 0.0 ) , 
          (textureFrontier_hmin - locCoords.y) / textureFrontier_hmin_width );
      }
      else {
        borderColor = vec3(0.0 , 0.0 , 0.0);
      }
    }

    if( borderColor.r >= 0 ) {
      outColor0.rgb = borderColor;
    }
  }
  else {
      outColor0.rgb = vec3(0);
  }
}