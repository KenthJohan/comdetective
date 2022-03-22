#include "flecs.h"
#include "eg_serialport.h"
#include "eg_libserialport.h"
#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>



struct sp_event_set * global_events = NULL;


#define SP_EXIT_ON_ERROR(r) sp_exit_on_error(r,__FILE__,__LINE__)
static void sp_exit_on_error (enum sp_return r, char const * file, int line)
{
	if (r < 0)
	{
		fprintf (stderr, "%s:%i: %i:%s", file, line, r, sp_last_error_message());
		perror ("");
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
/*
	EgSerialPort * p = ecs_term(it, EgSerialPort, 1);
	for (int i = 0; i < it->count; i ++)
	{
		ecs_entity_t e = it->entities[i];
		if (p[i].status != EG_SP_STATUS_OPEN)
		{
			p[i].status = EG_SP_STATUS_UNDEFINED;
			ecs_enable_component(it->world, e, EgSerialPort, false);
		}
		//char const * name = ecs_get_name(it->world, e);
		//printf("%s: ecs_is_component_enabled: %d\n", name, ecs_is_component_enabled(it->world, e, EgSerialPort));
	}
	*/
}


static void System_Ports_Pull(ecs_iter_t *it)
{
	struct sp_port ** port;
	enum sp_return r;
	r = sp_list_ports(&port);
	SP_EXIT_ON_ERROR(r);
	for (struct sp_port ** p = port; (*p) != NULL; ++p)
	{
		char * name = sp_get_port_name(*p);
		ecs_entity_t e = ecs_entity_init(it->world, &(ecs_entity_desc_t){
		.name = name,
		.add = {ecs_id(EgSerialPort)}
		});
	}
	sp_free_port_list(port);
}


static void System_Ports_Read(ecs_iter_t *it)
{
	enum sp_return r;
	struct sp_port * port = NULL;
	EgSerialPort * p = ecs_term(it, EgSerialPort, 1);
	for (int i = 0; i < it->count; i ++)
	{
		char const * name = ecs_get_name(it->world, it->entities[i]);

		//Temporary:
		/*
		if (p[i].status == EG_SP_STATUS_UNDEFINED)
		{
			printf("Set status to EG_SP_STATUS_OPEN\n");
			p[i].status = EG_SP_STATUS_OPENING;
		}
		*/

		switch(p[i].status)
		{
		case EG_SP_STATUS_UNDEFINED:
			p[i].status = EG_SP_STATUS_CLOSED;
			break;

		case EG_SP_STATUS_CLOSING:
			port = p[i]._internal;
			r = sp_close(port);
			printf("sp_close %s:%i\n", name, r);
			SP_EXIT_ON_ERROR(r);
			sp_free_port(port);
			p[i]._internal = NULL;
			p[i].status = EG_SP_STATUS_CLOSED;
			break;

		case EG_SP_STATUS_OPENING:
			r = sp_get_port_by_name(name, &port);
			if (r == SP_OK)
			{
				r = sp_open(port, SP_MODE_READ);
				printf("sp_open %s:%i\n", name, r);
				if (r == SP_OK)
				{
					p[i].status = EG_SP_STATUS_OPEN;
					r = sp_set_baudrate(port, 115200);
					SP_EXIT_ON_ERROR(r);
					r = sp_set_bits(port, 8);
					SP_EXIT_ON_ERROR(r);
					r = sp_set_parity(port, SP_PARITY_NONE);
					SP_EXIT_ON_ERROR(r);
					r = sp_set_stopbits(port, 1);
					SP_EXIT_ON_ERROR(r);
					r = sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
					SP_EXIT_ON_ERROR(r);
					r = sp_add_port_events(global_events, port, SP_EVENT_RX_READY);
					SP_EXIT_ON_ERROR(r);
					p[i]._internal = port;
				}
				else
				{
					p[i].status = EG_SP_STATUS_OPEN_ERROR;
				}
			}
			break;

		case EG_SP_STATUS_OPEN:
			if(p[i]._internal)
			{
				port = p[i]._internal;
				int bufsize = 100;
				char buf[100] = {0};
				int r;
				//r = sp_input_waiting(port);
				//printf("sp_input_waiting %i %s\n", r, sp_last_error_message());
				r = sp_nonblocking_read(port, buf, bufsize);
				//printf("sp_nonblocking_read %i\n", r);
				if (r > 0)
				{
					printf("Port %s: %.*s\n", name, r, buf);
				}
			}
			break;


		default:
			break;

		}



	}
}


static void * reader_thread(void * arg)
{
	while(1)
	{
		sp_wait(global_events, 10000);
	}
}







void EgLibserialportImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgLibserialport);
	ecs_set_name_prefix(world, "Eg");



	{
		enum sp_return r;
		sp_new_event_set(&global_events);
		SP_EXIT_ON_ERROR(r);
	}


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
	/*
	ecs_system_init(world, &(ecs_system_desc_t)
	{
	.entity.name = "System_Ports_Reset",
	.query.filter.terms = {{ .id = ecs_id(EgSerialPort) }},
	.callback = System_Ports_Reset,
	.entity.add = { EcsPreUpdate },
	.interval = 1.0f
	});
	*/


	// This system resets the port information so that
	ecs_system_init(world, &(ecs_system_desc_t)
	{
	.entity.name = "System_Ports_Read",
	.query.filter.expr = "[inout] EgSerialPort",
	.callback = System_Ports_Read,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});

	//ecs_os_thread_new(reader_thread, NULL);


	ecs_entity_init(world, &(ecs_entity_desc_t){
	.name = "Test",
	.add = {ecs_id(EgSerialPort)}
	});

}
