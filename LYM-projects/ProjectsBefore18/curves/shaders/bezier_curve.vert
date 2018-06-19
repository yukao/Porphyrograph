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

#version 420

// vertex position update
layout (binding = 0) uniform samplerRect vp_decal; // control point positions

/////////////////////////////////////
// INPUT
layout(location = 0) in vec3 position;                     // espace objet
// layout(location = 1) in vec2 texcoord;

out vec2 decalCoords;

void main()
{
	// could  be .ba instead of .rg
	vec2 newPosition = texture(vp_decal , 
		                       vec2(int(position.x),int(position.y))).rg;
    gl_Position = vec4( newPosition, 0., 1. );

  	// // forwards texture coordinates
  	// decalCoords = texcoord;
}