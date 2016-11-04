/*
 * Gnome - A metronome for the Samsung Gear S2
 * Copyright 2016 Aaron Lehrian
 */

#include "data.h"

static void
handle_preference(const char *key, int *output, int default_value){
	bool existing;
	if(!preference_is_existing(key, &existing)){
		if(existing){
			if(preference_get_int(key, output)){
				dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get %s preference", key);
				*output = default_value;
			}
		}else{
			if(preference_set_int(key, default_value)){
				dlog_print(DLOG_ERROR, LOG_TAG, "Failed to set %s preference", key);
			}
			*output = default_value;
		}
	}else{
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to probe %s preference", key);
		*output = default_value;
	}
}

void
get_preferences(void *data){
	appdata_s *appdata = data;
	handle_preference("r", &(appdata->metronome.color.r), DEFAULT_R);
	handle_preference("g", &(appdata->metronome.color.g), DEFAULT_G);
	handle_preference("b", &(appdata->metronome.color.b), DEFAULT_B);
	handle_preference("a", &(appdata->metronome.color.a), DEFAULT_A);
	int tempo;
	handle_preference("default_tempo", &tempo, DEFAULT_TEMPO);
	appdata->metronome.tempo = (double)tempo;
}

void
set_preference(const char *key, int value){
	if(preference_set_int(key, value)){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to set %s preference", key);
	}
}
