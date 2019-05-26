#pragma once

#define MANAGER_BUFFER_CHUNK_SIZE 10

typedef struct
{
	void* ptr;
	u8 type;
	VOIDFUNCPARAM update;
	VOIDFUNCPARAM render;
	VOIDFUNCPARAM free;
} entity;

typedef struct
{
	entity* entities;
	u16 count;
	u16 allocated;
	u16 overallocated;
} manager;

void* manager_find_entity(manager* m, const u8 type)
{
	if (m == NULL || m->entities == NULL) return NULL;
	
	void* result = NULL;

	for (u16 h = 0; h < m->count; h++)
	{
		entity* e = &m->entities[h];
		
			if (e->ptr != NULL && e->type == type)
		{
			result = e->ptr;
			break;
		}
	}	

	return result;
}

list* manager_find_entities(manager* m, const u8 type)
{
	if (m == NULL) return NULL;

	list* result = calloc(1, sizeof(list));

	for (u16 h = 0; h < m->count; h++)
	{
		entity* e = &m->entities[h];
		
		if (e->ptr != NULL && e->type == type)
		{
			result->count++;

			if (result->count == 1)
				result->items = malloc(sizeof(void*));
			else
				result->items = realloc(
					result->items, 
					result->count * sizeof(void*));
						
			result->items[result->count - 1] = e->ptr;
		} 
	}	
  
	return result;
}

u16 manager_count_entities(manager* m, const u8 type)
{
	if (m == NULL) return 0;

	u16 result = 0;

	for (u16 h = 0; h < m->count; h++)
	{
		entity* e = &m->entities[h];
		
		if (e->ptr != NULL && e->type == type)
			result++;
	}	
  
	return result;
}

void manager_remove_entity(manager* m, const void* entity_ptr)
{
	if (m == NULL) return;

	for (u16 h = 0; h < m->count; h++)
	{
		entity* e = &m->entities[h];

		if (e->ptr == entity_ptr)
		{
			e->ptr = NULL;
			break;
		}
	}	
}

void manager_dynamic_resize(manager* m)
{
	if (m->allocated <= m->count)
	{
		m->allocated += MANAGER_BUFFER_CHUNK_SIZE;
		m->entities = realloc(m->entities, m->allocated * sizeof(entity));
		m->overallocated = 0;
	}
	else
	{
		if (m->allocated > m->count + MANAGER_BUFFER_CHUNK_SIZE)
		{
			m->overallocated++;

			if (m->overallocated > 60)
			{
				m->allocated -= MANAGER_BUFFER_CHUNK_SIZE;
				m->entities = realloc(m->entities, m->allocated * sizeof(entity));
				m->overallocated = 0;
			}
		}
		else m->overallocated = 0;
	}
}

void manager_internal_update(manager* m)
{
	if (m == NULL) return;

	for (u16 h = 0; h < m->count; h++)
	{
		entity* e = &m->entities[h];
		
		if (e->ptr != NULL && e->update != NULL)
			e->update(e->ptr);
	}

	manager_dynamic_resize(m);
}

void manager_internal_render(manager* m)
{
	if (m == NULL) return;

	for (u16 h = 0; h < m->count; h++)
	{
		entity* e = &m->entities[h];
		
		if (e->ptr != NULL && e->render != NULL)
			e->render(e->ptr);
	}
}

void manager_remove_all(manager* m)
{
	if (m == NULL) return;

	for (u16 h = 0; h < m->count; h++)
	{
		entity* e = &m->entities[h];

		if (e->ptr != NULL && e->free != NULL)
			e->free(e->ptr);

		e->ptr = NULL;
	}
}

void manager_add_entity(manager* m, 
	void* entity_ptr, 
	const u8 type,
	void(* update)(void*),
	void(* render)(void*),
	void(* free)(void*)
	)
{
	if (m->count >= m->allocated)
		manager_dynamic_resize(m);

	entity* n = NULL;

	for (u16 h = 0; h < m->count; h++)
	{
		if ((&m->entities[h])->ptr == NULL)
		{
			n = &m->entities[h];
			break;
		}
	}

	if (n == NULL)
	{
		n = &m->entities[m->count];
		m->count++;
	}	

	n->ptr = entity_ptr;
	n->type = type;
	n->update = update;
	n->render = render;
	n->free = free;
}

bool manager_contains_entity(manager* m, const void* entity_ptr)
{
	bool result = false;

	for (u16 h = 0; h < m->count; h++)
	{
		entity* e = &m->entities[h];
		
		if (e->ptr != NULL && e->ptr == entity_ptr)
		{
			result = true;
			break;
		}
	}	

	return result;	
}

////////////////////////////////////////////////////////////////////////////////
// helpers

manager* mng = NULL;

void manager_free()
{
	if (mng == NULL) return;

	manager_remove_all(mng);

	free(mng->entities);
	free(mng);
	mng->entities = NULL;
	mng = NULL;
}

void manager_alloc()
{
	mng = calloc(1, sizeof(manager));

	mng->allocated = MANAGER_BUFFER_CHUNK_SIZE;
	mng->count =  0;

	mng->entities = calloc(mng->allocated, sizeof(entity));
}

void manager_update()
{
	if (mng == NULL) return;
	manager_internal_update(mng);
}

void manager_render()
{
	if (mng == NULL) return;
	manager_internal_render(mng);
}

void manager_add(
	void* entity_ptr, 
	const u8 type,
	void(* update)(void*),
	void(* render)(void*),
	void(* free)(void*))
{
	if (mng == NULL) return;
	manager_add_entity(mng, entity_ptr, type, update, render, free);
}

void manager_remove(const void* entity_ptr)
{
	if (mng == NULL) return;
	manager_remove_entity(mng, entity_ptr);
}

void* manager_find(const u8 type)
{
	if (mng == NULL) return NULL;
	return manager_find_entity(mng, type);
}

void manager_clear()
{
	if (mng == NULL) return;
	manager_remove_all(mng);	
}

list* manager_list(const u8 type)
{
	if (mng == NULL) return NULL;
	return manager_find_entities(mng, type);
}

u8 manager_count(const u8 type)
{
	if (mng == NULL) return 0;
	return manager_count_entities(mng, type);
}

bool manager_contains(const void* entity_ptr)
{
	if (mng == NULL) return false;
	return manager_contains_entity(mng, entity_ptr);
}