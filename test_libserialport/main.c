#include <libserialport.h>
#include <stdio.h>
#include <unistd.h>
#include "flecs.h"

int main(int argc, char **argv)
{
	ecs_log_set_level(0);
	ecs_world_t *world = ecs_init();
	ecs_singleton_set(world, EcsRest, {0});

	struct sp_port **port_list;
	while(1)
	{
		enum sp_return result = sp_list_ports(&port_list);
		if (result != SP_OK)
		{
			printf("sp_list_ports() failed!\n");
			return -1;
		}
		for (int i = 0; port_list[i] != NULL; i++)
		{
			struct sp_port *port = port_list[i];
			char *port_name = sp_get_port_name(port);
			printf("Found port: %s\n", port_name);
		}
		sp_free_port_list(port_list);
		ecs_os_sleep(1,1);
		printf("\n\n\n");
	}
	return 0;
}
