#include "app_components.h"


ECS_COMPONENT_DECLARE(App_GUI_Window);

void AppComponentsImport(ecs_world_t *world)
{
	ECS_MODULE(world, AppComponents);
	ECS_COMPONENT_DEFINE(world, App_GUI_Window);
	ecs_set_name_prefix(world, "App");
}
