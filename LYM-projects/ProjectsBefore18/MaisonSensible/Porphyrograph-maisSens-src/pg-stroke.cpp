/*! \file pg-stroke.cpp
 * 
 *
 *     File pg-stroke.cpp
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

#include "pg-all_include.h"


bool isDrawingOn = false;


////////////////////////////////////////////////////////////////
// SPLINES
////////////////////////////////////////////////////////////////
#define Max(x,y)  ((x>y)?x:y)
#define Min(x,y)  ((y>x)?x:y)
#define Lerp(x,y,s)  (x*(1-s) + y*s)

//////////////////////////////////////////////////////////////////
// MEMORY STORAGE OF THE CURRENT STROKE  
//////////////////////////////////////////////////////////////////
// update of the tables that contain thecurrrentTrack stroke parameters
void updateMouseEnvironmentVariablesAndTables( double theTime ) {
  // the storage in the preceding mouse position variable is
  // made in the script of the scene XML
  // -1 is background layer
	
	if( /*(impact_period > 0 && FrameNo % impact_period == 0) ||*/ newImpact ) {
		mouse_x_prev = -1000.0f;
		mouse_y_prev = -1000.0f;
	    // message that gives explicitely the position of the impact
		if( newImpact ) {
			CurrentMousePos_x = (int)impactX;
			CurrentMousePos_y = (int)impactY;
		}
		// steady impact in the middle of the screen
		//else if( !impact_random ) {
		//	CurrentMousePos_x = 1000 + (int)(((double)rand()/(double)RAND_MAX) * 10);
		//	CurrentMousePos_y = 128  + (int)(((double)rand()/(double)RAND_MAX) * 10);
		//}
		// random impact all over the screen
		//else {
		//	CurrentMousePos_x = (int)(((double)rand()/(double)RAND_MAX) * 1024) + 512;
		//	CurrentMousePos_y = (int)(((double)rand()/(double)RAND_MAX) * 256);
		//}
		PreviousCurrentMousePos_x = CurrentMousePos_x;
		PreviousCurrentMousePos_y = CurrentMousePos_y;
		// printf("Impact!!! %d %d\n" , CurrentMousePos_x , CurrentMousePos_y );
	}
	// repeats the random values that were made at preceding step so that they
	// are given twice and a stroke takes place
	if( (impact_period > 0 && FrameNo % impact_period == 1) || newImpact ) {
		CurrentMousePos_x = PreviousCurrentMousePos_x + 1;
		CurrentMousePos_y = PreviousCurrentMousePos_y + 1;
		newImpact = false;
	}
	//printf("Impact!!! impact_period %d FrameNo %d activeDrawingStroke%d\n" , impact_period , FrameNo , activeDrawingStroke );

  // memory of current and preceding values
  mouse_x = (float)CurrentMousePos_x;
  mouse_y = (float)CurrentMousePos_y;

  // memory about drawing at preceding step
  bool wasDrawingOn = isDrawingOn;
  isDrawingOn = (activeDrawingStroke > 0);

  	//printf("Impact!!! isDrawingOn %d wasDrawingOn %d\n" , isDrawingOn , wasDrawingOn );

    if( (mouse_x_prev != CurrentMousePos_x
	 || mouse_y_prev != CurrentMousePos_y)	&& isDrawingOn ) {
      // printf("mouse pos %dx%d %d\n",CurrentMousePos_x,CurrentMousePos_y,0);
      tracks_Color_r[ 0 ] = local_color_r
		  + color_spectrum[0] * color_spectrum_coef;
      tracks_Color_g[ 0 ] = local_color_g
		  + color_spectrum[1] * color_spectrum_coef;
      tracks_Color_b[ 0 ] = local_color_b
		  + color_spectrum[2] * color_spectrum_coef;
      tracks_Color_a[ 0 ] = local_color_a
		  + color_spectrum[3];
       // printf("Track %d color a %.2f %.2f %.2f %.2f\n" , 0, local_color_r
	 //+ color_spectrum[1] , 
	 //local_color_g 
	 //+ color_spectrum[1] , 
	 //local_color_b 
	 //+ color_spectrum[2] , 
	 //local_color_a
	 //+ color_spectrum[3]  ); 
      tracks_BrushID[ 0 ] = (int)(brushID * (1.0 + brush_radius_vol_coef * brush_radius_vol));
#ifdef WACOM_TABLET
      tracks_RadiusX[ 0 ] 
	= brushRadius;
      tracks_RadiusY[ 0 ]
	= brushRadius;
#else
     tracks_RadiusX[ 0 ] = brushRadius;
     tracks_RadiusY[ 0 ] = brushRadius;
#endif
      // printf("Track %d brush ID radius %d %.2f\n" , 0, brushID, brushRadius );
    }

}


