#pragma once
#include "flecs.h"


typedef struct
{
	ecs_string_t name;
	ecs_i32_t buadrate;
} EgSerialPort;


extern ECS_COMPONENT_DECLARE(EgSerialPort);


void FlecsComponentsEgSerialPortImport(ecs_world_t *world);

void eg_serialport_update(ecs_world_t *world);
