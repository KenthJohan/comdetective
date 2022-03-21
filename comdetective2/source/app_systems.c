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


void AppSystemsImport(ecs_world_t *world)
{
	ECS_MODULE(world, AppSystems);
	ECS_IMPORT(world, EgSerialport);
	ECS_IMPORT(world, AppComponents);


	ECS_SYSTEM(world, App_GUI_Window_Draw, EcsOnUpdate,
	[inout] App_GUI_Window,
	[inout] EgSerialPort);
}
