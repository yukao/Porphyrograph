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
layout(triangle_strip, max_vertices = 4) out;

uniform vec3 uniform_ParticleSplat_gs_3fv_part_size_partType_highPitchPulse;

in VertexData {
    float radius;
    vec3 color;
    vec2 speed;
    vec2 replayTrackLocation;
} VertexIn[];
 
out VertexData {
    vec3 color;
    vec2 texCoord;
} VertexOut;

///////////////////////
// QUAD
///////////////////////
const vec2 quadGeometry[4] = 
       {{1, 1}, {1,-1}, {-1,1}, {1,-1}};
const vec2 quadTexCoord[4] = 
       {{1, 1}, {1,0}, {0,1}, {1,0}};

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
    internalRadiusTimesRadiusParameter *= distToReplayTrack * 0.5;


    // emits the polygon (a 4 vertices triangle strip)
    int partType = int(uniform_ParticleSplat_gs_3fv_part_size_partType_highPitchPulse.y);
    for(int ind = 0 ; ind < 4 ; ind++) {
        gl_Position = position0 
              + vec4( internalRadiusTimesRadiusParameter * quadGeometry[ind], 0, 0);

        VertexOut.color = VertexIn[0].color;
        VertexOut.texCoord = quadTexCoord[ind];
        EmitVertex();
    }
   
   EndPrimitive();
}