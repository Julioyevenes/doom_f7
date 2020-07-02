// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include "config.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_event.h"
#include "d_main.h"
#include "i_video.h"
#include "z_zone.h"

#include "tables.h"
#include "doomkeys.h"

#include <stdint.h>
#include <stdbool.h>

#include "stm32f769i_discovery_lcd.h"
#include "usbh_hid.h"

extern USBH_HandleTypeDef hUSBHost[5];
extern uint8_t host_state;

// The screen buffer; this is modified to draw things to the screen

byte *I_VideoBuffer = NULL;

// If true, game is running as a screensaver

boolean screensaver_mode = false;

// Flag indicating whether the screen is currently visible:
// when the screen isnt visible, don't render the screen

boolean screenvisible;

// Mouse acceleration
//
// This emulates some of the behavior of DOS mouse drivers by increasing
// the speed when the mouse is moved fast.
//
// The mouse input values are input directly to the game, but when
// the values exceed the value of mouse_threshold, they are multiplied
// by mouse_acceleration to increase the speed.

float mouse_acceleration = 2.0;
int mouse_threshold = 10;

// Gamma correction level to use

int usegamma = 0;

int usemouse = 0;

// If true, keyboard mapping is ignored, like in Vanilla Doom.
// The sensible thing to do is to disable this if you have a non-US
// keyboard.

int vanilla_keyboard_mapping = true;


typedef struct
{
	byte r;
	byte g;
	byte b;
} col_t;

// Palette converted to RGB565

static uint16_t rgb565_palette[256];

typedef enum
{
	PRESS_KEY,
	RELEASE_KEY,
	UPDATE_KEY
} keystate_typedef;

static keystate_typedef keystate;
static uint8_t key, lastkeys[6];
static bool last_lalt, last_lshift, last_lctrl;

void I_InitGraphics (void)
{
	/* LCD DSI initialization in mode Video Burst */
	/* Initialize DSI LCD */
	while(BSP_LCD_InitEx(LCD_ORIENTATION_LANDSCAPE) != LCD_OK);

	BSP_LCD_SetBrightness(100);

	BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

	BSP_LCD_SelectLayer(1);

	BSP_LCD_SetLayerWindow(	1,
				(BSP_LCD_GetXSize() - 640) / 2,
				(BSP_LCD_GetYSize() - 400) / 2,
				640, 400 );

	I_VideoBuffer = (byte*) Z_Malloc (SCREENWIDTH * SCREENHEIGHT, PU_STATIC, NULL);

	screenvisible = true;
}

void I_ShutdownGraphics (void)
{
	Z_Free (I_VideoBuffer);
}

void I_StartFrame (void)
{
}

void I_GetEvent (void)
{
	event_t event;
	HID_KEYBD_Info_TypeDef *k_pinfo;
	uint8_t i;

	if(host_state == HOST_USER_CLASS_ACTIVE)
	{
		k_pinfo = USBH_HID_GetKeybdInfo(&hUSBHost[3]);
		if(k_pinfo != NULL)
		{
			for(keystate = 0; keystate < 3; keystate++)
			{
				for(i = 0; i < 6; i++)
				{
					switch(keystate)
					{
						case PRESS_KEY:
							event.type = ev_keydown;
							key = k_pinfo->keys[i];
							break;

						case RELEASE_KEY:
							event.type = ev_keyup;
							key = lastkeys[i];
							if (key == k_pinfo->keys[i])
								key = 0;
							break;

						case UPDATE_KEY:
							lastkeys[i] = k_pinfo->keys[i];
							break;
					}

					if( (key != 0) && (keystate != UPDATE_KEY) )
					{
						event.data1 = -1;
						event.data2 = -1;
						event.data3 = -1;
						event.data4 = -1;

						switch(key)
						{
							case KB_KEY_SPACEBAR:
								event.data1 = KEY_FIRE;
								break;

							case KB_KEY_ENTER:
								event.data1 = KEY_ENTER;
								break;

							case KB_KEY_ESCAPE:
								event.data1 = KEY_ESCAPE;
								break;

							case KB_KEY_TAB:
								event.data1 = KEY_TAB;
								break;

							case KB_KEY_P:
								event.data1 = KEY_PAUSE;
								break;

							case KB_KEY_F2:
								event.data1 = KEY_F2;
								break;

							case KB_KEY_F3:
								event.data1 = KEY_F3;
								break;

							case KB_KEY_W:
								event.data1 = KEY_UPARROW;
								break;

							case KB_KEY_S:
								event.data1 = KEY_DOWNARROW;
								break;

							case KB_KEY_A:
								event.data1 = KEY_STRAFE_L;
								break;

							case KB_KEY_D:
								event.data1 = KEY_STRAFE_R;
								break;

							case KB_KEY_U:
								event.data1 = KEY_PGUP;
								break;

							case KB_KEY_J:
								event.data1 = KEY_INS;
								break;

							case KB_KEY_Q:
								event.data1 = '[';
								break;

							case KB_KEY_E:
								event.data1 = ']';
								break;

							case KB_KEY_1_EXCLAMATION_MARK:
								event.data1 = '1';
								break;

							case KB_KEY_2_AT:
								event.data1 = '2';
								break;

							case KB_KEY_3_NUMBER_SIGN:
								event.data1 = '3';
								break;

							case KB_KEY_4_DOLLAR:
								event.data1 = '4';
								break;

							case KB_KEY_5_PERCENT:
								event.data1 = '5';
								break;

							case KB_KEY_6_CARET:
								event.data1 = '6';
								break;

							case KB_KEY_RIGHTARROW:
								event.data1 = KEY_RIGHTARROW;
								break;

							case KB_KEY_LEFTARROW:
								event.data1 = KEY_LEFTARROW;
								break;
						}

						D_PostEvent (&event);
					}
				}
			}

			if(last_lalt != k_pinfo->lalt)
			{
				last_lalt = k_pinfo->lalt;

				event.type = last_lalt ? ev_keydown : ev_keyup;
				event.data1 = KEY_USE;
				event.data2 = -1;
				event.data3 = -1;
				event.data4 = -1;

				D_PostEvent (&event);
			}
			else if(last_lshift != k_pinfo->lshift)
			{
				last_lshift = k_pinfo->lshift;

				event.type = last_lshift ? ev_keydown : ev_keyup;
				event.data1 = KEY_RSHIFT;
				event.data2 = -1;
				event.data3 = -1;
				event.data4 = -1;

				D_PostEvent (&event);
			}
			else if(last_lctrl != k_pinfo->lctrl)
			{
				last_lctrl = k_pinfo->lctrl;

				event.type = last_lctrl ? ev_keydown : ev_keyup;
				event.data1 = '/';
				event.data2 = -1;
				event.data3 = -1;
				event.data4 = -1;

				D_PostEvent (&event);
			}
		}

		HID_MOUSE_Info_TypeDef *m_pinfo;

		m_pinfo = USBH_HID_GetMouseInfo(&hUSBHost[4]);
		if(m_pinfo != NULL)
		{
			event.type = ev_mouse;
			event.data1 = m_pinfo->buttons[2] << 2 | \
				      	  m_pinfo->buttons[1] << 1 | \
						  m_pinfo->buttons[0];
			event.data2 = m_pinfo->x;
			event.data3 = 0;
			event.data4 = -1;

			D_PostEvent (&event);
		}
	}
}

void I_StartTic (void)
{
	I_GetEvent();
}

void I_UpdateNoBlit (void)
{
}

void I_FinishUpdate (void)
{
	int x, y, pixel;
	byte index;

	for (y = 0; y < SCREENHEIGHT; y++)
	{
		for (x = 0; x < SCREENWIDTH; x++)
		{
			pixel = ((y * 2) * (SCREENWIDTH * 2)) + (x * 2);
			index = I_VideoBuffer[y * SCREENWIDTH + x];

			((uint16_t*)LCD_FB_START_ADDRESS)[pixel    ] = rgb565_palette[index];
			((uint16_t*)LCD_FB_START_ADDRESS)[pixel + 1] = rgb565_palette[index];

			((uint16_t*)LCD_FB_START_ADDRESS)[(SCREENWIDTH * 2) + pixel    ] = rgb565_palette[index];
			((uint16_t*)LCD_FB_START_ADDRESS)[(SCREENWIDTH * 2) + pixel + 1] = rgb565_palette[index];
		}
	}
}

//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
	int i;
	col_t* c;

	for (i = 0; i < 256; i++)
	{
		c = (col_t*)palette;

		rgb565_palette[i] = RGB565( gammatable[usegamma][c->r],
					    			gammatable[usegamma][c->g],
									gammatable[usegamma][c->b] );

		palette += 3;
	}
}

// Given an RGB value, find the closest matching palette index.

int I_GetPaletteIndex (int r, int g, int b)
{
    int best, best_diff, diff;
    int i;
    col_t color;

    best = 0;
    best_diff = INT_MAX;

    for (i = 0; i < 256; ++i)
    {
    	color.r = RGB565_R(rgb565_palette[i]);
    	color.g = RGB565_G(rgb565_palette[i]);
    	color.b = RGB565_B(rgb565_palette[i]);

        diff = (r - color.r) * (r - color.r)
             + (g - color.g) * (g - color.g)
             + (b - color.b) * (b - color.b);

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }

        if (diff == 0)
        {
            break;
        }
    }

    return best;
}

void I_BeginRead (void)
{
}

void I_EndRead (void)
{
}

void I_SetWindowTitle (char *title)
{
}

void I_GraphicsCheckCommandLine (void)
{
}

void I_SetGrabMouseCallback (grabmouse_callback_t func)
{
}

void I_EnableLoadingDisk (void)
{
}

void I_BindVideoVariables (void)
{
}

void I_DisplayFPSDots (boolean dots_on)
{
}

void I_CheckIsScreensaver (void)
{
}
