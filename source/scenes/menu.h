#pragma once

label* lb_play;
label* lb_level;
label* lb_sound;
label* lb_dual;
i8 menu_selected = 0;
button* prior;
button* next;
gunit* locked;

void menu_update_ui()
{
	locked->visible = state.level > state.max;
	prior->visible = state.level > 1;
	next->visible = state.level < LEVEL_COUNT;

	label_change_text(lb_play, state.level > state.max ? "Locked" : "Play");

	snprintf(s_aux, sizeof(s_aux), "Level %i", state.level);
	label_change_text(lb_level, s_aux);

	snprintf(s_aux, sizeof(s_aux), "Sound %s", state.sound ? "On" : "Off");
	label_change_text(lb_sound, s_aux);	

	snprintf(s_aux, sizeof(s_aux), "Mode %s", state.dual ? "Dual" : "Single");
	label_change_text(lb_dual, s_aux);	
}

void menu_prior()
{
	state.level--;

	save_data(&state, DATA_FILE);
	audio_play_force(fx_tap);
}

void menu_next()
{
	state.level++;
	
	save_data(&state, DATA_FILE);
	audio_play_force(fx_tap);
}

void menu_sound()
{
	state.sound = !state.sound;
	muted = !state.sound;
	save_data(&state, DATA_FILE);
	audio_play_force(fx_tap);

	ambient_sync(amb);
}

void menu_dual()
{
	state.dual = !state.dual;
	save_data(&state, DATA_FILE);
	audio_play_force(fx_tap);
}

void menu_play()
{	
	if (state.level > state.max)
		return;

	audio_play_force(fx_tap);
	change_scene(3);
}

void menu_update()
{
	menu_update_ui();
}

scene* menu_alloc()
{
	bg = (color) BGCOLOR;

	scene* s = calloc(1, sizeof(scene));

	s->update = menu_update;
	s->prerender = shader_bg;

	// choose camera/shader
	renderer_camera(c_ui);
	renderer_shader(s_flat);
	
	menu_selected = 0;

	// locker
	locked = gunit_alloc("gui/locked.png");
	locked->pos = (vec) { MENU_POS - .8, 1 };
	locked->mdl->scale = f3_uni(.15);

	manager_add(
		locked,
		UNKNOWN, NULL, &gunit_render, &gunit_free);	

	// labels
	manager_add(
		label_alloc(f_normal, APP_NAME, (vec3) { MENU_POS, 6, 0 }, 2),
		UNKNOWN, &label_update, &label_render, &label_free);

	manager_add(
		lb_play = label_alloc(f_normal, "", 
		(vec3) { MENU_POS, 1, 0 }, 1),
		UNKNOWN, &label_update, &label_render, &label_free);

	manager_add(
		lb_level = label_alloc(f_normal, "", (vec3) { MENU_POS, -1, 0 }, 1),
		UNKNOWN, &label_update, &label_render, &label_free);

	manager_add(
		lb_sound = label_alloc(f_normal, "", 
		(vec3) { MENU_POS, -3.0, 0 }, 1),
		UNKNOWN, &label_update, &label_render, &label_free);

	manager_add(
		lb_dual = label_alloc(f_normal, "", 
		(vec3) { MENU_POS, -5, 0 }, 1),
		UNKNOWN, &label_update, &label_render, &label_free);

	lb_play->enabled = true;
	lb_play->click = menu_play;
	lb_play->key = VK_RETURN;

	lb_sound->enabled = true;
	lb_sound->click = menu_sound;

	lb_dual->enabled = true;
	lb_dual->click = menu_dual;


	// buttons
	prior = button_alloc("gui/prior.png", 
		(vec) { MENU_POS - 1.85, lb_level->pos.y + .04 });
	prior->model->scale = f3_uni(.35);
	prior->click = menu_prior;
	prior->offset = (vec) { -.025, -.015 };
	prior->key = VK_LEFT;

	next = button_alloc("gui/next.png", 
		(vec) { MENU_POS + 1.85, lb_level->pos.y + .04 });
	next->model->scale = f3_uni(.35);
	next->click = menu_next;
	next->offset = (vec) { .025, -.015 };
	next->key = VK_RIGHT;

	manager_add(prior, BUTTON, &button_update, &button_render, &button_free);
	manager_add(next, BUTTON, &button_update, &button_render, &button_free);		

	menu_update_ui();	

	return s;
}