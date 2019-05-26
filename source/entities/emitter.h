#pragma once

// PARTICLES
#define DISPERSION .6f
#define MAX_TTL 1000
#define INITIAL_BURST rand() % 100 / 10

typedef struct
{
    f32 ttl;

    vec3 scale;
    vec3 pos;
    vec3 speed;
    vec3 rot;
} particle;

typedef struct
{
	bool ended;
    u16 count;

    particle* particles;   
    gunit* unit;
    VOIDFUNC end;
} emitter;

void emitter_add_particle(emitter* e, const vec3 pos)
{
	particle* p = &e->particles[e->count];

	p->speed = (vec3) 
	{
	    DISPERSION * (f32)(rand() % 100) / 100.f / 2 * (rand() % 2 == 1 ? 1 : -1),
	    - DISPERSION * (f32)(rand() % 100) / 100.f / 2 * (rand() % 2 == 1 ? 1 : -1),
	    DISPERSION * (f32)(rand() % 100) / 100.f / 2 * (rand() % 2 == 1 ? 1 : -1)
	};

	f32 s = (f32)(1 + rand() % 4) / 50;
    
    p->scale = (vec3) { s, s, s };
    p->pos = pos;
    p->ttl = 20 + rand() % MAX_TTL;
    p->pos = f3_add(p->pos, f3_mul_f(p->speed, INITIAL_BURST));

	e->count++;
}

void emitter_update(emitter* e)
{	
	if (e->ended)
		return;

	u16 finished = 0;

	for (u16 h = 0; h < e->count; h++)
	{
		particle* p = &e->particles[h];

		if (p->ttl <= 0)
		{
			finished++;
			continue;
		}
		
        p->pos = f3_add(p->pos, p->speed);
        p->rot.x += delta;
        p->rot.y += delta;
        p->rot.z += delta;

        p->ttl -= delta * frame_target;
	}

	e->ended = finished == e->count;
	if (e->ended && e->end != NULL)
		e->end();
}

void emitter_render(emitter* e)
{
	if (e->ended)
		return;

	renderer_camera(c_game);

	for (u16 h = 0; h < e->count; h++)
	{
		if (e->particles[h].ttl <= 0)
			continue;

		e->unit->pos = f3_dropz(e->particles[h].pos);
		e->unit->mdl->scale = e->particles[h].scale;
		e->unit->mdl->rot = e->particles[h].rot;
		e->unit->mdl->scale = e->unit->mdl->scale;

 		gunit_render(e->unit);
	}
}

void emitter_free(emitter* e)
{	
	manager_remove(e);
	
	free(e->unit);
	free(e->particles);
	free(e);

	e = NULL;
}

emitter* emitter_alloc(const vec3 pos)
{
 	u8 max = 255;

	emitter* e = calloc(1, sizeof(emitter));

	e->particles = calloc(max, sizeof(particle));	

	for (u16 h = 0; h < max; h++)
		emitter_add_particle(e, pos);

	e->unit = gunit_cpy(g_snow);

	return e;
}