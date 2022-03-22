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
	EG_SP_STATUS_ERROR = 1,
	EG_SP_STATUS_ERROR_OPEN = 2,
	EG_SP_STATUS_CLOSING = 3,
	EG_SP_STATUS_CLOSED = 4,
	EG_SP_STATUS_OPEN = 5,
	EG_SP_STATUS_OPENING = 6,
} EgSpStatus;

static char const * EgSpStatus_tostr(EgSpStatus status)
{
	switch (status)
	{
	case EG_SP_STATUS_UNDEFINED: return "UNDEFINED";
	case EG_SP_STATUS_ERROR: return "ERROR";
	case EG_SP_STATUS_ERROR_OPEN: return "ERROR_OPEN";
	case EG_SP_STATUS_CLOSING: return "CLOSING";
	case EG_SP_STATUS_CLOSED: return "CLOSED";
	case EG_SP_STATUS_OPEN: return "OPEN";
	case EG_SP_STATUS_OPENING: return "OPENING";
	default: return "";
	}
}


typedef struct
{
	ecs_string_t name;
	ecs_string_t description;
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


