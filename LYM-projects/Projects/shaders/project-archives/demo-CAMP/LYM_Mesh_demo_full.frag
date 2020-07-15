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
// in vec4 glpositionOut; // position

/////////////////////////////////////
// INPUT

/* ORIGINAL FILE FROM LA MAISON SENSIBLE */
/**********************************************************************
uniform samplerRect fs_decal; // previous pass output

uniform ivec4 uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG;

uniform vec4 uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width;

uniform vec3 uniform_MeshDisplay_fs_3fv_blendTransp_isFlatRendering_whiteText;

uniform ivec2 uniform_MeshDisplay_fs_2i_FBOwh;
uniform vec4 uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth;
uniform vec4 uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth;

float textureFrontier_wmin 
= uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth.x; // 100
float textureFrontier_wmax 
= uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth.y; // 1770
float textureFrontier_wmin_width 
= uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth.z; // 50
float textureFrontier_wmax_width 
= uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth.w; // 10

float textureFrontier_hmin 
= uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth.x; // 100
float textureFrontier_hmax 
= uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth.y; // 1770
float textureFrontier_hmin_width 
= uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth.z; // 50
float textureFrontier_hmax_width 
= uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth.w; // 10

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

vec3 myMix( vec3 col1 , vec3 col2 ,float coef ) {
  float coefNew = pow( coef , 1.5 );
  return mix( col1 , col2 , coefNew);
}

float meshFrontier_X
= uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width.z; // 3.0
float meshFrontier_width 
= uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width.w; // 0.1

void main() {
  float FrameNo = uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width.x;
  float breathing = uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width.y;
  float transparency = uniform_MeshDisplay_fs_3fv_blendTransp_isFlatRendering_whiteText.x;
  bool isLeftCam = (uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG.y == 1);

  // flat rendering, just renders the texture
  if( uniform_MeshDisplay_fs_3fv_blendTransp_isFlatRendering_whiteText.y > 0 ) {
      outColor0 = texture(fs_decal, decalCoords ).grba;
      return;
  }

  // outputs the center of the screen for calibration purpose
  // of the image so that the points are visible
  //  if( uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG.z > 0
    // && abs(glpositionOut.x/glpositionOut.w) < 0.02 && glpositionOut.y/glpositionOut.w < -0.98 ) {
    // outColor0 = vec4(1.0,1.0,0.0,1.0);
    // outColor0.rgb  *= transparency;
    // return;
  // }

  if( uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG.x > 0 ) {
      outColor0 = vec4(1.0,0.0,0.0,1.0);
      outColor0.rgb  *= transparency;
      return;
  }
  // ONLY DISPLAYS MESH + CENTER & uniform dark blue background
  else if( uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG.w > 0 ) {
    // left is green
    if( isLeftCam ) {
      outColor0 = vec4(0.0,0.2,0.0,1.0);
      return;
    }
    // right is blue
    else {
      outColor0 = vec4(0.0,0.0,0.2,1.0);
      return;
    }
  }

  if( decalCoords.x >= 0.0 && 
	  decalCoords.y >= 0.0 && 
	  decalCoords.x <= uniform_MeshDisplay_fs_2i_FBOwh.x && 
	  decalCoords.y <= uniform_MeshDisplay_fs_2i_FBOwh.y ) {
    vec2 breathing_decalCoords = decalCoords 
          + breathing * vec2( 3 * sin( decalCoords.x / 179.0 + FrameNo/153.0 ) - 3, 
                              3 * sin( decalCoords.y / 91.0 + FrameNo/137.0) - 3 );

    if( breathing_decalCoords.x >= 0.0 
        && breathing_decalCoords.y >= 0.0 
        && breathing_decalCoords.x < uniform_MeshDisplay_fs_2i_FBOwh.x
        && breathing_decalCoords.y < uniform_MeshDisplay_fs_2i_FBOwh.y ) {
      if( uniform_MeshDisplay_fs_3fv_blendTransp_isFlatRendering_whiteText.z > 0) {
        outColor0 = vec4(1);
      }
      else {
        outColor0 = texture(fs_decal, breathing_decalCoords ).grba;
      }
    }
    else {
      outColor0 = vec4(0.0,00,0.0,1.0);
    }

    vec3 borderColor = vec3(-1);

    // side border interpolations
    if( decalCoords.x > textureFrontier_wmax  ) {
      if( decalCoords.x < textureFrontier_wmax + textureFrontier_wmax_width ) {
        borderColor = mix( outColor0.rgb , vec3(0.0 , 0.0 , 0.0 ) , 
          (decalCoords.x - textureFrontier_wmax)/ textureFrontier_wmax_width );
      }
      else {
        borderColor = vec3(0.0 , 0.0 , 0.0);
      }
    }
    if( decalCoords.x < textureFrontier_wmin  ) {
      if( decalCoords.x > textureFrontier_wmin - textureFrontier_wmin_width ) {
        borderColor = mix( outColor0.rgb , vec3(0.0 , 0.0 , 0.0 ) , 
          (textureFrontier_wmin - decalCoords.x) / textureFrontier_wmin_width );
      }
      else {
        borderColor = vec3(0.0 , 0.0 , 0.0);
      }
    }

    if( borderColor.r >= 0 ) {
      outColor0.rgb = borderColor;
    }

    // top/bottom border interpolations
    if( decalCoords.y > textureFrontier_hmax  ) {
      if( decalCoords.y < textureFrontier_hmax + textureFrontier_hmax_width ) {
        borderColor = mix( outColor0.rgb , vec3(0.0 , 0.0 , 0.0 ) , 
          (decalCoords.y - textureFrontier_hmax)/ textureFrontier_hmax_width );
      }
      else {
        borderColor = vec3(0.0 , 0.0 , 0.0);
      }
    }
    if( decalCoords.y < textureFrontier_hmin  ) {
      if( decalCoords.y > textureFrontier_hmin - textureFrontier_hmin_width ) {
        borderColor = mix( outColor0.rgb , vec3(0.0 , 0.0 , 0.0 ) , 
          (textureFrontier_hmin - decalCoords.y) / textureFrontier_hmin_width );
      }
      else {
        borderColor = vec3(0.0 , 0.0 , 0.0);
      }
    }

    if( borderColor.r >= 0 ) {
      outColor0.rgb = borderColor;
    }

    // mesh frontier management
    if( isLeftCam ) {
      if( positionOut.x <= meshFrontier_X - meshFrontier_width ) {
        borderColor = vec3(0.0,0.0,0.0);
      }
      else if( positionOut.x >= meshFrontier_X - meshFrontier_width 
        && positionOut.x <= meshFrontier_X + meshFrontier_width ) {
        borderColor = myMix( outColor0.rgb , vec3(0.0,0.0,0.0) , 
          ((meshFrontier_X + meshFrontier_width) - positionOut.x) / (2 * meshFrontier_width) );
      }
    }
    else {
      if( positionOut.x >= meshFrontier_X + meshFrontier_width ) {
        borderColor = vec3(0.0,0.0,0.0);
      }
      else if( positionOut.x >= meshFrontier_X - meshFrontier_width 
        && positionOut.x <= meshFrontier_X + meshFrontier_width ) {
        borderColor = myMix( outColor0.rgb , vec3(0.0,0.0,0.0) , 
            (positionOut.x - (meshFrontier_X - meshFrontier_width)) / (2 * meshFrontier_width) );
      }
    }

    if( borderColor.r >= 0 ) {
      outColor0.rgb = borderColor;
    }

    outColor0.rgb *= transparency;
  }
  else {
    outColor0 = vec4(0.0,0.0,0.0,1.0);
  }
  outColor0.rgb  *= transparency;
}
****************************************************/

/* simple light shading */

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // previous pass output

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  vec3 light = vec3(1,1,1);
  vec3 normedNormal = normalOut / length(normalOut);

  vec4 meshColor = texture(fs_decal, decalCoords * vec2(1024));

  light /= length(light);
  float gray = clamp( dot(normedNormal, light) , 0, 1);
  outColor0 = vec4( 0.5 * vec3(gray) + 0.5 * meshColor.rgb, 1);
}