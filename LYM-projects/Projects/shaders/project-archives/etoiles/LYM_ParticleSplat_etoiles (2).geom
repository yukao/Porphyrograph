/*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation; either version 2.1
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
* 02111-1607, USA.
*/
/*
based on Francesco Caruso GLSLParametricCurves
https://github.com/fcaruso/GLSLParametricCurve
*/

#version 420
layout(points) in;
layout(line_strip, max_vertices = 16) out;

#define MOTEUR 0
#define SUSTAIN 1
#define DOPPLER 2
#define GLISSANDO 3

uniform vec3 uniform_ParticleSplat_gs_3fv_part_size_partType_highPitchPulse;

in VertexData {
    float radius;
    vec3 color;
    vec2 speed;
    vec2 replayTrackLocation;
} VertexIn[];
 
out VertexData {
    vec3 color;
} VertexOut;

// const int nbBranches = 16;
// const float angle = (3.1415926535897932384626433832795)/nbBranches;
///////////////////////
// MOTEUR
///////////////////////
const vec2 gearMoteur[16] = 
       {{0.750, 0.000}, {0.750, 0.000}, {0.779, 0.977}, {0.779, 0.977}, 
        {-0.167, 0.731}, {-0.167, 0.731}, {-1.126, 0.542}, {-1.126, 0.542}, 
        {-0.676, -0.325}, {-0.676, -0.325}, {-0.278, -1.219}, {-0.278, -1.219}, 
        {0.468, -0.586}, {0.468, -0.586}, {1.250, -0.000}, {1.250, -0.000}};
const vec2 antiGearMoteur[16] = 
       {{1.500, 0.000}, {1.370, 0.610}, {0.335, 0.372}, {0.464, 1.427}, 
        {-0.157, 1.492}, {-0.750, 1.299}, {-0.405, 0.294}, {-1.467, 0.312}, 
        {-1.467, -0.312}, {-1.214, -0.882}, {-0.250, -0.433}, {-0.157, -1.492}, 
        {0.464, -1.427}, {1.004, -1.115}, {0.457, -0.203}, {1.500, -0.000}};

///////////////////////
// SUSTAIN
///////////////////////
const vec2 circleSustain[16] = 
       {{1.000, 0.000}, {0.901, 0.434}, {0.623, 0.782}, {0.223, 0.975}, 
        {-0.223, 0.975}, {-0.623, 0.782}, {-0.901, 0.434}, {-1.000, 0.000}, 
        {-0.901, -0.434}, {-0.623, -0.782}, {-0.223, -0.975}, {0.223, -0.975}, 
        {0.623, -0.782}, {0.901, -0.434}, {1.000, -0.000}, {0.901, 0.434}};
const vec2 spikyCircleSustain[16] = 
       {{0.500, 0.000}, {1.351, 0.651}, {0.312, 0.391}, {0.334, 1.462}, 
        {-0.111, 0.487}, {-0.935, 1.173}, {-0.450, 0.217}, {-1.500, 0.000}, 
        {-0.450, -0.217}, {-0.935, -1.173}, {-0.111, -0.487}, {0.334, -1.462}, 
        {0.312, -0.391}, {1.351, -0.651}, {0.500, -0.000}, {1.351, 0.651}};

///////////////////////
// DOPPLER
///////////////////////
const vec2 lineDoppler[4] = 
       {{0.000, 1.000}, {1.000, 0.000}, {0.000, 1.000}, {1.000, 0.000}};
const vec2 triangleDoppler[4] = 
       {{1.000, 0.000}, {-0.500, 0.866}, {-0.500, -0.866}, {1.000, -0.000}};

///////////////////////
// GLISSANDO
///////////////////////
/* const vec2 starGlissando[16] = 
       {{1.000, 0.000}, {0.000, 0.000}, {0.924, 0.383}, {0.000, 0.000}, 
        {0.707, 0.707}, {0.000, 0.000}, {0.383, 0.924}, {0.000, 0.000}, 
        {0.000, 1.000}, {0.000, 0.000}, {-0.383, 0.924}, {-0.000, 0.000}, 
        {-0.707, 0.707}, {-0.000, 0.000}, {-0.924, 0.383}, {-0.000, 0.000}};
 */
const vec2 starGlissando[16] = 
       {{0.200, 0.000}, {0.254, 0.051}, {0.294, 0.122}, {0.313, 0.209}, 
        {0.308, 0.308}, {0.275, 0.411}, {0.212, 0.511}, {0.119, 0.601}, 
        {0.302, 0.729}, {0.384, 0.574}, {0.419, 0.419}, {0.411, 0.275}, 
        {0.366, 0.152}, {0.292, 0.058}, {0.200, 0.000}, {0.100, -0.020}};

const float canSurfCorners[2] = {-1, 1};


void main()
{
   
    vec4 position0 = gl_in[0].gl_Position;
    float distToReplayTrack = 1;
    if(VertexIn[0].replayTrackLocation.x >= -1) {
        distToReplayTrack = length(position0.xy-VertexIn[0].replayTrackLocation);
    }
    
    // sets the center in the middle of the particle
    // and calculates the size of the quad
    float internalRadiusTimesRadiusParameter 
      = VertexIn[0].radius * uniform_ParticleSplat_gs_3fv_part_size_partType_highPitchPulse.x;
    // internalRadiusTimesRadiusParameter *= distToReplayTrack * 0.5;


    // emits the polygon (a 16 vertices triangle strip)
    int partType = int(uniform_ParticleSplat_gs_3fv_part_size_partType_highPitchPulse.y);
    if(partType == MOTEUR) {
        vec2 barycentricWeights;
        float currPoint = position0.x;
        float sum = 0;
        for(int i = 0 ; i < 2 ; i++) {
            barycentricWeights[i] 
                = abs(currPoint - canSurfCorners[i]);
            sum += barycentricWeights[i];
        }
        barycentricWeights /= sum;

        for(int ind = 0 ; ind < 16 ; ind++) {
            vec2 barycenter = barycentricWeights[0] * gearMoteur[ind]
                            + barycentricWeights[1] * antiGearMoteur[ind];

            gl_Position = position0 
                  + vec4( internalRadiusTimesRadiusParameter * barycenter, 0, 0);

            VertexOut.color = VertexIn[0].color;
            EmitVertex();
        }
    }
    else if(partType == SUSTAIN) {
        float barycentricWeight;
        barycentricWeight
                = clamp(uniform_ParticleSplat_gs_3fv_part_size_partType_highPitchPulse.z,0,1);

        for(int ind = 0 ; ind < 16 ; ind++) {
            vec2 barycenter = barycentricWeight * spikyCircleSustain[ind]
                            + (1 - barycentricWeight) * circleSustain[ind];

            gl_Position = position0 
                  + vec4( internalRadiusTimesRadiusParameter * barycenter, 0, 0);

            VertexOut.color = VertexIn[0].color;
            EmitVertex();
        }
    }
    else if(partType == DOPPLER) {
        float barycentricWeight;
        float currPoint = position0.x;
        barycentricWeight = abs(currPoint);
        float xSign = sign(currPoint);

        for(int ind = 0 ; ind < 4 ; ind++) {
            vec2 barycenter = barycentricWeight * lineDoppler[ind]
                            + (1 - barycentricWeight) * triangleDoppler[ind];
            barycenter.x *= xSign;

            gl_Position = position0 
                  + vec4( internalRadiusTimesRadiusParameter * barycenter, 0, 0);

            VertexOut.color = VertexIn[0].color;
            EmitVertex();
        }
    }
    else if(partType == GLISSANDO) {
        vec2 normalizedSpeed = VertexIn[0].speed;
        float linSpeed = length(normalizedSpeed);
        normalizedSpeed /= linSpeed;
        mat2 rotation = mat2(normalizedSpeed,-normalizedSpeed.y,normalizedSpeed.x);

        vec2 newPos = internalRadiusTimesRadiusParameter * linSpeed * starGlissando[0];
        newPos = rotation * newPos;
        gl_Position = position0 + vec4(newPos , 0, 0);
        VertexOut.color = VertexIn[0].color;
        EmitVertex();
        for(int ind = 1 ; ind < 16 ; ind++) {
            vec2 newPos = internalRadiusTimesRadiusParameter * linSpeed * starGlissando[ind];
            newPos = rotation * newPos;
            gl_Position = position0 + vec4(newPos , 0, 0);

            VertexOut.color = VertexIn[0].color;
            EmitVertex();
        }
    }
   
   EndPrimitive();
}