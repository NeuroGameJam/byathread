#pragma once

#include "external/AL/al.h"
#include "external/AL/alc.h"

ALCdevice *device;
ALCcontext *context;

void audio_free()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	debug("closing audio\n");
}

void audio_debug()
{
	u16 err = alGetError();

	if (err == AL_NO_ERROR)
		return;

	switch (err)
	{
		case AL_NO_ERROR: debug("AL_NO_ERROR\n"); break;
		case AL_INVALID_NAME: debug("AL_INVALID_NAME\n"); break;
		case AL_INVALID_ENUM: debug("AL_INVALID_ENUM\n"); break;
		case AL_INVALID_VALUE: debug("AL_INVALID_VALUE\n"); break;
		case AL_INVALID_OPERATION: debug("AL_INVALID_OPERATION\n"); break;
		case AL_OUT_OF_MEMORY: debug("AL_OUT_OF_MEMORY\n"); break;
		default: debug("No such error code\n");
	}
}

void audio_play(sound* s, const bool loop)
{
	if (muted) return;

	alSourcei(s->id, AL_LOOPING, loop ? 1 : 0);

	if (s->pan != 0)
	{
		alSourcei(s->id, AL_SOURCE_RELATIVE, AL_FALSE);
		//-1...1

		alSource3f(s->id, AL_POSITION, 
			s->pan,
			0.f,
			(f32)sqrt(1 - pow(s->pan, 2)));
	}

	if (s->pitch != 0)
		alSourcef(s->id, AL_PITCH, s->pitch);

	if (s->gain != 0)
		alSourcef(s->id, AL_GAIN, s->gain);

	alSourcePlay(s->id);
}

void audio_stop(sound* s)
{
	alSourceStop(s->id);
}

bool audio_play_single(sound* s)
{
	ALint state;
	alGetSourcei(s->id, AL_SOURCE_STATE, &state);

	/*
	Possible values of state
	AL_INITIAL
	AL_STOPPED
	AL_PLAYING
	AL_PAUSED
	*/

	if (state != AL_PLAYING)
	{
		audio_play(s, false);
		return true;
	}

	return false;
}

void audio_play_force(sound* s)
{
    if (muted) return;

	ALint state;
	alGetSourcei(s->id, AL_SOURCE_STATE, &state);

	if (state != AL_PLAYING)
		audio_play(s, false);
	else
	{
		alSourceRewind(s->id);
		audio_play(s, false);
	}
}

void audio_alloc()
{
    device = alcOpenDevice(NULL);

    if (!device)
    {
    	debug("can't open audio\n");
        return;
    }

    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);	
	
	audio_debug();

    debug("Audio initiated\n");
}
