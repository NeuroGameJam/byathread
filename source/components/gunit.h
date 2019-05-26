#pragma once

#define GUNIT 255

typedef struct
{
	bool original;
	vec pos;
	f32 rot;
	bool collidable;
	bool visible;

	tex* tex;
	model* mdl;

	vec scale; // = size
	quad bounds;
	collider collider;	

	bool collided;
} gunit;

void gunit_free(gunit* g)
{
	manager_remove(g);

	model_free(g->mdl);

	if (g->original)
		free(g->tex);

	free(g);
	g = NULL;
}

void gunit_collider(gunit* g)
{	
	g->bounds = calculate_bounds(
		g->pos, 
		g->rot,
		g->scale,
		F2_ONE);

	g->collider = collider_from_quad(g->bounds);
}

void gunit_collisions(gunit* g)
{	
	if (!g->visible || !g->collidable)
		return;

	// update collider
	gunit_collider(g);	

	list* l = manager_list(GUNIT);

	g->collided = false;

	for (u16 h = 0; h < l->count; h++)
	{
		gunit* u = l->items[h];

		if (u == g || !u->visible || !u->collidable) continue;

		collision r = collision_test(&g->collider, &u->collider, F2_ZERO);

		if (r.intersect)
		{
			g->pos.x += r.min.x;
			g->pos.y += r.min.y;

			g->collided = true;
		}
	}

	list_free(l);
}

void gunit_render(gunit* g)
{
	if (!g->visible)
		return;

	g->mdl->pos.x = g->pos.x;
	g->mdl->pos.y = g->pos.y;
	g->mdl->rot.z = g->rot;

	model_render(g->mdl);
}

void gunit_size(gunit* g)
{
	g->mdl->scale.x = g->tex->size.w / 100;
	g->mdl->scale.y = g->tex->size.h / 100;

	g->scale = (vec) { g->mdl->scale.x, g->mdl->scale.y };
}

gunit* gunit_alloc(const char* filename)
{
	gunit* g = calloc(1, sizeof(gunit));
	
	g->mdl = model_cpy(m_frame);
	g->tex = tex_load(filename);

	g->original = true;
	g->visible = true;
	g->mdl->texture_id = g->tex->id;

	//scale from img size
	gunit_size(g);

	return g;
}

gunit* gunit_cpy(const gunit* original)
{
	gunit* copy = malloc(sizeof(gunit));
	memcpy(copy, original, sizeof(gunit));

	copy->mdl = model_cpy(original->mdl);
	copy->tex = tex_cpy(original->tex);

	copy->original = false;	

	return copy;
}
