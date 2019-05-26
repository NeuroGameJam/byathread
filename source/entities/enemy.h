#pragma once

void enemy_free(enemy* e)
{
	manager_remove(e);

	free(e);
	e = NULL;
}

void enemy_state(enemy* e, const bool state)
{
	e->enabled = state;
}

void enemy_update(enemy* e)
{
	if (! e->enabled) return;

	e->pos = e->start;
	e->pos.x += e->radius;
	rotate(&e->pos, e->start, to_radians(e->angle));

	e->angle += delta * e->speed * (e->direction ? -1.0 : 1.0);
	e->angle = angular_wrap(e->angle);
	
	e->unit->pos = e->pos;
	e->unit->scale = f2_add(e->unit->scale, F2_ONE);
	gunit_collider(e->unit);
	e->unit->scale = f2_sub(e->unit->scale, F2_ONE);
}

void enemy_render(enemy* e)
{
	if (! e->enabled) return;
	
	gunit_render(e->unit);	
}

enemy* enemy_alloc(const vec pos, const f32 radius, const f32 angle)
{
	enemy* e = calloc(1, sizeof(enemy));

	e->start = pos;
	e->pos = pos;
	e->radius = radius;
	e->angle = angle;
	e->speed = state.level * .75f;
	e->direction = rand() % 2 == 1;
	e->enabled = true;

	e->unit = gunit_cpy(g_bad);

	return e;
}

void enemy_add(const vec pos, const f32 radius, const f32 angle)
{
	manager_add(
		enemy_alloc(pos, radius, angle),
		ENEMY,
		&enemy_update,
		&enemy_render,
		&enemy_free
		);
}