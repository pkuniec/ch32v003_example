#pragma once

#include <inttypes.h>

#define mbReceiving			0x01
#define mbGap				0x02
#define mbParseFrame		0x04
#define mbError				0x08
#define mbPrepareReq		0x10
#define mbSendRequest		0x20
#define mbReset				0x40

#define mbIllegalFunction 	0x01
#define mbIllegal_data_addr	0x02
#define mbIllegal_data_val 	0x03


#define mbT15	15
#define mbT35	35

#define buff_size   12
#define mbFuncCount	4

typedef struct {
	uint8_t func_num;
	void (*func_name)(void);
} T_modbusfunc;


typedef struct {
	volatile uint8_t BusState;
	uint8_t Addres;
	uint8_t timer;
    uint8_t buff[buff_size];
    uint8_t idx;
	T_modbusfunc funcs[mbFuncCount];
} T_modbus;


void modbusInit(void);
void modbusSendException(uint8_t exceptionCode);
void modbusSetAddres(uint8_t addr);
void modbusTickTimer(void);
void modbusReset(void);
void modbus_event(void);
void modbusResponse(void);
int8_t modbus_putdata(uint8_t data);

uint8_t modbusGetBusState(void);
uint16_t crc16_update(uint16_t crc, uint8_t a);
uint16_t modbusCRC(uint8_t len);

int8_t mbRegisterFunc(uint8_t num, void (*func_name)(void) );
int8_t mbUnregisterFunc(uint8_t num);

void modbusFunc01(void);
void modbusFunc05(void);
void modbusFunc03(void);
void modbusFunc06(void);