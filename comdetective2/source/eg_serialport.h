// The implementation of serialport is not here.
// This will only contain component of serialport.
#pragma once
#include "flecs.h"


typedef enum {
	EG_SP_PARITY_INVALID = -1,
	EG_SP_PARITY_NONE = 0,
	EG_SP_PARITY_ODD = 1,
	EG_SP_PARITY_EVEN = 2,
	EG_SP_PARITY_MARK = 3,
	EG_SP_PARITY_SPACE = 4
} EgSpParity;

typedef enum {
	EG_SP_STATUS_UNDEFINED = 0,
	EG_SP_STATUS_OPEN_ERROR = 1,
	EG_SP_STATUS_CLOSING = 2,
	EG_SP_STATUS_CLOSED = 3,
	EG_SP_STATUS_OPEN = 4,
	EG_SP_STATUS_OPENING = 5,
} EgSpStatus;


typedef struct
{
	ecs_string_t name;
	ecs_i32_t usb_vid;
	ecs_i32_t usb_pid;
	ecs_i32_t usb_bus;
	ecs_i32_t usb_address;
	ecs_i32_t buadrate;
	EgSpParity parity;
	ecs_i32_t bits;
	EgSpStatus status;
	void * _internal;
} EgSerialPort;

typedef struct
{
	int dummy;
} EgSerialPortSingleton;





extern ECS_COMPONENT_DECLARE(EgSerialPortSingleton);
extern ECS_COMPONENT_DECLARE(EgSerialPort);
extern ECS_COMPONENT_DECLARE(EgSpParity);
extern ECS_COMPONENT_DECLARE(EgSpStatus);


void EgSerialportImport(ecs_world_t *world);


