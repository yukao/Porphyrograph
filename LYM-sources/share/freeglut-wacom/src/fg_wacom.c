/*
 * freeglut_wacom.c
 *
 * wacom tablet handling code
 *
 * Written by Christian Jacquemmin, LIMSI-CNRS & Univ. Paris-Sud 
 *  		Based on TiltTest
 *  		Copyright (C) 1998  LCS/Telegraphics
 * 		Copyright (c) Wacom Company, Ltd. 2010 All Rights Reserved
 * 		All rights reserved.
 *		Based on prstest from LCS Telegraphics  RICO 4/1/92
 *		Modified by Ken Loftus @ WACOM Technology Corp 2/15/95
 *		Project file created by Kim Ritchie @ WACOM Technology Corp 8/2004
 *		Modified by Robert Cohn @ WACOM Technology Corp 2/26/2010
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <GL/freeglut.h>
#include "fg_internal.h"
#ifdef HAVE_SYS_PARAM_H
#    include <sys/param.h>
#endif

#if TARGET_HOST_MS_WINDOWS && !defined(_WIN32_WCE)
#    include <windows.h>
#include	<assert.h>
#define PACKETDATA      (PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE | \
						 PK_ORIENTATION | PK_CURSOR)
#define PACKETMODE      0
#include "pktdef.h"			// NOTE: get from wactab header package
#include "msgpack.h"
#endif

#define WACOM_DEBUG

// Ignore warnings about using unsafe string functions.
#pragma warning( disable : 4996 )

/* converts FIX32 to double */
#define FIX_DOUBLE(x)   ((double)(INT(x))+((double)FRAC(x)/65536))
#define pi 3.14159265359

//////////////////////////////////////////////////////////////////////////////
// Function pointers to Wintab functions exported from wintab32.dll. 
typedef UINT ( API * WTINFOA ) ( UINT, UINT, LPVOID );
typedef HCTX ( API * WTOPENA )( HWND, LPLOGCONTEXTA, BOOL );
typedef BOOL ( API * WTGETA ) ( HCTX, LPLOGCONTEXT );
typedef BOOL ( API * WTSETA ) ( HCTX, LPLOGCONTEXT );
typedef BOOL ( API * WTCLOSE ) ( HCTX );
typedef BOOL ( API * WTENABLE ) ( HCTX, BOOL );
typedef BOOL ( API * WTPACKET ) ( HCTX, UINT, LPVOID );
typedef BOOL ( API * WTOVERLAP ) ( HCTX, BOOL );
typedef BOOL ( API * WTSAVE ) ( HCTX, LPVOID );
typedef BOOL ( API * WTCONFIG ) ( HCTX, HWND );
typedef HCTX ( API * WTRESTORE ) ( HWND, LPVOID, BOOL );
typedef BOOL ( API * WTEXTSET ) ( HCTX, UINT, LPVOID );
typedef BOOL ( API * WTEXTGET ) ( HCTX, UINT, LPVOID );
typedef BOOL ( API * WTQUEUESIZESET ) ( HCTX, int );
typedef int  ( API * WTDATAPEEK ) ( HCTX, UINT, UINT, int, LPVOID, LPINT);
typedef int  ( API * WTPACKETSGET ) (HCTX, int, LPVOID);

// TODO - add more wintab32 function defs as needed

//////////////////////////////////////////////////////////////////////////////
char*	gpszProgramName = "freeglut_wacom";

// Loaded Wintab32 API functions.
extern HINSTANCE ghWintab;

extern WTINFOA gpWTInfoA;
extern WTOPENA gpWTOpenA;
extern WTGETA gpWTGetA;
extern WTSETA gpWTSetA;
extern WTCLOSE gpWTClose;
extern WTPACKET gpWTPacket;
extern WTENABLE gpWTEnable;
extern WTOVERLAP gpWTOverlap;
extern WTSAVE gpWTSave;
extern WTCONFIG gpWTConfig;
extern WTRESTORE gpWTRestore;
extern WTEXTSET gpWTExtSet;
extern WTEXTGET gpWTExtGet;
extern WTQUEUESIZESET gpWTQueueSizeSet;
extern WTDATAPEEK gpWTDataPeek;
extern WTPACKETSGET gpWTPacketsGet;

// TODO - add more wintab32 function pointers as needed

/*
 * Definition of "PG_wacom_tablet" structure
 */
/* joystick ID constants */
#define TABLETID1         0
#define TABLETID2         1

struct PG_wacom_tablet
{
  UINT            tablet_id;           /* internal ID for tablet */
  HCTX            hTab;		       /* Handle for Tablet Context */
  POINT           ptNew;               /* XY value storage */
  UINT            prsNew;              /* Pressure value storage */
  UINT            curNew;              /* Cursor number storage */
  ORIENTATION     ortNew;              /* Tilt value storage */
  double          aziFactor;           /* Azimuth factor */
  double          altFactor;           /* Altitude factor */
  double          altAdjust;           /* Altitude zero adjust */
  BOOL            tilt_support;        /* Is tilt supported */
};

#define MAX_NUM_TABLETS  1
static struct PG_wacom_tablet *pg_wacom_tablet [ MAX_NUM_TABLETS ];
BOOL pg_wacom_tabletActive = FALSE;

//////////////////////////////////////////////////////////////////////////////
BOOL LoadWintab( void );
void UnloadWintab( void );

void ShowError( char *pszErrorMessage );

//////////////////////////////////////////////////////////////////////////////
#ifdef WACOM_DEBUG

void WacomTrace( char *lpszFormat, ...);

#define WACOM_ASSERT( x ) assert( x )
#define WACOM_TRACE(...)  WacomTrace(__VA_ARGS__)
#else
#define WACOM_TRACE(...)
#define WACOM_ASSERT( x )

#endif // WACOM_DEBUG


#ifdef WACOM_DEBUG

void WacomTrace( char *lpszFormat, ...);

#define WACOM_ASSERT( x ) assert( x )
#define WACOM_TRACE(...)  WacomTrace(__VA_ARGS__)
#else
#define WACOM_TRACE(...)
#define WACOM_ASSERT( x )

#endif // WACOM_DEBUG

//////////////////////////////////////////////////////////////////////////////
HINSTANCE ghWintab = NULL;

WTINFOA gpWTInfoA = NULL;
WTOPENA gpWTOpenA = NULL;
WTGETA gpWTGetA = NULL;
WTSETA gpWTSetA = NULL;
WTCLOSE gpWTClose = NULL;
WTPACKET gpWTPacket = NULL;
WTENABLE gpWTEnable = NULL;
WTOVERLAP gpWTOverlap = NULL;
WTSAVE gpWTSave = NULL;
WTCONFIG gpWTConfig = NULL;
WTRESTORE gpWTRestore = NULL;
WTEXTSET gpWTExtSet = NULL;
WTEXTGET gpWTExtGet = NULL;
WTQUEUESIZESET gpWTQueueSizeSet = NULL;
WTDATAPEEK gpWTDataPeek = NULL;
WTPACKETSGET gpWTPacketsGet = NULL;

// TODO - add more wintab32 function pointers as needed

char* pszProgramName = NULL;

#define GETPROCADDRESS(type, func) \
	gp##func = (type)GetProcAddress(ghWintab, #func); \
	if (!gp##func){ WACOM_ASSERT(FALSE); UnloadWintab(); return FALSE; }

//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Find wintab32.dll and load it.  
//		Find the exported functions we need from it.
//
//	Returns
//		TRUE on success.
//		FALSE on failure.
//
BOOL LoadWintab( void )
{
	ghWintab = LoadLibraryA(  "Wintab32.dll" );
	if ( !ghWintab )
	{
		DWORD err = GetLastError();
		WACOM_TRACE("LoadLibrary error: %i\n", err);
		ShowError("Could not load Wintab32.dll");
		return FALSE;
	}

	// Explicitly find the exported Wintab functions in which we are interested.
	// We are using the ASCII, not unicode versions (where applicable).
	GETPROCADDRESS( WTOPENA, WTOpenA );
	GETPROCADDRESS( WTINFOA, WTInfoA );
	GETPROCADDRESS( WTGETA, WTGetA );
	GETPROCADDRESS( WTSETA, WTSetA );
	GETPROCADDRESS( WTPACKET, WTPacket );
	GETPROCADDRESS( WTCLOSE, WTClose );
	GETPROCADDRESS( WTENABLE, WTEnable );
	GETPROCADDRESS( WTOVERLAP, WTOverlap );
	GETPROCADDRESS( WTSAVE, WTSave );
	GETPROCADDRESS( WTCONFIG, WTConfig );
	GETPROCADDRESS( WTRESTORE, WTRestore );
	GETPROCADDRESS( WTEXTSET, WTExtSet );
	GETPROCADDRESS( WTEXTGET, WTExtGet );
	GETPROCADDRESS( WTQUEUESIZESET, WTQueueSizeSet );
	GETPROCADDRESS( WTDATAPEEK, WTDataPeek );
	GETPROCADDRESS( WTPACKETSGET, WTPacketsGet );


	// TODO - don't forget to NULL out pointers in UnloadWintab().
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Uninitializes use of wintab32.dll
//
// Returns
//		Nothing.
//
void UnloadWintab( void )
{
	WACOM_TRACE( "UnloadWintab()\n" );

	if ( ghWintab )
	{
		FreeLibrary( ghWintab );
		ghWintab = NULL;
	}

	gpWTOpenA			= NULL;
	gpWTClose			= NULL;
	gpWTInfoA			= NULL;
	gpWTPacket			= NULL;
	gpWTEnable			= NULL;
	gpWTOverlap			= NULL;
	gpWTSave			= NULL;
	gpWTConfig			= NULL;
	gpWTGetA				= NULL;
	gpWTSetA				= NULL;
	gpWTRestore			= NULL;
	gpWTExtSet			= NULL;
	gpWTExtGet			= NULL;
	gpWTQueueSizeSet	= NULL;
	gpWTDataPeek		= NULL;
	gpWTPacketsGet		= NULL;
}



//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Display error to user.
//
void ShowError( char *pszErrorMessage )
{
	WACOM_TRACE( "ShowError()\n" );

	WACOM_ASSERT( pszErrorMessage );

	MessageBoxA( NULL, pszErrorMessage, gpszProgramName, MB_OK | MB_ICONHAND );
}



#ifdef WACOM_DEBUG

//////////////////////////////////////////////////////////////////////////////

void WacomTrace( char *lpszFormat, ...)
{
	char szTraceMessage[ 128 ];

	int nBytesWritten;

	va_list args;

	WACOM_ASSERT( lpszFormat );

	va_start( args, lpszFormat );

	nBytesWritten = _vsnprintf( szTraceMessage, sizeof( szTraceMessage ) - 1, 
		lpszFormat, args );

	if ( nBytesWritten > 0 )
	{
		OutputDebugStringA( szTraceMessage );
	}

	va_end( args );
}

#endif // WACOM_DEBUG

/*
 * Wintab library invocation.
 */
void pg_wacom_tabletWintabInit( void )
{
    if ( !LoadWintab( ) )
    {
	    ShowError( "Wintab not available" );
    }

    /* check if WinTab available. */
    if (!gpWTInfoA(0, 0, NULL)) {
	    MessageBox(NULL, "WinTab Services Not Available.", gpszProgramName, 
			MB_OK | MB_ICONHAND);
    }
}

/*
 * Allocation of the tablet data.
 */
void pg_wacom_tabletInit(HWND hWnd)
{
    double          tpvar;            /* A temp for converting fix to double */
    char            WName[50];        /* String to hold window name */
    struct          tagAXIS TpOri[3]; /* The capabilities of tilt */
    LOGCONTEXT      lcMine;           /* The context of the tablet */
    AXIS            TabletX, TabletY; /* The maximum tablet size */
    int		    ident;

    pg_wacom_tabletActive = TRUE;

    /* check if WACOM available. */
    gpWTInfoA(WTI_DEVICES, DVC_NAME, WName);
    if (strncmp(WName,"WACOM",5)) {
	printf( "Wacom Tablet Not Installed %s.", gpszProgramName );
	pg_wacom_tabletActive = FALSE;
    }

    /* tablet data initialization */
    for( ident = 0 ; ident < MAX_NUM_TABLETS ; ident++ ) {
      if( pg_wacom_tablet[ ident ] )
	  fgError( "illegal attempt to initialize tablet device again" );

      pg_wacom_tablet[ ident ] =
	  (struct PG_wacom_tablet * )calloc( sizeof(struct PG_wacom_tablet ), 1 );

      /* default initialization */
	  pg_wacom_tablet[ident]->ptNew.x = (LONG)-10000;
	  pg_wacom_tablet[ident]->ptNew.y = (LONG)-10000;
	  pg_wacom_tablet[ident]->curNew = (UINT)100;
	  pg_wacom_tablet[ident]->prsNew = (UINT)0;
	  pg_wacom_tablet[ident]->hTab = NULL;
	  pg_wacom_tablet[ ident ]->aziFactor = 1;       /* Azimuth factor */
      pg_wacom_tablet[ ident ]->altFactor = 1;       /* Altitude factor */
      pg_wacom_tablet[ ident ]->altAdjust = 1;       /* Altitude zero adjust */
      pg_wacom_tablet[ ident ]->tilt_support = TRUE; /* Is tilt supported */

      /* get info about tilt */
      pg_wacom_tablet[ ident ]->tilt_support = gpWTInfoA(WTI_DEVICES,DVC_ORIENTATION,&TpOri);
      if (pg_wacom_tablet[ ident ]->tilt_support) {
	  /* does the tablet support azimuth and altitude */
	  if (TpOri[0].axResolution && TpOri[1].axResolution) {

	      /* convert azimuth resulution to double */
	      tpvar = FIX_DOUBLE(TpOri[0].axResolution);
	      /* convert from resolution to radians */
	      pg_wacom_tablet[ ident ]->aziFactor = tpvar/(2*pi);  
	
	      /* convert altitude resolution to double */
	      tpvar = FIX_DOUBLE(TpOri[1].axResolution);
	      /* scale to arbitrary value to get decent line length */ 
	      pg_wacom_tablet[ ident ]->altFactor = tpvar/1000; 
	      /* adjust for maximum value at vertical */
	      pg_wacom_tablet[ ident ]->altAdjust = (double)TpOri[1].axMax/pg_wacom_tablet[ ident ]->altFactor;
	  }
	  else {  /* no so dont do tilt stuff */
	      pg_wacom_tablet[ ident ]->tilt_support = FALSE;
	  }
      }

      switch( ident )
      {
      case 0:
	  pg_wacom_tablet[ ident ]->tablet_id = TABLETID1;
	  break;
      case 1:
	  pg_wacom_tablet[ ident ]->tablet_id = TABLETID2;
	  break;
      }
    }

    /* get default region */
    gpWTInfoA(WTI_DEFCONTEXT, 0, &lcMine);

    /* modify the digitizing region */
    lcMine.lcOptions |= CXO_MESSAGES;
    lcMine.lcPktData = PACKETDATA;
    lcMine.lcPktMode = PACKETMODE;
    lcMine.lcMoveMask = PACKETDATA;
    lcMine.lcBtnUpMask = lcMine.lcBtnDnMask;

    /* Set the entire tablet as active */
    gpWTInfoA(WTI_DEVICES,DVC_X,&TabletX);
    gpWTInfoA(WTI_DEVICES,DVC_Y,&TabletY);
    lcMine.lcInOrgX = 0;
    lcMine.lcInOrgY = 0;
    lcMine.lcInExtX = TabletX.axMax;
    lcMine.lcInExtY = TabletY.axMax;

    /* output the data in screen coords */
    lcMine.lcOutOrgX = lcMine.lcOutOrgY = 0;
    lcMine.lcOutExtX = GetSystemMetrics(SM_CXSCREEN);
    /* move origin to upper left */
    lcMine.lcOutExtY = -GetSystemMetrics(SM_CYSCREEN);

    /* open the region */
    for( ident = 0 ; ident < MAX_NUM_TABLETS ; ident++ ) {
      pg_wacom_tablet[ ident ]->hTab = gpWTOpenA(hWnd, &lcMine, TRUE);
      if (!pg_wacom_tablet[ ident ]->hTab) {
	    printf( "Could Not Open Tablet Context in WinTab.\n" );
	    pg_wacom_tabletActive = FALSE;
      }

      if( pg_wacom_tabletActive ) {
	  if (pg_wacom_tablet[ ident ]->tilt_support)
	      printf( "Wacom driver with tilt support detected\n" );
	  else
	      printf( "Wacom driver without tilt support detected\n" );
      }
      else
	  printf( "No Wacom driver detected\n" );
    }
}

void pg_wacom_tabletMsg(SFG_Window* window, WPARAM wParam, LPARAM lParam)
{
	POINT 		ptOld;
	UINT  		prsOld;
	UINT  		curOld;
	ORIENTATION		ortOld;
	int			ident;
	PACKET	        pkt;             /* the current packet */

	if (!pg_wacom_tabletActive) {
		for (ident = 0; ident < MAX_NUM_TABLETS; ident++) {
			// old co-ordinates used for comparisons 
			ptOld = pg_wacom_tablet[ident]->ptNew;
			prsOld = pg_wacom_tablet[ident]->prsNew;
			curOld = pg_wacom_tablet[ident]->curNew;
			ortOld = pg_wacom_tablet[ident]->ortNew;

			// save new co-ordinates 
			pg_wacom_tablet[ident]->ptNew.x = (LONG)-10000;
			pg_wacom_tablet[ident]->ptNew.y = (LONG)-10000;
			pg_wacom_tablet[ident]->curNew = (UINT)100;
			pg_wacom_tablet[ident]->prsNew = (UINT)0;

			if (pg_wacom_tablet[ident]->ptNew.x != ptOld.x ||
				pg_wacom_tablet[ident]->ptNew.y != ptOld.y ||
				pg_wacom_tablet[ident]->prsNew != prsOld) {
				INVOKE_WCB(*window, WacomTablet,
					(pg_wacom_tablet[ident]->ptNew.x,
						pg_wacom_tablet[ident]->ptNew.y,
						pg_wacom_tablet[ident]->prsNew / 1023.0F, 0.0F, 0.0F, 0));
				INVOKE_WCB(*window, WacomTabletCursor,
					(pg_wacom_tablet[ident]->ptNew.x,
						pg_wacom_tablet[ident]->ptNew.y,
						pg_wacom_tablet[ident]->prsNew / 1023.0F, 0.0F, 0.0F, 0,
						pg_wacom_tablet[ident]->curNew));

				//   printf("pt [%d,%d] press %.3f\n" , 
				//pg_wacom_tablet[ ident ]->ptNew.x ,
				//pg_wacom_tablet[ ident ]->ptNew.y ,
				//pg_wacom_tablet[ ident ]->prsNew / 1023.0 );
			}
		}
		return;
	}

	if (gpWTPacket((HCTX)lParam, wParam, &pkt)) {
		for (ident = 0; ident < MAX_NUM_TABLETS; ident++) {
			/* old co-ordinates used for comparisons */
			ptOld = pg_wacom_tablet[ident]->ptNew;
			prsOld = pg_wacom_tablet[ident]->prsNew;
			curOld = pg_wacom_tablet[ident]->curNew;
			ortOld = pg_wacom_tablet[ident]->ortNew;

			/* save new co-ordinates */
			pg_wacom_tablet[ident]->ptNew.x = (UINT)pkt.pkX;
			pg_wacom_tablet[ident]->ptNew.y = (UINT)pkt.pkY;
			pg_wacom_tablet[ident]->curNew = pkt.pkCursor;
			pg_wacom_tablet[ident]->prsNew = pkt.pkNormalPressure;
			if (pg_wacom_tablet[ident]->tilt_support) {
				pg_wacom_tablet[ident]->ortNew = pkt.pkOrientation;
			}

			/* If the data change, update */
			// pkCursor values
			// Index 0 – Puck-like device #1
			// Index 1 – Stylus-like device #1
			// Index 2 – Inverted stylus-like device #1
			// Index 3 – Puck-like device #2
			// Index 4 – Stylus-like device #2
			// Index 5 – Inverted stylus-like device #2
			if (pg_wacom_tablet[ident]->tilt_support) {
				if (pg_wacom_tablet[ident]->ptNew.x != ptOld.x ||
					pg_wacom_tablet[ident]->ptNew.y != ptOld.y ||
					pg_wacom_tablet[ident]->prsNew != prsOld ||
					pg_wacom_tablet[ident]->ortNew.orAzimuth != ortOld.orAzimuth ||
					pg_wacom_tablet[ident]->ortNew.orAltitude != ortOld.orAltitude ||
					pg_wacom_tablet[ident]->ortNew.orTwist != ortOld.orTwist) {
					INVOKE_WCB(*window, WacomTablet,
						(pg_wacom_tablet[ident]->ptNew.x,
							pg_wacom_tablet[ident]->ptNew.y,
							pg_wacom_tablet[ident]->prsNew / 1023.0F,
							pg_wacom_tablet[ident]->ortNew.orAzimuth / 3600.0F * 2.0F * (float)pi,
							1.0F - pg_wacom_tablet[ident]->ortNew.orAltitude / 900.0F,
							pg_wacom_tablet[ident]->ortNew.orTwist));
					INVOKE_WCB(*window, WacomTabletCursor,
						(pg_wacom_tablet[ident]->ptNew.x,
							pg_wacom_tablet[ident]->ptNew.y,
							pg_wacom_tablet[ident]->prsNew / 1023.0F,
							pg_wacom_tablet[ident]->ortNew.orAzimuth / 3600.0F * 2.0F * (float)pi,
							1.0F - pg_wacom_tablet[ident]->ortNew.orAltitude / 900.0F,
							pg_wacom_tablet[ident]->ortNew.orTwist,
							pg_wacom_tablet[ident]->curNew));

					//   printf("pt [%d,%d] press %.3f az %.3f alt %.3f twist %d\n" , 
					   //pg_wacom_tablet[ ident ]->ptNew.x ,
					   //pg_wacom_tablet[ ident ]->ptNew.y ,
					   //pg_wacom_tablet[ ident ]->prsNew / 1023.0 ,
					   //pg_wacom_tablet[ ident ]->ortNew.orAzimuth / 3600.0 ,
					   //1.0 - pg_wacom_tablet[ ident ]->ortNew.orAltitude /900.0 ,
					   //pg_wacom_tablet[ ident ]->ortNew.orTwist );
				}
			}
			else {
				if (pg_wacom_tablet[ident]->ptNew.x != ptOld.x ||
					pg_wacom_tablet[ident]->ptNew.y != ptOld.y ||
					pg_wacom_tablet[ident]->prsNew != prsOld) {
					INVOKE_WCB(*window, WacomTablet,
						(pg_wacom_tablet[ident]->ptNew.x,
							pg_wacom_tablet[ident]->ptNew.y,
							pg_wacom_tablet[ident]->prsNew / 1023.0F, 0.0F, 0.0F, 0));
					INVOKE_WCB(*window, WacomTabletCursor,
						(pg_wacom_tablet[ident]->ptNew.x,
							pg_wacom_tablet[ident]->ptNew.y,
							pg_wacom_tablet[ident]->prsNew / 1023.0F, 0.0F, 0.0F, 0,
							pg_wacom_tablet[ident]->curNew));

					//   printf("pt [%d,%d] press %.3f\n" , 
					   //pg_wacom_tablet[ ident ]->ptNew.x ,
					   //pg_wacom_tablet[ ident ]->ptNew.y ,
					   //pg_wacom_tablet[ ident ]->prsNew / 1023.0 );
				}
			}
			/* if the cursor changes update the cursor name */
			if (pg_wacom_tablet[ident]->curNew != curOld) {
				printf("cursor [%d]\n", pg_wacom_tablet[ident]->curNew);
			}
		}
	}
	/*
	else {
		for (ident = 0; ident < MAX_NUM_TABLETS; ident++) {
			// old co-ordinates used for comparisons
			ptOld = pg_wacom_tablet[ident]->ptNew;
			prsOld = pg_wacom_tablet[ident]->prsNew;
			curOld = pg_wacom_tablet[ident]->curNew;
			ortOld = pg_wacom_tablet[ident]->ortNew;

			// save new co-ordinates 
			pg_wacom_tablet[ident]->ptNew.x = (LONG)-10000;
			pg_wacom_tablet[ident]->ptNew.y = (LONG)-10000;
			pg_wacom_tablet[ident]->curNew = (UINT)100;
			pg_wacom_tablet[ident]->prsNew = (UINT)0;

			if (pg_wacom_tablet[ident]->ptNew.x != ptOld.x ||
				pg_wacom_tablet[ident]->ptNew.y != ptOld.y ||
				pg_wacom_tablet[ident]->prsNew != prsOld) {
				INVOKE_WCB(*window, WacomTablet,
					(pg_wacom_tablet[ident]->ptNew.x,
						pg_wacom_tablet[ident]->ptNew.y,
						pg_wacom_tablet[ident]->prsNew / 1023.0F, 0.0F, 0.0F, 0));
				INVOKE_WCB(*window, WacomTabletCursor,
					(pg_wacom_tablet[ident]->ptNew.x,
						pg_wacom_tablet[ident]->ptNew.y,
						pg_wacom_tablet[ident]->prsNew / 1023.0F, 0.0F, 0.0F, 0,
						pg_wacom_tablet[ident]->curNew));

				//   printf("pt [%d,%d] press %.3f\n" , 
				//pg_wacom_tablet[ ident ]->ptNew.x ,
				//pg_wacom_tablet[ ident ]->ptNew.y ,
				//pg_wacom_tablet[ ident ]->prsNew / 1023.0 );
			}
		}
	}
	*/
}

/*** END OF FILE ***/
