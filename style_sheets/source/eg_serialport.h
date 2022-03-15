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



typedef struct
{
	ecs_string_t name;
	ecs_i32_t buadrate;
	EgSpParity parity;
	ecs_i32_t bits;
} EgSerialPort;


extern ECS_COMPONENT_DECLARE(EgSerialPort);
extern ECS_COMPONENT_DECLARE(EgSpParity);


void FlecsComponentsEgSerialPortImport(ecs_world_t *world);

void eg_serialport_update(ecs_world_t *world);
