#include <Arduino.h>
#include "gfx.h"
#include "DisplayUtils.h"


/**
* @brief   Load a set of mouse calibration data
* @return  A pointer to the data or NULL on failure
*
* @param[in] instance		The mouse input instance number
* @param[in] data			Where the data should be placed
* @param[in] sz    		The size in bytes of the data to retrieve.
*
* @note    This routine is provided by the user application. It is only
*          called if GINPUT_TOUCH_USER_CALIBRATION_LOAD has been set to TRUE in the
*          users gfxconf.h file.
*/
static const uint8_t calibration_data[] = { 0x3A, 0x3B, 0x05, 0xBE, 0x84, 0xC2, 0x22, 0xBB,
                                            0x3E, 0xE7, 0x81, 0x43, 0x8A, 0xE6, 0x40, 0xBB,
                                            0x93, 0xAF, 0x2D, 0x3E, 0x73, 0xEE, 0x27, 0xC1 };

bool_t LoadMouseCalibration(unsigned instance, void *data, size_t sz)
{
    assert(sizeof(calibration_data) == sz);
    memcpy(data, (void*)calibration_data, sz);
    return TRUE;
}

/**
* @brief   Save a set of mouse calibration data
* @return  TRUE if the save operation was successful.
*
* @param[in] instance		The mouse input instance number
* @param[in] data          The data to save
* @param[in] sz    		The size in bytes of the data to retrieve.
*
* @note    This routine is provided by the user application. It is only
*          called if GINPUT_TOUCH_USER_CALIBRATION_SAVE has been set to TRUE in the
*          users gfxconf.h file.
*/
#if GINPUT_TOUCH_USER_CALIBRATION_SAVE
bool_t SaveMouseCalibration(unsigned instance, const void *data, size_t sz)
{
    Serial.printf("\nCalibration data:\n");
    for (size_t i = 0; i < sz; ++i)
    {
        Serial.printf("0x%02X, ", ((uint8_t*)data)[i]);
    }
    Serial.printf("\n\n");

    return TRUE;
}
#endif
