/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

#include "..\source\cfg.h"

/* The operating system to use. One of these must be defined - preferably in your Makefile */
#define GFX_USE_OS_FREERTOS     TRUE
#define GFX_OS_NO_INIT          TRUE
//#define GFX_USE_OS_ARDUINO                           TRUE

//    #define GFX_CPU                                  1


///////////////////////////////////////////////////////////////////////////
// GDISP                                                                 //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GDISP                                TRUE


/* Features for the GDISP sub-system. */
#define GDISP_NEED_VALIDATION                        TRUE
#define GDISP_NEED_CLIP                              TRUE
#define GDISP_NEED_CIRCLE                            TRUE
#define GDISP_NEED_CONVEX_POLYGON                    TRUE
#define GDISP_NEED_ARC                               TRUE
//#define GDISP_NEED_SCROLL                            TRUE
#define GDISP_NEED_CONTROL                           TRUE
#define GDISP_NEED_TEXT                              TRUE
    #define GDISP_INCLUDE_FONT_UI2                   TRUE

#define GDISP_NEED_IMAGE                             TRUE
    #define GDISP_NEED_IMAGE_GIF                     TRUE

#define GDISP_DEFAULT_ORIENTATION                    GDISP_ROTATE_LANDSCAPE
#define GDISP_NEED_MULTITHREAD                       TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS12_AA           TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS16_AA           TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS20_AA           TRUE

///////////////////////////////////////////////////////////////////////////
// GWIN                                                                  //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GWIN                                 TRUE

#define GWIN_NEED_WINDOWMANAGER                      TRUE
	#define GWIN_NEED_FLASHING						 TRUE

#define GWIN_NEED_CONSOLE                            TRUE
    #define GWIN_CONSOLE_USE_HISTORY                 TRUE
        #define GWIN_CONSOLE_HISTORY_AVERAGING       TRUE
        #define GWIN_CONSOLE_HISTORY_ATCREATE        TRUE
#define GWIN_NEED_GRAPH                              TRUE

#define GWIN_NEED_WIDGET                             TRUE
    #define GWIN_NEED_LABEL                          TRUE
        #define GWIN_LABEL_ATTRIBUTE                 TRUE
    #define GWIN_NEED_BUTTON                         TRUE
//        #define GWIN_BUTTON_LAZY_RELEASE             TRUE
    #define GWIN_NEED_SLIDER                         TRUE
    #define GWIN_NEED_CHECKBOX                       TRUE
    #define GWIN_NEED_IMAGE                          TRUE
    #define GWIN_NEED_RADIO                          TRUE
    #define GWIN_NEED_LIST                           TRUE
        #define GWIN_NEED_LIST_IMAGES                TRUE
    #define GWIN_NEED_PROGRESSBAR                    TRUE
        #define GWIN_PROGRESSBAR_AUTO                TRUE

#define GWIN_NEED_CONTAINERS                         TRUE
    #define GWIN_NEED_CONTAINER                      TRUE


///////////////////////////////////////////////////////////////////////////
// GEVENT                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GEVENT                               TRUE

///////////////////////////////////////////////////////////////////////////
// GTIMER                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GTIMER                               TRUE

///////////////////////////////////////////////////////////////////////////
// GQUEUE                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GQUEUE                               TRUE

#define GQUEUE_NEED_ASYNC                            TRUE

///////////////////////////////////////////////////////////////////////////
// GINPUT                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GINPUT                               TRUE

#define GINPUT_NEED_MOUSE                            TRUE
#define GINPUT_TOUCH_USER_CALIBRATION_SAVE           TRUE
#define GINPUT_TOUCH_USER_CALIBRATION_LOAD		     TRUE
//#define GINPUT_NEED_TOGGLE                           TRUE
//#define GINPUT_NEED_DIAL                             TRUE


#endif /* _GFXCONF_H */

