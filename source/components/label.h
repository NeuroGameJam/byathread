#pragma once

#define LABEL_MAX 256

const vec LABEL_OFFSET = { .1, -.1 };

typedef struct
{
	font* font;
	char* text;
	char* oldtext;	

	size size;
	f32 scale;
	f32 rot;
	vec3 pos;
	bool visible;
	bool enabled;

	tex* tex;
	model* model;

	char key;
	bool down;
	VOIDFUNC click;
	VOIDFUNC pressing;

	shader* sh;
	cam* cm;
} label;

void label_free(label* l)
{
	if (l == NULL) return;
	
	manager_remove(l);
	
	free(l->text);
	free(l->oldtext);

	model_free(l->model);
	tex_free(l->tex);

	free(l);
	l = NULL;
}

bool label_has_touch(label* l)
{
	vec target = cam_unproject(cm, (vec3) { l->pos.x, l->pos.y, 0 });

	return touching_at(target);
}

bool label_was_touched(label* l)
{
	vec target = cam_unproject(cm, (vec3) { l->pos.x, l->pos.y, 0 });

	return touched_at(target);
}

void label_update_texture(label* l) // for main thread
{
	if (strcmp(l->text, l->oldtext) == 0)
		return;

	memcpy(l->oldtext, l->text, LABEL_MAX);

	l->size = font_measure(l->font, l->text);

	img* i = img_alloc(l->size);

	font_render(l->font, l->text, i);


	if (l->tex == NULL)
	{
		l->tex = tex_alloc(i);

		if (l->tex->id == 0)
			debug("alloc tex %d text => |%s|\n", l->tex->id, l->text);
	}		
	else
	{
		tex_update(l->tex, i);

		if (l->tex->id == 0)
			debug("update tex %d text => |%s|\n", l->tex->id, l->text);
	}		

	img_free(i);

	l->model->scale = (vec3)
	{ 
		(l->size.w / 200.f) * l->scale,
		1,
		(l->size.h / 200.f) * l->scale
	};

	//debug("label %s %f %f\n", l->text, l->size.w, l->size.h);	

	if (l->tex->id == 0)
		debug("tex %d text => |%s|\n", l->tex->id, l->text);
}

void label_change_text(label* l, const char* text)
{
	if (strlen(text) > 0 && strcmp(l->text, text) == 0)
		return;

	memset(l->text, '\0', LABEL_MAX);
	
	// max chars
	u16 len = strlen(text);
	if (len > LABEL_MAX - 1) len = LABEL_MAX - 1; // keep last \0
	
	memcpy(l->text, text, len);
}

void label_update(label* l)
{
	// keep it updating or position won't occour ??
	if (! l->visible) return;

	l->model->pos = (vec3) { l->pos.x, l->pos.y, l->pos.z };	

	if (l->enabled)
	{
		if (key_down(l->key) || label_has_touch(l))
		{
			l->down = true;
			if (l->pressing != NULL)
				l->pressing();
		}
		else if (l->down || key_up(l->key))
		{			
			l->down = false;

			if ((key_up(l->key) || label_was_touched(l)) && 
				l->click != NULL)
				l->click();
		}

		if (l->down)
		{
			l->model->pos.x += LABEL_OFFSET.x;
			l->model->pos.y += LABEL_OFFSET.y;
		}			
	}

	label_update_texture(l);
}

void label_render(label* l)
{
	if (! l->visible)
		return;
		
	if (l->tex == NULL)
		return;

	l->model->rot.x = l->rot;
	l->model->texture_id = l->tex->id;

	renderer_shader(l->sh);
	renderer_camera(l->cm);
	model_render(l->model);
}

label* label_alloc(
	font* font,
	const char* text, 
	const vec3 pos, 
	const f32 scale)
{
	label* l = calloc(1, sizeof(label));

	l->text = calloc(LABEL_MAX, 1);
	l->oldtext = calloc(LABEL_MAX, 1);

	l->visible = true;
	l->enabled = false;
	l->font = font;
	l->pos = pos;
	l->scale = scale;
	l->rot = 90;

	l->model = model_cpy(m_plane);

	label_change_text(l, text);

	l->sh = sh;
	l->cm = cm;

	return l;
}