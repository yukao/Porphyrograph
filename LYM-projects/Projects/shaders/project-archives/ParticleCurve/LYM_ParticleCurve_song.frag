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

// comet texture
uniform sampler2D fs_comet; // control point positions

in VertexData {
    vec2 texCoord;
} VertexIn;
 
out vec4 fColor;

void main()
{
	// fColor =  vec4( 1., 1., 1., 1. );
	 fColor = texture(fs_comet , VertexIn.texCoord);// * 0.1 + 0.9*vec4( 1., 1., 1., 1. );
}