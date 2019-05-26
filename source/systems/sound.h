#pragma once

sound* sound_load(const char* filename)
{
    sound* s = calloc(1, sizeof(sound));
  
    holder* dh = load(filename);

    holder* decomp = decompress(dh->data);
    u8* data_ptr = decomp->data;
    data_ptr += RIFF_SIZE; // RIFF header
  
    free(dh->data);
    free(dh);
    
    s->sample_rate = 44100;
    s->sample_size = 2; // 16 bit per sample (u16)
    s->stereo = false;
	s->sample_count = (decomp->length - RIFF_SIZE) / s->sample_size;

    // UPLOAD TO SOUND CARD
    ALenum format = 0;

    if (! s->stereo)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;
	
    // Create an audio source - id
    ALuint source;
    alGenSources(1, &source); 	 	
	audio_debug();

    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    // Convert loaded data to OpenAL buffer
    //----------------------------------------
    ALuint buffer;
    alGenBuffers(1, &buffer); // Generate pointer to buffer

    u32 data_size = s->sample_count * s->sample_size; // Size in bytes

    alBufferData(buffer, format, data_ptr, data_size, s->sample_rate);
    alSourcei(source, AL_BUFFER, buffer);

    s->id = source;
    s->buffer = buffer;
    s->format = format;
    
    s->pan = 0;
    s->pitch = 0;
    s->velocity = 0;
    s->gain = 0;

    ///

    debug("[%i %i %s] PCM loaded (rate: %i, sample: %i, stereo: %s)\n",
        source, 
        buffer, 
        filename,
        s->sample_rate,
        s->sample_size,
        s->stereo ? "stereo" : "mono");

    free(decomp->data);
    free(decomp);

    return s;
}

void sound_free(sound* s)
{
    if (s->length_in_samples != 0)
    {
        debug("SOUND %i\n", s->id);

        alDeleteSources(1, &s->id);
        alDeleteBuffers(1, &s->buffer);  
    }

    free(s);
	s = NULL;
}
