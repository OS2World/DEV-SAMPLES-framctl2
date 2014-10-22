/************************************************************************/
/*                                                                      */
/* Program name:   FrameCtl.C                                           */
/* Title:          A Picture Perfect Control                            */
/*                 OS/2 Magazine - GUI Corner                           */
/*                 October 1996 issue                                   */
/*                                                                      */
/* Author:         Mark Benge   IBM Corp.                               */
/*                 Matt Smith   Prominare Inc.                          */
/*                                                                      */
/* Description:    Illustrates how to add frame control extensions.     */
/*                                                                      */
/* DISCLAIMER OF WARRANTIES:                                            */
/* -------------------------                                            */
/* The following [enclosed] code is sample code created by IBM          */
/* Corporation and Prominare Inc.  This sample code is not part of any  */
/* standard IBM product and is provided to you solely for the purpose   */
/* of assisting you in the development of your applications.  The code  */
/* is provided "AS IS", without warranty of any kind.  Neither IBM nor  */
/* Prominare shall be liable for any damages arising out of your        */
/* use of the sample code, even if they have been advised of the        */
/* possibility of such damages.                                         */
/************************************************************************/

#define INCL_DOS                   /* Include OS/2 DOS Kernal           */
#define INCL_GPI                   /* Include OS/2 PM GPI Interface     */
#define INCL_WIN                   /* Include OS/2 PM Windows Interface */

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "framectl.h"

/************************************************************************/
/* This module contains example installable control that can be used    */
/* by any OS/2 2.x and Warp Presentation Manager application.  The      */
/* sample demonstrates the principles of adding frame control           */
/* extensions such that other extensions can be added using this as     */
/* a model.                                                             */
/*                                                                      */
/* Filename : FrameCtl.C                                                */
/* Version  : 1.00                                                      */
/* Created  : 1996-06-07                                                */
/* Revised  :                                                           */
/* Released :                                                           */
/*                                                                      */
/* Routines:  MRESULT EXPENTRY FrameWndProc( HWND hWnd, ULONG msg,      */
/*                                           MPARAM mp1, MPARAM mp2 )   */
/*            int main(int argc, char* argv[] )                         */
/*                                                                      */
/* Copyright ¸ International Business Machines Corp., 1991,1992,1993.   */
/* Copyright ¸ 1989-1993  Prominare Inc.  All Rights Reserved.          */
/*                                                                      */
/************************************************************************/

HWND hwndHelpBtn;                  /* Help Button Handle                */
HWND hwndComboBox;                 /* Combo Box Handle                  */
PFNWP DefFrameWndProc;             /* Frame Control Subclass Procedure  */

LONG lMinTitleHeight = 0;          /* Height of the title bar           */
LONG lMinMenuHeight = 0;           /* Height of the menu bar            */

LONG colorArray[] = { CLR_BLUE, CLR_WHITE, CLR_RED,
                      CLR_GREEN, CLR_BLACK };

/************************************************************************/
/* Prototypes                                                           */
/************************************************************************/
int main( int, char* [] );
MRESULT EXPENTRY FrameWndProc( HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
LONG CalcMenuBarMinWidth( HWND hWnd );

/* --- FrameWndProc ----------------------------------- [ Private ] --- */
/*                                                                      */
/*     This function is used to process the messages for the frame      */
/*     control window.                                                  */
/*                                                                      */
/*     Upon Entry:                                                      */
/*                                                                      */
/*     HWND   hWnd; = Window Handle                                     */
/*     ULONG  msg;  = PM Message                                        */
/*     MPARAM mp1;  = Message Parameter 1                               */
/*     MPARAM mp2;  = Message Parameter 2                               */
/*                                                                      */
/*     Upon Exit:                                                       */
/*                                                                      */
/* -------------------------------------------------------------------- */
MRESULT EXPENTRY FrameWndProc( HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch ( msg )
  {
    case WM_FORMATFRAME :
    {
      /******************************************************************/
      /* Query the number of standard frame controls                    */
      /******************************************************************/
      ULONG ulStdCtlCount = (ULONG)DefFrameWndProc( hWnd, msg, mp1, mp2 );
      ULONG ulIdx = ulStdCtlCount;

      /******************************************************************/
      /* Access the SWP array that is passed to us                      */
      /******************************************************************/
      ULONG i;
      PSWP swpArr = (PSWP)mp1;

      for (i=0; i < ulStdCtlCount; i++)
      {
        if ( WinQueryWindowUShort( swpArr[i].hwnd, QWS_ID ) == FID_TITLEBAR )
        {
          /**************************************************************/
          /* Initialize the SWPs for our graphic button control.        */
          /* Since the SWP array for the std frame controls is 0-based  */
          /* and occupy indexes 0 thru n-1 (where n is the total        */
          /* count), we use index n for our graphic button control.     */
          /* Please note that the width and height of our button is the */
          /* same as the height of the title bar.                       */
          /**************************************************************/
          swpArr[ulIdx].fl = SWP_MOVE | SWP_SIZE | SWP_NOADJUST;
          swpArr[ulIdx].cy = swpArr[i].cy;
          swpArr[ulIdx].cx = swpArr[ulIdx].cy;
          swpArr[ulIdx].y  = swpArr[i].y;
          swpArr[ulIdx].hwndInsertBehind = HWND_TOP;

          /**************************************************************/
          /* Calculate the x-position for the help graphic button, and  */
          /* set its window handle.                                     */
          /**************************************************************/
          swpArr[ulIdx].x = swpArr[i].x + swpArr[i].cx - swpArr[ulIdx].cx;
          swpArr[ulIdx].hwnd = hwndHelpBtn;

          /**************************************************************/
          /* Adjust width of the title bar to accomodate the help       */
          /* graphic button.                                            */
          /**************************************************************/
          swpArr[i].cx -= swpArr[ulIdx].cx;

          /**************************************************************/
          /* Save the minimum title bar height that we'll use in        */
          /* WM_QUERYTRACKINFO processing to calculate the minimum      */
          /* tracking height.                                           */
          /**************************************************************/
          lMinTitleHeight = (swpArr[i].cy > lMinTitleHeight) ? swpArr[i].cy
                                                             : lMinTitleHeight;
        }

        if ( WinQueryWindowUShort( swpArr[i].hwnd, QWS_ID ) == FID_MENU )
        {
          /**************************************************************/
          /* Initialize the SWP for our drop down combo box.  Since     */
          /* the SWP array for the std frame controls is 0-based and    */
          /* occupy indexes 0 thru n-1 (where n is the total count),    */
          /* and the graphic buttions occupy n and n+1, we start with   */
          /* index n+2 for our drop down combo box.                     */
          /**************************************************************/
          /**************************************************************/
          /* These values are the same for both controls, so we init    */
          /* them all at once.  Please note that the width and height   */
          /* of our buttons is the same as the height of the title bar. */
          /**************************************************************/
          swpArr[ulIdx+1].fl = SWP_MOVE | SWP_SIZE | SWP_NOADJUST;
          swpArr[ulIdx+1].cy = COMBOBOX_HEIGHT;
          swpArr[ulIdx+1].cx = COMBOBOX_WIDTH;
          swpArr[ulIdx+1].y  = swpArr[i].y -
                               (COMBOBOX_HEIGHT - swpArr[i].cy - 1);
          swpArr[ulIdx+1].hwndInsertBehind = HWND_TOP;

          /**************************************************************/
          /* Calculate the x-position for the help graphic button, and  */
          /* set its window handle.                                     */
          /**************************************************************/
          swpArr[ulIdx+1].x = swpArr[i].x + swpArr[i].cx - swpArr[ulIdx+1].cx;
          swpArr[ulIdx+1].hwnd = hwndComboBox;

          /**************************************************************/
          /* Adjust the width of the menu bar to accomodate our drop    */
          /* down combo box.                                            */
          /**************************************************************/
          swpArr[i].cx -= swpArr[ulIdx+1].cx;

          /**************************************************************/
          /* Save the minimum menu bar height that we'll use in         */
          /* WM_QUERYTRACKINFO processing to calculate the minimum      */
          /* tracking height.                                           */
          /**************************************************************/
          lMinMenuHeight = (swpArr[i].cy > lMinMenuHeight) ? swpArr[i].cy
                                                           : lMinMenuHeight;
        }
      }

      /******************************************************************/
      /* Increment the number of frame controls to include our graphic  */
      /* button control and drop down combo box.                        */
      /******************************************************************/
      return( (MRESULT)(ulIdx + 2) );
    }

    case WM_QUERYFRAMECTLCOUNT :
      /******************************************************************/
      /* Query the standard frame controls count and increment to       */
      /* include our graphic button control and drop down combo box.    */
      /******************************************************************/
      return( (MRESULT)((ULONG)DefFrameWndProc( hWnd, msg, mp1, mp2 ) + 2) );

    case WM_QUERYTRACKINFO :
    {
      /******************************************************************/
      /* Query the default tracking information for the standard frame  */
      /* control.                                                       */
      /******************************************************************/
      BOOL rc = (BOOL)DefFrameWndProc( hWnd, msg, mp1, mp2 );
      PTRACKINFO pTrackInfo = (PTRACKINFO)mp2;

      /******************************************************************/
      /* Calculate the minimum width that we require for the menu bar.  */
      /******************************************************************/
      LONG lMinMenuBarWidth = CalcMenuBarMinWidth( hWnd );

      if ( lMinMenuBarWidth == MIT_ERROR )
        return( (MRESULT)FALSE );

      /******************************************************************/
      /* Calculate and set the minimum tracking width and height.  Note */
      /* that we only use the menu bar to calculate the minmum width.   */
      /* You may want to expand this calculation to include the title   */
      /* bar or other factors.                                          */
      /******************************************************************/
      pTrackInfo->ptlMinTrackSize.x = COMBOBOX_WIDTH +
                                      lMinMenuBarWidth +
                                      (WinQuerySysValue( HWND_DESKTOP,
                                                         SV_CXSIZEBORDER ) * 2);
      pTrackInfo->ptlMinTrackSize.y = lMinTitleHeight + lMinMenuHeight +
                                      (WinQuerySysValue( HWND_DESKTOP,
                                                         SV_CYSIZEBORDER ) * 2);
      return( (MRESULT)TRUE );
    }

    case WM_COMMAND :
      if ( SHORT1FROMMP(mp2) == CMDSRC_MENU )
      {
        /****************************************************************/
        /* Process the Exit request.                                    */
        /****************************************************************/
        if ( (USHORT)mp1 == MI_FILE_EXIT )
        {
          /**************************************************************/
          /* Close the frame extensions sample application.             */
          /**************************************************************/
          WinPostMsg( hWnd, WM_CLOSE, 0, 0 );
          return( (MRESULT)FALSE );
        }
        else
        {
          /**************************************************************/
          /* Place holder for next issue.                               */
          /**************************************************************/
          if ( (USHORT)mp1 == MI_CAT_TBD )
          {
            /************************************************************/
            /*                                                          */
            /************************************************************/
            return( (MRESULT)FALSE );
          }
        }
      }
      return( DefFrameWndProc( hWnd, msg, mp1, mp2 ) );

    case WM_CONTROL :
      if ( (SHORT1FROMMP(mp1) == COMBOBOX_ID)  &&
           (SHORT2FROMMP(mp1) == CBN_LBSELECT) )
      {
        /****************************************************************/
        /* Process the list box selection notification.                 */
        /****************************************************************/
        LONG lFgnColor;
        LONG lSelIndex = (LONG)WinSendMsg( (HWND)mp2,
                                           LM_QUERYSELECTION,
                                           MPFROMLONG(LIT_FIRST),
                                           0 );
        if ( lSelIndex == LIT_NONE )
          return( FALSE );

        lFgnColor = colorArray[lSelIndex];
        WinSetPresParam( WinWindowFromID( hWnd, FID_CLIENT ),
                         PP_FOREGROUNDCOLORINDEX,
                         4UL,
                         &lFgnColor );

      }
      return( DefFrameWndProc( hWnd, msg, mp1, mp2 ) );

    case WM_HELP :
      /******************************************************************/
      /* Process a click on the help graphic button.                    */
      /******************************************************************/
      if ( (USHORT)mp1 == HELP_BUTTON_ID )
      {
        /****************************************************************/
        /* Display the help panel for this sample.                      */
        /****************************************************************/
        return( WinSendMsg( WinQueryHelpInstance( hWnd ),
                            HM_DISPLAY_HELP,
                            mp1,
                            HM_RESOURCEID ) );
      }

    default:
      return( DefFrameWndProc( hWnd, msg, mp1, mp2 ) );
  }

  return( (MRESULT)FALSE );
}


/* --- CalcMenuBarMinWidth ---------------------------- [ Private ] --- */
/*                                                                      */
/*     This function is used to dynamically calculate the minimum menu  */
/*     bar width required to display the entire menu bar.               */
/*                                                                      */
/*     Upon Entry:                                                      */
/*                                                                      */
/*     HWND   hWnd; = Frame Window Handle                               */
/*                                                                      */
/*     Upon Exit:                                                       */
/*                                                                      */
/* -------------------------------------------------------------------- */
LONG CalcMenuBarMinWidth( HWND hWnd )
{
  RECTL rect;
  LONG i;
  LONG lMinWidth = 0;
  HWND hwndMenuBar = WinWindowFromID( hWnd, FID_MENU );

  /**********************************************************************/
  /* Get the count of the menu items on the menu bar.                   */
  /**********************************************************************/
  LONG lMenuItemsCount = (LONG)WinSendMsg( hwndMenuBar,
                                           MM_QUERYITEMCOUNT,
                                           0,
                                           0 );

  for ( i=0; i < lMenuItemsCount; i++ )
  {
    /********************************************************************/
    /* Get the id of the menu item.                                     */
    /********************************************************************/
    LONG itemID = (LONG)WinSendMsg( hwndMenuBar,
                                     MM_ITEMIDFROMPOSITION,
                                     MPFROMLONG( i ),
                                     0 );
    if ( itemID == MIT_ERROR )
      return( MIT_ERROR );

    /********************************************************************/
    /* Get the rectangle for the menu item.                             */
    /********************************************************************/
    if ( WinSendMsg( hwndMenuBar,
                     MM_QUERYITEMRECT,
                     MPFROM2SHORT((USHORT)itemID, FALSE),
                     MPFROMP(&rect) ) )
    {
      /******************************************************************/
      /* Calculate the item's width and add it to the width of the      */
      /* other items.                                                   */
      /******************************************************************/
      lMinWidth += (rect.xRight - rect.xLeft);
    }
    else
      return( MIT_ERROR );
  }

  return( lMinWidth );
}


/* --- main ----------------------------------------------------------- */
/*     Main function.                                                   */
/* -------------------------------------------------------------------- */
int main( int argc, char* argv[] )
{
  HAB      hAB;                    /* Anchor Block Handle               */
  HMQ      hMQ;                    /* Message Queue Handle              */
  HWND     hwndClient;             /* Client Handle                     */
  HWND     hwndFrame;              /* Frame Handle                      */
  QMSG     qmsg;                   /* PM Message Queue Holder           */
  ULONG    flCreateFlags;          /* Window Creation Flags             */
  LONG     lFgnColor;              /* Foreground Color Holder           */
  LONG     lBgnColor;              /* Background Color Holder           */
  HWND     hwndHelp;               /* Help Instance Handle              */
  HELPINIT helpInit;               /* Help Instance Structure           */
  LBOXINFO lboxinfo;               /* List Box Info Structure           */
  BOOL     bWarpV4 = FALSE;        /* OS/2 Warp V4.0 indicator          */
  ULONG    ulVersion;              /* Version Return Variable           */
  LONG     lCount,
           lWidth,
           lHeight;
  ULONG    i;
  PSZ      itemsArray[] = { "Blue Text",  "White Text", "Red Text",
                            "Green Text", "Black Text" };

  /**********************************************************************/
  /* Initialize the program for PM, create the message queue, and set   */
  /* the codepage.                                                      */
  /**********************************************************************/
  hAB = WinInitialize( 0 );
  hMQ = WinCreateMsgQueue( hAB, 0 );
  WinSetCp( hMQ, 850 );

  /**********************************************************************/
  /* Create a standard frame window, specifying a static text control   */
  /* for the client area.                                               */
  /**********************************************************************/
  flCreateFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_MENU | FCF_SIZEBORDER |
                  FCF_MINMAX | FCF_VERTSCROLL | FCF_HORZSCROLL | FCF_TASKLIST;

  hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                  0,
                                  &flCreateFlags,
                                  WC_STATIC,
                                  "Frame Extensions",
                                  (WS_VISIBLE | SS_TEXT | DT_CENTER |
                                   DT_VCENTER),
                                  (HMODULE)0L,
                                  FRAME_WND_ID,
                                  &hwndClient );
  if ( !hwndFrame )
    return( TRUE );

  /**********************************************************************/
  /* Set the text for the static text control as well as the            */
  /* foreground and background color of the control.                    */
  /**********************************************************************/
  lFgnColor = colorArray[0];
  WinSetPresParam( hwndClient, PP_FOREGROUNDCOLORINDEX, 4UL, &lFgnColor );
  lBgnColor = SYSCLR_DIALOGBACKGROUND;
  WinSetPresParam( hwndClient, PP_BACKGROUNDCOLORINDEX, 4UL, &lBgnColor );

  WinSetWindowText( hwndClient, "Frame Extensions Test" );

  /**********************************************************************/
  /* Create and associate the help instance.                            */
  /**********************************************************************/
  helpInit.cb                       = sizeof(HELPINIT);
  helpInit.ulReturnCode             = 0;
  helpInit.pszTutorialName          = 0;
  helpInit.phtHelpTable             = (PHELPTABLE)MAKELONG(0, 0xffff);
  helpInit.hmodHelpTableModule      = 0;
  helpInit.hmodAccelActionBarModule = 0;
  helpInit.idAccelTable             = 0;
  helpInit.idActionBar              = 0;
  helpInit.pszHelpWindowTitle       = "Frame Extensions Sample";
  helpInit.fShowPanelId             = CMIC_HIDE_PANEL_ID;
  helpInit.pszHelpLibraryName       = "framectl.hlp";

  hwndHelp = WinCreateHelpInstance( hAB, &helpInit );
  if ( !hwndHelp )
    return( TRUE );

  if ( !WinAssociateHelpInstance( hwndHelp, hwndFrame ) )
    return( TRUE );

  /**********************************************************************/
  /* Use the "new look" help bitmap for OS/2 Warp V4 (aka Merlin) if    */
  /* V4 is detected.                                                    */
  /**********************************************************************/
  ulVersion = WinQueryVersion( hAB );
  if ( ((ulVersion & 0xFF) == 20) &&
       (((ulVersion >> 8) & 0xFF) == 40) )
  {
    bWarpV4 = TRUE;
  }

  /**********************************************************************/
  /* Create the help graphic button which we will use as a frame        */
  /* extension on the title bar.                                        */
  /**********************************************************************/
  hwndHelpBtn = WinCreateWindow( hwndFrame,
                                 WC_BUTTON,
                                 ( (bWarpV4) ? "#400" : "#300" ),
                                 (BS_BITMAP | BS_PUSHBUTTON | BS_NOBORDER |
                                  BS_NOPOINTERFOCUS | BS_AUTOSIZE | BS_HELP |
                                  WS_CLIPSIBLINGS | WS_VISIBLE),
                                 0L, 0L, -1L, -1L,
                                 hwndFrame,
                                 HWND_TOP,
                                 HELP_BUTTON_ID,
                                 (PVOID)NULL,
                                 (PVOID)NULL );
  if ( !hwndHelpBtn )
    return( TRUE );

  /**********************************************************************/
  /* Create the drop down combo box which we will use as a frame        */
  /* extension on the menu bar.                                         */
  /**********************************************************************/
  hwndComboBox = WinCreateWindow( hwndFrame,
                                 WC_COMBOBOX,
                                 (PSZ)NULL,
                                 (CBS_DROPDOWNLIST |  WS_VISIBLE),
                                 0L, 0L, -1L, -1L,
                                 hwndFrame,
                                 HWND_TOP,
                                 COMBOBOX_ID,
                                 (PVOID)NULL,
                                 (PVOID)NULL );
  if ( !hwndComboBox )
    return( TRUE );

#if 0
  lboxinfo.lItemIndex = LIT_END;
  lboxinfo.ulItemCount = 5;

  lCount = (LONG)WinSendMsg( hwndComboBox, LM_INSERTMULTITEMS,
                             &lboxinfo, itemsArray[0] );
#endif

  /**********************************************************************/
  /* Populate the drop down combo box.                                  */
  /**********************************************************************/
  for (i = 0; i < 5; i++)
  {
    WinSendMsg( hwndComboBox, LM_INSERTITEM, (MPARAM)LIT_END, itemsArray[i] );
  }

  WinSendMsg( hwndComboBox, LM_SELECTITEM, 0L, (MPARAM)TRUE );

  /**********************************************************************/
  /* Subclass the frame control.  The subclass procedure is where we    */
  /* will add the frame extensions during processing of WM_FORMATFRAME. */
  /**********************************************************************/
  DefFrameWndProc = WinSubclassWindow( hwndFrame, (PFNWP)FrameWndProc );

  if ( !DefFrameWndProc )
    return( TRUE );

  /**********************************************************************/
  /* Indicate to PM that the frame needs updating.                      */
  /**********************************************************************/
  WinSendMsg( hwndFrame, WM_UPDATEFRAME, (MPARAM)~0, NULL );

  /**********************************************************************/
  /* Position (center on the desktop), size, and show the frame window. */
  /**********************************************************************/
  lWidth = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
  lHeight = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );
  WinSetWindowPos( hwndFrame,
                   HWND_TOP,
                   (lWidth - FRAME_WIDTH)/ 2,
                   (lHeight - FRAME_HEIGHT)/ 2,
                   FRAME_WIDTH,
                   FRAME_HEIGHT,
                   SWP_MOVE | SWP_SIZE | SWP_SHOW | SWP_ACTIVATE );

  /**********************************************************************/
  /* Message dispatch loop.                                             */
  /**********************************************************************/
  while ( WinGetMsg( hAB, &qmsg, (HWND)NULL, 0, 0 ) )
    WinDispatchMsg( hAB, &qmsg );

  /**********************************************************************/
  /* Termination processing                                             */
  /**********************************************************************/
  WinDestroyHelpInstance( hwndHelp );
  WinDestroyWindow( hwndFrame );
  WinDestroyMsgQueue( hMQ );
  WinTerminate( hAB );
  return( FALSE );
}

