#pragma once

char assets_path[255] = "";

void find_assets_path()
{
	if (strlen(assets_path) != 0)
		return;

    #if __APPLE__
        strcpy(assets_path, absolute_path);
        strcat(assets_path, "/assets.dat");
    #elif __ANDROID__    
        strcat(assets_path, "assets.dat");    
    #else
        strcat(assets_path, "assets/assets.dat");
    #endif
}

void internal_load_file(holder* dh, const char* filename)
{
    const u16 sizeOfLong = 8;
    u64 length = 0;
    bool found = false;
    char asset_filename[255];
    
    u8* pos = db_master->data;

    do
    {
        memcpy(&length, pos, sizeOfLong);
        pos += sizeOfLong;
        memcpy(&asset_filename, pos, length);
		asset_filename[length] = '\0';
        pos += length;
        memcpy(&length, pos, sizeOfLong); // size of file
        pos += sizeOfLong;                
                
        found = strcmp(filename, asset_filename) == 0;

        if (! found)
            pos += length;
    } while (!found);

    dh->length = (u32)length;
    dh->data = (u8 *)malloc(length);
    memcpy(dh->data, pos, sizeof(u8) * length);
}

void internal_load_direct_file(holder* dh, const char* filename)
{
	find_assets_path();

    FILE* assets;

    #if __ANDROID__
    FILE* android_fopen(const char* fname, const char* mode);
    assets = android_fopen(assets_path, "rb");
    #else
    assets = fopen(assets_path, "rb");
    #endif

    const u16 sizeOfLong = 8;
    u64 length = 0;
    bool found = false;
    char asset_filename[255];

	do
	{
		if (length > 0)
			fseek(assets, length, SEEK_CUR);

		fread(&length, sizeOfLong, 1, assets); // size of filename

		fread(asset_filename, 1, length, assets);

		asset_filename[length] = '\0';

		// TODO - check this - changed hardcoded 8 by sizeof(long)
		// this must be fixed cause it depends on the platform
		// .net long = 8
		// https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/sizeof
		fread(&length, sizeOfLong, 1, assets);

		// too much output even for debug purposes
		//LOG("Finding %s - found %s with %lld bytes", filename.c_str(), aux.c_str(), length);

		found = strcmp(filename, asset_filename) == 0;
	} while (!found);    

    dh->length = (u32)length;
    dh->data = (u8 *)malloc(length);
    fread(dh->data, sizeof(u8), length, assets);

    fclose(assets);
}

holder* load(const char* filename)
{
	holder* dh = calloc(1, sizeof(holder));

    if (db_master != NULL)
    	internal_load_file(dh, filename);
    else
		internal_load_direct_file(dh, filename);

	return dh;
}

char* load_text(const char* filename)
{
    holder* dh = load(filename);

    char* contents = (char*)malloc(dh->length+1);
    memcpy(contents, dh->data, dh->length);
    contents[dh->length] = 0x0;
    
    free(dh->data);
    free(dh);
   
    return contents;
}

void loader_free()
{
    if (db_master == NULL)
        return;

    free(db_master->data);
    free(db_master);
    
    db_master = NULL;
}

void loader_alloc()
{
    if (db_master != NULL)
    	return;

    db_master = calloc(1, sizeof(holder));

    find_assets_path();

    FILE* assets;

    #if __ANDROID__
    FILE* android_fopen(const char* fname, const char* mode);
    assets = android_fopen(assets_path, "rb");
    #else
    assets = fopen(assets_path, "rb");
    #endif
   
    // let it crash if not null

    fseek(assets, 0, SEEK_END);
    u64 length = ftell(assets);
    rewind(assets);
        
    db_master->length = (u32)length;
    db_master->data = (u8 *)malloc(length);
    fread(db_master->data, sizeof(u8), length, assets);
    
    fclose(assets);   
}

////////////////////////////////////////////////////////////////////////////////
// data structures load/save

void load_data(void* data, const char* filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        debug("File %s could not be opened.\n", filename);
        return;
    }

    fread(data, sizeof(data), 1, file);

    fclose(file);

    debug("Data file loaded.\n");
}

void save_data(void* data, const char* filename)
{
    FILE* file = fopen(filename, "w+");

    if (file == NULL)
    {
        debug("File %s could not be saved.\n", filename);
        return;
    }

    fwrite(data, sizeof(data), 1, file);
    fclose(file);

    debug("Data file saved.\n");
}

////////////////////////////////////////////////////////////////////////////////
// io - lzw

// maximum bits allowed to read
const i32 MAX_BITS = 14;

// hash bit to use with the hasing algorithm to find correct index
const i32 HASH_BIT = 6; // MAX_BITS - 8;

// max value allowed based on max bits
const i32 MAX_VALUE = 16383; // (1 << MAX_BITS) - 1;

// max code possible
const i32 MAX_CODE = 16382; // MAX_VALUE - 1;

//must be bigger than the maximum allowed by maxbits and prime
const i32 TABLE_SIZE = 18041;

#define TABLE_SIZE 18041
#define RESULT_BLOCK 16384

i32 code_table[TABLE_SIZE];
i32 prefix_table[TABLE_SIZE];
i32 char_table[TABLE_SIZE];

u64 bit_buffer; // bit buffer to temporarily store bytes read from the files
i32 bit_counter; // counter for knowing how many bits are in the bit buffer
i32 read_counter; // counter for knowing how many bytes were read from source

i32 read_code(u8* source)
{
    u32 return_val;

    while (bit_counter <= 24) // fill up buffer
    {
        u8 val = source[read_counter];
        read_counter++;
        bit_buffer |= (u64)val << (24 - bit_counter);
        bit_counter += 8;
    }

    return_val = (u32)bit_buffer >> (32 - MAX_BITS);
    bit_buffer <<= MAX_BITS;
    bit_counter -= MAX_BITS;

    i32 temp = (i32)return_val;
    return temp;
}

holder* decompress(u8* source)
{
    bit_buffer = 0;
    bit_counter = 0;

    read_counter = 0;   
    u8* lzw_result = malloc(RESULT_BLOCK);
    i32 write_pos = 0;
    u32 write_block = 0;
    u32 write_blocks = 1;

    i32 next_code = 256;
    i32 new_code, old_code;
    u8 ch;
    i32 current_code, counter;
    u8 decode_stack[TABLE_SIZE];

    old_code = read_code(source);
    ch = (u8)old_code;

    // write first byte since it is plain ascii
    lzw_result[write_pos++] = (u8)old_code;
    write_block++;

    new_code = read_code(source);

    while (new_code != MAX_VALUE) // read file all file
    {
        if (new_code >= next_code)
        { 
            //fix for prefix+chr+prefix+char+prefx special case
            decode_stack[0] = ch;
            counter = 1;
            current_code = old_code;
        }
        else
        {
            counter = 0;
            current_code = new_code;
        }

        // decode string by cycling back through the prefixes
        while (current_code > 255)
        {
            decode_stack[counter] = (u8)char_table[current_code];
            ++counter;
            if (counter >= MAX_CODE)
                printf("oh crap"); // debug
            current_code = prefix_table[current_code];
        }

        decode_stack[counter] = (u8)current_code;
        ch = decode_stack[counter]; // set last char used

        while (counter >= 0) // write out decode_stack
        {
            lzw_result[write_pos++] = decode_stack[counter];
			write_block++;
            --counter;
			
			if (write_block == RESULT_BLOCK)
			{
				write_block = 0;
				write_blocks++;
				lzw_result = realloc(lzw_result, write_blocks * RESULT_BLOCK);
			}
        }

        if (next_code <= MAX_CODE) // insert into tables
        {
            prefix_table[next_code] = old_code;
            char_table[next_code] = ch;
            ++next_code;
        }

        old_code = new_code;
        new_code = read_code(source);
    }

    holder* result = malloc(sizeof(holder));
    result->length = write_pos;
    result->data = malloc(write_pos);
    memcpy(result->data, lzw_result, write_pos);

    free(lzw_result);

    return result; // do not forget to free it afterwards
}

////////////////////////////////////////////////////////////////////////////////
// io - keyboard

bool key_down(u8 key)
{
	return pressed[key];
}

bool key_up(u8 key)
{
	return released[key];
}

////////////////////////////////////////////////////////////////////////////////
// io - mouse

vec calc_touch(const f32 x, const f32 y)
{
	// debug("touch detected at %f %f\n", x, y);

    return (vec) 
    { 
		2.0f * (x + 0.5f) / screen.w - 1.0f,
		-1 * (2.0f * (y + 0.5f) / screen.h - 1.0f)
    };
}

vec calc_touch_vec(const vec xy)
{    
    return calc_touch(xy.x, xy.y);
}

////////////////////////////////////////////////////////////////////////////////
// io - touch

bool touch_in_range(const vec target, const vec touch)
{
	if (f2_equals(touch, F2_ZERO)) return false;

	return f2_length(f2_sub(target, touch)) < TOUCH_RADIUS;
}

bool touching()
{
    bool result = false;

    for (u8 h = 0; h < MAX_TOUCH; h++)
    {
        result = ! f2_equals(touches_down[h], F2_ZERO);
        if (result)
            break;
    }

    return result;
}

bool touching_at(const vec target)
{
    bool result = false;

    for (u8 h = 0; h < MAX_TOUCH; h++)
    {
        result = touch_in_range(target, touches_down[h]);
        if (result)
        	break;
    }

    return result;
}

bool touched()
{
    bool result = false;

    for (u8 h = 0; h < MAX_TOUCH; h++)
    {
        result = ! f2_equals(touches_up[h], F2_ZERO);

        if (result)
            break;
    }

    return result;
}

bool touched_half(bool right)
{
    bool result = false;

    for (u8 h = 0; h < MAX_TOUCH; h++)
    {
        result = ! f2_equals(touches_up[h], F2_ZERO);

        result &=  
            (! right && touches_up[h].x < 0) ||
            (right && touches_up[h].x > 0);

        if (result)
            break;
    }

    return result;
}

bool touched_at(const vec target)
{
    bool result = false;

    for (u8 h = 0; h < MAX_TOUCH; h++)
    {
        result = touch_in_range(target, touches_up[h]);
        if (result)
        {
        	touches_up[h] = F2_ZERO;
        	break;
        }
    }

    return result;
}

void clear_input()
{
    /*
	memset(&touches_down, 0, sizeof(touches_down));
    memset(&touches_up, 0, sizeof(touches_up));
    memset(&pressed, 0, sizeof(pressed));
	//memset(&released, 0, sizeof(released));
    */

    memset(&released, 0, sizeof(released));
    memset(&touches_up, 0, sizeof(touches_up));
	key_any = false;
}

void clear_fullinput()
{
    memset(&touches_down, 0, sizeof(touches_down));
    memset(&touches_up, 0, sizeof(touches_up));
    memset(&pressed, 0, sizeof(pressed));
    memset(&released, 0, sizeof(released));

    key_any = false;
}