#pragma once

#include "main.h"

////////////////////////////////////////////////////////////////////////////////

i32 init(struct engine* engine)
{
	i32 deviceWidth;
	i32 deviceHeight;

	const EGLint eglAttribs[] =
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,

		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     8,
		EGL_STENCIL_SIZE,   8,

		/* SAMPLES AUTO - or it won't work on Marco's smaller tablet*/
		// non specified - better on xiaomi
		// specified - better on lg fino
		//EGL_SAMPLE_BUFFERS, 1,
		//EGL_SAMPLES, 4, // 4x Antialiasing if activated (Free on MALI GPUs)

		EGL_NONE
	};

	const EGLint eglContextAttribs[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	EGLint format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	* sample, we have a very simplified selection process, where we pick
	* the first EGLConfig that matches our criteria */
	eglChooseConfig(display, eglAttribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

	context = eglCreateContext(display, config, NULL, eglContextAttribs);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
	{
		debug("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &deviceWidth);
	eglQuerySurface(display, surface, EGL_HEIGHT, &deviceHeight);

	screen.w = deviceWidth;
	screen.h = deviceHeight;	

	ratio = SIDES.w / SIDES.h;

	engine->display = display;
	engine->context = context;
	engine->surface = surface;

	debug("new Game Instance");
	game = game_alloc();	

	elapsed = now_ms();
	engine->running = 1;

	return 0;
}

void update(struct engine* engine)
{
	delta = (now_ms() - elapsed);
	delta /= frame_target;
	elapsed = now_ms();

	// TODO - SOLVE THE MISTERY
	//delta = 1;

	average = (delta + average) / 2;
	
	// No display.
	if (engine->display == NULL)
		return;

/*
	debug("total touches a enviar %i", touchCollection.size());

	for (int h = 0; h < touchCollection.size(); h++)
	{
		TouchEvent touch = touchCollection.at(h);
		debug("pointer %lu: event %i (%f,%f)", touch.id, touch.eventId, touch.position.x, touch.position.y);
	}

	debug("------------------------------------");
*/	
	
	/*
	memset(&touches_down, 0, sizeof(touches_down));
	memset(&touches_up, 0, sizeof(touches_up));
	memset(&released, 0, sizeof(released));
	key_any = false;
	*/

	memset(&touches_down, 0, sizeof(touches_down));
	for (u8 h = 0; h < MAX_TOUCH; h++)
	{
		if (android_touches_down[h].filled)
		 	touches_down[h] = calc_touch_vec(android_touches_down[h].position);
	}

    if (has_new_scene)
        internal_next_scene();

    game->update();
	game->render();

	glFinish();
	eglSwapBuffers(engine->display, engine->surface);	

	if (quit)
		exit(0);
}

static void terminate(struct engine* engine)
{
	if (engine->display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		if (engine->context != EGL_NO_CONTEXT)
			eglDestroyContext(engine->display, engine->context);

		if (engine->surface != EGL_NO_SURFACE)
			eglDestroySurface(engine->display, engine->surface);

		eglTerminate(engine->display);

		game->free(); // destructor called
		free(game);
		game = NULL;
	}

	engine->running = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

static i32 inputHandler(struct android_app* app, AInputEvent* event)
{
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
		return 0;

	u32 flags = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;

	u32 pointerID;
	i32 pointerIndex;

	f32 x;
	f32 y;

	switch (flags)
	{
		case AMOTION_EVENT_ACTION_DOWN:
		case AMOTION_EVENT_ACTION_POINTER_DOWN:

		 	//LOG ("AMOTION_EVENT_ACTION_DOWN");

			pointerIndex = (AMotionEvent_getAction(event) &
							AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			pointerID  = AMotionEvent_getPointerId(event, pointerIndex);

			x = AMotionEvent_getX(event, pointerIndex);
			y = AMotionEvent_getY(event, pointerIndex);

			for (u8 h = 0; h < MAX_TOUCH; h++)
			{
				if (!android_touches_down[h].filled)
				{
					android_touches_down[h].filled = true;
					android_touches_down[h].id = pointerID;
					android_touches_down[h].position = (vec){ x, y };
					break;
				}
			}

			debug("AMOTION_EVENT_ACTION_DOWN %i\n", pointerID);
			break;


		case AMOTION_EVENT_ACTION_MOVE:

			pointerIndex = (AMotionEvent_getAction(event) &
							AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> 
							AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			pointerID  = AMotionEvent_getPointerId(event, pointerIndex);

			x = AMotionEvent_getX(event, pointerIndex);
			y = AMotionEvent_getY(event, pointerIndex);

			for (u8 h = 0; h < MAX_TOUCH; h++)
			{
				if (android_touches_down[h].filled &&
					android_touches_down[h].id == pointerID)
				{
					android_touches_down[h].position = (vec){ x, y };
					break;
				}
			}

			debug("AMOTION_EVENT_ACTION_MOVE %i\n", pointerID);

			break;

		case AMOTION_EVENT_ACTION_UP:
		case AMOTION_EVENT_ACTION_POINTER_UP:
		case AMOTION_EVENT_ACTION_CANCEL:
		case AMOTION_EVENT_ACTION_OUTSIDE:

			pointerIndex = (AMotionEvent_getAction(event) &
							AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> 
							AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			pointerID  = AMotionEvent_getPointerId(event, pointerIndex);

			x = AMotionEvent_getX(event, pointerIndex);
			y = AMotionEvent_getY(event, pointerIndex);

			for (u8 h = 0; h < MAX_TOUCH; h++)
			{
				if (android_touches_down[h].filled && 
					android_touches_down[h].id == pointerID)
				{
					touches_up[h] = calc_touch_vec((vec) { x, y });

					android_touches_down[h].filled = false;
					break;
				}
			}		

			debug("AMOTION_EVENT_ACTION_UP %i\n", pointerID);

			break;
		default:
			// ignore - eg: volume keys
		break;
	}

	return 0;
}

void commandHandler(struct android_app* app, i32 cmd)
{
	struct engine* engine = (struct engine*)app->userData;

	switch (cmd)
	{
		case APP_CMD_SAVE_STATE:
			// TODO
			/*
			engine->app->savedState = malloc(sizeof(struct Constants::RecordState));
			*((struct Constants::RecordState*)engine->app->savedState) = Constants::State;
			engine->app->savedStateSize = sizeof(struct Constants::RecordState);
			*/

			debug("SAVE STATE");

			break;

		case APP_CMD_INIT_WINDOW:

			debug("INIT Window");

			if (engine->app->window != NULL)
			{
				debug("INIT Window - WINDOW DOES NOT EXISTS");

				if (game == NULL)
				{
					// define global asset manager for android
					assetManager = engine->app->activity->assetManager;

				    char aux[255] = "";
				    memcpy(aux, 
				    	engine->app->activity->internalDataPath, 
				    	strlen(engine->app->activity->internalDataPath));
				    strcat(aux, "/");

				    path = strdup(aux);

					init(engine);				
					update(engine);
				}
			}
			else debug("INIT Window - WINDOW DOES EXISTS");

			break;

		case APP_CMD_TERM_WINDOW:

			debug("TERMINATE");
			// The window is being hidden or closed, clean it up.
			terminate(engine);
			break;

		case APP_CMD_LOST_FOCUS:
			engine->running = 0;
			update(engine);

			debug("LOST FOCUS");

			break;

		case APP_CMD_GAINED_FOCUS:
			engine->running = 1;
			update(engine);

			debug("GAINED FOCUS");

			break;
	}
}


/**
 * Main entry point
 */
void android_main(struct android_app* state)
{
	debug("android main");
	//app_dummy();

	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = commandHandler;
	state->onInputEvent = inputHandler;
	engine.app = state;
	game = NULL;

	ANativeActivity_setWindowFlags(engine.app->activity, 0
		| AWINDOW_FLAG_FULLSCREEN
		| AWINDOW_FLAG_KEEP_SCREEN_ON
		, 0
	);

	// Read all pending events.
	while (1)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(0, NULL, &events,(void**)&source)) >= 0)
		{
			// Process this event.
			if (source != NULL)
				source->process(state, source);

   			// Check if we are exiting.
			if (state->destroyRequested != 0)
			{
				terminate(&engine);
				return;
			}
		}

		// Update/Draw the current frame
		update(&engine);
	}
}