#pragma once

void connector_free(connector* c)
{
	manager_remove(c);

	free(c);
	c = NULL;	
}

void connector_update(connector* c)
{
	if (c->time > 0)
	{
		c->time -= delta * frame_target;

		f32 percentage = 1.0 - c->time / (MOVE_TIME * 2);

		if (percentage >= 1)
			percentage = 1;
		
		c->pos = f2_lerp(c->start, c->target, (percentage));
		
		f32 len = f2_length(f2_sub(c->target, c->start));

		c->unit->mdl->scale.y = percentage * len * 2;
		c->unit->visible = true;
	}
}

void connector_render(connector* c)
{
	renderer_camera(c_game);

	c->unit->pos = c->pos;
	c->unit->pos.x += c->offset;
	
	gunit_render(c->unit);
}

connector* connector_alloc(const vec start, const vec target, const f32 offset)
{
	connector* c = calloc(1, sizeof(connector));

	//debug("add connector start %f %f\n", start.x, start.y);
	//debug("add connector target %f %f\n", target.x, target.y);

	c->start = start;
	c->target = f2_div_f(f2_add(start, target), 2.0);
	c->pos = start;
	c->offset = offset;
	c->time = MOVE_TIME * 2;

	c->unit = gunit_cpy(g_connector);
	c->unit->visible = false;
	c->unit->rot = from_direction(f2_sub(target, start));

	return c;
}

void connector_add(const vec start, const vec target, const f32 offset)
{
	manager_add(
		connector_alloc(start, target, offset),
		CONNECTOR,
		&connector_update,
		&connector_render,
		&connector_free
		);
}