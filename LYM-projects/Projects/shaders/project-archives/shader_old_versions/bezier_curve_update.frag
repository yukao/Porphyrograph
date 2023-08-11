/***********************************************************************
File: effe/shaders/bezier-curve-update-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// obtained from Vertex Program
layout (binding = 0) uniform samplerRect fs_posSpeedAtPrecedingFrame; // position at previous frame
layout (binding = 1) uniform samplerRect fs_controlPoint_positions; // initial control points positions
uniform int  flashPositionTexture;
uniform float  noiseScale;
uniform float  damping;

/////////////////////////////////////
// INPUT
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// UPDATED OUTPUT
out vec4 outColor0;

// Some useful functions
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }
float snoise(vec2 v , float param_noiseScale) {
    v *= param_noiseScale;

    // Precompute values for skewed triangular grid
    const vec4 C = vec4(0.211324865405187,
                        // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  
                        // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  
                        // -1.0 + 2.0 * C.x
                        0.024390243902439); 
                        // 1.0 / 41.0

    // First corner (x0)
    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    // Other two corners (x1, x2)
    vec2 i1 = vec2(0.0);
    i1 = (x0.x > x0.y)? vec2(1.0, 0.0):vec2(0.0, 1.0);
    vec2 x1 = x0.xy + C.xx - i1;
    vec2 x2 = x0.xy + C.zz;

    // Do some permutations to avoid
    // truncation effects in permutation
    i = mod289(i);
    vec3 p = permute(
            permute( i.y + vec3(0.0, i1.y, 1.0))
                + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(
                        dot(x0,x0), 
                        dot(x1,x1), 
                        dot(x2,x2)
                        ), 0.0);

    m = m*m ;
    m = m*m ;

    // Gradients: 
    //  41 pts uniformly over a line, mapped onto a diamond
    //  The ring size 17*17 = 289 is close to a multiple 
    //      of 41 (41*7 = 287)

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt(a0*a0 + h*h);
    m *= 1.79284291400159 - 0.85373472095314 * (a0*a0+h*h);

    // Compute final noise value at P
    vec3 g = vec3(0.0);
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * vec2(x1.x,x2.x) + h.yz * vec2(x1.y,x2.y);
    return (130.0 * dot(m, g) + 1.0) / 2.0;
}

void main()
{
  vec4 currentPositionSpeed;
  if(flashPositionTexture == 1) {
  	// copies the input texture
  	currentPositionSpeed 
      = vec4(texture(fs_controlPoint_positions, decalCoords ).rg,0,0);
  }
  else {
    // copies position and speed at preceding frame
    currentPositionSpeed = texture(fs_posSpeedAtPrecedingFrame, decalCoords );

  	// for the head of the curve, takes acceleration from noise strength field
    int index = int(decalCoords.x) % 4;
    if( index == 0 ) {
      vec2 double_accelerations 
          = vec2(snoise( decalCoords , noiseScale * 100 ),
                 snoise( decalCoords + vec2(2.0937,9.4872) , noiseScale * 100 ));
      currentPositionSpeed.zw += 0.2 * (double_accelerations-vec2(0.5,0.5));
      currentPositionSpeed.zw -= length(currentPositionSpeed.zw) * damping * currentPositionSpeed.zw;
    }
    // for the rest of the curve, it follows the head
    else {
      vec4 headCurrentPositionSpeed 
            = texture(fs_posSpeedAtPrecedingFrame, decalCoords-vec2(index,0));
      float headCurrentSpeed = length( headCurrentPositionSpeed.zw);
      vec2  lookatHead = normalize(headCurrentPositionSpeed.xy - currentPositionSpeed.xy);
      currentPositionSpeed.zw = headCurrentSpeed *0.9* lookatHead;
      if(index ==1 || index==2) {
        currentPositionSpeed.zw += (snoise( decalCoords + vec2(9.4872,2.0937) , noiseScale * 100 ) - 0.5)
          * vec2(lookatHead.y,-lookatHead.x);
      }
    }

    currentPositionSpeed.xy += currentPositionSpeed.zw;
    if(currentPositionSpeed.x < 0 || currentPositionSpeed.x > width) {
        currentPositionSpeed.z *= -1;
    }
    if(currentPositionSpeed.y < 0 || currentPositionSpeed.y > height) {
        currentPositionSpeed.w *= -1;
    }
  }
  outColor0 = currentPositionSpeed;
}