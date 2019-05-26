#pragma once

void rope_update(rope* r)
{
	list* l = manager_list(PLAYER);

	vec med = f2_med(
		((player*)l->items[0])->pos,
		((player*)l->items[1])->pos);

	vec dist = f2_sub(
		((player*)l->items[0])->pos,
		((player*)l->items[1])->pos);

	dist.x += PLAYER_OFFSET;

	free(l);

	f32 len = fabs(dist.y);

	r->pos = med;
	r->mdl->scale.y = len;
	r->mdl->scale.x = 10;
}

rope* rope_alloc()
{
	return gunit_cpy(g_rope);
}

void rope_add()
{
	manager_add(
		rope_alloc(),
		UNKNOWN,
		&rope_update,
		&gunit_render,
		&gunit_free
		);
}