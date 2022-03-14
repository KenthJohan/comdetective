#include "flecs.h"
#include "eg_serialport.h"
#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>


ECS_COMPONENT_DECLARE(EgSerialPort);



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
		ecs_entity_t e = ecs_lookup(world, sp_get_port_name(*p));
		if (e == 0)
		{
			e = ecs_entity_init(world, &(ecs_entity_desc_t) {
			.name = name
			});
		}


		int buadrate = -100;
		struct sp_port_config *config;

		r = sp_open(*p, SP_MODE_READ_WRITE);
		SP_EXIT_ON_ERROR(r);
		r = sp_new_config(&config);
		SP_EXIT_ON_ERROR(r);
		//r = sp_set_config_baudrate(config, 9600);
		//SP_EXIT_ON_ERROR(r);
		//r = sp_set_config(*p, config);
		SP_EXIT_ON_ERROR(r);
		r = sp_get_config(*p, config);
		SP_EXIT_ON_ERROR(r);
		r = sp_get_config_baudrate(config, &buadrate);
		SP_EXIT_ON_ERROR(r);
		r = sp_close(*p);
		SP_EXIT_ON_ERROR(r);


		EgSerialPort port;
		port.name = "Portname";
		port.buadrate = buadrate;
		sp_free_config(config);
		ecs_set_ptr(world, e, EgSerialPort, &port);
	}
	sp_free_port_list(port);
}












void FlecsComponentsEgSerialPortImport(ecs_world_t *world)
{
	ECS_MODULE(world, FlecsComponentsEgSerialPort);

	ECS_COMPONENT_DEFINE(world, EgSerialPort);

	ecs_set_name_prefix(world, "Eg");

	ecs_struct_init(world, &(ecs_struct_desc_t) {
	.entity.entity = ecs_id(EgSerialPort),
	.members = {
	{
	.name = "name",
	.type = ecs_id(ecs_string_t)
	},
	{
	.name = "buadrate",
	.type = ecs_id(ecs_i32_t),
	.unit = EcsBitsPerSecond
	},
	}
	});
}
