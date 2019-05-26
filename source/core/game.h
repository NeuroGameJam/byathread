#pragma once

u8 next_scene;
bool has_new_scene;	
scene* current = NULL;

void scene_alloc(); // forward declaration

void game_free_current_scene()
{
	if (current != NULL)
	{
		if (current->free != NULL)
			current->free();

		free(current);
		current = NULL;
	}
}

void game_free()
{	
	game_free_current_scene();
    
	manager_free();
	renderer_free();
	audio_free();    
    common_free();
    loader_free(); 
}

void change_scene(const u8 scene)
{  
    if (has_new_scene)
        return;

    next_scene = scene;
    has_new_scene = true;
}

void internal_next_scene()
{
	manager_clear();    

	game_free_current_scene();

    // allocate/init new scene
    clear_input();
    scene_alloc();

    has_new_scene = false;
    next_scene = 0;
}

void game_update()
{            
    if (current != NULL && ! has_new_scene)
    {
    	manager_update();

    	if (current->update != NULL)
        	current->update();
    }

    clear_input();
}

void game_render()
{
    if (current != NULL)
    {
    	renderer_clear();

    	if (current->prerender != NULL)
    		current->prerender();

    	manager_render();

    	if (current->render != NULL)
    		current->render();

    	if (current->posrender != NULL)
    		current->posrender();
    }
}

scene* game_alloc()
{
	scene* s = calloc(1, sizeof(scene));

	s->update = game_update;
	s->render = game_render;
	s->free = game_free;
	
	renderer_alloc();
	audio_alloc();
	manager_alloc();	

	// after starting for example sound
	loader_alloc();
	common_alloc();	

    has_new_scene = false;
    next_scene = 0;
    change_scene(1);

	return s;
}