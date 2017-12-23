/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

//#error __FILE__

EXTERNC void init_board();

EXTERNC void post_init_board(GDisplay *g);
EXTERNC void setpin_reset(GDisplay *g, bool_t state);
EXTERNC void set_backlight(GDisplay *g, uint8_t percent);
EXTERNC void setreadmode(GDisplay *g);
EXTERNC void setwritemode(GDisplay *g);

EXTERNC bool_t display_pressed();

EXTERNC void acquire_bus(uint8_t cs_pin);
EXTERNC void release_bus(uint8_t cs_pin);
EXTERNC void write_index(uint16_t index);
EXTERNC void write_data(uint16_t data);

EXTERNC uint8_t read_data(uint8_t port);
EXTERNC uint16_t read_data16(uint16_t port);

#endif /* _GDISP_LLD_BOARD_H */
