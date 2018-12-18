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

typedef struct _KeyPressed_TypeDef
{
	uint8_t ascii[6];
} KeyPressed_TypeDef;

static  const  int8_t  HID_KEYBRD_Key[] = {
  '\0',  '`',  '1',  '2',  '3',  '4',  '5',  '6',
  '7',  '8',  '9',  '0',  '-',  '=',  '\0', '\r',
  '\t',  'q',  'w',  'e',  'r',  't',  'y',  'u',
  'i',  'o',  'p',  '[',  ']',  '\\',
  '\0',  'a',  's',  'd',  'f',  'g',  'h',  'j',
  'k',  'l',  ';',  '\'', '\0', '\n',
  '\0',  '\0', 'z',  'x',  'c',  'v',  'b',  'n',
  'm',  ',',  '.',  '/',  '\0', '\0',
  '\0',  '\0', '\0', ' ',  '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '\0', '\0', '\0', '\r', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '7',  '4',  '1',
  '\0',  '/',  '8',  '5',  '2',
  '0',   '*',  '9',  '6',  '3',
  '.',   '-',  '+',  '\0', '\n', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0'
};

static  const  int8_t  HID_KEYBRD_ShiftKey[] = {
  '\0', '~',  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
  '_',  '+',  '\0', '\0', '\0', 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',
  'I',  'O',  'P',  '{',  '}',  '|',  '\0', 'A',  'S',  'D',  'F',  'G',
  'H',  'J',  'K',  'L',  ':',  '"',  '\0', '\n', '\0', '\0', 'Z',  'X',
  'C',  'V',  'B',  'N',  'M',  '<',  '>',  '?',  '\0', '\0',  '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0',    '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

static  const  uint8_t  HID_KEYBRD_Codes[] = {
  0,     0,    0,    0,   31,   50,   48,   33,
  19,   34,   35,   36,   24,   37,   38,   39,       /* 0x00 - 0x0F */
  52,    51,   25,   26,   17,   20,   32,   21,
  23,   49,   18,   47,   22,   46,    2,    3,       /* 0x10 - 0x1F */
  4,    5,    6,    7,    8,    9,   10,   11,
  43,  110,   15,   16,   61,   12,   13,   27,       /* 0x20 - 0x2F */
  28,   29,   42,   40,   41,    1,   53,   54,
  55,   30,  112,  113,  114,  115,  116,  117,       /* 0x30 - 0x3F */
  118,  119,  120,  121,  122,  123,  124,  125,
  126,   75,   80,   85,   76,   81,   86,   89,       /* 0x40 - 0x4F */
  79,   84,   83,   90,   95,  100,  105,  106,
  108,   93,   98,  103,   92,   97,  102,   91,       /* 0x50 - 0x5F */
  96,  101,   99,  104,   45,  129,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x60 - 0x6F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x70 - 0x7F */
  0,    0,    0,    0,    0,  107,    0,   56,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x80 - 0x8F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x90 - 0x9F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xA0 - 0xAF */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xB0 - 0xBF */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xC0 - 0xCF */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xD0 - 0xDF */
  58,   44,   60,  127,   64,   57,   62,  128        /* 0xE0 - 0xE7 */
};

KeyPressed_TypeDef KeyASCII, lastKeyASCII;

void USBH_HID_GetASCIICode_Multi(HID_KEYBD_Info_TypeDef *info)
{
  uint8_t i;

  if((info->lshift == 1) || (info->rshift))
  {
	for(i = 0; i < 6 ; i++)
	{
  		KeyASCII.ascii[i] =  HID_KEYBRD_ShiftKey[HID_KEYBRD_Codes[info->keys[i]]];
	}
  }
  else
  {
	for(i = 0; i < 6 ; i++)
	{
  		KeyASCII.ascii[i] =  HID_KEYBRD_Key[HID_KEYBRD_Codes[info->keys[i]]];
	}
  }
}

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

static bool last_lalt, last_lshift;

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
		k_pinfo = USBH_HID_GetKeybdInfo(&hUSBHost[4]);
		if(k_pinfo != NULL)
		{
			USBH_HID_GetASCIICode_Multi(k_pinfo);

			for(i = 0; i < 6; i++)
			{
				if( ( lastKeyASCII.ascii[i] != KeyASCII.ascii[i] ) && ( lastKeyASCII.ascii[i] == 0 || KeyASCII.ascii[i] == 0 ) )
				{
					event.type = KeyASCII.ascii[i] ? ev_keydown : ev_keyup;
					event.data1 = -1;
					event.data2 = -1;
					event.data3 = -1;
					event.data4 = -1;

					if(event.type != ev_keyup)
						lastKeyASCII.ascii[i] = KeyASCII.ascii[i];

					switch(lastKeyASCII.ascii[i])
					{
						case ' ':
							event.data1 = KEY_FIRE;
							break;

						case '\n':
							event.data1 = KEY_ENTER;
							break;

						case '\r':
							event.data1 = KEY_ESCAPE;
							break;

						case '\t':
							event.data1 = KEY_TAB;
							break;

						case 'p':
							event.data1 = KEY_PAUSE;
							break;

						case '8':
							event.data1 = KEY_F2;
							break;

						case '9':
							event.data1 = KEY_F3;
							break;

						case 'w':
							event.data1 = KEY_UPARROW;
							break;

						case 's':
							event.data1 = KEY_DOWNARROW;
							break;

						case 'a':
							event.data1 = KEY_STRAFE_L;
							break;

						case 'd':
							event.data1 = KEY_STRAFE_R;
							break;

						case 'u':
							event.data1 = KEY_PGUP;
							break;

						case 'j':
							event.data1 = KEY_INS;
							break;

						case 'q':
							event.data1 = '[';
							break;

						case 'e':
							event.data1 = ']';
							break;

						case '1':
							event.data1 = '1';
							break;

						case '2':
							event.data1 = '2';
							break;

						case '3':
							event.data1 = '3';
							break;

						case '4':
							event.data1 = '4';
							break;

						case '5':
							event.data1 = '5';
							break;

						case '6':
							event.data1 = '6';
							break;
					}

					D_PostEvent (&event);

					lastKeyASCII.ascii[i] = KeyASCII.ascii[i];
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
		}

		HID_MOUSE_Info_TypeDef *m_pinfo;

		m_pinfo = USBH_HID_GetMouseInfo(&hUSBHost[3]);
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
