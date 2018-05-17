/*! \file pg-bezierCurve.h
*
*
*     File pg-bezierCurve.h
*
*
*
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

#ifndef PG_BEZIERCURVE_H
#define PG_BEZIERCURVE_H

#define PG_NB_BEZIER_CURVES 3000
#define randomValue ((float)rand()/(float)RAND_MAX)
#define PG_CURVE_DEGREE 3

#define PG_NB_MEMORIZED_PASS 2
#define NB_ATTACHMENTS 1

#define PG_WIDTH 1024
#define PG_HEIGHT 768

extern int leftWindowWidth;
extern int window_height;

extern GLuint drawBuffers[16];
	
extern GLuint FBO_BezierUpdate[PG_NB_MEMORIZED_PASS];
extern GLuint FBO_BezierUpdate_texID[PG_NB_MEMORIZED_PASS];
extern GLuint FBO_BezierEcho[PG_NB_MEMORIZED_PASS];
extern GLuint FBO_BezierEcho_texID[PG_NB_MEMORIZED_PASS];
extern GLuint FBO_BezierCurves;
extern GLuint FBO_BezierCurves_texID;

extern GLuint pg_bezierCurve_Pos_Texture_texID;
extern GLfloat *pg_bezierCurve_Pos_Texture;

extern cv::Mat comet_image;
extern GLuint comet_texture_2D_texID;

#endif
