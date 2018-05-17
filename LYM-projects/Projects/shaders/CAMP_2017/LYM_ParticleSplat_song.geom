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
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float uniform_ParticleSplat_fs_1fv_partRadius;

in VertexData {
    float radius;
    vec3 color;
} VertexIn[];
 
out VertexData {
    vec2 texCoord;
    vec3 color;
} VertexOut;

void main()
{
   
    vec4 position0 = gl_in[0].gl_Position;
    
    // sets the center in the middle of the particle
    // and calculates the size of the quad
    float internalRadiusTimesRadiusParameter 
      = VertexIn[0].radius * uniform_ParticleSplat_fs_1fv_partRadius;

    // emits the quad (a 4 vertices triangle strip)
    gl_Position = position0 
        + vec4( -internalRadiusTimesRadiusParameter,
                -internalRadiusTimesRadiusParameter,0,0);
    VertexOut.texCoord = vec2(0,0);
    VertexOut.color = VertexIn[0].color;
    EmitVertex();
   
    gl_Position = position0 
        + vec4( -internalRadiusTimesRadiusParameter,
                 internalRadiusTimesRadiusParameter,0,0);
    VertexOut.texCoord = vec2(1,0);
    VertexOut.color = VertexIn[0].color;
    EmitVertex();
   
    gl_Position = position0 
        + vec4(  internalRadiusTimesRadiusParameter,
                -internalRadiusTimesRadiusParameter,0,0);
    VertexOut.texCoord = vec2(0,1);
    VertexOut.color = VertexIn[0].color;
    EmitVertex();
   
    gl_Position = position0 
        + vec4( internalRadiusTimesRadiusParameter,
                internalRadiusTimesRadiusParameter,0,0);
    VertexOut.texCoord = vec2(1,1);
    VertexOut.color = VertexIn[0].color;
    EmitVertex();
   
   EndPrimitive();
}