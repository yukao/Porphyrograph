////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg_wav_RW.cpp
// 
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include "pg-all_include.h"

#define PG_IMAGE_W 1024
#define PG_IMAGE_H 1024
#define PG_BRUSH_SIZE 2048
#define PG_NB_SPLINES_MAX 1000

char *ErrorStr = NULL;

float **pg_TrackPos_x_source = NULL;
float **pg_TrackPos_y_source = NULL;
float **pg_TrackPos_xL_source = NULL;
float **pg_TrackPos_yL_source = NULL;
float **pg_TrackPos_xR_source = NULL;
float **pg_TrackPos_yR_source = NULL;
int *pg_TrackNBSplines;


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// SVG FILE LOADING
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A SVG FILE
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// ERRORS
/////////////////////////////////////////////////////////////////
void ReportError( char *errorString ) {
  
  fprintf( stderr , "%s\n" , errorString );
}

/////////////////////////////////////////////////////////////////
// NUMBER SCANNING
/////////////////////////////////////////////////////////////////
long     pg_ScanIntegerString( int *p_c  ,
			    int withTrailingSpaceChars , 
			    char *charstring , int *ind ) {
  long            i = 0L;
  short           neg = 0;

  if( *p_c == '-') {
    neg = 1;
    *p_c = charstring[*ind];
    (*ind)++;
  }
  else if( *p_c == '+') {
    *p_c = charstring[*ind];
    (*ind)++;
  }
  if (_Num(*p_c)) {
    while ( _Num(*p_c) ) {
      i = i*10L + (long)(*p_c - '0');
      *p_c = charstring[*ind];
      (*ind)++;
    }

    if( withTrailingSpaceChars ) {
      while( _SpaceChar(*p_c) ) {
	*p_c = charstring[*ind];
	(*ind)++;
      }
    }
    if( neg == 1)
      i = -i;
    return i;
  }
  else {
    return 0L;
  }
}

float    pg_ScanFloatString( int *p_c  ,
		   int withTrailingSpaceChars , 
		   char *charstring , int *ind ) {
  float           resul, dec = 0.1F;
  short           neg = 0;

  // arithmetic expression
  if( *p_c == '-' ) {
    neg = 1;
    *p_c = charstring[*ind];
    (*ind)++;
  }
  else if( *p_c == '+' ) {
    *p_c = charstring[*ind];
    (*ind)++;
  }
  resul = (float) pg_ScanIntegerString( p_c , false ,charstring,ind );
  if( *p_c == '.') {
    *p_c = charstring[*ind];
    (*ind)++;

    while ( _Num(*p_c) ) {
      resul += dec * (*p_c - '0');
      dec *= 0.1F;
      *p_c = charstring[*ind];
      (*ind)++;
    }
  }
  // exponent
  if(  *p_c == 'e' ) {
    *p_c = charstring[*ind];
    (*ind)++;
    float exponent = (float)pg_ScanIntegerString( p_c , false ,charstring,ind );
    resul *= powf( 10.0 , exponent );
 }
  if( withTrailingSpaceChars ) {
    while( _SpaceChar(*p_c) ) {
      *p_c = charstring[*ind];
      (*ind)++;
    }
  }
  if( neg == 1)
    resul = -resul;
  return resul;
}

//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A PATH STRING
//////////////////////////////////////////////////////////////////
int pg_LoadTrackFromPath( char *pathString , int indTrack , 
			float *translation ) {
  int            indSpline = 0;
  int            curChar = ' ';
  int            indChar = 0;
  float          currentPoint[2] = { 0.0 , 0.0 };  
  float          curveInitialPoint[2] = { 0.0 , 0.0 };
  float          polyCurveInitialPoint[2] = { 0.0 , 0.0 };

  while( _SpaceChar(curChar) ) {
    curChar = pathString[ indChar++ ];
  }
  while( indChar <= (int)strlen( pathString ) && curChar != EOF  && curChar != '"' ) {
    switch( curChar ) {
    case 'm':
    case 'M':
      {
	// RULES
	// Start a new sub-path at the given (x,y) coordinate. M
	// (uppercase) indicates that absolute coordinates will
	// follow; m (lowercase) indicates that relative coordinates
	// will follow. If a moveto is followed by multiple pairs of
	// coordinates, the subsequent pairs are treated as implicit
	// lineto commands. Hence, implicit lineto commands will be
	// relative if the moveto is relative, and absolute if the
	// moveto is absolute. If a relative moveto (m) appears as the
	// first element of the path, then it is treated as a pair of
	// absolute coordinates. In this case, subsequent pairs of
	// coordinates are treated as relative even though the initial
	// moveto is interpreted as an absolute moveto.
	int nbMovePoints = 0;
	bool relative = (curChar == 'm');
	curChar = pathString[ indChar++ ];
	while( _SpaceChar(curChar) ) {
	  curChar = pathString[ indChar++ ];
	}
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 
	  if( relative  && indSpline != 0 )
	    currentPoint[0] += pg_ScanFloatString( &curChar , true  , 
					 pathString , &indChar );
	  else
	    currentPoint[0] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  if( curChar == ',' ) {
	    curChar = pathString[ indChar++ ];
	    while( _SpaceChar(curChar) ) {
	      curChar = pathString[ indChar++ ];
	    }
	  }
	  else {
	    sprintf( ErrorStr , "Expected char [%c] not found!" , ',' ); ReportError( ErrorStr ); throw 17;
	  }
	  if( relative  && indSpline != 0 )
	    currentPoint[1] += pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  else
	    currentPoint[1] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  while( _SpaceChar(curChar) ) {
	    curChar = pathString[ indChar++ ];
	  }
	  nbMovePoints++;
	  if( indSpline < PG_NB_SPLINES_MAX ) {
	    pg_TrackPos_x_source[ indTrack ][ indSpline ] 
	      = currentPoint[0] + translation[0];
	    pg_TrackPos_y_source[ indTrack ][ indSpline ] 
	      = currentPoint[1] + translation[1];
	    indSpline++;
	  }
	  else {
	    sprintf( ErrorStr , "Error: number of splines above PG_NB_SPLINES_MAX [%d]!" , indSpline ); ReportError( ErrorStr ); throw 17;
	  }
	}
	polyCurveInitialPoint[0] = currentPoint[0];
	polyCurveInitialPoint[1] = currentPoint[1];
      }
      break;
    case 'c':
    case 'C':
      {
	// RULES 
	// Draws a cubic Bézier curve from the current point to
	// (x,y) using (x1,y1) as the control point at the beginning
	// of the curve and (x2,y2) as the control point at the end of
	// the curve. C (uppercase) indicates that absolute
	// coordinates will follow; c (lowercase) indicates that
	// relative coordinates will follow. Multiple sets of
	// coordinates may be specified to draw a polybézier. At the
	// end of the command, the new current point becomes the final
	// (x,y) coordinate pair used in the polybézier.
	int nbCurvePoints = 0;
	bool relative = (curChar == 'c');
	curChar = pathString[ indChar++ ];
	while( _SpaceChar(curChar) ) {
	  curChar = pathString[ indChar++ ];
	}

	curveInitialPoint[0] = currentPoint[0];
	curveInitialPoint[1] = currentPoint[1];
	
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 
	  if( relative )
	    currentPoint[0] = pg_ScanFloatString( &curChar , true  , 
						  pathString , &indChar ) + curveInitialPoint[0];
	  else
	    currentPoint[0] = pg_ScanFloatString( &curChar , true  , 
						  pathString , &indChar );
	  if( curChar == ',' ) {
	    curChar = pathString[ indChar++ ];
	    while( _SpaceChar(curChar) ) {
	      curChar = pathString[ indChar++ ];
	    }
	  }
	  else {
	    sprintf( ErrorStr , "Expected char [%c] not found!" , ',' ); ReportError( ErrorStr ); throw 17;
	  }

	  if( relative )
	    currentPoint[1] = pg_ScanFloatString( &curChar , true  , 
						  pathString , &indChar ) + curveInitialPoint[1];
	  else
	    currentPoint[1] = pg_ScanFloatString( &curChar , true  , 
						  pathString , &indChar );

	  while( _SpaceChar(curChar) ) {
	    curChar = pathString[ indChar++ ];
	  }
	  nbCurvePoints++;

	  if( (nbCurvePoints) % 3 == 0 ) {
	    if( indSpline < PG_NB_SPLINES_MAX ) {	    
	      pg_TrackPos_x_source[ indTrack ][ indSpline ] 
		= currentPoint[0] + translation[0];
	      pg_TrackPos_y_source[ indTrack ][ indSpline ] =
		currentPoint[1] + translation[1];
	      
	      curveInitialPoint[0] = currentPoint[0];
	      curveInitialPoint[1] = currentPoint[1];
	      indSpline++;
	    }
	    else {
	      sprintf( ErrorStr , "Error: number of splines above PG_NB_SPLINES_MAX [%d]!" , indSpline ); ReportError( ErrorStr ); throw 17;
	    }
	  }
	  // right tangent of the first (and current) point
	  else if( (nbCurvePoints) % 3 == 1  && indSpline > 0 ) {
	    if( indSpline < PG_NB_SPLINES_MAX ) {
	      pg_TrackPos_xR_source[ indTrack ][ indSpline - 1 ] 
		= currentPoint[0] + translation[0];
	      pg_TrackPos_yR_source[ indTrack ][ indSpline - 1 ] 
		= currentPoint[1] + translation[1];
	    }
	    else {
	      sprintf( ErrorStr , "Error: number of splines above PG_NB_SPLINES_MAX [%d]!" , indSpline ); ReportError( ErrorStr ); throw 17;
	    }
	  }
	  // left tangent of the second (and next) point
	  else if( (nbCurvePoints) % 3 == 2  ) {
	    if( indSpline < PG_NB_SPLINES_MAX ) {
	      pg_TrackPos_xL_source[ indTrack ][ indSpline ]
		= currentPoint[0] + translation[0];
	      pg_TrackPos_yL_source[ indTrack ][ indSpline ]
		= currentPoint[1] + translation[1];
	    }
	    else {
	      sprintf( ErrorStr , "Error: number of splines above PG_NB_SPLINES_MAX [%d]!" , indSpline ); ReportError( ErrorStr ); throw 17;
	    }
	  }
	}
	if( nbCurvePoints % 3 != 0 ) {
	  printf( "Bezier curve points are expected to be given by groups of 3 %d next %c %f %f!\n" , nbCurvePoints , curChar , currentPoint[0] , currentPoint[1]);
	}
      }
      break;
    case 'l':
    case 'L':
      {
	// RULES 
	// Draw a line from the current point to the given (x,y)
	// coordinate which becomes the new current point. L
	// (uppercase) indicates that absolute coordinates will
	// follow; l (lowercase) indicates that relative coordinates
	// will follow. A number of coordinates pairs may be specified
	// to draw a polyline. At the end of the command, the new
	// current point is set to the final set of coordinates
	// provided.
	bool relative = (curChar == 'l');
	curChar = pathString[ indChar++ ];
	while( _SpaceChar(curChar) ) {
	  curChar = pathString[ indChar++ ];
	}

	if( indSpline < PG_NB_SPLINES_MAX ) {
	  pg_TrackPos_x_source[ indTrack ][ indSpline ]
	    = currentPoint[0] + translation[0];
	  pg_TrackPos_y_source[ indTrack ][ indSpline ] 
	    = currentPoint[1] + translation[1];

	  indSpline++;
	}
	else {
	  sprintf( ErrorStr , "Error: number of splines above PG_NB_SPLINES_MAX [%d]!" , indSpline ); ReportError( ErrorStr ); throw 17;
	}
	
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 

	  if( relative )
	    currentPoint[0] += pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  else
	    currentPoint[0] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  if( curChar == ',' ) {
	    curChar = pathString[ indChar++ ];
	    while( _SpaceChar(curChar) ) {
	      curChar = pathString[ indChar++ ];
	    }
	  }
	  else {
	    sprintf( ErrorStr , "Expected char [%c] not found!" , ',' ); ReportError( ErrorStr ); throw 17;
	  }
	  if( relative )
	    currentPoint[1] += pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  else
	    currentPoint[1] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  while( _SpaceChar(curChar) ) {
	    curChar = pathString[ indChar++ ];
	  }

	  if( indSpline < PG_NB_SPLINES_MAX ) {
	      pg_TrackPos_x_source[ indTrack ][ indSpline ] 
		= currentPoint[0] + translation[0];
	      pg_TrackPos_y_source[ indTrack ][ indSpline ] 
		= currentPoint[1] + translation[1];
	      
	      // left tangent of current point is preceding point
	      // and right tangent of preceding point is the current point
	      pg_TrackPos_xL_source[ indTrack ][ indSpline ] 
		= pg_TrackPos_x_source[ indTrack ][ indSpline - 1 ];
	      pg_TrackPos_yL_source[ indTrack ][ indSpline ]
		= pg_TrackPos_y_source[ indTrack ][ indSpline -1 ];
	      pg_TrackPos_xR_source[ indTrack ][ indSpline - 1 ] 
		= pg_TrackPos_x_source[ indTrack ][ indSpline ];
	      pg_TrackPos_yR_source[ indTrack ][ indSpline - 1 ]
		= pg_TrackPos_y_source[ indTrack ][ indSpline ];

	    // printf("new line to %d %f %f\n" , indSpline ,
	    // 	   pg_TrackPos_x_source[ indTrack ][ indSpline ] , 
	    // 	   pg_TrackPos_y_source[ indTrack ][ indSpline ]);
	    indSpline++;
	  }
	  else {
	    sprintf( ErrorStr , "Error: number of splines above PG_NB_SPLINES_MAX [%d]!" , indSpline ); ReportError( ErrorStr ); throw 17;
	  }
	}
      }
      break;
    case 'z':
    case 'Z':
      {
	// RULES 
	// Close the current subpath by drawing a straight line
	// from the current point to current subpath's initial
	// point. Since the Z and z commands take no parameters, they
	// have an identical effect.
	curChar = pathString[ indChar++ ];
	while( _SpaceChar(curChar) ) {
	  curChar = pathString[ indChar++ ];
	}
	// printf("cut\n");
	// if the current point is the initial point, there is nothing to do
	// the loop is closed
	// otherwise adds a line from current point to initial point
	if( abs(pg_TrackPos_x_source[ indTrack ][ indSpline - 1 ] 
		- pg_TrackPos_x_source[ indTrack ][ 0 ]) > 0.00001
	    || (pg_TrackPos_y_source[ indTrack ][ indSpline - 1 ] 
		- pg_TrackPos_y_source[ indTrack ][ 0 ]) > 0.00001 ) {
	  if( indSpline < PG_NB_SPLINES_MAX ) {
	    // curent point is initial point
	    pg_TrackPos_x_source[ indTrack ][ indSpline ] 
	      = polyCurveInitialPoint[0] + translation[0];
	    pg_TrackPos_y_source[ indTrack ][ indSpline ] 
	      = polyCurveInitialPoint[1] + translation[1];

	    // left point of current point is preceding point
	    pg_TrackPos_xL_source[ indTrack ][ indSpline ]
	      = pg_TrackPos_x_source[ indTrack ][ indSpline - 1 ];
	    pg_TrackPos_yL_source[ indTrack ][ indSpline ] 
	      = pg_TrackPos_y_source[ indTrack ][ indSpline - 1 ];

	    // and right point of preceding point is current point
	    pg_TrackPos_xR_source[ indTrack ][ indSpline - 1 ]
	      = pg_TrackPos_x_source[ indTrack ][ indSpline ];
	    pg_TrackPos_yR_source[ indTrack ][ indSpline - 1 ] 
	      = pg_TrackPos_y_source[ indTrack ][ indSpline ];
	    indSpline++;
	  }
	  else {
	    sprintf( ErrorStr , "Error: number of splines above PG_NB_SPLINES_MAX [%d]!" , indSpline ); ReportError( ErrorStr ); throw 17;
	  }
	}
      }
      break;
    default:
      {
	sprintf( ErrorStr , "Unknown SVG path tag [%c] %d!" , curChar , curChar ); ReportError( ErrorStr ); throw(100);
      }
      break;
    }  
  }
  return indSpline;
}

//////////////////////////////////////////////////////////////////
// INITIALIZATION OF THE TABLES THAT CONTAIN THE STROKE PARAMETERS
//////////////////////////////////////////////////////////////////
void pg_initStrokes( void ) {
  // looks for specific variables (brush radius, local color, brush

  // mouse pointer tracks recording initialization
  pg_TrackPos_x_source = new float*[ PG_NB_TRACKS ];
  pg_TrackPos_y_source = new float*[ PG_NB_TRACKS ];
  pg_TrackPos_xR_source = new float*[ PG_NB_TRACKS ];
  pg_TrackPos_yR_source = new float*[ PG_NB_TRACKS ];
  pg_TrackPos_xL_source = new float*[ PG_NB_TRACKS ];
  pg_TrackPos_yL_source = new float*[ PG_NB_TRACKS ];
  for( int ind = 0 ; ind < PG_NB_TRACKS ; ind++ ) {
    pg_TrackPos_x_source[ ind ] 
      = new float[ PG_NB_SPLINES_MAX ];
    pg_TrackPos_y_source[ ind ] 
      = new float[ PG_NB_SPLINES_MAX ];
    pg_TrackPos_xL_source[ ind ] 
      = new float[ PG_NB_SPLINES_MAX ];
    pg_TrackPos_yL_source[ ind ] 
      = new float[ PG_NB_SPLINES_MAX ];
    pg_TrackPos_xR_source[ ind ] 
      = new float[ PG_NB_SPLINES_MAX ];
    pg_TrackPos_yR_source[ ind ] 
      = new float[ PG_NB_SPLINES_MAX ];
    for( int indSpline = 0 ; indSpline < PG_NB_TRACKS ; indSpline++ ) {
      pg_TrackPos_x_source[ ind ][ indSpline ] = 0.0f;
      pg_TrackPos_y_source[ ind ][ indSpline ] = 0.0f;
      pg_TrackPos_xL_source[ ind ][ indSpline ] = 0.0f;
      pg_TrackPos_yL_source[ ind ][ indSpline ] = 0.0f;
      pg_TrackPos_xR_source[ ind ][ indSpline ] = 0.0f;
      pg_TrackPos_yR_source[ ind ][ indSpline ] = 0.0f;
    }
  }
  pg_TrackNBSplines = new int[ PG_NB_TRACKS ];
}

void pg_readsvg( char *fileName , int indTrack , float boundingBoxX[2] , float boundingBoxY[2] ) {
  string         val;
  float          translation[2] = { 0.0 , 0.0 };
  string line;

  std::ifstream fin(fileName);
  if( !fin ) {
    sprintf( ErrorStr , "Error: file [%s] not found!" , fileName ); ReportError( ErrorStr ); throw 11;
  }
  printf("Loading %s\n" , fileName );

  // scans the svg file
  while( true ) {
    std::getline(fin, line);
    if( fin.eof() ) {
       break;
    }

    ////////////////////////////
    ////// group transformation tag
    std::size_t found = line.find("translate");
    if (found!=std::string::npos) {
      found = line.find("(",found+1);
      if (found!=std::string::npos) {
        std::stringstream  sstream(line);
		sstream.seekg(found+1);
		if (found!=std::string::npos) {
		  sstream >> translation[0];
		  sstream.seekg (1, std::ios::cur);
		  sstream >> translation[1];
		}
	  }
    }
    ////////////////////////////
    ////// path tag
    else {
      found = line.find("<path");
      if (found!=std::string::npos) {
		found = line.find("d=\"",found+1);
		while ( found==std::string::npos && !fin.eof() ) {      
			  std::getline(fin, line);
		  found = line.find("d=\"",found+1);
		}
		if (found!=std::string::npos) {
		  // copies the beginning of the path
		  val = line.substr( found + 3 );

		  std::getline(fin, line);
		  found = line.find("z\"",found+1);
		  std::size_t found2 = line.find("Z\"",found+1);
		  while ( found==std::string::npos && found2==std::string::npos && !fin.eof() ) {
			val += line;

			std::getline(fin, line);
			found = line.find("z\"",found+1);
			// std::size_t found2 = line.find("Z\"",found+1);
		  }

		  // printf( "path values [%s]\n", (char *)val.c_str() );
		  pg_TrackNBSplines[indTrack]
			= pg_LoadTrackFromPath( (char *)val.c_str() , indTrack , translation );
	
		  // control points
		  float P[2*4];
		  for (int i = 1 ; i < pg_TrackNBSplines[indTrack] ; i++ ) {
			P[2*0] = pg_TrackPos_x_source[ indTrack ][ i - 1 ];
			P[2*0 + 1] = pg_TrackPos_y_source[ indTrack ][ i - 1 ];
			P[2*1] = pg_TrackPos_xR_source[ indTrack ][ i - 1 ];
			P[2*1 + 1] = pg_TrackPos_yR_source[ indTrack ][ i - 1 ];
			P[2*2] = pg_TrackPos_xL_source[ indTrack ][ i ];
			P[2*2 + 1] = pg_TrackPos_yL_source[ indTrack ][ i ];
			P[2*3] = pg_TrackPos_x_source[ indTrack ][ i ];
			P[2*3 + 1] = pg_TrackPos_y_source[ indTrack ][ i ];
			BezierBoundingBox( boundingBoxX , boundingBoxY , P );

			//  printf("spline %d L %f %f (%f %f) R %f %f\n" , i ,
			  //  pg_TrackPos_xL_source[ indTrack ][ i ] , 
			  //  pg_TrackPos_yL_source[ indTrack ][ i ] ,
			  //  pg_TrackPos_x_source[ indTrack ][ i ] , 
			  //  pg_TrackPos_y_source[ indTrack ][ i ] ,
			  //  pg_TrackPos_xR_source[ indTrack ][ i ] , 
			  //  pg_TrackPos_yR_source[ indTrack ][ i ]);
		  }
		}
	  }
    }
  }
  val.clear();
  fin.close();
}

void pg_load_svg_track( char *fileName , int indTrack  , float boundingBoxX[2] , float boundingBoxY[2] ) {
  // loads track
  pg_readsvg( fileName , indTrack , boundingBoxX , boundingBoxY );
}

//////////////////////////////////////////////////////////////////////
// SVG TO IMAGE
//////////////////////////////////////////////////////////////////////
void bezier_val( float param , float P0[2] , float P1[2] , 
		 float P2[2] , float P3[2] , float resul[2] ) {
  resul[0] = ((1.0f - param) * (1.0f - param) * (1.0f - param)) * P0[0]
    + (3.0f * param * (1.0f - param) * (1.0f - param)) * P1[0]
    + (3.0f * param * param * (1.0f - param)) * P2[0]
    + (param * param * param) * P3[0];
  resul[1] = ((1.0f - param) * (1.0f - param) * (1.0f - param)) * P0[1]
    + (3.0f * param * (1.0f - param) * (1.0f - param)) * P1[1]
    + (3.0f * param * param * (1.0f - param)) * P2[1]
    + (param * param * param) * P3[1];
}
void bezier_prime_val( float param , float P0[2] , float P1[2] , 
		 float P2[2] , float P3[2] , float resul[2] ) {
  resul[0] = (3.0f * (1.0f - param) * (1.0f - param)) * (P1[0] - P0[0])
    + (6.0f * param * (1.0f - param)) * (P2[0] - P1[0])
    + (3.0f * param * param) * (P3[0] - P2[0]);
  resul[1] = (3.0f * (1.0f - param) * (1.0f - param)) * (P1[1] - P0[1])
    + (6.0f * param * (1.0f - param)) * (P2[1] - P1[1])
    + (3.0f * param * param) * (P3[1] - P2[1]);
}
// to be impoved in the future by sampling a few points on the curve
// whose bb can be much smaller than the one of the conotrol points
// but never larger...
void BezierBoundingBox( float boundingBoxX[2] , float boundingBoxY[2] , float P[2*4] ) {
  for( int ind = 0 ; ind < 4 ; ind++ ) {
	boundingBoxX[0] = min( boundingBoxX[0] , P[2*ind] );
	boundingBoxX[1] = max( boundingBoxX[1] , P[2*ind] );
	boundingBoxY[0] = min( boundingBoxY[0] , P[2*ind + 1] );
	boundingBoxY[1] = max( boundingBoxY[1] , P[2*ind + 1] );
  }
}

void pg_svg2bitmap( int indTrack , int w , int h , uchar* dataDestination , float scaleXY[2] , 
					float brushRadius , int brushSize , uchar* dataBrush ) {
  int intRadius = int(ceil(brushRadius));

  for (int i = 1 ; i < pg_TrackNBSplines[indTrack] ; i++ ) {
	printf("spline %d (%f %f) R %f %f L %f %f (%f %f)\n" , i ,
		pg_TrackPos_x_source[ indTrack ][ i - 1 ] , 
		pg_TrackPos_y_source[ indTrack ][ i - 1 ] ,
		pg_TrackPos_xR_source[ indTrack ][ i - 1 ] , 
		pg_TrackPos_yR_source[ indTrack ][ i - 1 ] ,
		pg_TrackPos_xL_source[ indTrack ][ i ] , 
		pg_TrackPos_yL_source[ indTrack ][ i ] ,
		pg_TrackPos_x_source[ indTrack ][ i ] , 
		pg_TrackPos_y_source[ indTrack ][ i ] );

	// control points
	float P[2*4];
	P[2*0] = pg_TrackPos_x_source[ indTrack ][ i - 1 ];
	P[2*0 + 1] = pg_TrackPos_y_source[ indTrack ][ i - 1 ];
	P[2*1] = pg_TrackPos_xR_source[ indTrack ][ i - 1 ];
	P[2*1 + 1] = pg_TrackPos_yR_source[ indTrack ][ i - 1 ];
	P[2*2] = pg_TrackPos_xL_source[ indTrack ][ i ];
	P[2*2 + 1] = pg_TrackPos_yL_source[ indTrack ][ i ];
	P[2*3] = pg_TrackPos_x_source[ indTrack ][ i ];
	P[2*3 + 1] = pg_TrackPos_y_source[ indTrack ][ i ];

	// local bounding box to decide the parameter scale
	// so that there approximately one point per pixel
	float localBoundingBoxX[2] = { FLT_MAX , -FLT_MAX};
	float localBoundingBoxY[2] = { FLT_MAX , -FLT_MAX};
	BezierBoundingBox( localBoundingBoxX , localBoundingBoxY , P );
	float width = localBoundingBoxX[1]-localBoundingBoxX[0];
	float height = localBoundingBoxY[1]-localBoundingBoxY[0];
	printf("spline %d WxH (%f %f)\n" , i , width , height );
	width *= scaleXY[0]; height *= scaleXY[1];
	for( int ind = 0 ; ind < 4 ; ind++ ) {
	  P[2*ind] *= scaleXY[0];
	  P[2*ind + 1] *= scaleXY[1];
	}

	float nbSteps = max( width , height );
	for( int indStep = 0 ; indStep < int(nbSteps) ; indStep++ ) {
	  float point[2];
	  bezier_val( indStep / nbSteps , &P[0] , &P[2] , &P[4] , &P[6] , point );
	  float col = point[0]; float row = point[1];
	  row = h - 1 - row;

	  for( int radX = -intRadius ; radX < intRadius ; radX++ ) {
		for( int radY = -intRadius ; radY < intRadius ; radY++ ) {
		  // destination in the output image
		  int curColDestination = int(round(col + radX));
		  int curRowDestination = int(round(row + radY));
		  curColDestination = max(0 , min( curColDestination , w - 1 ));
		  curRowDestination = max(0 , min( curRowDestination , h - 1 ));
		  int redDestination = (curRowDestination * w + curColDestination) * 3;

		  // source in the brush
		  int brushPositionCol = int(brushSize / 2 + (brushSize / 2) * float(radX) / float(intRadius));
		  int brushPositionRow = int(brushSize / 2 + (brushSize / 2) * float(radY) / float(intRadius));
		  brushPositionCol = max(0 , min( brushPositionCol , brushSize - 1 ));
		  brushPositionRow = max(0 , min( brushPositionRow , brushSize - 1 ));
		  int redBrush = (brushPositionRow * w + brushPositionCol) * 3;

		  for( int i = 0 ; i < 3 ; i++ ) {
			dataDestination[ redDestination + i ] 
			     = max(dataBrush[ redBrush + i ],dataDestination[ redDestination + i ]);
		  }
		}
	  }
	}
  }
}

//////////////////////////////////////////////////////////////////////
// SAVE IMAGE
//////////////////////////////////////////////////////////////////////

/* Attempts to save PNG to file. */
void pg_writepng( char *fname , int w , int h , cv::Mat *imgOutput ) {

  cv::Mat result;
  cv::flip(*(imgOutput), result , 0); // vertical flip
#ifndef OPENCV_3
  cv::cvtColor(result, result, CV_RGB2BGR);
#else
  cv::cvtColor(result, result, cv::COLOR_RGB2BGR);
#endif
  std::vector<int> params;
  params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  params.push_back(9);
  cv::imwrite( fname , result , params );

  delete imgOutput;
}

//////////////////////////////////////////////////////////////////////
// LOAD IMAGE
//////////////////////////////////////////////////////////////////////

/* Attempts to load PNG to file. */
int pg_readpng( char *fname , int w , int h , bool invert , cv::Mat *imgInput ) {
#ifndef OPENCV_3
	cv::Mat img = cv::imread(fname, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
	printf("Loading %s\n" , fname );
	// Check for invalid input
	if (!img.data) {
		sprintf(ErrorStr, "Could not open or find the 2D image %s!", fname); // ReportError(ErrorStr); throw 425;
		img.release();
		*(fname) = 0;
		return 0;
	}
	// Check for image size
	if (img.cols != w || img.rows != h) {
		sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!",
			fname, img.cols, w, img.rows,
			h);/// ReportError(ErrorStr); throw 425;
		img.release();
		*(fname) = 0;
		return 0;
	}

	int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA :
		(img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
	// int glColorType = (imgInput->channels() == 4) ? GL_RGBA : GL_RGB;
	if (img.channels() >= 3) {
		cv::cvtColor(img, img, colorTransform);
	}
	if (invert)
		cv::flip(img, *(imgInput), 0); // vertical flip
	else
		*(imgInput) = img;
#else
	cv::Mat img = cv::imread(fname, cv::IMREAD_UNCHANGED);   // Read the file
	// Check for invalid input
	if (!img.data) {
		sprintf(ErrorStr, "Could not open or find the 2D image %s!", fname); // ReportError(ErrorStr); throw 425;
		img.release();
		*(fname) = 0;
		return 0;
	}
	// Check for image size
	if (img.cols != w || img.rows != h) {
		sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!",
			fname, img.cols, w, img.rows,
			h);/// ReportError(ErrorStr); throw 425;
		img.release();
		*(fname) = 0;
		return 0;
	}

	int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA :
		(img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
	// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
	if( img.channels() >= 3 ) {
		cv::cvtColor(img, img, colorTransform);
	} 
	if( invert ) 
		cv::flip(img, *(imgInput) , 0); // vertical flip
	else
		*(imgInput) = img;
#endif
	img.release();
	return 1;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  char SVGfname[512];
  char PNGfname[512];
  char BrushFileName[512];
  float BrushRadius = 3.5f;
  ErrorStr = new char[1024];
  strcpy( SVGfname , "./textures/test.svg" );
  strcpy( PNGfname , "./textures/test.png" );
  strcpy( BrushFileName , "./textures/pen.png" );

  cv::Mat *imgOutput = new cv::Mat( PG_IMAGE_H , PG_IMAGE_W , CV_8UC3 );
  memset( (void *)imgOutput->data , 0 , PG_IMAGE_H * PG_IMAGE_W * 3 );

  cv::Mat *imgPen = new cv::Mat( 2048 , 2048 , CV_8UC3 );
  pg_readpng( BrushFileName , PG_BRUSH_SIZE , PG_BRUSH_SIZE , true , imgPen );

  pg_initStrokes();

  int indTrack = 0;
  float boundingBoxX[2] = { FLT_MAX , -FLT_MAX };
  float boundingBoxY[2] = { FLT_MAX , -FLT_MAX };
  pg_load_svg_track( SVGfname , indTrack , boundingBoxX , boundingBoxY );

  float scaleXY[2];
  // we don't scale the image if it fits
  scaleXY[0] = min( 1.0f , PG_IMAGE_W / (boundingBoxX[1] - boundingBoxX[0]) );
  scaleXY[1] = min( 1.0f , PG_IMAGE_H / (boundingBoxY[1] - boundingBoxY[0]) );
  // we dont distort the image and take the smallest scale of  and Y
  scaleXY[0] = min( scaleXY[0] , scaleXY[1] );
  scaleXY[1] = scaleXY[0];
  printf("figure WxH (%f<->%f %f<->%f) scale (%fx%f) \n" , boundingBoxX[0] , boundingBoxX[1] , 
									 boundingBoxY[0] , boundingBoxY[1] , scaleXY[0] , scaleXY[1]);
  pg_svg2bitmap( 0 , PG_IMAGE_W , PG_IMAGE_H , imgOutput->data , scaleXY , 
				 BrushRadius , PG_BRUSH_SIZE , imgPen->data );

  pg_writepng( PNGfname , PG_IMAGE_W , PG_IMAGE_H , imgOutput );
  return 0;
}
