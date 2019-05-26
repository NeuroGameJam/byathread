#pragma once

const vec BUTTON_OFFSET = { .1, -.1 };

typedef struct
{
	bool visible;
	bool enabled;
	bool down;
	//u8 touch_id;

	vec pos;

	tex* tex;
	model* model;

	char key;
	vec offset;

	VOIDFUNC click;
	VOIDFUNC pressing;
} button;

void button_free(button* btn)
{	
	manager_remove(btn);
	free(btn->model);

	tex_unload(btn->tex);
	tex_free(btn->tex);

	free(btn);
	btn = NULL;
}

bool button_has_touch(button* btn)
{
	vec target = cam_unproject(cm, (vec3) { btn->pos.x, btn->pos.y, 0 });

	return touching_at(target);
}

bool button_was_touched(button* btn)
{
	vec target = cam_unproject(cm, (vec3) { btn->pos.x, btn->pos.y, 0 });

	return touched_at(target);
}

void button_update(button* btn)
{
	// keep it updating or position won't occour
	// if (! btn->visible) return;

	if (btn->visible && btn->enabled)
	{
		if (key_down(btn->key) || button_has_touch(btn))
		{
			btn->down = true;
			if (btn->pressing != NULL)
				btn->pressing();
		}
		else if (btn->down || key_up(btn->key))
		{			
			btn->down = false;

			if ((key_up(btn->key) || button_was_touched(btn)) && 
				btn->click != NULL)
				btn->click();
		}
	}
	
	btn->model->pos = (vec3) { btn->pos.x, btn->pos.y, 0 }; 

	if (btn->down)
	{
		btn->model->pos.x += btn->offset.x;
		btn->model->pos.y += btn->offset.y;
	}
}

void button_render(button* btn)
{
	if (! btn->visible)
		return;

	model_render(btn->model);
}

button* button_alloc(const char* filename, const vec pos)
{
	button* btn = malloc(sizeof(button));

	btn->visible = true;
	btn->enabled = true;
	btn->down = false;
	btn->pos = pos;

	btn->tex = tex_load(filename);

	btn->model = model_cpy(m_frame);

	btn->model->scale = f3_uni(.75);
	btn->model->texture_id = btn->tex->id;	
	
	btn->click = NULL;
	btn->pressing = NULL;

	btn->offset = BUTTON_OFFSET;

	return btn;
}