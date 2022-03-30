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
	EG_SP_TRANSPORT_NATIVE = 0,
	EG_SP_TRANSPORT_USB = 1,
	EG_SP_TRANSPORT_BLUETOOTH = 2
} EgSpTransport;


typedef enum {
	// The next state can by any.
	EG_SP_STATUS_UNDEFINED,

	// This states that the port has a unknown error.
	EG_SP_STATUS_ERROR,

	// This states that the port got an error from opening a port.
	EG_SP_STATUS_ERROR_OPEN,

	// This states that the port got an error from closing a port.
	EG_SP_STATUS_ERROR_CLOSE,

	// This states that the port got an error from updating a port.
	EG_SP_STATUS_ERROR_UPDATE,

	// This action states that a flecs system should try to close the port.
	// The next state can be CLOSED or ERROR_CLOSE.
	EG_SP_STATUS_CLOSE,

	// This states that the port is closed.
	// The next state can by any.
	EG_SP_STATUS_CLOSED,

	// This action states that a flecs system should try to open the port.
	// The next state can be OPENED or ERROR_OPEN.
	EG_SP_STATUS_OPEN,

	// This states that the port is opening.
	// The next state can be any.
	EG_SP_STATUS_OPENED,

	// This action states that a flecs system should try to update the port using configration stored in flecs component.
	// The next state can be OPEN or ERROR_UPDATE.
	EG_SP_STATUS_UPDATE
} EgSpStatus;

static char const * EgSpStatus_tostr(EgSpStatus status)
{
	switch (status)
	{
	case EG_SP_STATUS_UNDEFINED: return "UNDEFINED";
	case EG_SP_STATUS_ERROR: return "ERROR";
	case EG_SP_STATUS_ERROR_OPEN: return "ERROR_OPEN";
	case EG_SP_STATUS_ERROR_CLOSE: return "ERROR_CLOSE";
	case EG_SP_STATUS_ERROR_UPDATE: return "ERROR_UPDATE";
	case EG_SP_STATUS_CLOSE: return "CLOSE";
	case EG_SP_STATUS_CLOSED: return "CLOSED";
	case EG_SP_STATUS_OPEN: return "OPEN";
	case EG_SP_STATUS_OPENED: return "OPENED";
	case EG_SP_STATUS_UPDATE: return "UPDATE";
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
	ecs_string_t usb_serial;
	ecs_string_t usb_product;
	ecs_string_t usb_manufacturer;
	ecs_i32_t buadrate;
	EgSpTransport transport;
	ecs_string_t bluetooth_mac_address;
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
extern ECS_COMPONENT_DECLARE(EgSpTransport);


void EgSerialportImport(ecs_world_t *world);


