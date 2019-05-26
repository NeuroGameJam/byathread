#pragma once

// https://audioprograming.wordpress.com/2012/03/03/android-audio-streaming-with-opensl-es-and-the-ndk/

////////////////////////////////////////////////////////////////////////////////
// platform includes and defines

#define VK_ESCAPE 0x25
#define VK_BACK 0x25

#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_SPACE 0x08
#define VK_RETURN 0x0D
#define VK_TAB 0x09
#define VK_CONTROL 0x11

bool VkKeyScan (char c) { return false; };
bool VkKeyScanEx (char c, u8 i) { return false; };
bool VkKeyScanA(char dummy) { return false; }

#include <android/window.h>
#include <android/log.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>

#include <android_native_app_glue.h>

#include <pthread.h>
#include <unistd.h>

AAssetManager* assetManager;

#ifdef DEBUG
#undef debug
#define debug(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, APP, __VA_ARGS__))
#endif

static i32 android_read(void* cookie, char* buf, i32 size)
{
	return AAsset_read((AAsset*)cookie, buf, size);
}

static i32 android_write(void* cookie, const char* buf, i32 size)
{
	return 0; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, i32 whence)
{
	return AAsset_seek((AAsset*)cookie, offset, whence);
}

static i32 android_close(void* cookie)
{
	AAsset_close((AAsset*)cookie);
	return 0;
}

FILE* android_fopen(const char* fname, const char* mode)
{
	if (mode[0] == 'w')
		return fopen(fname, mode);

	if (mode[0] == 'i') // internal to use normal fopen
		return fopen(fname, "r");

	AAsset* asset = AAssetManager_open(assetManager, fname, 0);
	if (!asset) return NULL;

	return funopen(asset, android_read, android_write, android_seek, android_close);
}

f32 min(f32 a, f32 b)
{
	return a > b ? b : a;
}

f32 max(f32 a, f32 b)
{
	return a > b ? a : b;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

scene* game;

struct engine
{
	struct android_app* app;

	int running;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
};

typedef struct
{
	bool filled;
	u8 id;
	vec position;
} touch;

static touch android_touches_down[MAX_TOUCH];

f64 elapsed;
f64 average;

f64 now_ms() 
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (f64)(1000.0 * res.tv_sec + (f64) res.tv_nsec / 1e6);
}