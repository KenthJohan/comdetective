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
	MySingleton *s = ecs_term(it, MySingleton, 1);
	MyComponent *c = ecs_term(it, MyComponent, 2);
	printf("sys1 %x %x %i\n", s, c, it->count);
}

void sys2(ecs_iter_t * it)
{
	printf("sys2");
}

int main(int argc, char **argv)
{
	ecs_log_set_level(0);
	ecs_world_t *world = ecs_init();
	ECS_COMPONENT(world, MySingleton);
	ECS_COMPONENT(world, MyComponent);

	//ecs_singleton_set(world, MySingleton, {0});
	ecs_entity_t e1 = ecs_new(world, MySingleton);
	ecs_entity_t e11 = ecs_new_w_pair(world, EcsChildOf, e1);
	ecs_entity_t e12 = ecs_new_w_pair(world, EcsChildOf, e1);
	ecs_add(world, e11, MyComponent);
	ecs_add(world, e12, MyComponent);

	ecs_system_init(world, &(ecs_system_desc_t)
	{
	.query.filter.expr = "MySingleton(self|parent), ?MyComponent",
	.query.filter.instanced = true,
	.callback = sys1,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});

	/*
	ecs_system_init(world, &(ecs_system_desc_t)
	{
	.query.filter.expr = "$MySingleton, ?MyComponent",
	.callback = sys2,
	.entity.add = { EcsOnUpdate },
	.interval = 1.0f
	});
	*/

	while(1)
	{
		ecs_progress(world, 0);
		ecs_os_sleep(0,1000000);
	}

	return 0;
}
