// A flecs wrapper over libserialport.
#pragma once
#include "flecs.h"

void Module_EgLibSerialPortImport(ecs_world_t *world);

void eg_serialport_update(ecs_world_t *world);
