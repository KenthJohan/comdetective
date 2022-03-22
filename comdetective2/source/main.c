// https://www.flecs.dev/explorer/

#define GS_IMPL
#define GS_IMMEDIATE_DRAW_IMPL
#define GS_GUI_IMPL
#include <gs/gs.h>
#include <gs/util/gs_idraw.h>
#include <gs/util/gs_gui.h>
#include "flecs.h"

#include <stdio.h>
#include <string.h>

#include "eg_libserialport.h"
#include "eg_serialport.h"
#include "app_components.h"
#include "app_systems.h"






static void System_Draw_Serialports(ecs_world_t * world, ecs_query_t *query, gs_gui_context_t * gui)
{
	gs_gui_window_begin(gui, "Window", gs_gui_rect(350, 40, 600, 500));
	gs_gui_container_t* cnt = gs_gui_get_current_container(gui);
	ecs_iter_t it = ecs_query_iter(world, query);
	while (ecs_query_next(&it))
	{
		EgSerialPort *p = ecs_term(&it, EgSerialPort, 1);
		for (int i = 0; i < it.count; i ++)
		{
			const float m = cnt->body.w * 1.0f;
			gs_gui_layout_row(gui, 1, (int[]){m}, 0);

			char const * name = ecs_get_name(world, it.entities[i]);
			if (name == NULL) {continue;}

			char buf[100];
			snprintf(buf, 100, "%s (%s)", name, EgSpStatus_tostr(p[i].status));
			if (gs_gui_button(gui, buf))
			{
				if (p[i].status == EG_SP_STATUS_CLOSED)
				{
					p[i].status = EG_SP_STATUS_OPENING;
				}
				else if (p[i].status == EG_SP_STATUS_OPEN)
				{
					p[i].status = EG_SP_STATUS_CLOSING;
				}
			}
		}
	}
	gs_gui_window_end(gui);
}









typedef struct
{
	gs_command_buffer_t cb;
	gs_gui_context_t gui;
	const char* asset_dir;
	gs_asset_font_t font;
	gs_gui_style_sheet_t style_sheet;
	ecs_world_t * world;

	ecs_query_t * query_ports;
} app_t;

static void gui_cb(gs_gui_context_t* ctx, struct gs_gui_customcommand_t* cmd);
static void app_load_style_sheet(bool destroy);

static void app_init()
{
	ecs_log_set_level(0);
	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, EgSerialport);
	ECS_IMPORT(world, EgLibserialport);
	ECS_IMPORT(world, AppComponents);
	ECS_IMPORT(world, AppSystems);
	ecs_singleton_set(world, EcsRest, {0});
	ecs_singleton_set(world, EgSerialPortSingleton, {0});





	app_t* app = gs_user_data(app_t);
	app->world = world;
	app->cb = gs_command_buffer_new();
	gs_gui_init(&app->gui, gs_platform_main_window());
	app->asset_dir = gs_platform_dir_exists("./assets") ? "./assets" : "../assets";


	app->query_ports = ecs_query_init(world, &(ecs_query_desc_t) {
	.filter.expr = "EgSerialPort",
	.filter.instanced = true
	});

	// Load in custom font file and then initialize gui font stash
	gs_snprintfc(FONT_PATH, 256, "%s/%s", app->asset_dir, "fonts/mc_regular.otf");
	gs_asset_font_load_from_file(FONT_PATH, &app->font, 20);
	gs_gui_init_font_stash(&app->gui, &(gs_gui_font_stash_desc_t){
	.fonts = (gs_gui_font_desc_t[]){{.key = "mc_regular", .font = &app->font}},
	.size = 1 * sizeof(gs_gui_font_desc_t)
	});

	// Load style sheet from file now
	app_load_style_sheet(false);
}



static void app_update()
{
	app_t* app = gs_user_data(app_t);
	gs_command_buffer_t* cb = &app->cb;
	gs_gui_context_t* gui = &app->gui;
	const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window());
	if (gs_platform_key_pressed(GS_KEYCODE_ESC)) {gs_quit();}

	gs_gui_begin(gui, fbs);
	System_Draw_Serialports(app->world, app->query_ports, gui);
	//gs_gui_demo_window(gui, gs_gui_rect(200, 100, 500, 250), NULL);
	//gs_gui_style_editor(gui, NULL, gs_gui_rect(350, 250, 300, 240), NULL);
	gs_gui_end(gui);

	gs_graphics_clear_desc_t clear = {.actions = &(gs_graphics_clear_action_t){.color = {0.05f, 0.05f, 0.05f, 1.f}}};
	gs_graphics_renderpass_begin(cb, (gs_handle(gs_graphics_renderpass_t)){0});
	{
		gs_graphics_clear(cb, &clear);
		gs_graphics_set_viewport(cb,0,0,(int)fbs.x,(int)fbs.y);
		gs_gui_render(gui, cb);
	}
	gs_graphics_renderpass_end(cb);
	gs_graphics_command_buffer_submit(cb);
	ecs_progress(app->world, 0);
}

static void app_shutdown()
{
	app_t* app = gs_user_data(app_t);
	gs_gui_free(&app->gui);
}

gs_app_desc_t gs_main(int32_t argc, char** argv)
{
	return (gs_app_desc_t) {
		.user_data = gs_malloc_init(app_t),
		.init = app_init,
		.update = app_update,
		.shutdown = app_shutdown,
		.window_width = 1024,
		.window_height = 760
	};
}





static void app_load_style_sheet(bool destroy)
{
	app_t* app = gs_user_data(app_t);
	if (destroy) {
		gs_gui_style_sheet_destroy(&app->gui, &app->style_sheet);
	}
	gs_snprintfc(TMP, 256, "%s/%s", app->asset_dir, "style_sheets/gui.ss");
	app->style_sheet = gs_gui_style_sheet_load_from_file(&app->gui, TMP);
	gs_gui_set_style_sheet(&app->gui, &app->style_sheet);
}











