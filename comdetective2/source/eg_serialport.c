#include "flecs.h"
#include "eg_serialport.h"
#include <stdio.h>
#include <stdlib.h>


ECS_COMPONENT_DECLARE(EgSerialPortSingleton);
ECS_COMPONENT_DECLARE(EgSerialPort);
ECS_COMPONENT_DECLARE(EgSpParity);
ECS_COMPONENT_DECLARE(EgSpStatus);
ECS_COMPONENT_DECLARE(EgSpTransport);



void EgSerialportImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgSerialport);
	ecs_set_name_prefix(world, "Eg");

	ECS_COMPONENT_DEFINE(world, EgSerialPortSingleton);
	ECS_COMPONENT_DEFINE(world, EgSerialPort);
	ECS_COMPONENT_DEFINE(world, EgSpParity);
	ECS_COMPONENT_DEFINE(world, EgSpStatus);
	ECS_COMPONENT_DEFINE(world, EgSpTransport);

	ecs_enum_init(world, &(ecs_enum_desc_t) {
	.entity.entity = ecs_id(EgSpTransport), // Make sure to use existing id
	.constants = {
	{ .name = "NATIVE", .value = EG_SP_TRANSPORT_NATIVE },
	{ .name = "USB", .value = EG_SP_TRANSPORT_USB },
	{ .name = "BLUETOOTH", .value = EG_SP_TRANSPORT_BLUETOOTH }
	}
	});

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
	{ .name = "UNDEFINED", .value = EG_SP_STATUS_UNDEFINED },
	{ .name = "ERROR", .value = EG_SP_STATUS_ERROR },
	{ .name = "OPEN_ERROR", .value = EG_SP_STATUS_ERROR_OPEN },
	{ .name = "ERROR_CLOSE", .value = EG_SP_STATUS_ERROR_CLOSE },
	{ .name = "ERROR_UPDATE", .value = EG_SP_STATUS_ERROR_UPDATE },
	{ .name = "CLOSING", .value = EG_SP_STATUS_CLOSE },
	{ .name = "CLOSED", .value = EG_SP_STATUS_CLOSED },
	{ .name = "OPEN", .value = EG_SP_STATUS_OPEN },
	{ .name = "OPENED", .value = EG_SP_STATUS_OPENED },
	{ .name = "UPDATE", .value = EG_SP_STATUS_UPDATE },
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
	.name = "description",
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
	.name = "usb_serial",
	.type = ecs_id(ecs_string_t)
	},
	{
	.name = "usb_product",
	.type = ecs_id(ecs_string_t)
	},
	{
	.name = "usb_manufacturer",
	.type = ecs_id(ecs_string_t)
	},
	{
	.name = "buadrate",
	.type = ecs_id(ecs_i32_t),
	.unit = EcsBitsPerSecond
	},
	{
	.name = "bluetooth_mac_address",
	.type = ecs_id(ecs_string_t)
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
	{
	.name = "_internal",
	.type = ecs_id(ecs_uptr_t)
	},
	}});
}
