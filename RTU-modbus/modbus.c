/*************************************************************************
 *	file : modbus.c
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

#include "modbus.h"
#include "stdio.h"
#include "string.h"

static uint16_t change_high_low(uint16_t a) {
	uint8_t h;
	h = a >> 8;
	return (a << 8 | (uint16_t)h);
}
/*
 * @brief : Universal function for reading the input from the slave
 * @param : pointer to handle that controls the communication bus( COM port), defined in modbus.h
 * @param : modbus read function
 * @param : modbus slave address
 * @param : coil staring address
 * @param : number of coils to read
 * @param : read data from slave
 * @param : lenght of data array
 * @ret	 : success(0) or fail response
 */
int MODBUS_read_function(MODBUS_HandleTypeDef* bus,uint8_t function ,uint8_t slave_address, uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len) {
	uint8_t data_transfer[10];
	uint16_t CRC16, CRC16_read;
	uint8_t L;
	uint32_t start_time;
	*response_len = 0;
	data_transfer[0] = slave_address;
	data_transfer[1] = function; 
	data_transfer[2] = (uint8_t)(starting_address >> 8);
	data_transfer[3] = (uint8_t)(starting_address & 0x00ff);
	data_transfer[4] = (uint8_t)(number_of_points >> 8);
	data_transfer[5] = (uint8_t)(number_of_points & 0x00ff);
	CRC16 = usMBCRC16(data_transfer, 6 , 0xff , 0xff);
	data_transfer[6] = (uint8_t)(CRC16 & 0x00ff); // CRC16 low byte first
	data_transfer[7] = (uint8_t)(CRC16 >> 8);

	bus->COM_write(data_transfer, 8, bus->response_timeout);

	bus->COM_read(data_transfer, 3, bus->response_timeout);
	if (data_transfer[0] != slave_address) return - 1; //fail
	if (data_transfer[1] != function) return -1; //fail

	L = data_transfer[2];
	uint16_t read_bytes = 0 ;
	#ifndef WIN32
	start_time = HAL_GetTick();
	#else
	start_time = 0;
	#endif
	do{
		read_bytes += bus->COM_read(response_data + read_bytes, L - read_bytes , bus->response_timeout);
		#ifndef WIN32
		if( HAL_GetTick() - start_time > 350) return -1;
		#else
		if( ++start_time >= 3) return -1;
		#endif
	}
	while(  read_bytes < L );
	//CRC16_read = ((uint16_t)(response_data[L + 1]) << 8 ) | response_data[ L ];
	bus->COM_read(&data_transfer[3], 2, bus->response_timeout);
	CRC16_read = ((uint16_t)(data_transfer[4]) << 8) | data_transfer[3];
	CRC16 = usMBCRC16(data_transfer, 3 , 0xff , 0xff);
	CRC16 = usMBCRC16(response_data, L ,CRC16 >> 8, CRC16 & 0xff);
	if (CRC16 != CRC16_read) return -1;
	*response_len = L;
	return 0;
}
/*
* @brief : modbus read coil status Function 0x01
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @ret	 : success(0) or fail response
*/
int MODBUS_read_coils(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len) {
	return MODBUS_read_function(bus , MB_FUNC_READ_COILS , slave_address , starting_address , number_of_points , response_data , response_len);
}

/*
* @brief : modbus read input status Function 0x02
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @ret	 : success(0) or fail response
*/
int MODBUS_read_discrete_inputs(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len){
	return MODBUS_read_function(bus , MB_FUNC_READ_DISCRETE_INPUTS , slave_address , starting_address , number_of_points , response_data , response_len);
}
/*
* @brief : modbus read Holding Register Function 0x03
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @ret	 : success(0) or fail response
*/
int MODBUS_read_holding_registers(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_points, uint16_t* response_data, uint8_t* response_len , uint8_t change_high_low_flag){
	uint8_t L;
	int ret_val = MODBUS_read_function(bus , MB_FUNC_READ_HOLDING_REGISTER , slave_address , starting_address , number_of_points , (uint8_t*)response_data , &L);
	*response_len = L / 2;
	if (change_high_low_flag) {
		for (int i = 0; i < (L / 2); i++) {
			response_data[i] = change_high_low(response_data[i]);
			//change_high_low(response_data[i]);
		}
	}
	return ret_val;
}

/*
* @brief : modbus read Input register 0x04
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @ret	 : success(0) or fail response
*/
int MODBUS_read_input_registers(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len , uint8_t change_high_low_flag){
	uint8_t L;
	uint16_t *data_16 = (uint16_t*)response_data;
	int ret_val = MODBUS_read_function(bus , MB_FUNC_READ_INPUT_REGISTER , slave_address , starting_address , number_of_points , response_data , &L);
	if (change_high_low_flag) {
		for (int i = 0; i < (L / 2) ; i++) {
			data_16[i] = change_high_low(data_16[i]);
			//change_high_low(data_16[i]);
		}
	}
	*response_len = L ;
	return ret_val;
}


/*
* @brief : universal function for writing single data to slave
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus function
* @param : modbus slave address
* @param : coil staring address
* @param : preset data
* @ret	 : success(0) or fail response
*/
int MODBUS_write_single_function(MODBUS_HandleTypeDef* bus, uint8_t function , uint8_t slave_address, uint16_t starting_address , uint16_t presetdata){
	uint8_t data_transfer[10];
	uint16_t CRC16, CRC16_read;
	//uint8_t L;
	uint16_t add,data;
	data_transfer[0] = slave_address;
	data_transfer[1] = function;
	data_transfer[2] = (uint8_t)(starting_address >> 8);
	data_transfer[3] = (uint8_t)(starting_address & 0x00ff);
	data_transfer[4] = (uint8_t)(presetdata >> 8);
	data_transfer[5] = (uint8_t)(presetdata & 0x00ff);
	CRC16 = usMBCRC16(data_transfer, 6 , 0xff , 0xff);
	data_transfer[6] = (uint8_t)(CRC16 & 0x00ff); // CRC16 low byte first
	data_transfer[7] = (uint8_t)(CRC16 >> 8);

	bus->COM_write(data_transfer, 8, bus->response_timeout);

	bus->COM_read(data_transfer, 8, bus->response_timeout);
	if (data_transfer[0] != slave_address) return - 1; //fail
	
	if (data_transfer[1] != function) return data_transfer[2]; //retrun exeption code
	
	add = ((uint16_t)data_transfer[2] << 8 ) | data_transfer[ 3 ];
	if (slave_address != add) return -1;
	
	data = ((uint16_t)data_transfer[4] << 8 ) | data_transfer[ 5 ];
	if (presetdata != data) return -1;
	
	CRC16_read = ((uint16_t)data_transfer[7] << 8 ) | data_transfer[ 6 ];
	CRC16 = usMBCRC16(data_transfer, 6 , 0xff , 0xff);
	if (CRC16 != CRC16_read) return -1;
	
	return 0;
}
/*
* @brief : modbus Focre single coil 0x05
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : preset data
* @ret	 : success(0) or fail response
*/
int MODBUS_write_single_coil(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address , uint16_t presetdata){
	
	return MODBUS_write_single_function(bus , MB_FUNC_WRITE_SINGLE_COIL , slave_address , starting_address , presetdata);
}
/*
* @brief : modbus Focre single register 0x06
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : preset data
* @ret	 : success(0) or fail response
*/
int MODBUS_write_single_register(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address , uint16_t presetdata){
	
	return MODBUS_write_single_function(bus , MB_FUNC_WRITE_REGISTER , slave_address , starting_address , presetdata);
}

/*
* @brief : modbus preset multiple registers 0x10
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : preset data
* @ret	 : success(0) or fail response
*/
int MODBUS_write_multiple_registers(MODBUS_HandleTypeDef* bus, uint8_t slave_address, uint16_t starting_address, uint16_t number_of_registers ,uint8_t bytes_count , uint16_t *data , uint8_t change_high_low_flag){
	uint8_t data_transfer[10];
	uint16_t CRC16, CRC16_read;
	//uint8_t L;
	uint16_t add, data_in;
	if (bytes_count != (number_of_registers * 2)) return -1;
	if (change_high_low_flag) {
		for (uint8_t i = 0; i < number_of_registers; i++) {
			data[i] = change_high_low(data[i]);
			 //change_high_low(data[i]);
		}
	}
	data_transfer[0] = slave_address;
	data_transfer[1] = 0x10;
	data_transfer[2] = (uint8_t)(starting_address >> 8);
	data_transfer[3] = (uint8_t)(starting_address & 0x00ff);
	data_transfer[4] = (uint8_t)(number_of_registers >> 8);
	data_transfer[5] = (uint8_t)(number_of_registers & 0x00ff);
	data_transfer[6] = bytes_count;
	CRC16 = usMBCRC16(data_transfer, 7, 0xff, 0xff);
	CRC16 = usMBCRC16(data, bytes_count, (uint8_t)(CRC16 >> 8), (uint8_t)(CRC16 & 0x00ff));
	bus->COM_write(data_transfer, 7, bus->response_timeout);
	bus->COM_write(data, bytes_count, bus->response_timeout);
	data_transfer[0] = (uint8_t)(CRC16 & 0x00ff); // CRC16 low byte first
	data_transfer[1] = (uint8_t)(CRC16 >> 8);
	bus->COM_write(data_transfer, 2, bus->response_timeout);

	bus->COM_read(data_transfer, 8, bus->response_timeout);

	if (change_high_low_flag) {
		for (uint8_t i = 0; i < number_of_registers; i++) {
			data[i] = change_high_low(data[i]);
			 //change_high_low(data[i]);
		}
	}
	if (data_transfer[0] != slave_address) return -1; //fail

	if (data_transfer[1] != 0x10) return data_transfer[2]; //retrun exception code

	add = ((uint16_t)data_transfer[2] << 8) | data_transfer[3];
	if (starting_address != add) return -1;

	data_in = ((uint16_t)data_transfer[4] << 8) | data_transfer[5];
	if (number_of_registers != data_in) return -1;

	CRC16 = usMBCRC16(data_transfer, 6, 0xff, 0xff);
	CRC16_read = ((uint16_t)data_transfer[7] << 8) | data_transfer[6];
	CRC16 = usMBCRC16(data_transfer, 6, 0xff, 0xff);
	if (CRC16 != CRC16_read) return -1;

	return 0;
}
/*************************** End of file ****************************/
