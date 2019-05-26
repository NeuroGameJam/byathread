#pragma once

void model_free(model* m)
{
	if (m == NULL) return;

    if (m->original)
    {
    	if (m->uv_id > 0)
    	{
	    	u32 id = m->uv_id;
	    	glDeleteTextures(1, &id);
    	}

    	// release vbo
		if (m->vbo[0] > 0) glDeleteBuffers(1, &m->vbo[0]);
		if (m->vbo[1] > 0) glDeleteBuffers(1, &m->vbo[1]);
		if (m->vbo[2] > 0) glDeleteBuffers(1, &m->vbo[2]);
    }

    free(m);
    m = NULL;
}

void mesh_alloc_vbo(u32* vbo, vec3* data, u16 count)
{
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(vec3) * count, 
        data, GL_STATIC_DRAW);	
}

u8* model_alloc_mesh(model* m, u8* data)
{	
    // vertices
    memcpy(&m->vertex_count, data, sizeof(u16));    
    data += sizeof(u16);

    if (m->vertex_count)
    {
	    mesh_alloc_vbo(&m->vbo[0], (vec3*)data, m->vertex_count);	    
	    data += m->vertex_count * sizeof(vec3);

	    memcpy(&m->size, data, sizeof(vec3));    
	    data += sizeof(vec3);
    }

    // texture coordinates
    memcpy(&m->texcoord_count, data, sizeof(u16));
    data += sizeof(u16);

    if (m->texcoord_count)
    {
    	mesh_alloc_vbo(&m->vbo[1], (vec3*)data, m->texcoord_count);
	    data += m->texcoord_count * sizeof(vec3);
    }

    // normals
    memcpy(&m->normal_count, data, sizeof(u16));
    data += sizeof(u16);

    if (m->normal_count)
    {
    	mesh_alloc_vbo(&m->vbo[2], (vec3*)data, m->normal_count);
	    data += m->normal_count * sizeof(vec3);
    }

	return data;
}

void model_alloc_material(model* m, u8* data)
{
    u16 color_count;

    memcpy(&color_count, data, sizeof(u16));
    data += sizeof(u16);

    if (color_count)
    {
	    m->texture_id = 
		m->uv_id = create_colors_texture(color_count, (color*)data);

		data += color_count * sizeof(color);
    }
}

model* model_alloc(const char* filename)
{
	model* m = calloc(1, sizeof(model));

	m->visible = true;
	m->original = true;

    holder* dh = load(filename);
    u8* data = dh->data;    

    // 1. mesh
    data = model_alloc_mesh(m, data);

    // 2. materials
    model_alloc_material(m, data);

	free(dh->data);
	free(dh);

	return m;
}

model* model_cpy(const model* m)
{
	model* copy = malloc(sizeof(model));
	memcpy(copy, m, sizeof(model));

	copy->original = false;

	return copy;
}
