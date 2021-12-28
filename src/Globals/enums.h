/**
 * @file enums.h
 * @author Talha Cetin (talhacetin96@hotmail.com)
 * @brief Global enum variables
 * @version 0.1
 * @date 2021-12-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#if !defined(ENUMS_H)
#define ENUMS_H

enum calismaModu
{
    HATA = 0,
    DIAGNOSTIC = 1,
    NORMAL = 2
};

enum girisStateMachine
{
    ERROR_STATE = 0,
    WiFi_SCAN_STATE = 1,
    WiFi_CONNECT_STATE = 2,
    TCP_CONNECT_STATE = 3,
    TCP_GET_MACHINEID_STATE = 4,
    EEPROM_MACHINEID_WRITE_STATE = 5,
    END_OF_GIRIS_STATE_MACHINE = 6
};

#endif // ENUMS_H
