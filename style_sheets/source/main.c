#define GS_IMPL
#define GS_IMMEDIATE_DRAW_IMPL
#define GS_GUI_IMPL
#include <gs/gs.h> 
#include <gs/util/gs_idraw.h>
#include <gs/util/gs_gui.h>

#include <libserialport.h>
#include <stdio.h>
#include <string.h>

typedef struct 
{
    gs_command_buffer_t cb;
    gs_gui_context_t gui;
    const char* asset_dir;
    gs_asset_font_t font;
    gs_gui_style_sheet_t style_sheet;
} app_t; 

void gui_cb(gs_gui_context_t* ctx, struct gs_gui_customcommand_t* cmd);
void app_load_style_sheet(bool destroy);

void app_init()
{
    app_t* app = gs_user_data(app_t);
    app->cb = gs_command_buffer_new(); 
    gs_gui_init(&app->gui, gs_platform_main_window()); 
    app->asset_dir = gs_platform_dir_exists("./assets") ? "./assets" : "../assets";
    
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

#define SP_EXIT_ON_ERROR(r) sp_exit_on_error(r,__FILE__,__LINE__)
void sp_exit_on_error (enum sp_return r, char const * file, int line)
{
	if (r < 0)
	{
		fprintf (stderr, "%s:%i: ", file, line);
		perror (sp_last_error_message ());
		exit (EXIT_FAILURE);
	}
}

void print_devices(gs_gui_context_t* gui)
{
	gs_gui_window_begin(gui, "Window", gs_gui_rect(350, 40, 600, 500));
	gs_gui_container_t* cnt = gs_gui_get_current_container(gui);
	const float m = cnt->body.w * 1.0f;
	gs_gui_layout_row(gui, 1, (int[]){m}, 0);
	struct sp_port ** port;
	enum sp_return r;
	r = sp_list_ports(&port);
	SP_EXIT_ON_ERROR (r);
	for (struct sp_port ** p = port; (*p) != NULL; ++p)
	{
		char buf[100];
		//printf ("q %x\n", *p);
		//printf ("%10s : %s\n", sp_get_port_name(*p), sp_get_port_description(*p));
		snprintf(buf, 100, "%10s : %s\n", sp_get_port_name(*p), sp_get_port_description(*p));
		if (gs_gui_button(gui, buf))
		{
			printf("%s\n", sp_get_port_name(*p));
		};
	}
	sp_free_port_list(port);
	gs_gui_window_end(gui);
}



void test_window(gs_gui_context_t* gui)
{
	gs_gui_window_begin(gui, "Window", gs_gui_rect(350, 40, 600, 500));
	{
		// Cache the current container
		gs_gui_container_t* cnt = gs_gui_get_current_container(gui);
		const float m = cnt->body.w * 1.0f;

		gs_gui_layout_row(gui, 1, (int[]){m}, 0);

		gs_gui_layout_row(gui, 2, (int[]){200, 0}, 0);

		gs_gui_text(gui, "A regular element button.");
		gs_gui_button(gui, "button1");

		gs_gui_text(gui, "A regular element label.");
		gs_gui_label(gui, "label");

		gs_gui_text(gui, "Button with classes: {.c0 .btn}");
		gs_gui_button_ex(gui, "button##btn",
		&(gs_gui_selector_desc_t){.classes = {"c0", "btn"}}, 0x00);

		gs_gui_text(gui, "Label with id #lbl and class .c0");
		gs_gui_label_ex(gui, "label##lbl",
		&(gs_gui_selector_desc_t){.id = "lbl", .classes ={"c0"}}, 0x00);

		// gs_gui_layout_row(gui, 2, (int[]){m, -m}, 0);
		// gs_gui_layout_next(gui); // Empty space at beginning
		gs_gui_layout_row(gui, 1, (int[]){0}, 0);
		if (gs_gui_button_ex(gui, "reload style sheet", &(gs_gui_selector_desc_t){.classes = {"reload_btn"}}, 0x00)) {
			app_load_style_sheet(true);
		}
	}
	gs_gui_window_end(gui);
}


void app_update()
{
    app_t* app = gs_user_data(app_t);
    gs_command_buffer_t* cb = &app->cb;
    gs_gui_context_t* gui = &app->gui;
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window());
    const float t = gs_platform_elapsed_time(); 
    const gs_gui_style_sheet_t* ss = &app->style_sheet;

    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) {
        gs_quit();
    }

    // Begin new frame for gui
    gs_gui_begin(gui, fbs); 

    const gs_vec2 ws = gs_v2(500.f, 300.f);
	int32_t opt =
	GS_GUI_OPT_NOCLIP |
	GS_GUI_OPT_NOFRAME |
	GS_GUI_OPT_FORCESETRECT |
	GS_GUI_OPT_NOTITLE |
	GS_GUI_OPT_DOCKSPACE |
	//GS_GUI_OPT_FULLSCREEN |
	GS_GUI_OPT_NOMOVE |
	GS_GUI_OPT_NOBRINGTOFRONT |
	GS_GUI_OPT_NOFOCUS |
	GS_GUI_OPT_NORESIZE;
	gs_gui_window_begin_ex(gui, "#root", gs_gui_rect(350, 40, 600, 500), NULL, NULL, opt);
	//gs_gui_window_begin(gui, "Window", gs_gui_rect((fbs.x - ws.x) * 0.5f, (fbs.y - ws.y) * 0.5f, ws.x, ws.y));
	{
	}
	gs_gui_window_end(gui);
	print_devices(gui);
	gs_gui_demo_window(gui, gs_gui_rect(200, 100, 500, 250), NULL);
	gs_gui_style_editor(gui, NULL, gs_gui_rect(350, 250, 300, 240), NULL);


    // End gui frame
    gs_gui_end(gui);

    // Do rendering
    gs_graphics_clear_desc_t clear = {.actions = &(gs_graphics_clear_action_t){.color = {0.05f, 0.05f, 0.05f, 1.f}}};
    gs_graphics_begin_render_pass(cb, (gs_handle(gs_graphics_render_pass_t)){0});
    {
        gs_graphics_clear(cb, &clear);
        gs_graphics_set_viewport(cb,0,0,(int)fbs.x,(int)fbs.y);
        gs_gui_render(gui, cb);
    }
    gs_graphics_end_render_pass(cb);
    
    //Submits to cb
    gs_graphics_submit_command_buffer(cb);
}

void app_shutdown()
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

void app_load_style_sheet(bool destroy)
{
    app_t* app = gs_user_data(app_t);
    if (destroy) {
        gs_gui_style_sheet_destroy(&app->gui, &app->style_sheet);
    }
    gs_snprintfc(TMP, 256, "%s/%s", app->asset_dir, "style_sheets/gui.ss");
    app->style_sheet = gs_gui_style_sheet_load_from_file(&app->gui, TMP);
    gs_gui_set_style_sheet(&app->gui, &app->style_sheet);
}











