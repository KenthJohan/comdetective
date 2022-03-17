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


static void System_Pull_Plugged(ecs_iter_t *it)
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
		ecs_entity_t e = ecs_lookup(it->world, name);
		if (e == 0)
		{
			e = ecs_entity_init(it->world, &(ecs_entity_desc_t){
			.name = name
			});
			r = sp_open(*p, SP_MODE_READ_WRITE);
			SP_EXIT_ON_ERROR(r);
			EgSerialPort egport;
			get_info(*p, &egport);
			ecs_set_ptr(it->world, e, EgSerialPort, &egport);
			r = sp_close(*p);
			SP_EXIT_ON_ERROR(r);
		}
	}
	sp_free_port_list(port);
}




//typedef void (*ecs_iter_action_t)(ecs_iter_t *it);
static void System_Pull_Unplugged(ecs_iter_t *it)
{
	EgSerialPort *p = ecs_term(it, EgSerialPort, 1);
	for (int i = 0; i < it->count; i ++)
	{
		char const * name = ecs_get_name(it->world, it->entities[i]);
		struct sp_port * port;
		enum sp_return r;
		r = sp_get_port_by_name(name, &port);
		if (r != SP_OK)
		{
			ecs_delete(it->world, it->entities[i]);
			continue;
		}
		r = sp_open(port, SP_MODE_READ_WRITE);
		if (r != SP_OK)
		{
			ecs_delete(it->world, it->entities[i]);
		}
		else
		{
			r = sp_close(port);
			SP_EXIT_ON_ERROR(r);
		}
		sp_free_port(port);
	}
}


/*
static void Trigger1(ecs_iter_t *it)
{
	EgSerialPort *p = ecs_term(it, EgSerialPort, 1);
	for (int i = 0; i < it->count; i ++)
	{

	}
}
*/



void Module_EgLibSerialPortImport(ecs_world_t *world)
{
	ECS_MODULE(world, Module_EgLibSerialPort);

	// This systems pulls avialable USB serial ports.
	// It will cause a initial lag for every new port.
	// If a USB serial port does not exist as entity then it will
	// be added then opened then retreive info then closed.
	// TODO: Only trigger this system when a USB serial port is plugged.
	ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t)
	{
	.query.filter.terms = {{ .id = ecs_id(EgSerialPortSingleton) }},
	.callback = System_Pull_Plugged,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});

	// This system causes lag because it opens and closes USB serial ports.
	// This is to check if a USB serial port has been unplugged.
	// If the USB serial port has been unplugged then the corresponding entity will be deleted.
	// TODO: Only trigger this system when a USB serial port is unplugged.
	ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t)
	{
	.query.filter.terms = {{ .id = ecs_id(EgSerialPort) }},
	.callback = System_Pull_Unplugged,
	.entity.add = { EcsOnUpdate },
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
