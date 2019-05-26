#pragma once

#define LOADING_TIME 4000

f32 l_timer = 0;
u8 l_step = 0;

char* text0 = "Butterflies are deep and\npowerful representations\nof life.";
char* text1 = "Many cultures associate the\nbutterfly with our souls.";
char* text2 = "Their presence is the reflection\nof the local environment.";
char* text3 = "Butterfly means\n    endurance\n      change\n       hope\n        life.";
char* text4 = "... this is a game\n... no butterflies were harmed\n\n        build a wall\n\n            of information.";
char* text5 = "we're \"by a thread\"\nsync with NATURE!";

void loading_step();

void loading_text(const char* text)
{
	manager_add(
		label_alloc(f_normal, text, F3_ZERO, 1.75),
		UNKNOWN, &label_update, &label_render, &label_free);	
}

void loading_skip()
{
	l_step = 6;
	audio_play_force(fx_tap);
	loading_step();
}

void loading_step()
{
	manager_clear();
	l_timer = 0;

	if (l_step < 6)
	{
		label* label_skip = label_alloc(f_normal, "skip", (vec3) { 4.0, -8.25, 0 }, 1);
		label_skip->click = loading_skip;
		label_skip->enabled = true;

		manager_add(
			label_skip,
			UNKNOWN, &label_update, &label_render, &label_free);
	}

	switch (l_step)
	{
		case 0: loading_text(text0); break;
		case 1: loading_text(text1); break;
		case 2: loading_text(text2); break;
		case 3: loading_text(text3); break;
		case 4: loading_text(text4); break;
		case 5: loading_text(text5); break;
		case 6:
			current->prerender = shader_bg;

			manager_add(
				label_alloc(f_normal, APP_NAME, (vec3) { 0, 6, 0 }, 2),
				UNKNOWN, &label_update, &label_render, &label_free);

			manager_add(
				label_alloc(f_normal, " a game by raul.rita\nfx/audio by joao.costa", 
				(vec3) { 0, -8, 0 }, 1.0),
				UNKNOWN, &label_update, &label_render, &label_free);			
		break;
		case 7:
			change_scene(2);
		break;
	}
}

void loading_update()
{
	l_timer += delta * frame_target;

	if (key_any || touched() || l_timer > LOADING_TIME)
	{
		l_step++;
		loading_step();
	}
}

scene* loading_alloc()
{
	scene* s = calloc(1, sizeof(scene));

	s->update = loading_update;	

	// choose camera/shader
	renderer_camera(c_ui);
	renderer_shader(s_flat);

	loading_step();

	glDisable(GL_DEPTH_TEST);

 	return s;
}