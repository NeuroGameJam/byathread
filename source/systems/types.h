#pragma once

typedef struct
{
    // Sound source type
    u32 id;     // OpenAL audio source id
    
    u32 buffer; // OpenAL audio buffer id
    u32 format; // OpenAL audio format specifier
    
    u32 sample_count;
    u32 sample_rate;
    u32 sample_size;
    u32 length_in_samples;
    f32 length_in_seconds;
    
    bool stereo;

    f32 pan;
    f32 pitch;
    f32 velocity;
    f32 gain;
} sound;

typedef struct
{
    u16 id;
      
    i16 vertex_location;
    i16 texture_location;
    i16 normal_location;    
    i16 tint_location;
    i16 time_location;
    i16 resolution_location;
    i16 reference_location;

    i16 light_location;
    i16 pov_location;
    
    i16 model_matrix;
    i16 normal_matrix;
    i16 view_matrix;
    i16 mvp_matrix;
} shader;

typedef struct
{	
	size size;
	u32 length;
	
	u8* pixels;
} img;

typedef struct
{
	u16 id;
	u16 x;
	u16 y;
	u16 width;
	u16 height;
	i16 offset_x;
	i16 offset_y;
	i16 advance_x;
} character;

typedef struct
{
	u16 line;
	u16 height;
	u16 count;	

	img* sheet;
	character* chars;
} font;

typedef struct
{
    u16 id;
    size size;
    bool original;
} tex;

typedef struct
{
	bool visible;
	bool original; // holds uv texture

	u16 vertex_count;
	u16 texcoord_count;
	u16 normal_count;

	u16 texture_id;
	u16 uv_id;	

    vec3 pos;
    vec3 rot;    
    vec3 scale;
       
    vec3 size;

	u32 vbo[3];
} model;

typedef struct
{
    vec3 pos;
    f32 power;
} light;

typedef struct
{    
	bool ortho;
	
    f32 fov;
    f32 nnear;
    f32 nfar;
  
    vec3 pov;
    vec3 look;
    vec3 reference;

    mat m_view;
    mat m_projection;

    light light;
} cam;

typedef struct
{
    bool will_intersect;
    bool intersect;
    vec min;
} collision;

typedef struct
{
    f32 min;
    f32 max;
} min_max;

typedef struct
{
    vec points[4];
    vec edges[4];
    vec center;
} collider;
