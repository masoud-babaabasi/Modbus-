#ifndef __TCP_MODEBUS__
#define __TCP_MODEBUS__
#include <stdint.h>
#include "application.h"

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

extern uint8_t tcp_modbus_ip[4];
typedef struct {
	uint8_t IP[4];
	uint8_t SUBNET[4];
	uint8_t GATEWAY[4];
} network_HANDLE;

int TCP_MODBUS_init(uint8_t ip_1, uint8_t ip_2, uint8_t ip_3, uint8_t ip_4);
int TCP_MODBUS_read_coils(uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len);
int TCP_MODBUS_read_discrete_inputs(uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len);
int TCP_MODBUS_read_holding_registers(uint16_t starting_address, uint16_t number_of_points, uint16_t* response_data, uint8_t* response_len, uint8_t change_high_low_flag);
int TCP_MODBUS_read_input_registers(uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len, uint8_t change_high_low_flag);

int TCP_MODBUS_write_single_coil(uint16_t starting_address, uint16_t presetdata);
int TCP_MODBUS_write_single_register(uint16_t starting_address, uint16_t presetdata);
int TCP_MODBUS_write_multiple_registers(uint16_t starting_address, uint16_t number_of_registers, uint8_t bytes_count, uint16_t *data, uint8_t change_high_low_flag);

#endif
/*************************** End of file ****************************/
