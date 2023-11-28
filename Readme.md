# Modbus C library
Since a lot of industry devices use Modbus standards for communication, it is often the case that we need to incorporate this protocol in our projects. So I have written two separate libraries for Modbus RTU and TCP modes. For both of these libraries, the user should provide the necessary functions for reading from and writing on the communication bus and all the code is independent from the platform used. The functions implemented for Modbus protocols are :
| Function code |   Function action     |
|---------------|-----------------------|
|   0x01        |   Read coils          |
|   0x02        |  Read discrete inputs |
|   0x03        | Read holding register |
|   0x04        | Read input register   |
|   0x05        |   Write single coil   |
|   0x06        |   Write register      |
|   0x10        |Write multiple register|

All of the library functions are named accordingly. All functions return 0 on successful execution. Read functions have an argument pointer, pointing to a buffer to store received data; also an `uint8_t *` argument called `response_len` in which the length of the received data is stored. 

## MODBUS RTU
In the `modbus.h` file, a structure is defined as `MODBUS_HandleTypeDef` which contains a function pointer for bus communication. The user should make an instance of this structure in the project and fill it with proper function pointers. All of the Modbus functions need a pointer to this structure to work properly.
## MODBUS TCP
This library is written a little differently; the user must define the network communication function with the prototypes and the exact same names below in the project:
```C
int initialize_ethernet(network_HANDLE *device);
int write_ethernet(uint8_t *buff, uint32_t numBytestoWrite);
int read_ethernet(uint8_t *buf, uint32_t numBytestoRead);
int Deinitialize_ethernet();
```
`network_HANDLE` structure is defined in the `tcp_modbus.h` file and contains TCP address information. These functions are defined as external functions in the `tcp_modbus.c` file.