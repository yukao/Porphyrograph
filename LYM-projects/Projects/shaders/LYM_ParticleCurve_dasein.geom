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
* 02111-1307, USA.
*/
/*
based on Francesco Caruso GLSLParametricCurves
https://github.com/fcaruso/GLSLParametricCurve
*/

#version 400
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec3 uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse;

in VertexData {
    vec2 texCoord;
    vec2 normal;
    float radius;
    vec3 color;
    vec2 speed;
    vec2 replayTrackLocation;
} VertexIn[2];
 
out VertexData {
    vec2 texCoord;
    vec3 color;
} VertexOut;

void main()
{
   
    vec4 position0 = gl_in[0].gl_Position;
    vec4 position1 = gl_in[1].gl_Position;

    float distToReplayTrack = 1;
    if(VertexIn[0].replayTrackLocation.x >= -1) {
        distToReplayTrack = length(position0.xy-VertexIn[0].replayTrackLocation);
    }

    gl_Position = position0;
    VertexOut.texCoord = vec2(VertexIn[0].texCoord.x,0);
    VertexOut.color = VertexIn[0].color;
    EmitVertex();
   
    gl_Position = position0 + VertexIn[0].radius * vec4(VertexIn[0].normal,0,0)
                                * uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse.x
                                * distToReplayTrack;
    VertexOut.texCoord = vec2(VertexIn[0].texCoord.x,1);
    VertexOut.color = VertexIn[0].color;
    EmitVertex();
   
    gl_Position = position1;
    VertexOut.texCoord = vec2(VertexIn[1].texCoord.x,0);
    VertexOut.color = VertexIn[1].color;
    EmitVertex();
   
    gl_Position = position1 + VertexIn[1].radius * vec4(VertexIn[1].normal,0,0)
                                * uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse.x
                                * distToReplayTrack;
    VertexOut.texCoord = vec2(VertexIn[1].texCoord.x,1);
    VertexOut.color = VertexIn[1].color;
    EmitVertex();
   
    // EndPrimitive();
}