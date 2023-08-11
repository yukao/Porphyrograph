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

#version 420

////////////////////////////////////////////////////////////////////
// MACROS
#define graylevel(col) ((col.r+col.g+col.b)/3.0)

// comet texture
layout (binding = 2) uniform sampler2D uniform_ParticleSplat_texture_fs_decal; // splat texture

in VertexData {
    vec3 color;
    vec2 texCoord;
} VertexIn;
 
out vec4 fColor;

void main() {
	 fColor = texture(uniform_ParticleSplat_texture_fs_decal , VertexIn.texCoord);
	// fColor.rgb = VertexIn.color;
	fColor.rgb = VertexIn.color;
	if( int(graylevel(fColor.rgb) * 100) % 2 == 0) {
			 fColor.a = texture(uniform_ParticleSplat_texture_fs_decal , VertexIn.texCoord).r;
	}
	else {
			 fColor.a = texture(uniform_ParticleSplat_texture_fs_decal , VertexIn.texCoord).g;
	}
	// fColor.rgb = vec3(1,0,0);
	// fColor.a = 1;
}