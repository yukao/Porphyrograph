/*! \file pg-udp.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-udp.h
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

#ifndef PG_SVG_TO_BITMAP_H
#define PG_SVG_TO_BITMAP_H

#define _SpaceChar(c) (c == ' ' || c == '\n' || c == 13 || c == '\t')
#define _Num(c) (c >= '0' && c <= '9')

void ReportError( char *errorString );
long     pg_ScanIntegerString( int *p_c  ,
			    int withTrailingSpaceChars , 
			    char *charstring , int *ind );
int pg_LoadTrackFromPath( char *pathString , int indTrack , 
			float *translation );
void pg_initStrokes( void );
void pg_readsvg( char *fileName , int indTrack , float boundingBoxX[2] , float boundingBoxY[2] );
void pg_load_svg_track( char *fileName , int indTrack , float boundingBoxX[2] , float boundingBoxY[2] );
void bezier_val( float param , float P0[2] , float P1[2] , 
		 float P2[2] , float P3[2] , float resul[2] );
void bezier_prime_val( float param , float P0[2] , float P1[2] , 
		 float P2[2] , float P3[2] , float resul[2] );
void BezierBoundingBox( float boundingBoxX[2] , float boundingBoxY[2] , float P[2*4] );
void pg_svg2bitmap( int indTrack , int w , int h , uchar* dataDestination , float scaleXY[2] , 
					float brushRadius , int brushSize , uchar* dataBrush );
void pg_writepng( char *fname , int w , int h , cv::Mat *imgOutput );
int pg_readpng( char *fname , int w , int h , bool invert , cv::Mat *imgInput );
#endif
