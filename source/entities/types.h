#pragma once

typedef struct
{
	u8 level;
	u8 max;
	bool sound;
	bool dual;
	u16 xp;
	u16 score;
	u16 score1;
	u16 score2;
	f32 time;
} data;

typedef struct
{
	u8 current;
	u8 count;
	gunit* spots;

} world;

typedef struct
{
	vec pos;
	
	vec start;
	vec target;	
	f32 time;
	bool finished;

	gunit* unit;
	gunit* lwing;
	gunit* rwing;

	f32 wtime;
	bool wdirection;

	u8 score;
	u8 current;
	u8 id;
	
} player;

typedef struct
{
	vec pos;
	bool enabled;

	vec start;
	f32 radius;
	f32 angle;
	f32 speed;
	bool direction;

	gunit* unit;

} enemy;

typedef struct
{
	vec pos;
	
	vec start;
	vec target;	
	f32 offset;
	f32 time;

	gunit* unit;

} connector;

typedef struct
{
	f64 time;
	u8 beat; // 1 - 4
	u8 sub; // 1-4
	u8 tempo;

	pthread_t thread;

} ambient;

typedef struct
{	
	vec pos;
	f32 time;
	bool direction;

	gunit* unit;

} flock;

typedef gunit rope;


// forward
vec world_find_next(const f32 y);
void enemies_disable(const vec pos);
void enemies_enable(const vec pos);
void connector_add(const vec start, const vec target, const f32 offset);

void ambient_free(ambient* d);
ambient* ambient_alloc();