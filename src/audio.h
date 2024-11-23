#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <ALmixer/ALmixer.h>
#include <AL/al.h>
#include <stdlib.h>
#include <assert.h>

#define SAMPLE_RATE (44100)

typedef struct Audio_t{
    ALmixer_Data *data; 
    ALint channel;
    int volume;
} Audio_t;

void audio_init();
Audio_t* audio_open(char* file);
void audio_start(Audio_t *a); 
void audio_stop();

#endif // !AUDIO_H
