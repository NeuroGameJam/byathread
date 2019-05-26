#pragma once

void player_free(player* p)
{
	manager_remove(p);

	free(p);
	p = NULL;
}

void player_finished()
{
	bool go_to_stats = true;

	if (state.dual)
	{
		list* l = manager_list(PLAYER);

		for (u8 h = 0; h < l->count; h++)
		{
			player* e = l->items[h];
			if (! e->finished)
			{
				go_to_stats = false;
				break;
			}
		}

		free(l);
	}

	if (go_to_stats)
		change_scene(4);	
}

f32 player_offset(player* p)
{
	if (state.dual)
	{
		if (p->id == 1)
			return -PLAYER_OFFSET;
		else
			return PLAYER_OFFSET;
	}
	else
		return 0;
}

void player_collisions(player* p)
{
	list* l = manager_list(ENEMY);

	gunit* g = p->unit;

	for (u16 h = 0; h < l->count; h++)
	{
		enemy* e = l->items[h];

		if (! e->enabled) continue;

		gunit* u = e->unit;

		collision r = collision_test(&g->collider, &u->collider, F2_ZERO);

		if (r.intersect)
		{
			g->pos.x += r.min.x;
			g->pos.y += r.min.y;

			g->collided = true;
			break;
		}
	}

	list_free(l);	
}

void player_wings(player* p)
{
	p->wtime += delta * frame_target / 2;

	if (p->wtime > WINGS_TIME)
	{
		p->wtime = 0;
		p->wdirection = !p->wdirection;
	}

	f32 percentage = p->wtime / WINGS_TIME;	

	if (! p->wdirection)
		percentage = 1.0 - percentage;

	percentage = clamp(percentage, .30, .50);	
	
	p->lwing->mdl->scale.x = percentage;
	p->lwing->visible = percentage > 0 && percentage < 1;

	p->rwing->mdl->scale.x = p->lwing->mdl->scale.x;
	p->rwing->visible = p->lwing->visible;
}

void player_update(player* p)
{	
	if (p->time == 0)
	{
		if (! p->finished)
		{	
			bool move = ! state.dual && (key_any || touched());
			
			if (! move && state.dual)
			{
				move = 
					touched_half(p->id == 2) ||
					(key_up(VK_LEFT) && p->id == 1) ||
					(key_up(VK_RIGHT) && p->id == 2);

				list* l = manager_list(PLAYER);

				u8 min = fmin(
					((player*)l->items[0])->current, 
					((player*)l->items[1])->current);

				u8 max = fmax(
					((player*)l->items[0])->current, 
					((player*)l->items[1])->current);

				u8 diff = max - min;

				free(l);

				move &= diff <= 2 || min == p->current;;
			}

			if (move)
			{
				started = true;
				p->start = p->pos;
				p->target = world_find_next(p->pos.y);
				p->time = MOVE_TIME;
			}

			//debug("start %f %f\n", p->start.x, p->start.y);
			//debug("target %f %f\n", p->target.x, p->target.y);
		}
	}
	else
	{
		p->time -= delta * frame_target;

		f32 percentage = 1.0 - p->time / MOVE_TIME;
		
		p->pos = f2_lerp(p->start, p->target, percentage);

		if (! p->unit->collided)
		{
			p->unit->pos = p->pos;
			gunit_collider(p->unit);
			player_collisions(p);

			if (p->unit->collided)
			{
				enemies_enable(p->target);

				p->target = p->start;
				p->start = p->pos;
				p->time = MOVE_TIME;

				audio_play_force(fx_bad_hit);
			}
		}

		if (p->time < 0)
		{
			emitter* em = emitter_alloc(f2_addz(p->pos));
			manager_add(
				em,
				EMITTER,
				&emitter_update,
				&emitter_render,
				&emitter_free);

			if (! p->unit->collided)
			{
				p->score += 2;
				p->current++;
				connector_add(p->start, p->target, player_offset(p));
				
				if (p->target.y == LEVEL_SPACING.y * (LEVEL_SIZE - 1))
				{					
					p->finished = true;					
					em->end = player_finished;

					audio_play_force(fx_win);
				}
				else
				{
					if (! state.dual)
					{
						if (rand() % 2 == 0)
							audio_play_force(fx_normal_hit1);
						else
							audio_play_force(fx_normal_hit2);
					}
					else if (p->id == 1)
						audio_play_force(fx_normal_hit1);
					else
						audio_play_force(fx_normal_hit2);
				}
			}
			else if (p->score > 2)
				p->score -= 3;
			else
				p->score =  0;

			switch (p->id)
			{
				case 1: state.score1 = p->score; break;
				case 2: state.score2 = p->score; break;
			}

			p->pos = p->target;
			p->time = 0;
			p->unit->collided = false;
			
			//debug("at target\n");
			//debug("target %f %f\n", p->target.x, p->target.y);

			list* l = manager_list(PLAYER);
			bool disable = true;
			for (u8 h = 0; h < l->count; h++)
			{
				player* e = l->items[h];
				if (e->pos.y < p->pos.y)
				{
					disable = false;
					break;
				}
			}

			free(l);	

			if (disable)		
				enemies_disable(p->target);
		}
	}

	player_wings(p);
}

void player_render(player* p)
{
	p->unit->pos = p->pos;
	p->unit->pos.x += player_offset(p);
	gunit_render(p->unit);

	p->lwing->pos = p->pos;
	p->lwing->pos.x += player_offset(p);
	p->lwing->pos.x -= PLAYER_WING;
	gunit_render(p->lwing);

	p->rwing->pos = p->pos;
	p->rwing->pos.x += player_offset(p);
	p->rwing->pos.x += PLAYER_WING;
	gunit_render(p->rwing);
}

player* player_alloc()
{
	player* p = calloc(1, sizeof(player));

	p->id = 1;
	if (state.dual && manager_count(PLAYER) > 0)
		p->id = 2;

	p->unit = gunit_cpy(g_actor);
	p->lwing = gunit_cpy(g_lwing);
	p->rwing = gunit_cpy(g_rwing);

	return p;
}

void player_add()
{
	manager_add(
		player_alloc(),
		PLAYER,
		&player_update,
		&player_render,
		&player_free
		);
}