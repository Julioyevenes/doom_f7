/**
  ******************************************************************************
  * @file    usbh_hid_keybd.h
  * @author  MCD Application Team
  * @version V3.2.2
  * @date    07-July-2015
  * @brief   This file contains all the prototypes for the usbh_hid_keybd.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive -----------------------------------------------*/
#ifndef __USBH_HID_KEYBD_H
#define __USBH_HID_KEYBD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid.h"
#include "usbh_hid_keybd.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */
  
/** @defgroup USBH_HID_KEYBD
  * @brief This file is the Header file for usbh_hid_keybd.c
  * @{
  */ 


/** @defgroup USBH_HID_KEYBD_Exported_Types
  * @{
  */ 
#define K_NONE                               0x00
#define K_ERRORROLLOVER                      0x01
#define K_POSTFAIL                           0x02
#define K_ERRORUNDEFINED                     0x03
#define K_A                                  0x04
#define K_B                                  0x05
#define K_C                                  0x06
#define K_D                                  0x07
#define K_E                                  0x08
#define K_F                                  0x09
#define K_G                                  0x0A
#define K_H                                  0x0B
#define K_I                                  0x0C
#define K_J                                  0x0D
#define K_K                                  0x0E
#define K_L                                  0x0F
#define K_M                                  0x10
#define K_N                                  0x11
#define K_O                                  0x12
#define K_P                                  0x13
#define K_Q                                  0x14
#define K_R                                  0x15
#define K_S                                  0x16
#define K_T                                  0x17
#define K_U                                  0x18
#define K_V                                  0x19
#define K_W                                  0x1A
#define K_X                                  0x1B
#define K_Y                                  0x1C
#define K_Z                                  0x1D
#define K_1_EXCLAMATION_MARK                 0x1E
#define K_2_AT                               0x1F
#define K_3_NUMBER_SIGN                      0x20
#define K_4_DOLLAR                           0x21
#define K_5_PERCENT                          0x22
#define K_6_CARET                            0x23
#define K_7_AMPERSAND                        0x24
#define K_8_ASTERISK                         0x25
#define K_9_OPARENTHESIS                     0x26
#define K_0_CPARENTHESIS                     0x27
#define K_ENTER                              0x28
#define K_ESCAPE                             0x29
#define K_BACKSPACE                          0x2A
#define K_TAB                                0x2B
#define K_SPACEBAR                           0x2C
#define K_MINUS_UNDERSCORE                   0x2D
#define K_EQUAL_PLUS                         0x2E
#define K_OBRACKET_AND_OBRACE                0x2F
#define K_CBRACKET_AND_CBRACE                0x30
#define K_BACKSLASH_VERTICAL_BAR             0x31
#define K_NONUS_NUMBER_SIGN_TILDE            0x32
#define K_SEMICOLON_COLON                    0x33
#define K_SINGLE_AND_DOUBLE_QUOTE            0x34
#define K_GRAVE ACCENT AND TILDE             0x35
#define K_COMMA_AND_LESS                     0x36
#define K_DOT_GREATER                        0x37
#define K_SLASH_QUESTION                     0x38
#define K_CAPS LOCK                          0x39
#define K_F1                                 0x3A
#define K_F2                                 0x3B
#define K_F3                                 0x3C
#define K_F4                                 0x3D
#define K_F5                                 0x3E
#define K_F6                                 0x3F
#define K_F7                                 0x40
#define K_F8                                 0x41
#define K_F9                                 0x42
#define K_F10                                0x43
#define K_F11                                0x44
#define K_F12                                0x45
#define K_PRINTSCREEN                        0x46
#define K_SCROLL LOCK                        0x47
#define K_PAUSE                              0x48
#define K_INSERT                             0x49
#define K_HOME                               0x4A
#define K_PAGEUP                             0x4B
#define K_DELETE                             0x4C
#define K_END1                               0x4D
#define K_PAGEDOWN                           0x4E
#define K_RIGHTARROW                         0x4F
#define K_LEFTARROW                          0x50
#define K_DOWNARROW                          0x51
#define K_UPARROW                            0x52
#define K_KEYPAD_NUM_LOCK_AND_CLEAR          0x53
#define K_KEYPAD_SLASH                       0x54
#define K_KEYPAD_ASTERIKS                    0x55
#define K_KEYPAD_MINUS                       0x56
#define K_KEYPAD_PLUS                        0x57
#define K_KEYPAD_ENTER                       0x58
#define K_KEYPAD_1_END                       0x59
#define K_KEYPAD_2_DOWN_ARROW                0x5A
#define K_KEYPAD_3_PAGEDN                    0x5B
#define K_KEYPAD_4_LEFT_ARROW                0x5C
#define K_KEYPAD_5                           0x5D
#define K_KEYPAD_6_RIGHT_ARROW               0x5E
#define K_KEYPAD_7_HOME                      0x5F
#define K_KEYPAD_8_UP_ARROW                  0x60
#define K_KEYPAD_9_PAGEUP                    0x61
#define K_KEYPAD_0_INSERT                    0x62
#define K_KEYPAD_DECIMAL_SEPARATOR_DELETE    0x63
#define K_NONUS_BACK_SLASH_VERTICAL_BAR      0x64
#define K_APPLICATION                        0x65
#define K_POWER                              0x66
#define K_KEYPAD_EQUAL                       0x67
#define K_F13                                0x68
#define K_F14                                0x69
#define K_F15                                0x6A
#define K_F16                                0x6B
#define K_F17                                0x6C
#define K_F18                                0x6D
#define K_F19                                0x6E
#define K_F20                                0x6F
#define K_F21                                0x70
#define K_F22                                0x71
#define K_F23                                0x72
#define K_F24                                0x73
#define K_EXECUTE                            0x74
#define K_HELP                               0x75
#define K_MENU                               0x76
#define K_SELECT                             0x77
#define K_STOP                               0x78
#define K_AGAIN                              0x79
#define K_UNDO                               0x7A
#define K_CUT                                0x7B
#define K_COPY                               0x7C
#define K_PASTE                              0x7D
#define K_FIND                               0x7E
#define K_MUTE                               0x7F
#define K_VOLUME_UP                          0x80
#define K_VOLUME_DOWN                        0x81
#define K_LOCKING_CAPS_LOCK                  0x82
#define K_LOCKING_NUM_LOCK                   0x83
#define K_LOCKING_SCROLL_LOCK                0x84
#define K_KEYPAD_COMMA                       0x85
#define K_KEYPAD_EQUAL_SIGN                  0x86
#define K_INTERNATIONAL1                     0x87
#define K_INTERNATIONAL2                     0x88
#define K_INTERNATIONAL3                     0x89
#define K_INTERNATIONAL4                     0x8A
#define K_INTERNATIONAL5                     0x8B
#define K_INTERNATIONAL6                     0x8C
#define K_INTERNATIONAL7                     0x8D
#define K_INTERNATIONAL8                     0x8E
#define K_INTERNATIONAL9                     0x8F
#define K_LANG1                              0x90
#define K_LANG2                              0x91
#define K_LANG3                              0x92
#define K_LANG4                              0x93
#define K_LANG5                              0x94
#define K_LANG6                              0x95
#define K_LANG7                              0x96
#define K_LANG8                              0x97
#define K_LANG9                              0x98
#define K_ALTERNATE_ERASE                    0x99
#define K_SYSREQ                             0x9A
#define K_CANCEL                             0x9B
#define K_CLEAR                              0x9C
#define K_PRIOR                              0x9D
#define K_RETURN                             0x9E
#define K_SEPARATOR                          0x9F
#define K_OUT                                0xA0
#define K_OPER                               0xA1
#define K_CLEAR_AGAIN                        0xA2
#define K_CRSEL                              0xA3
#define K_EXSEL                              0xA4
#define K_KEYPAD_00                          0xB0
#define K_KEYPAD_000                         0xB1
#define K_THOUSANDS_SEPARATOR                0xB2
#define K_DECIMAL_SEPARATOR                  0xB3
#define K_CURRENCY_UNIT                      0xB4
#define K_CURRENCY_SUB_UNIT                  0xB5
#define K_KEYPAD_OPARENTHESIS                0xB6
#define K_KEYPAD_CPARENTHESIS                0xB7
#define K_KEYPAD_OBRACE                      0xB8
#define K_KEYPAD_CBRACE                      0xB9
#define K_KEYPAD_TAB                         0xBA
#define K_KEYPAD_BACKSPACE                   0xBB
#define K_KEYPAD_A                           0xBC
#define K_KEYPAD_B                           0xBD
#define K_KEYPAD_C                           0xBE
#define K_KEYPAD_D                           0xBF
#define K_KEYPAD_E                           0xC0
#define K_KEYPAD_F                           0xC1
#define K_KEYPAD_XOR                         0xC2
#define K_KEYPAD_CARET                       0xC3
#define K_KEYPAD_PERCENT                     0xC4
#define K_KEYPAD_LESS                        0xC5
#define K_KEYPAD_GREATER                     0xC6
#define K_KEYPAD_AMPERSAND                   0xC7
#define K_KEYPAD_LOGICAL_AND                 0xC8
#define K_KEYPAD_VERTICAL_BAR                0xC9
#define K_KEYPAD_LOGIACL_OR                  0xCA
#define K_KEYPAD_COLON                       0xCB
#define K_KEYPAD_NUMBER_SIGN                 0xCC
#define K_KEYPAD_SPACE                       0xCD
#define K_KEYPAD_AT                          0xCE
#define K_KEYPAD_EXCLAMATION_MARK            0xCF
#define K_KEYPAD_MEMORY_STORE                0xD0
#define K_KEYPAD_MEMORY_RECALL               0xD1
#define K_KEYPAD_MEMORY_CLEAR                0xD2
#define K_KEYPAD_MEMORY_ADD                  0xD3
#define K_KEYPAD_MEMORY_SUBTRACT             0xD4
#define K_KEYPAD_MEMORY_MULTIPLY             0xD5
#define K_KEYPAD_MEMORY_DIVIDE               0xD6
#define K_KEYPAD_PLUSMINUS                   0xD7
#define K_KEYPAD_CLEAR                       0xD8
#define K_KEYPAD_CLEAR_ENTRY                 0xD9
#define K_KEYPAD_BINARY                      0xDA
#define K_KEYPAD_OCTAL                       0xDB
#define K_KEYPAD_DECIMAL                     0xDC
#define K_KEYPAD_HEXADECIMAL                 0xDD
#define K_LEFTCONTROL                        0xE0
#define K_LEFTSHIFT                          0xE1
#define K_LEFTALT                            0xE2
#define K_LEFT_GUI                           0xE3
#define K_RIGHTCONTROL                       0xE4
#define K_RIGHTSHIFT                         0xE5
#define K_RIGHTALT                           0xE6
#define K_RIGHT_GUI                          0xE7

typedef struct 
{
  uint8_t state;
  uint8_t lctrl;
  uint8_t lshift;
  uint8_t lalt;
  uint8_t lgui;
  uint8_t rctrl;
  uint8_t rshift;
  uint8_t ralt;
  uint8_t rgui;
  uint8_t keys[6];
} 
HID_KEYBD_Info_TypeDef;

USBH_StatusTypeDef USBH_HID_KeybdInit(USBH_HandleTypeDef *phost);
HID_KEYBD_Info_TypeDef *USBH_HID_GetKeybdInfo(USBH_HandleTypeDef *phost);
uint8_t USBH_HID_GetASCIICode(HID_KEYBD_Info_TypeDef *info);

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __USBH_HID_KEYBD_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

