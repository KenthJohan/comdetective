// https://www.flecs.dev/explorer/
#include <libserialport.h>
#include <stdio.h>
#include <unistd.h>
#include "flecs.h"
#include "../comdetective2/source/eg_serialport.h"


void tf1(ecs_iter_t *it)
{
	EgSerialPort * p = ecs_term(it, EgSerialPort, 1);
	for (int i = 0; i < it->count; i ++)
	{
		p[i].exists = false;
		ecs_entity_t e = it->entities[i];
		char const * name = ecs_get_name(it->world, e);
		ecs_enable_component(it->world, e, EgSerialPort, false);
		printf("%s: ecs_is_component_enabled: %d\n", name, ecs_is_component_enabled(it->world, e, EgSerialPort));
	}
}

void tf2(ecs_iter_t *it)
{
	struct sp_port **port_list;
	enum sp_return result = sp_list_ports(&port_list);
	if (result != SP_OK)
	{
		printf("sp_list_ports() failed!\n");
		return;
	}
	for (int i = 0; port_list[i] != NULL; i++)
	{
		struct sp_port *port = port_list[i];
		char *name = sp_get_port_name(port);
		ecs_entity_t e = ecs_entity_init(it->world, &(ecs_entity_desc_t) {
		.name = name,
		.add = {ecs_id(EgSerialPort)}
		});
		//ecs_add(it->world, e, EgSerialPort);
		ecs_enable_component(it->world, e, EgSerialPort, true);
		printf("Found port: %s\n", name);
		EgSerialPort p = {.exists = true};
		ecs_set_ptr(it->world, e, EgSerialPort, &p);
	}
	sp_free_port_list(port_list);
	printf("\n\n\n");

}

typedef struct
{
	ecs_i32_t dummy;
} MuComp;

int main(int argc, char **argv)
{
	ecs_log_set_level(0);
	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, Module_EgSerialPort);
	ecs_singleton_set(world, EcsRest, {0});
	ecs_singleton_set(world, EgSerialPortSingleton, {0});
	ECS_COMPONENT(world, MuComp);


	ecs_struct_init(world, &(ecs_struct_desc_t) {
	.entity.entity = ecs_id(MuComp),
	.members = {
	{
	.name = "dummy",
	.type = ecs_id(ecs_i32_t)
	},
	}});

	ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t)
	{
	.query.filter.expr = "[inout] EgSerialPort",
	.callback = tf1,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});

	ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t)
	{
	.query.filter.expr = "$EgSerialPortSingleton",
	.callback = tf2,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});

	ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t) {
	.name = "Test",
	.add = {ecs_id(EgSerialPort), ecs_id(MuComp)}
	});
	ecs_enable_component(world, e, EgSerialPort, false);

	while(1)
	{
		ecs_progress(world, 0);
		ecs_os_sleep(0,1000000);
	}

	//t1();
	return 0;
}
