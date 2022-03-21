#include "flecs.h"
#include "eg_serialport.h"
#include "eg_libserialport.h"
#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>


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


static void get_info(struct sp_port * port, EgSerialPort * egport)
{
	struct sp_port_config *config;
	enum sp_return r;

	r = sp_new_config(&config);
	SP_EXIT_ON_ERROR(r);

	int buadrate = -100;
	int bits;
	enum sp_parity parity;
	int usb_vid;
	int usb_pid;
	int usb_bus;
	int usb_address;

	sp_get_port_usb_bus_address(port, &usb_bus, &usb_address);
	sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);

	SP_EXIT_ON_ERROR(r);
	r = sp_get_config(port, config);
	SP_EXIT_ON_ERROR(r);
	r = sp_get_config_baudrate(config, &buadrate);
	SP_EXIT_ON_ERROR(r);
	r = sp_get_config_parity(config, &parity);
	SP_EXIT_ON_ERROR(r);
	r = sp_get_config_bits(config, &bits);
	SP_EXIT_ON_ERROR(r);

	egport->name = "Portname";
	egport->buadrate = buadrate;
	egport->bits = bits;
	egport->parity = (EgSpParity)parity;
	egport->usb_vid = usb_vid;
	egport->usb_pid = usb_pid;
	egport->usb_bus = usb_bus;
	egport->usb_address = usb_address;
	sp_free_config(config);
}



static void System_Ports_Reset(ecs_iter_t *it)
{
	EgSerialPort * p = ecs_term(it, EgSerialPort, 1);
	for (int i = 0; i < it->count; i ++)
	{
		ecs_entity_t e = it->entities[i];
		p[i].status = EG_SP_STATUS_UNDEFINED;
		ecs_enable_component(it->world, e, EgSerialPort, false);
		//char const * name = ecs_get_name(it->world, e);
		//printf("%s: ecs_is_component_enabled: %d\n", name, ecs_is_component_enabled(it->world, e, EgSerialPort));
	}
}


static void System_Ports_Pull(ecs_iter_t *it)
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
		ecs_entity_t e = ecs_entity_init(it->world, &(ecs_entity_desc_t){
		.name = name
		});
		ecs_enable_component(it->world, e, EgSerialPort, true);
		EgSerialPort * egport = ecs_get_mut(it->world, e, EgSerialPort, NULL);
		egport->status = EG_SP_STATUS_CLOSED;


		/*
		ecs_entity_t e = ecs_lookup(it->world, name);
		if (e == 0)
		{
			r = sp_open(*p, SP_MODE_READ_WRITE);
			SP_EXIT_ON_ERROR(r);
			get_info(*p, &egport);
			ecs_set_ptr(it->world, e, EgSerialPort, &egport);
			r = sp_close(*p);
			SP_EXIT_ON_ERROR(r);
		}
		*/
	}
	sp_free_port_list(port);
}











void EgLibserialportImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgLibserialport);
	ecs_set_name_prefix(world, "Eg");

	// This systems pulls avialable USB serial ports.
	// TODO: Only trigger this system when a USB serial port is plugged.
	ecs_system_init(world, &(ecs_system_desc_t)
	{
	.entity.name = "System_Ports_Pull",
	.query.filter.terms = {{ .id = ecs_id(EgSerialPortSingleton) }},
	.callback = System_Ports_Pull,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});

	// This system resets the port information so that
	ecs_system_init(world, &(ecs_system_desc_t)
	{
	.entity.name = "System_Ports_Reset",
	.query.filter.terms = {{ .id = ecs_id(EgSerialPort) }},
	.callback = System_Ports_Reset,
	.entity.add = { EcsPreUpdate },
	.interval = 1.0f
	});

	/*
	ecs_entity_t t1 = ecs_trigger_init(world, &(ecs_trigger_desc_t){
	.term.id = ecs_id(EgSerialPort),
	.events = {EcsOnAdd},
	.callback = Trigger1
	});
	*/

}
