#include "flecs.h"
#include "eg_serialport.h"
#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>


ECS_COMPONENT_DECLARE(EgSerialPort);
ECS_COMPONENT_DECLARE(EgSpParity);


#define SP_EXIT_ON_ERROR(r) sp_exit_on_error(r,__FILE__,__LINE__)
static void sp_exit_on_error (enum sp_return r, char const * file, int line)
{
	if (r < 0)
	{
		fprintf (stderr, "%s:%i: ", file, line);
		perror (sp_last_error_message ());
		exit (EXIT_FAILURE);
	}
}


void eg_serialport_update(ecs_world_t *world)
{
	struct sp_port ** port;
	enum sp_return r;
	r = sp_list_ports(&port);
	SP_EXIT_ON_ERROR(r);
	for (struct sp_port ** p = port; (*p) != NULL; ++p)
	{
		//char buf[100];
		//snprintf(buf, 100, "%10s : %s\n", sp_get_port_name(*p), sp_get_port_description(*p));
		char * name = sp_get_port_name(*p);

		ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
			.name = name
		});


		struct sp_port_config *config;
		int buadrate = -100;
		int bits;
		enum sp_parity parity;
		int usb_vid;
		int usb_pid;

		sp_get_port_usb_vid_pid(*p, &usb_vid, &usb_pid);


		r = sp_open(*p, SP_MODE_READ_WRITE);
		SP_EXIT_ON_ERROR(r);
		{
			r = sp_new_config(&config);
			{
				SP_EXIT_ON_ERROR(r);
				//r = sp_set_config_baudrate(config, 9600);
				//SP_EXIT_ON_ERROR(r);
				//r = sp_set_config(*p, config);
				SP_EXIT_ON_ERROR(r);
				r = sp_get_config(*p, config);
				SP_EXIT_ON_ERROR(r);
				r = sp_get_config_baudrate(config, &buadrate);
				SP_EXIT_ON_ERROR(r);
				r = sp_get_config_parity(config, &parity);
				SP_EXIT_ON_ERROR(r);
				r = sp_get_config_bits(config, &bits);
				SP_EXIT_ON_ERROR(r);
			}
			sp_free_config(config);
			SP_EXIT_ON_ERROR(r);
		}
		r = sp_close(*p);
		SP_EXIT_ON_ERROR(r);


		EgSerialPort port;
		port.name = "Portname";
		port.buadrate = buadrate;
		port.bits = bits;
		port.parity = (EgSpParity)parity;
		port.usb_vid = usb_vid;
		port.usb_pid = usb_pid;
		ecs_set_ptr(world, e, EgSerialPort, &port);
	}
	sp_free_port_list(port);
}





static void System_Update(ecs_iter_t *it)
{
	EgSerialPort *p = ecs_term(it, EgSerialPort, 1);
	for (int i = 0; i < it->count; i ++)
	{
		char const * name = ecs_get_name(it->world, it->entities[i]);
		struct sp_port * port;
		enum sp_return r = sp_get_port_by_name(name, &port);
		if (r == SP_OK)
		{
			//https://cpp.hotexamples.com/examples/-/-/sp_free_port/cpp-sp_free_port-function-examples.html
			sp_free_port(port);
		}
	}
}






void Module_EgSerialPortImport(ecs_world_t *world)
{
	ECS_MODULE(world, Module_EgSerialPort);

	ECS_COMPONENT_DEFINE(world, EgSerialPort);
	ECS_COMPONENT_DEFINE(world, EgSpParity);

	ecs_set_name_prefix(world, "Eg");

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
	}});


	ECS_SYSTEM(world, System_Update, EcsOnUpdate, EgSerialPort);

}
