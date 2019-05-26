#pragma once

void img_color(img* i, const u16 x, const u16 y, const color c)
{
	u32 dpos = x * 4 + y * (u16)i->size.w * 4;

	i->pixels[dpos] = c.r;
	i->pixels[dpos + 1] = c.g;
	i->pixels[dpos + 2] = c.b;
	i->pixels[dpos + 3] = c.a;
}

void img_black_alpha(img* i, const u16 x, const u16 y, const f32 a)
{
	u32 dpos = x * 4 + y * i->size.w * 4;
	i->pixels[dpos] = 0;
	i->pixels[dpos + 1] = 0;
	i->pixels[dpos + 2] = 0;
	i->pixels[dpos + 3] = a;
}

void img_free(img* i)
{
	free(i->pixels);
	free(i);
	i = NULL;
}

img* img_alloc(const size ssize)
{
    img* i = calloc(1, sizeof(img));
 
 	i->size = (size)
 	{
 		max(1, ssize.w),
 		max(1, ssize.h),
 	};

 	i->length = i->size.w * 4 * i->size.h;
    i->pixels = calloc(1, i->length);

    return i;
}

img* img_load(const char* filename)
{
	img* i = calloc(1, sizeof(img));

    holder* dh = load(filename);

    u16 width = 0;
    u16 height = 0;

    u8* pos = dh->data;
    
    memcpy(&width, pos, sizeof(u16));
    memcpy(&height, pos + 2, sizeof(u16));

    holder* decomp = decompress(pos + 4);
    
    i->pixels = malloc(decomp->length);
    i->length = decomp->length;
    memcpy(i->pixels, decomp->data, decomp->length);
    
    holder_free(decomp);
    holder_free(dh);

    i->size = (size){ width, height };

    return i;
}

img* img_sub(img* s, const rect r)
{
    img* i = calloc(1, sizeof(img));
    
    i->size = (size){ r.w, r.h };
    i->pixels = calloc(1, r.w * r.h * 4);
    i->length = r.w * r.h * 4;    

    u32 sinc = s->size.w * 4;
    u32 dinc = r.w * 4;

    for (u32 row = 0; row < r.h; row++)
    {
        u32 spos = (row + r.y) * sinc + r.x * 4;
        u32 dpos = row * dinc;
    
        memcpy(i->pixels + dpos, s->pixels + spos, dinc);
    }
    
    return i;
}

img* img_scale(img* s, u8 times)
{
    img* i = calloc(1, sizeof(img));
    
    i->size = (size){ s->size.w * times, s->size.h * times };
    i->pixels = calloc(1, i->size.w * i->size.h * times * 4);
    i->length = i->size.w * i->size.h * times * 4;

    u32 sinc = s->size.w * 4;
    u32 dinc = i->size.w * 4;

	for (u32 row = 0; row < s->size.h; row++)
    for (u32 col = 0; col < s->size.w; col++)    
    {
        u32 spos = (row * sinc) + col * 4;

		for (u32 th = 0; th < times; th++)
        for (u32 tw = 0; tw < times; tw++)
        {
	        u32 dpos = (row * times + th) * dinc + (col * times + tw) * 4;
	    
	        memcpy(i->pixels + dpos, s->pixels + spos, 4);
        }
    }
    
    return i;
}

void blit(img* dest, const img* b, const rect rsource, const vec pos)
{
	if (pos.x > dest->size.w) return;
	if (pos.y > dest->size.h) return;

	if (pos.x + rsource.w < 0) return;
	if (pos.y + rsource.h < 0) return;

	// image source validation
	/*
	if (
		b->source.x + b->source.w > b->size.w || 
		b->source.y + b->source.h > b->size.h)
	{		
		debug("wrong image source\n");
		return;
	}
	*/

	rect source = rsource;

	i32 px = roundf(pos.x);
	i32 py = roundf(pos.y);

	i32 pw = roundf(source.w);
	i32 ph = roundf(source.h);

	if (px < 0) { pw += px; source.x += abs(px); px = 0; }
	if (py < 0) { ph += py; source.y += abs(py); py = 0; }
	if (px + pw > dest->size.w) { pw = dest->size.w - px; }
	if (py + ph > dest->size.h) { ph = dest->size.h - py; }

	u32 sinc = b->size.w * 4;
	u32 dinc = dest->size.w * 4;

	for (u32 col = 0; col < pw; col++)
	{
		for (u32 row = 0; row < ph; row++)
		{
			u32 spos = (row + source.y) * sinc + (source.x + col) * 4;

			if (b->pixels[spos + 3] == 0)
				continue;

			u32 dpos = (row + py) * dinc + (px + col) * 4;

			/* debug			
			dest->pixels[dpos] = 255;
			dest->pixels[dpos + 1] = 255;
			dest->pixels[dpos + 2] = 255;
			dest->pixels[dpos + 3] = 128;
			*/
			
			memcpy(dest->pixels + dpos, b->pixels + spos, 4);
		}
	}
}
