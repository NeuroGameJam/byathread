#pragma once

void ambient_free(ambient* d)
{
	pthread_detach(d->thread);

	free(d);
	d = NULL;	
}

void ambient_sync(ambient* d)
{
	if (muted)
	{
		audio_stop(fx_wind);	
	}
	else
		audio_play(fx_wind, true);
}

void ambient_update(ambient* d, const f64 milliseconds)
{
	if (muted)
		return;

	// ignore long delays
	// let the cpu catch up
	if (milliseconds > 1000)
		return;

	d->time += milliseconds;
	f64 sub_beat_time = 60000.f / d->tempo / 4.f;

	//debug("time %f %f %f\n", d->time, sub_beat_time, milliseconds);

	if (d->time > sub_beat_time)
	{		
		//debug("beat %f\n", sub_beat_time);

		d->time -= sub_beat_time;

		d->sub++;
		if (d->sub > 4)
		{
			d->sub = 1;
			d->beat++;
		}

		if (d->beat > 4)
		{
			d->beat = 1;					
			audio_play_single(fx_metronome);
		}

		if (d->beat != 1 && rand() % 10 == 1)
			audio_play_single(fx_phrase[rand() % 7]);
	}
}


void* ambient_thread(void* ptr)
{
	ambient* d = (ambient*)ptr;

	f64 counter = 0;
	f64 last = 0;
	f64 milliseconds;

	while (1)
	{
		last = counter;
		counter = now_ms();
		milliseconds = counter - last;

		ambient_update(d, milliseconds);

		usleep(5);
	}
}

ambient* ambient_alloc()
{
	ambient* d = calloc(1, sizeof(ambient));

	d->tempo = 60;

	pthread_create(&d->thread, NULL, ambient_thread, d);

	if (! muted)
		audio_play(fx_wind, true);

	return d;
}
