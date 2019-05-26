#pragma once

void gameplay_camfollow_single()
{
	player* p = manager_find(PLAYER);

	vec3 ppos = f2_addz(p->pos);	
	vec3 diff = f3_sub(ppos, c_game->look);

	c_game->look = f3_add(c_game->look, diff);
	c_game->pov = f3_add(c_game->pov, diff);

	cam_update(c_game);
}

void gameplay_camfollow_dual()
{
	list* l = manager_list(PLAYER);

	vec med = f2_med(
		((player*)l->items[0])->pos,
		((player*)l->items[1])->pos);

	vec3 diff = f3_sub(f2_addz(med), c_game->look);

	vec dist = f2_sub(
		((player*)l->items[0])->pos,
		((player*)l->items[1])->pos);

	free(l);
	
	c_game->look = f3_add(c_game->look, diff);
	c_game->pov = f3_add(c_game->pov, diff);
	c_game->fov = max(10, min(40, f2_length(dist) / 1.25));

	//debug("%f\n", f2_length((vec) { dist.x, dist.y }));	

	cam_update(c_game);
}

void gameplay_camfollow()
{
	if (manager_count(PLAYER) == 0) return;

	if (! state.dual)
		gameplay_camfollow_single();
	else
		gameplay_camfollow_dual();
}

void gameplay_back_update(label* l)
{
	cm = c_ui;
	label_update(l);
	cm = c_game;
}

void gameplay_back()
{
	audio_play_force(fx_tap);
	change_scene(2);
	clear_fullinput();
}

void gameplay_update()
{
	if (started)
		state.time += delta * frame_target;

	state.score = state.score1 + state.score2;
	snprintf(s_aux, sizeof(s_aux), "SCORE: %i", state.score);
	label_change_text(label_score, s_aux);

	u16 m = state.time / 1000 / 60;
	u16 s = state.time  / 1000 - m * 60;

	snprintf(s_aux, sizeof(s_aux), "%02d:%02d", m, s);
	label_change_text(label_time, s_aux);

	gameplay_camfollow();
}

void gameplay_render()
{
	renderer_camera(c_game);
	renderer_shader(s_flat);	
}

scene* gameplay_alloc()
{
	scene* s = calloc(1, sizeof(scene));

	s->update = gameplay_update;
	s->prerender = shader_bg;
	s->render = gameplay_render;

	// choose camera/shader
	renderer_camera(c_game);
	renderer_shader(s_flat);

	renderer_nodepth();

	world_init();

	player_add();

	if (state.dual)
	{
		player_add();
		rope_add();
	}

	renderer_camera(c_ui);
	renderer_shader(s_flat);

	label_time = label_alloc(f_normal, "", (vec3){ 0, 8.5, 0 }, 1.0);
	manager_add(label_time, LABEL, 
		&label_update, &label_render, &label_free);

	snprintf(s_aux, sizeof(s_aux), "LEVEL: %i/%i", state.level, LEVEL_COUNT);
	manager_add(
		label_alloc(f_normal, s_aux, (vec3){ -4.0, 8.5, 0 }, 1.0),
		LABEL, &label_update, &label_render, &label_free);

	label_score = label_alloc(f_normal, "", (vec3){ 4.0, 8.5, 0 }, 1.0);
	manager_add(
		label_score,
		LABEL, &label_update, &label_render, &label_free);

	if (state.dual)
	{
		gunit* split1 = gunit_cpy(g_connector);
		
		split1->pos.y = 4;
		split1->mdl->scale.y = 8;
		split1->mdl->scale.x = .05;		

		gunit* split2 = gunit_cpy(split1);
		split2->pos.y = -4.10;
		
		manager_add(split1, UNKNOWN, NULL, &gunit_render, &gunit_free);
		manager_add(split2, UNKNOWN, NULL, &gunit_render, &gunit_free);
	}

	mountains_alloc();
	snow_alloc();

	label* label_back = label_alloc(f_normal, "back", (vec3) { 4.0, -8.25, 0 }, 1);
	label_back->enabled = true;
	label_back->click = gameplay_back;	

	manager_add(label_back, LABEL, &gameplay_back_update, &label_render, &label_free);

	renderer_camera(c_game);
	renderer_shader(s_flat);

	data_reset();

 	return s;
}