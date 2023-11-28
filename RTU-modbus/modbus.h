/*************************************************************************
 *	file : modbus.h
 *	universal functions for comunication over the modbus protocol
 *	Author : Masoud Babaabasi
 *	Date : January 2023
 *
 *
 *
 *
 *
 *
 *************************************************************************
 */

#ifndef __MODBUS_H
#define __MODBUS_H

#include "mbcrc.h"
#include <stdint.h>

#define MB_ADDRESS_BROADCAST    ( 0 )   /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN          ( 1 )   /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX          ( 247 ) /*! Biggest possible slave address. */
#define MB_FUNC_NONE                          (  0 )
#define MB_FUNC_READ_COILS                    (  1 )
#define MB_FUNC_READ_DISCRETE_INPUTS          (  2 )
#define MB_FUNC_WRITE_SINGLE_COIL             (  5 )
#define MB_FUNC_WRITE_MULTIPLE_COILS          ( 15 )
#define MB_FUNC_READ_HOLDING_REGISTER         (  3 )
#define MB_FUNC_READ_INPUT_REGISTER           (  4 )
#define MB_FUNC_WRITE_REGISTER                (  6 )
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS      ( 16 )
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS  ( 23 )
#define MB_FUNC_DIAG_READ_EXCEPTION           (  7 )
#define MB_FUNC_DIAG_DIAGNOSTIC               (  8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT        ( 11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG        ( 12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID          ( 17 )
#define MB_FUNC_ERROR                         ( 128 )

typedef struct __MODEBUS_HandleTypeDef
{
	uint8_t response_timeout;

	uint32_t(*COM_initialize)(const char* _comport, int _baudrate, int timeout , int parity , int stop);
	uint32_t(*COM_read)(uint8_t* pBuf, uint16_t BytesToRead , uint16_t timout); // return number of bytes read
	uint32_t(*COM_write)(uint8_t* pBuff, uint16_t BytesToWrite,uint16_t timout); // returns number of bytes written
} MODBUS_HandleTypeDef;

int MODBUS_read_coils(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address , uint16_t number_of_points, uint8_t* response_data , uint8_t* response_len);
int MODBUS_read_discrete_inputs(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len);
int MODBUS_read_holding_registers(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_points, uint16_t* response_data, uint8_t* response_len , uint8_t change_high_low_flag);
int MODBUS_read_input_registers(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len , uint8_t change_high_low_flag);

int MODBUS_write_single_coil(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address , uint16_t presetdata);
int MODBUS_write_single_register(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t presetdata);
int MODBUS_write_multiple_registers(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_registers ,uint8_t bytes_count , uint16_t* data, uint8_t change_high_low_flag);

#endif
