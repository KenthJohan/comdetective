#include "app_systems.h"
#include "app_components.h"
#include "eg_serialport.h"


static void App_GUI_Window_Draw(ecs_iter_t *it)
{
	App_GUI_Window *w = ecs_term(it, App_GUI_Window, 1);
	for (int i = 0; i < it->count; i ++)
	{

	}
}


void Module_AppSystemsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Module_AppSystems);
	ECS_IMPORT(world, Module_EgSerialPort);
	ECS_IMPORT(world, Module_AppComponents);


	ECS_SYSTEM(world, App_GUI_Window_Draw, EcsOnUpdate,
	[inout] App_GUI_Window,
	[inout] EgSerialPort);
}
