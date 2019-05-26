#pragma once

void blit(img* dest, const img* b, const rect source, const vec pos);

size font_measure(font* f, const char* text)
{	
	size result = F2_ZERO;
	f32 advance = 0;
	character ch;

	result.h = f->height;

	for (u16 h = 0; h < strlen(text); h++)
	{
		char letter = text[h];

		if (letter == '\n')
		{
			if (advance > result.w)
				result.w = advance;

			advance = 0;
			result.h += f->height + f->line;
			continue;
		}

		ch.id = 0;

		for (u16 i = 0; i < f->count; i++)
		{
			if (f->chars[i].id == letter)
			{
				ch = f->chars[i];
				break;
			}
		}

		if (ch.id == 0)
		{
			debug("character %i not found\n", letter);
			continue;
		}
		
		advance += ch.advance_x;
	}

	if (advance > result.w)
		result.w = advance;	

	// make size even
	result.w = ceil(result.w / 2) * 2;
	result.h = ceil(result.h / 2) * 2;

	return result;
}

void font_render(font* f, const char* text, img* dest)
{	
	rect source = F4_ZERO;
	vec advance = F2_ZERO;
	vec offset = F2_ZERO;
	character ch;

	for (u16 h = 0; h < strlen(text); h++)
	{
		char letter = text[h];

		if (letter == '\n')
		{
			advance.x = 0;
			advance.y += f->height / 2 + f->line;
			continue;
		}

		ch.id = 0;

		for (u16 i = 0; i < f->count; i++)
		{
			if (f->chars[i].id == letter)
			{
				ch = f->chars[i];
				break;
			}
		}

		if (ch.id == 0)
		{
			debug("character %i not found\n", letter);
			continue;
		}
				
		source = (rect)
		{
			ch.x,
			ch.y,
			ch.width,
			ch.height
		};

		offset.x = ch.offset_x;
		offset.y = ch.offset_y;

		// TODO - problem is that 0, 0 with offset gets chopped
		blit(dest, f->sheet, source, f2_add(advance, offset));

		advance.x += ch.advance_x;
	}
}

void font_free(font* f)
{
	img_free(f->sheet);
    free(f->chars);
    free(f);
    f = NULL;
}

font* font_load(const char* filename)
{
    char path[255] = "";
    strcpy(path, filename);
    strcat(path, ".fnt");

    font* f = calloc(1, sizeof(font));

    holder* dh = load(path);

    f->count = dh->length / sizeof(character);
    f->chars = calloc(f->count, sizeof(character));
    memcpy(f->chars, dh->data, dh->length);
    
    character ch;

    for (u16 h = 0; h < f->count; h++)
    {
        ch = f->chars[h];

        //if (ch.height + ch.offset_y > f->height)
        //    f->height = ch.height + ch.offset_y;
        if (ch.height > f->height)
            f->height = ch.height;
    }

    f->line = f->height / 4; // default

    debug("font line: %i\n", f->line);

    holder_free(dh);

    memset(&path, 0, 255);
    strcpy(path, filename);
    strcat(path, ".png");

    f->sheet = img_load(path);

    return f;
}
