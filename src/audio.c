#include "audio.h"
#include <ALmixer/ALmixer.h>

void audio_init() {
  ALmixer_Init(ALMIXER_DEFAULT_FREQUENCY, ALMIXER_DEFAULT_NUM_SOURCES,
               ALMIXER_DEFAULT_REFRESH);
}

Audio_t *audio_open(char *path) {
  Audio_t *a = malloc(sizeof(Audio_t));
  static int channels;
  ALmixer_Data *data = ALmixer_LoadStream(
      path, ALMIXER_DEFAULT_BUFFERSIZE, ALMIXER_DEFAULT_QUEUE_BUFFERS,
      ALMIXER_DEFAULT_STARTUP_BUFFERS,
      ALMIXER_DEFAULT_BUFFERS_TO_QUEUE_PER_UPDATE_PASS, AL_FALSE);
  ALint channel = ALmixer_FindFreeChannel(channels);
  int volume = 100;
  a->channel = channel;
  a->data = data;
  a->volume = volume;
  return a;
}

void audio_start(Audio_t *a) { ALmixer_PlayChannel(a->channel, a->data, 0); }

void audio_pause(Audio_t *a) { ALmixer_PauseChannel(a->channel); }

void audio_kill(Audio_t *a) {
    ALmixer_HaltChannel(a->channel);
  ALmixer_FreeData(a->data);
  free(a);
}
