/*
 * Gnome - A metronome for the Samsung Gear S2
 * Copyright 2016 Aaron Lehrian
 */

#ifndef __gnome_H__
#define __gnome_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include "view.h"
#include "data.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "gnome"

#if !defined(PACKAGE)
#define PACKAGE "com.aaronlehrian.gnome"
#endif

#define GNOME_EDJ "/opt/usr/apps/com.aaronlehrian.gnome/res/gnome.edj"

#define TICK_LENGTH 300

typedef struct {
	int r;
	int g;
	int b;
	int a;
} color_t;

typedef enum {
	GNOME_STATE_ON = 0,
	GNOME_STATE_OFF
} gnome_state_t;

typedef struct {
	double tempo;
	useconds_t sleep_time;
	color_t color;
	gnome_state_t state;
	double tap_time;
	double prev_time;
} metronome_t;

typedef struct appdata {
	Evas_Object *window;
	Evas_Object *conformant;
	Evas_Object *layout;
	Eext_Circle_Surface *circle_surface;
	Evas_Object *tempo_slider;
	Evas_Object *background;
	Evas_Object *more_option;
	metronome_t metronome;
	Ecore_Thread *thread;
} appdata_s;

void win_delete_request_cb(void *data, Evas_Object *sender, void *event_info);
void value_changed_cb(void *data, Evas_Object *sender, void *event_info);
void screen_pressed_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);

#endif
