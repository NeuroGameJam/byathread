#pragma once

void stats_update()
{
	if (key_any || touched_half(true))
		change_scene(3);
	else if (touched_half(false))
		change_scene(2);
}

void stats_render()
{
	
}

scene* stats_alloc()
{
	scene* s = calloc(1, sizeof(scene));

	s->update = stats_update;
	s->prerender = shader_bg;
	s->render = stats_render;

	state.score = state.score1 + state.score2;

	renderer_camera(c_ui);
	renderer_shader(s_flat);

	manager_add(
		label_alloc(f_normal, APP_NAME, (vec3) { 0, 6, 0 }, 2),
		UNKNOWN, &label_update, &label_render, &label_free);

	u16 m = state.time / 1000 / 60;
	u16 ss = state.time  / 1000 - m * 60;

	snprintf(s_aux, sizeof(s_aux), 
		"LEVEL %i COMPLETED\nSCORE: %i\nTIME: %02d:%02d", 
		state.level, state.score, m, ss);

	manager_add(
		label_alloc(f_normal, s_aux, (vec3){ 0, 0, 0 }, 1.5),
		LABEL, &label_update, &label_render, &label_free);

	manager_add(
		label_alloc(f_normal, 
		"TOUCH LEFT SIDE TO GO BACK\nTOUCH RIGHT SIDE TO START THE NEXT LEVEL", 
		(vec3){ 0, -8, 0 }, 0.75),
		LABEL, &label_update, &label_render, &label_free);


	if (state.level < LEVEL_COUNT)
		state.level++;

	if (state.max < state.level)
		state.max = state.level;

	state.xp += state.score;

	// reset data
	data_reset();

	save_data(&state, DATA_FILE);

 	return s;
}