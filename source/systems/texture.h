#pragma once

u16 tex_upload(img* i)
{
    u32 id = 0;

	glGenTextures(1, &id); // Generate Pointer to the texture

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        i->size.w,
        i->size.h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        i->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Unbind current texture
    glBindTexture(GL_TEXTURE_2D, 0);

	debug("[TEX ID %i] Loaded texture to GPU (%fx%f)\n", id, 
		i->size.w, i->size.h);

    return (u16)id;
}

u16 create_colors_texture(const u16 color_count, const color* colors)
{
	u16 texture_size = 100;

	img* img = img_alloc((size) { texture_size * color_count, texture_size });

    for (u16 h = 0; h < color_count; h++)
    {
        color c = colors[h];

        for (u16 x = 0; x < texture_size; x++)
            for (u16 y = 0; y < texture_size; y++)
            	img_color(img, h * texture_size + x, y, c);
    }

    u16 result = tex_upload(img);

    img_free(img);

    return result;
}

u16 create_color_texture(const color c)
{
	img* img = img_alloc((size) { 1, 1 });

	img_color(img, 0, 0, c);

    u16 result = tex_upload(img);

    img_free(img);

    return result;
}

/////

void tex_unload_by_id(u32 id)
{
    if (id != 0)
    {
        glDeleteTextures(1, &id);

        //debug("[TEX ID %i] Unloaded texture data from VRAM (GPU)\n", id);     
    }
}

void tex_unload(tex* t)
{
    if (t->id != 0)
    {
    	tex_unload_by_id(t->id);

        debug("[TEX ID %i] Unloaded texture data from VRAM (GPU)\n", t->id);
        t->id = 0;
    }
}

void tex_free(tex* t)
{
	if (t == NULL) return;

	if (t->original)
		tex_unload(t);

	free(t);
	t = NULL;
}

void tex_from_img(tex* t, img* i)
{
	t->size = i->size;
	t->id = tex_upload(i);
	t->original = true;
}

// same id is not guaranteed
void tex_update(tex* t, img* i)
{
	tex_unload(t);
	t->id = tex_upload(i);

	t->size = i->size;
}

tex* tex_load(const char* filename)
{
	tex* t = calloc(1, sizeof(tex));

	img* i = img_load(filename);

	tex_from_img(t, i);

	img_free(i);

	return t;
}

tex* tex_alloc_color(const color c)
{
	tex* t = calloc(1, sizeof(tex));

	t->id = create_color_texture(c);
	t->original = true;

	return t;
}

tex* tex_alloc_colors(const u16 color_count, const color* colors)
{
	tex* t = calloc(1, sizeof(tex));

	t->id = create_colors_texture(color_count, colors);
	t->original = true;

	return t;
}

tex* tex_alloc(img* i)
{
	tex* t = calloc(1, sizeof(tex));

	tex_from_img(t, i);

	return t;
}

tex* tex_cpy(const tex* t)
{
	tex* copy = malloc(sizeof(tex));
	memcpy(copy, t, sizeof(tex));

	copy->original = false;

	return copy;
}
