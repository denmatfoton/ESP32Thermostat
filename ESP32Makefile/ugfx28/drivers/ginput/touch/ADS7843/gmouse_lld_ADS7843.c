/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if (GFX_USE_GINPUT && GINPUT_NEED_MOUSE) 

#define GMOUSE_DRIVER_VMT		GMOUSEVMT_ADS7843
#include "../../../../src/ginput/ginput_driver_mouse.h"

// Get the hardware interface
#include "gmouse_lld_ADS7843_board.h"
#include "board_ILI9341.h"

#define CMD_X				0xD1
#define CMD_Y				0x91
#define CMD_ENABLE_IRQ		0x80


static const uint8_t CTRL_LO_DFR = 0b0011;
static const uint8_t CTRL_LO_SER = 0b0100;
static const uint8_t CTRL_HI_Y = 0b1001 << 4;
static const uint8_t CTRL_HI_X = 0b1101 << 4;


#define MODE_DFR           1


uint16_t _readLoop(uint8_t ctrl)
{
    uint16_t prev = 0xffff, cur = 0xffff;
    uint8_t i = 0;
    do
    {
        prev = cur;
        cur = read_data(0);
        cur = (cur << 4) | (read_data(ctrl) >> 4);  // 16 clocks -> 12-bits (zero-padded at end)
    }
    while ((prev != cur) && (++i < 0xff));

    return cur;
}


static bool_t MouseXYZ(GMouse* m, GMouseReading* pdr)
{
	(void)m;

	// No buttons
	pdr->buttons = 0;
	pdr->z = 0;
	
	if (display_pressed())
    {
		pdr->z = 1;						// Set to Z_MAX as we are pressed
#if 0
        acquire_bus(TOUCH_CS_PIN);
		
        read_data(CMD_X);				// Dummy read - disable PenIRQ
		pdr->x = read_data(CMD_X);		// Read X-Value

        read_data(CMD_Y);				// Dummy read - disable PenIRQ
		pdr->y = read_data(CMD_Y);		// Read Y-Value

        read_data(CMD_ENABLE_IRQ);		// Enable IRQ*/

		release_bus(TOUCH_CS_PIN);
#else
        uint8_t ctrl_lo = (MODE_DFR ? CTRL_LO_DFR : CTRL_LO_SER);

        acquire_bus(TOUCH_CS_PIN);
        read_data(CTRL_HI_X | ctrl_lo);  // Send first control byte
        pdr->x = _readLoop(CTRL_HI_X | ctrl_lo);
        pdr->y = _readLoop(CTRL_HI_Y | ctrl_lo);

        if (MODE_DFR) {
            // Turn off ADC by issuing one more read (throwaway)
            // This needs to be done, because PD=0b11 (needed for MODE_DFR) will disable PENIRQ
            read_data(0);  // Maintain 16-clocks/conversion; _readLoop always ends after issuing a control byte
            read_data(CTRL_HI_Y | CTRL_LO_SER);
        }
        read_data16(0);  // Flush last read, just to be sure

        release_bus(TOUCH_CS_PIN);
#endif
        //serial_printf("Touched: %d %d", pdr->x, pdr->y);
	}
	return TRUE;
}


static bool_t init_mouse(GMouse *m, unsigned driverinstance)
{
    //init_board();
}

const GMouseVMT const GMOUSE_DRIVER_VMT[1] = {{
	{
		GDRIVER_TYPE_TOUCH,
		GMOUSE_VFLG_TOUCH | GMOUSE_VFLG_CALIBRATE | GMOUSE_VFLG_CAL_TEST |
			GMOUSE_VFLG_ONLY_DOWN | GMOUSE_VFLG_POORUPDOWN,
		sizeof(GMouse)+GMOUSE_ADS7843_BOARD_DATA_SIZE,
		_gmouseInitDriver,
		_gmousePostInitDriver,
		_gmouseDeInitDriver
	},
	1,				// z_max - (currently?) not supported
	0,				// z_min - (currently?) not supported
	1,				// z_touchon
	0,				// z_touchoff
	{				// pen_jitter
		GMOUSE_ADS7843_PEN_CALIBRATE_ERROR,			// calibrate
		GMOUSE_ADS7843_PEN_CLICK_ERROR,				// click
		GMOUSE_ADS7843_PEN_MOVE_ERROR				// move
	},
	{				// finger_jitter
		GMOUSE_ADS7843_FINGER_CALIBRATE_ERROR,		// calibrate
		GMOUSE_ADS7843_FINGER_CLICK_ERROR,			// click
		GMOUSE_ADS7843_FINGER_MOVE_ERROR			// move
	},
    init_mouse, 	// init
	0,				// deinit
	MouseXYZ,		// get
	0,				// calsave
	0				// calload
}};

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */

