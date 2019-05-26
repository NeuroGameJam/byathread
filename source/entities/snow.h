#pragma once

void flock_free(flock* f)
{
	manager_remove(f);

	free(f);
	f = NULL;
}

void flock_update(flock* f)
{
	f->time += delta * frame_target;

	if (f->time > FLOCK_TIME)
	{
		f->time = 0;
		f->direction = !f->direction;
	}

	f32 percentage = f->time / FLOCK_TIME;	

	if (! f->direction)
		percentage = 1.0 - percentage;

	percentage = clamp(percentage, 0, 1);
	
	f->unit->rot += delta * .5;
	f->unit->mdl->scale = f3_uni(percentage / 10.0);
	f->unit->visible = percentage > 0 && percentage < 1;

	f->unit->pos = f->pos;
	f->unit->pos.x += sin(f->unit->rot / 10) / 100;
	f->unit->pos.y += cos(f->unit->rot / 10) / 100;
}

void flock_render(flock* f)
{
	gunit_render(f->unit);
}

flock* flock_alloc(const vec pos)
{
	flock* f = calloc(1, sizeof(flock));

	f->pos = pos;
	f->unit = gunit_cpy(g_snow);	
	f->unit->scale = F2_ZERO;

	f->time = rand() % FLOCK_TIME;
	f->direction = rand() % 2 == 1;

	f->unit->rot = rand() % 360;

	return f;
}

void flock_add(const vec pos)
{
	manager_add(
		flock_alloc(pos),
		UNKNOWN,
		&flock_update,
		&flock_render,
		&flock_free
		);
}

void snow_alloc()
{
	for (u8 h = 0; h < 10; h++)
	{
		vec pos = (vec)
		{
			-4 + rand() % 8,
			-3 - rand() % 5
		};

		flock_add(pos);
	}
}

void mountains_alloc()
{
	for (u8 h = 0; h < 10; h++)
	{
		vec pos = (vec)
		{
			-4 + rand() % 9,
			-7.5 - (rand() % 30) / 10.0
		};

		gunit* mountain = gunit_cpy(g_mountain);
		mountain->pos = pos;

		manager_add(
			mountain,
			UNKNOWN,
			NULL,
			&gunit_render,
			&gunit_free
			);
	}
}