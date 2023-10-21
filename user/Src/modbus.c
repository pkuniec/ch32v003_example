#include "uart.h"
#include "modbus.h"
#include "common.h"

static T_modbus modbus;


void modbusSendException(uint8_t exceptionCode) {
	uint16_t crc;
	modbus.buff[1] |= 0x80;
	modbus.buff[2] = exceptionCode;
	crc = modbusCRC(3);
	modbus.buff[3] = (uint8_t)(crc & 0xFF);
	modbus.buff[4] = (uint8_t)(crc >> 8);
	modbus.idx = 5;

	modbusResponse();
}

uint8_t modbusGetBusState(void) {
	return modbus.BusState;
}

void modbusInit(void) {
	modbus.Addres = 0;
	uint8_t *ptr = (uint8_t*)modbus.funcs;
	uint8_t len = sizeof( T_modbusfunc )*mbFuncCount;

	while (len--) {
		*ptr++ = 0;
	}
}

void modbusSetAddres(uint8_t addr) {
	modbus.Addres = addr;
}

// Put reciverd data to modbus buffer
int8_t modbus_putdata(uint8_t data) {

    modbus.BusState |= mbReceiving;
    modbus.timer = 0;

    if ( modbus.idx < buff_size ) {
        modbus.buff[modbus.idx++] = data;
        return 0;
    } else {
        return -1;
    }
}

// Calculating CRC sum
uint16_t crc16_update(uint16_t crc, uint8_t a) {
	crc ^= (uint16_t)a;
	for (uint8_t i = 0; i < 8; ++i) {
		if (crc & 1)
			crc = (crc >> 1) ^ 0xA001;
		else
			crc = (crc >> 1);
	}

	return crc;
}

// Calculating CRC for frame
uint16_t modbusCRC(uint8_t len) {
	uint16_t crc = 0xFFFF;

	// wyliczenie i sprawdzenie CRC
	for(uint8_t i=0; i<len; i++) {
		crc = crc16_update( crc, modbus.buff[i] );
	}

	return crc;
}

// Reset modbus state
void modbusReset(void) {
	modbus.idx = 0;
	modbus.timer = 0;
	modbus.BusState = 0;
}


// Modbus timer to check gap in frame
void modbusTickTimer(void) {
	if ( modbus.BusState & mbReceiving ) {
		modbus.timer++;

		// Gap detect
		if ( modbus.timer > mbT15 ) {
			modbus.BusState |= mbGap;
		}

		// Frame end parse (clear reciving)
		if ( modbus.timer > mbT35 ) {
			modbus.BusState = mbParseFrame;
		}
	}

	// Liczenie czasu do wyslania odpowiedzi
	if ( modbus.BusState & mbPrepareReq ) {
		modbus.timer--;

		if ( !modbus.timer ) {
			modbus.BusState = mbSendRequest;
		}
	}
}


void modbus_event(void) {
	uint16_t crc;
    uint8_t func_num = mbFuncCount;
	uint8_t error = 1;

	// Parsing rcived data frame
	if ( modbus.BusState & mbParseFrame ) {
		
		// frame to short
		if (modbus.idx < 5) {
			modbus.BusState = mbReset;
			return;
		}

		// frame not for us
		if ( modbus.buff[0] != modbus.Addres ) {
			modbus.BusState = mbReset;
			return;
		}

		// check CRC
		crc = (uint16_t) ((modbus.buff[modbus.idx-1] << 8) | modbus.buff[modbus.idx-2]);
		if ( modbusCRC(modbus.idx-2) != crc ) {
			modbus.BusState = mbReset;
			return;
		}

		// Frame OK
		// func_num - value to counting down from numbers of registered functions
		while( func_num-- ) {
			if ( modbus.funcs[func_num].func_num == modbus.buff[1] ) {
				modbus.funcs[func_num].func_name();
				error = 0;
				break;
			}
		}

		if ( error ) {
			modbusSendException( mbIllegalFunction );
		}

		modbus.BusState &= ~mbParseFrame;
	}

	// Sending rensponse
	if ( modbus.BusState & mbSendRequest ) {
        uart_cp2txbuf((uint8_t *)modbus.buff, modbus.idx);
        modbus.idx = 0;
        modbus.BusState &= ~mbSendRequest;
	}

	// Reset Bus
	if ( modbus.BusState & mbReset ) {
		modbusReset();
	}

}

void modbusResponse(void) {
	modbus.BusState |= mbPrepareReq;
	modbus.timer = mbT35;
}

int8_t mbRegisterFunc(uint8_t num, void (*func_name)(void) ) {
	for(uint8_t i=0; i<mbFuncCount; i++) {
		if ( !modbus.funcs[i].func_num ) {
			modbus.funcs[i].func_num = num;
			modbus.funcs[i].func_name = func_name;
			return 0;
		}
	}
	return -1;
}

int8_t mbUnregisterFunc(uint8_t num) {
	for(uint8_t i=0; i<mbFuncCount; i++) {
		if ( modbus.funcs[i].func_num == num ) {
			modbus.funcs[i].func_num = 0;
			modbus.funcs[i].func_name = 0;
			return 0;
		}
	}
	return -1;
}


// -----------------------------
// Modbus Funcion implrmrntation
// -----------------------------

// Function 0x01 - Read Single Coli
void modbusFunc01(void) {
    uint8_t coil = modbus.buff[3];
    uint8_t count = modbus.buff[4];

    if (coil > 8 ) {
		modbusSendException( mbIllegal_data_addr );
	}

    modbus.buff[2] = 1; // data 1 byte

    uint8_t bufIndex = 3; // index in buffer to starting write data
    modbus.buff[bufIndex++] = 0x10;
    uint16_t crc = modbusCRC( bufIndex );
    modbus.buff[bufIndex++] = (uint8_t)(crc & 0xFF);
    modbus.buff[bufIndex++] = (uint8_t)(crc >> 8);
    modbus.idx = bufIndex++;

    modbusResponse();
}

// Function 0x05 - Write Single Coil
void modbusFunc05(void) {
	uint8_t coil = modbus.buff[3];

	if (coil > 8 ) {
		modbusSendException( mbIllegal_data_addr );
	}

    if ( 0xFF == modbus.buff[4] ) {
        output_set(coil, 1);
    } else {
        output_set(coil, 0);
    }

	modbusResponse();
}


// Function 0x03 - Read Holding Registers
void modbusFunc03(void) {
    uint8_t reg_num = modbus.buff[3];
    uint8_t reg_cnt = modbus.buff[5];
    uint8_t err_l = 0;
    uint8_t *sysreg = GetRegHandler();
    uint16_t crc = 0;

	modbus.buff[2] = reg_cnt * 2;

    if ( reg_num > 1 ) {
        err_l = 1;
        modbusSendException( mbIllegal_data_addr );
    }

    if ( reg_num + reg_cnt > 2 ) {
        err_l = 2;
        modbusSendException( mbIllegal_data_val );
    }

    if ( !err_l ) {
        err_l = 3; // index in buffer to starting write data
        while(reg_cnt--) {
            modbus.buff[err_l++] = 0;
            modbus.buff[err_l++] = *sysreg;
        }

        crc = modbusCRC( err_l );
        modbus.buff[err_l++] = (uint8_t)(crc & 0xFF);
        modbus.buff[err_l++] = (uint8_t)(crc >> 8);
        modbus.idx = err_l++;

		modbusResponse();
	}
}


// Func 0x06 - Write Single Register
void modbusFunc06(void) {
	uint8_t reg_num = modbus.buff[3];
	uint8_t reg0_val = modbus.buff[4];
	uint8_t reg1_val = modbus.buff[5];

	modbus.buff[2] = 0x00;

	if ( reg_num > 0 ) {
		modbusSendException( mbIllegal_data_addr );
	}

	modbusResponse();
}