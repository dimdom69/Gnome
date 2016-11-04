/*
 * Gnome - A metronome for the Samsung Gear S2
 * Copyright 2016 Aaron Lehrian
 */

#ifndef DATA_H_
#define DATA_H_

#include <app_preference.h>
#include "gnome.h"

#define DEFAULT_R 255
#define DEFAULT_G 255
#define DEFAULT_B 255
#define DEFAULT_A 255
#define DEFAULT_TEMPO 120

void get_preferences(void *data);
void set_preference(const char *key, int value);

#endif
