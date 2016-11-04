/*
 * Gnome - A metronome for the Samsung Gear S2
 * Copyright 2016 Aaron Lehrian
 */

#ifndef VIEW_H_
#define VIEW_H_

#include "gnome.h"

#define TEMPO_MIN 40.0
#define TEMPO_MAX 300.0
#define TEMPO_STEP 4.0

bool create_base_gui(void *data);
void * tick(void *data);
void * tock(void *data);

#endif
