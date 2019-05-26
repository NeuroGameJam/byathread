#pragma once

vec world_find_next(const f32 y)
{
	vec result = F2_ZERO;

	list* l = manager_list(SPOT);

	for (u8 h = 0; h < l->count; h++)
	{
		gunit* spot = l->items[h];

		if (spot->pos.y > y)
		{
			result = spot->pos;
			break;
		}
	}
	
	free(l);

	return result;
}

void enemies_state(const vec pos, const bool enable)
{
	list* l = manager_list(ENEMY);

	for (u16 h = 0; h < l->count; h++)
	{
		enemy* e = l->items[h];

		if (f2_equals(pos, e->start))
		{
			enemy_state(e, enable);
		}
	}
	
	free(l);
}

void enemies_disable(const vec pos)
{
	enemies_state(pos, false);
}


void enemies_enable(const vec pos)
{
	enemies_state(pos, true);
}

void spot_add(const vec pos)
{
	gunit* s = gunit_cpy(g_spot);
	
	s->pos = pos;

	manager_add(s, SPOT,
		NULL, 
		&gunit_render,
		&gunit_free);	
}

void enemies_add(const vec pos)
{	
	enemy_add(pos, MIN_ENEMY_RADIUS, rand() % 360);
}

void world_init()
{
	u8 count = LEVEL_SIZE; // default - start, end

	// TODO - between start and end add option
	// rand() - higher level, more options
	
	i16 x = 0;
	i16 y = 0;
	vec pos;

	for (u8 h = 0; h < count; h++)
	{
		pos = (vec) { x, y };
		pos = f2_mul(pos, LEVEL_SPACING);

		spot_add(pos);

		if (h > 0)
		{
			enemies_add(pos);

			for (u8 h = 0; h < 4; h++)
				if (rand() % 10 < state.level) enemies_add(pos);
		}

		x += (-1 + rand() % 3);
		y += 1;
	}
}
