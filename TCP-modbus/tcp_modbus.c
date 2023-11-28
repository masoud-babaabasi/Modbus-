/***************************************
*	file : tcp_modbus.c
*	author : Masoud Babaabasi
*	Date : february 2023
*
*
****************************************/
#include "tcp_modbus.h"

uint8_t tcp_modbus_ip[4];
static uint16_t trans_id = 0;

/*
* @brief:	Implement this functions according to your project hardware.
* 			Provide the library with read , write and initialize TCP hardware.
*/
/*
*	@brief:	typedef struct {
*				uint8_t IP[4];
*				uint8_t SUBNET[4];
*				uint8_t GATEWAY[4];
*			} network_HANDLE;
*	@param: pointer to network_HANDLE variable
*	@return: 0 on success
*/
extern int initialize_ethernet(network_HANDLE *device);
/*
*	@brief:	write a buffer to TCP socket.
*	@param: pointer to buffer array
*	@param: number of bytes to write
*	@return: number of bytes actualy written
*/
extern int write_ethernet(uint8_t *buff, uint32_t numBytestoWrite);
/*
*	@brief:	read data from TCP socket.
*	@param: pointer to buffer array
*	@param: number of bytes to read
*	@return: number of bytes actualy read
*/
extern int read_ethernet(uint8_t *buf, uint32_t numBytestoRead);
/*
*	@brief:	close TCP socket and terminate connection
*	@return: 0 on success
*/
extern int Deinitialize_ethernet();

/*
*	@brief: change the MSB and LSB of a 16-bit data
*	@param: input 16-bit data
*	@return: changed MSB and LSB data
*/
static uint16_t change_high_low(uint16_t a) {
	uint8_t h;
	h = a >> 8;
	//a = a << 8 | (uint16_t)h;
	return (a << 8 | (uint16_t)h);
}
int TCP_MODBUS_init(uint8_t ip_1, uint8_t ip_2, uint8_t ip_3, uint8_t ip_4) {
	memset(tcp_modbus_ip, 0, 4);
	device_network.IP[0] = ip_1;
	device_network.IP[1] = ip_2;
	device_network.IP[2] = ip_3;
	device_network.IP[3] = ip_4;
	if (initialize_ethernet(&device_network) == 0) {
		tcp_modbus_ip[0] = ip_1;
		tcp_modbus_ip[1] = ip_2;
		tcp_modbus_ip[2] = ip_3;
		tcp_modbus_ip[3] = ip_4;
		return 0;
	}
	return -1;
}
/*
*@brief : Universal function for reading the input from the slave
* @param : modbus read function
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_read_function(uint8_t function, uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len) {
	uint8_t data_transfer[12];
	uint8_t L;
	uint16_t temp;
	*response_len = 0;
	trans_id++;
	data_transfer[0] = (uint8_t)(trans_id >> 8);
	data_transfer[1] = (uint8_t)(trans_id & 0x00ff);//transaction id
	data_transfer[2] = 0;// modbus
	data_transfer[3] = 0;//modbus
	data_transfer[4] = 0;//number of points
	data_transfer[5] = 0x06; // send pakcegs are always 12bytes and this section is always 6
	data_transfer[6] = 5; // unit identifier
	data_transfer[7] = function;

	data_transfer[8] = (uint8_t)(starting_address >> 8);
	data_transfer[9] = (uint8_t)(starting_address & 0x00ff);

	data_transfer[10] = (uint8_t)(number_of_points >> 8);
	data_transfer[11] = (uint8_t)(number_of_points & 0x00ff);

	write_ethernet(data_transfer, 12);
	do{
	L = read_ethernet(data_transfer, 9);
	if (L != 9) {
		return -1;
	}
	L = data_transfer[8];
	L = read_ethernet(response_data, L);

	temp = (uint16_t)data_transfer[1] | (uint16_t)data_transfer[0] << 8;
	}while( temp < trans_id );
	if (temp != trans_id) {
		memset(response_data , 0 , number_of_points * 2);
		return -1;
	}
	if (data_transfer[7] != function) {
		memset(response_data , 0 , number_of_points * 2);
		return -1;
	}
	if ( L != data_transfer[8] ){
		memset(response_data , 0 , number_of_points * 2);
		return -1;
	}
	*response_len = L;
	return 0;
}
/*
* @brief : modbus read coil status Function 0x01
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_read_coils(uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len) {
	return TCP_MODBUS_read_function(MB_FUNC_READ_COILS, starting_address, number_of_points, response_data, response_len);
}

/*
* @brief : modbus read input status Function 0x02
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_read_discrete_inputs(uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len) {
	return TCP_MODBUS_read_function(MB_FUNC_READ_DISCRETE_INPUTS, starting_address, number_of_points, response_data, response_len);
}
/*
* @brief : modbus read Holding Register Function 0x03
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @param : change high and low bytes of 16-bit data
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_read_holding_registers(uint16_t starting_address, uint16_t number_of_points, uint16_t* response_data, uint8_t* response_len, uint8_t change_high_low_flag) {
	uint8_t L;
	int ret_val = TCP_MODBUS_read_function(MB_FUNC_READ_HOLDING_REGISTER, starting_address, number_of_points, (uint8_t*)response_data, &L);
	*response_len = L / 2;
	if( ret_val == 0){
		if (change_high_low_flag) {
			for (int i = 0; i < (L / 2); i++) {
				response_data[i] = change_high_low(response_data[i]);
				//change_high_low(response_data[i]);
			}
		}
	}
	return ret_val;
}

/*
* @brief : modbus read Input register 0x04
* @param : coil staring address
* @param : number of coils to read
* @param : read data from slave
* @param : lenght of data array
* @param : change high and low bytes of 16-bit data
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_read_input_registers(uint16_t starting_address, uint16_t number_of_points, uint8_t* response_data, uint8_t* response_len, uint8_t change_high_low_flag) {
	uint8_t L;
	uint16_t *data_16 = (uint16_t*)response_data;
	int ret_val = TCP_MODBUS_read_function(MB_FUNC_READ_INPUT_REGISTER, starting_address, number_of_points, response_data, &L);
	if (change_high_low_flag) {
		for (int i = 0; i < (L / 2); i++) {
			data_16[i] = change_high_low(data_16[i]);
			//change_high_low(data_16[i]);
		}
	}
	*response_len = L;
	return ret_val;
}

/*
* @brief : universal function for writing single data to slave
* @param : modbus function
* @param : coil staring address
* @param : preset data
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_write_single_function(uint8_t function, uint16_t starting_address, uint16_t presetdata) {
	uint8_t data_transfer[12];
	uint16_t add, data;
	trans_id++;
	data_transfer[0] = (uint8_t)(trans_id >> 8);
	data_transfer[1] = (uint8_t)(trans_id & 0x00ff);//transaction id
	data_transfer[2] = 0;// modbus
	data_transfer[3] = 0;//modbus
	data_transfer[4] = 0;//number of points
	data_transfer[5] = 0x06; // send pakcegs are always 12bytes and this section is always 6
	data_transfer[6] = 1; // unit identifier
	data_transfer[7] = function;

	data_transfer[8] = (uint8_t)(starting_address >> 8);
	data_transfer[9] = (uint8_t)(starting_address & 0x00ff);

	data_transfer[10] = (uint8_t)(presetdata >> 8);
	data_transfer[11] = (uint8_t)(presetdata & 0x00ff);

	write_ethernet(data_transfer, 12);

	read_ethernet(data_transfer, 12);
	uint16_t temp = (uint16_t)data_transfer[1] | (uint16_t)data_transfer[0] << 8;
	if (temp != trans_id) return -1; //fail
	if (data_transfer[7] != function) return -1; //fail
	return 0;
}
/*
* @brief : modbus Focre single coil 0x05
* @param : coil staring address
* @param : preset data
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_write_single_coil(uint16_t starting_address, uint16_t presetdata) {
	uint8_t data, len;
	TCP_MODBUS_read_coils(starting_address, 1, &data, &len);
	if (((data & 0x01) && presetdata == 0xFF00) || (data == 0) && presetdata == 0x0000) return 0;
	return TCP_MODBUS_write_single_function(MB_FUNC_WRITE_SINGLE_COIL, starting_address, presetdata);
}
/*
* @brief : modbus Focre single register 0x06
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : preset data
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_write_single_register( uint16_t starting_address, uint16_t presetdata) {

	return TCP_MODBUS_write_single_function(MB_FUNC_WRITE_REGISTER, starting_address, presetdata);
}

/*
* @brief : modbus preset multiple registers 0x10
* @param : pointer to handle that controls the communication bus( COM port)
* @param : modbus slave address
* @param : coil staring address
* @param : preset data
* @param : change high and low bytes of 16-bit data
* @ret	 : success(0) or fail response
*/
int TCP_MODBUS_write_multiple_registers(uint16_t starting_address, uint16_t number_of_registers, uint8_t bytes_count, uint16_t *data, uint8_t change_high_low_flag) {
	uint8_t data_transfer[13];
	uint16_t add, data_in;
	if (bytes_count != (number_of_registers * 2)) return -1;
	if (change_high_low_flag) {
		for (uint8_t i = 0; i < number_of_registers; i++) {
			data[i] = change_high_low(data[i]);
			//change_high_low(data[i]);
		}
	}
	for (uint8_t i = 0; i < number_of_registers; i++) {
		if (i == 6) continue;
		if (TCP_MODBUS_write_single_register(starting_address, data[i]) != 0)
			break;
		starting_address++;
		if (change_high_low_flag){
			data[i] = change_high_low(data[i]);
			//change_high_low(data[i]);
		}
	}
	return 0;
	/*
	uint16_t n_points = bytes_count + 7;
	trans_id++;
	data_transfer[0] = (uint8_t)(trans_id >> 8);
	data_transfer[1] = (uint8_t)(trans_id & 0x00ff);//transaction id
	data_transfer[2] = 0;// modbus
	data_transfer[3] = 0;//modbus
	data_transfer[4] = (uint8_t)(n_points >> 8);
	data_transfer[5] = (uint8_t)(n_points | 0x00ff);
	data_transfer[6] = 1; // unit identifier

	data_transfer[7] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;

	data_transfer[8] = (uint8_t)(starting_address >> 8);
	data_transfer[9] = (uint8_t)(starting_address & 0x00ff);

	data_transfer[10] = (uint8_t)(number_of_registers >> 8);
	data_transfer[11] = (uint8_t)(number_of_registers & 0x00ff);

	data_transfer[12] = bytes_count;
	int ret_val;
	if( write_ethernet(data_transfer, 13) == 13) ret_val = write_ethernet((uint8_t*)data, bytes_count);
	else return -1;
	read_ethernet(data_transfer, 12);
	if (change_high_low_flag) {
		for (uint8_t i = 0; i < number_of_registers; i++) {
			data[i] = change_high_low(data[i]);
		}
	}
	uint16_t temp = (uint16_t)data_transfer[1] | (uint16_t)data_transfer[0] << 8;
	if (temp != trans_id) return -1; //fail
	if (data_transfer[7] != MB_FUNC_WRITE_MULTIPLE_REGISTERS) return -1; //fail

	add = ((uint16_t)data_transfer[8] << 8) | data_transfer[9];
	if (starting_address != add) return -1;

	data_in = ((uint16_t)data_transfer[10] << 8) | data_transfer[11];
	if (number_of_registers != data_in) return -1;
	return 0;
	*/
}
/*************************** End of file ****************************/
