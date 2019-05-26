#pragma once

////////////////////////////////////////////////////////////////////////////////
// global pragmas

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#endif

////////////////////////////////////////////////////////////////////////////////
// generic system includes

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
// generic macros

#define fabsf(x) fabs(x)
#define tanf(x) tan(x)
#define cosf(x) cos(x)
#define sinf(x) sin(x)
#define sqrtf(x) sqrt(x)
#define strtoll(x,x2,x3) strtol(x,x2,x3)

#define acosf(x) acos(x)
#define atan2f(x,x2) atan2(x,x2)
#define ceilf(x) ceil(x)
#define fmodf(x,x2) fmod(x,x2)
#define floorf(x) floor(x)

////////////////////////////////////////////////////////////////////////////////
// generic constants

#define PI2 3.14159265358979323846f * 2.0
#define PI 3.14159265358979323846f
#define HALF_PI 3.14159265358979323846f / 2.0
#define RIFF_SIZE 44
#define MAX_TOUCH 8
#define TOUCH_RADIUS .15
#define COLLIDER_POINT_SIZE 4

#define debug(...) (void)0

////////////////////////////////////////////////////////////////////////////////
// primary types

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32; // 4 bytes
typedef double f64; // 8 bytes

////////////////////////////////////////////////////////////////////////////////
// basic types

typedef union u2
{
	struct
	{
		u16 x, y;
	};

	struct
	{
		u16 w, h;
	};
} point, usize;

typedef union f2
{
	struct
	{
		f32 x, y;
	};

	struct
	{
		f32 w, h;
	};
} vec, size;

typedef union f3
{
    struct
    {
        f32 x, y, z;
    };

	struct
	{
		f32 w, h, d;
	};    
} vec3;

typedef struct f4
{
	f32 x, y, w, h;
} rect;

typedef struct
{
	u8 r, g, b, a;
} color, levels;

typedef struct
{
	vec tl, tr, bl, br;
} quad;

typedef struct
{
  f32 xx, xy, xz, xw;
  f32 yx, yy, yz, yw;
  f32 zx, zy, zz, zw;
  f32 wx, wy, wz, ww;
} mat;

////////////////////////////////////////////////////////////////////////////////
// basic function types

typedef void(* VOIDFUNC) ();
typedef void(* VOIDFUNCPARAM) (void*);

////////////////////////////////////////////////////////////////////////////////
// basic types constants

const vec F2_ZERO = { 0, 0 };
const vec F2_ONE = { 1, 1 };

const vec3 F3_ZERO = { 0, 0, 0 };
const vec3 F3_ONE = { 1, 1, 1 };

const rect F4_ZERO = { 0, 0, 0, 0 };
const rect F4_ONE = { 1, 1, 1, 1 };

const color BLACK = { 0, 0, 0, 255 };
const color WHITE = { 255, 255, 255, 255 };
const color TRANSPARENT = { 0, 0, 0, 0 };

const mat MAT_ZERO =
{
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

const mat MAT_ONE =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

////////////////////////////////////////////////////////////////////////////////
// core types

typedef struct
{
    u32 length;
    void *data;
} holder;

typedef struct
{
	VOIDFUNC update;
	VOIDFUNC prerender;
	VOIDFUNC render;
	VOIDFUNC posrender;
	VOIDFUNC free;
} scene;

typedef struct 
{
	void** items;
	u16 count;
} list;

void list_free(list* l)
{
	if (l->items != NULL) free(l->items);
	free(l);
	l = NULL;
}

void holder_free(holder* h)
{
	if (h->data != NULL) free(h->data);
	free(h);
	h = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// globals

char* path;
bool quit = false; // exits the application
bool reset = false;
bool change_aspect = false;
bool muted = false;
char s_aux[128];
scene* game;

f64 frame_target = 1000.f / 60.f;
f64 delta;

size screen;
size window_size;
f32 ratio; // holds virtual vs real

#ifndef SIDES
#define SIDES ((size){ 1920/2, 1080/2 })
#endif

#ifndef BGCOLOR
#define BGCOLOR { 0, 0, 0, 255 }
#endif

#ifndef DATA_FILE
#define DATA_FILE "assets/data.dat"
#endif

color bg = BGCOLOR;

// IO
bool pressed[256];
bool released[256];
bool key_any = false;

vec touches_down[MAX_TOUCH];
vec touches_up[MAX_TOUCH];

holder* db_master;

// forward declarations
void common_alloc();
void common_free();

void manager_alloc();
void manager_clear();
void manager_update();
void manager_render();
void manager_free();

void renderer_alloc();
void renderer_clear();
void renderer_free();

void audio_alloc();
void audio_free();
