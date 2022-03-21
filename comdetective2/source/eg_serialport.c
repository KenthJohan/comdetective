#include "flecs.h"
#include "eg_serialport.h"
#include <stdio.h>
#include <stdlib.h>


ECS_COMPONENT_DECLARE(EgSerialPortSingleton);
ECS_COMPONENT_DECLARE(EgSerialPort);
ECS_COMPONENT_DECLARE(EgSpParity);
ECS_COMPONENT_DECLARE(EgSpStatus);



void EgSerialportImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgSerialport);
	ecs_set_name_prefix(world, "Eg");

	ECS_COMPONENT_DEFINE(world, EgSerialPortSingleton);
	ECS_COMPONENT_DEFINE(world, EgSerialPort);
	ECS_COMPONENT_DEFINE(world, EgSpParity);
	ECS_COMPONENT_DEFINE(world, EgSpStatus);


	ecs_enum_init(world, &(ecs_enum_desc_t) {
	.entity.entity = ecs_id(EgSpParity), // Make sure to use existing id
	.constants = {
	{ .name = "INVALID", .value = EG_SP_PARITY_INVALID },
	{ .name = "NONE", .value = EG_SP_PARITY_NONE },
	{ .name = "ODD", .value = EG_SP_PARITY_ODD },
	{ .name = "EVEN", .value = EG_SP_PARITY_EVEN },
	{ .name = "MARK", .value = EG_SP_PARITY_MARK },
	{ .name = "SPACE", .value = EG_SP_PARITY_SPACE },
	}
	});

	ecs_enum_init(world, &(ecs_enum_desc_t) {
	.entity.entity = ecs_id(EgSpStatus), // Make sure to use existing id
	.constants = {
	{ .name = "UNDEFINED", .value = EG_SP_PARITY_INVALID },
	{ .name = "CLOSE", .value = EG_SP_PARITY_NONE },
	{ .name = "OPEN", .value = EG_SP_PARITY_ODD },
	{ .name = "ERROR", .value = EG_SP_PARITY_EVEN }
	}
	});

	ecs_struct_init(world, &(ecs_struct_desc_t) {
	.entity.entity = ecs_id(EgSerialPort),
	.members = {
	{
	.name = "name",
	.type = ecs_id(ecs_string_t)
	},
	{
	.name = "usb_vid",
	.type = ecs_id(ecs_i32_t)
	},
	{
	.name = "usb_pid",
	.type = ecs_id(ecs_i32_t)
	},
	{
	.name = "usb_bus",
	.type = ecs_id(ecs_i32_t)
	},
	{
	.name = "usb_address",
	.type = ecs_id(ecs_i32_t)
	},
	{
	.name = "buadrate",
	.type = ecs_id(ecs_i32_t),
	.unit = EcsBitsPerSecond
	},
	{ .name = "parity", .type = ecs_id(EgSpParity) },
	{
	.name = "bits",
	.type = ecs_id(ecs_i32_t),
	.unit = EcsBits
	},
	{
	.name = "status",
	.type = ecs_id(EgSpStatus)
	},
	}});
}
