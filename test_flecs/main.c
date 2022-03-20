#include <stdio.h>
#include "flecs.h"

typedef struct
{
	ecs_i32_t dummy;
} MySingleton;

typedef struct
{
	ecs_i32_t dummy;
} MyComponent;

void sys1(ecs_iter_t * it)
{
	printf("sys1\n");
}

void sys2(ecs_iter_t * it)
{
	printf("sys2\n");
}

int main(int argc, char **argv)
{
	ecs_log_set_level(0);
	ecs_world_t *world = ecs_init();
	ECS_COMPONENT(world, MySingleton);
	ECS_COMPONENT(world, MyComponent);

	ecs_singleton_set(world, MySingleton, {0});
	ECS_ENTITY(world, e, MyComponent);

	ecs_system_init(world, &(ecs_system_desc_t)
	{
	.query.filter.expr = "$MySingleton",
	.callback = sys1,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});

	ecs_system_init(world, &(ecs_system_desc_t)
	{
	.query.filter.expr = "$MySingleton, ?MyComponent",
	.callback = sys2,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});

	while(1)
	{
		ecs_progress(world, 0);
		ecs_os_sleep(0,1000000);
	}

	return 0;
}
