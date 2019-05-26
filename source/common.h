#pragma once

#define MENU_POS 0
#define LEVEL_SIZE 10
#define LEVEL_COUNT 10
#define LEVEL_SPACING (vec) { 6, 12 }
#define MOVE_TIME 500
#define MIN_ENEMY_RADIUS 2
#define PLAYER_OFFSET 1
#define PLAYER_WING .25
#define WINGS_TIME 500
#define FLOCK_TIME 5000

////////////////////////////////////////////////////////////////////////////////

data state;
bool started;

shader* s_flat;
shader* s_gradient;

cam* c_ui;
cam* c_game;

tex* t_black;
tex* t_white;
tex* t_shadow;

font* f_normal;

sound* fx_tap;
sound* fx_bad_hit;
sound* fx_normal_hit1;
sound* fx_normal_hit2;
sound* fx_win;
sound* fx_wind;
sound* fx_metronome;
sound* fx_phrase[7];

gunit* g_actor;
gunit* g_lwing;
gunit* g_rwing;
gunit* g_spot;
gunit* g_bad;
gunit* g_connector;
gunit* g_snow;
gunit* g_mountain;
gunit* g_rope;

label* label_score;
label* label_time;

ambient* amb = NULL; // null important - checking in menu

void common_free()
{
	//vox_free_pallete();

	// shaders
	shader_free(s_flat);
	shader_free(s_gradient);

	// free cams
	cam_free(c_ui);
	cam_free(c_game);

	// textures
	tex_free(t_black);
	tex_free(t_white);
	tex_free(t_shadow);

	// models
	model_free(m_quad);
	model_free(m_cube);
	model_free(m_plane);
	model_free(m_frame);

	// fonts
	font_free(f_normal);

	// fx sounds
	ambient_free(amb);

	sound_free(fx_tap);		
	sound_free(fx_bad_hit);
	sound_free(fx_normal_hit1);
	sound_free(fx_normal_hit2);
	sound_free(fx_win);
	sound_free(fx_wind);
	sound_free(fx_metronome);

	for (u8 h = 0; h < 7; h++)
		sound_free((sound*)fx_phrase[h]);

	// gunits
	gunit_free(g_actor);
	gunit_free(g_rwing);
	gunit_free(g_lwing);
	gunit_free(g_spot);
	gunit_free(g_bad);
	gunit_free(g_connector);
	gunit_free(g_snow);
	gunit_free(g_mountain);
	gunit_free(g_rope);

	debug("common free ended\n");
}

void common_alloc()
{
	//vox_alloc_pallete();

	// shaders
	s_flat = shader_alloc("shaders/flat");
	s_gradient = shader_alloc("shaders/gradient");

	// cams
	c_ui = cam_alloc();
	c_game = cam_alloc();

	c_ui->pov = (vec3) { 0, 0, -1 };
	c_ui->fov = 5;
	c_ui->nfar = 100.f;
	c_ui->nnear = 1.f;
	c_ui->light.pos = (vec3) { -1, 1, 0 };
	c_ui->light.power = 100;

	cam_update(c_ui);

	c_game->ortho = true;	
	c_game->pov = (vec3) { 0, 15, -1 };
	c_game->look = (vec3) { 0, 15, 0 };
	c_game->fov = 10;
	c_game->nfar = 50.f;
	c_game->nnear = 1.f;
	c_game->light.pos = (vec3) { -1, 1, 0 };
	c_game->light.power = 100;

	//f32 dist = sqrt(1 / 3.0);
	//c_game->pov = (vec3) { dist, dist, dist };

	cam_update(c_game);

	// textures
	t_black = tex_alloc_color((color) { 0, 0, 0, 255 });
	t_white = tex_alloc_color((color) { 255, 255, 255, 255 });
	t_shadow = tex_alloc_color((color) { 12, 12, 12, 200 });

	// models
	m_quad = model_alloc("models/quad.x");
	m_cube = model_alloc("models/cube.x");
	m_plane = model_alloc("models/plane.x");
	m_frame = model_alloc("models/frame.x");

	// fonts
	f_normal = font_load("fonts/normal");	
	f_normal->line = f_normal->height / 1.5;

	// fx sounds
	fx_tap = sound_load("sounds/tap.wav");
	fx_bad_hit = sound_load("sounds/bad_hit.wav");
	fx_normal_hit1 = sound_load("sounds/normal_hit1.wav");
	fx_normal_hit2 = sound_load("sounds/normal_hit2.wav");
	fx_win = sound_load("sounds/win.wav");
	fx_wind = sound_load("sounds/wind.wav");
	fx_metronome = sound_load("sounds/metronome.wav");

	for (u8 h = 1; h <= 7; h++)
	{
		snprintf(s_aux, sizeof(s_aux), "sounds/phrase%i.wav", h);
		fx_phrase[h-1] = sound_load(s_aux);
	}

	// gunits
	g_actor = gunit_alloc("textures/actor.png");
	g_lwing = gunit_alloc("textures/lwing.png");
	g_rwing = gunit_alloc("textures/rwing.png");
	g_spot = gunit_alloc("textures/spot.png");
	g_bad = gunit_alloc("textures/bad.png");
	g_connector = gunit_alloc("textures/connector.png");
	g_snow = gunit_alloc("textures/snow.png");
	g_mountain = gunit_alloc("textures/mountain.png");
	g_rope = gunit_alloc("textures/rope.png");

	load_data(&state, DATA_FILE);

	// defaults
	if (state.level == 0)
	{
		state.level = state.max = 1;
		state.sound = true;
		save_data(&state, DATA_FILE);
	}

/*
	#ifdef DEBUG	
	//state.max = LEVEL_COUNT;
	state.sound = true;
	state.level = 1;
	state.score = 0;
	#endif
*/
	muted = ! state.sound;

	amb = ambient_alloc();
}

///////

void render_collider(collider* c)
{
	return;
	
	m_cube->scale = f3_uni(.15);
	m_cube->rot = F3_ZERO;
	m_cube->texture_id = t_white->id;

	for (u8 h = 0; h < 4; h++)
	{
		m_cube->pos.x = c->points[h].x;
		m_cube->pos.y = c->points[h].y;
		m_cube->pos.z = 0;
		
		model_render(m_cube);
	}
}

////////////////////////////////////////////////////////////////////////////////
// HELPERS

void gunit_add(gunit* entity)
{
	manager_add(
		entity,
		GUNIT,
		NULL,
		&gunit_render,
		&gunit_free
		);
}

void shader_bg()
{
	bg = (color)BG;
    shader* bk = sh;
    renderer_shader(s_gradient);
    render_shader_fs(); 
    renderer_shader(bk);

}

void data_reset()
{
	state.score = 0;
	state.score1 = 0;
	state.score2 = 0;
	state.time = 0;	
}