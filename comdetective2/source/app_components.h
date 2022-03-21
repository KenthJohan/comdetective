#pragma once
#include "flecs.h"



typedef struct
{
	ecs_string_t name;
} App_GUI_Window;


extern ECS_COMPONENT_DECLARE(App_GUI_Window);





void AppComponentsImport(ecs_world_t *world);

