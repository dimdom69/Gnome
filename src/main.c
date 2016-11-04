/*
 * Gnome - A metronome for the Samsung Gear S2
 * Copyright 2016 Aaron Lehrian
 */

#include "gnome.h"

//Called when user requests to exit app
void
win_delete_request_cb(void *data, Evas_Object *sender, void *event_info)
{
	ui_app_exit();
}

//Thread that handles timing of metronome
//Calculates time needed to sleep and calls tick()/tock() to create a blink on the screen
//TODO: Clock-based rather than sleep-based
static void
run_metronome(void *data, Ecore_Thread *thread)
{
	appdata_s *appdata = data;
	Ecore_Cb _tick = (void(*)(void*))tick;
	Ecore_Cb _tock = (void(*)(void*))tock;
	int last_tempo = 0;

	for (int iteration = 0; ; iteration++){
		if (appdata->metronome.tempo == 0 || appdata->metronome.state == GNOME_STATE_OFF){
			if(usleep(100000)){ //Wait for tempo to be initialized
				dlog_print(DLOG_ERROR, LOG_TAG, "Failed to sleep");
			}
			continue;
		}
		if (last_tempo != appdata->metronome.tempo){ //Tempo has been changed, need to recalculate sleep time
			last_tempo = appdata->metronome.tempo;
			appdata->metronome.sleep_time = (useconds_t)floor( (60.0 / appdata->metronome.tempo) * 1000 * 1000 );
		}
		if(usleep(appdata->metronome.sleep_time - TICK_LENGTH)){
			dlog_print(DLOG_ERROR, LOG_TAG, "Failed to sleep");
		}
		ecore_main_loop_thread_safe_call_async(_tick, appdata); //Call tick()
		if(usleep(TICK_LENGTH)){ //Sleeps so that the tick is visible (for a very short time)
			dlog_print(DLOG_ERROR, LOG_TAG, "Failed to sleep");
		}
		ecore_main_loop_thread_safe_call_async(_tock, appdata); //Call tock
	}
}

//Updates data when the circle slider has been changed
void
value_changed_cb(void *data, Evas_Object *sender, void *event_info)
{
	appdata_s *appdata = data;
	char buffer[25];
	appdata->metronome.tempo = eext_circle_object_value_get(sender);
	snprintf(buffer, sizeof buffer, "%g", appdata->metronome.tempo);
	elm_object_part_text_set(appdata->layout, "elm.text.slider", buffer);
}

//Callback for the tap tempo functionality
//TODO: Calculate average based on 3 (or 4?) taps, smarter detection
void
screen_pressed_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	appdata_s *appdata = data;
	struct timeval tv;
	double time_milliseconds;
	double elapsed_time;
	char buffer[25];

	if(gettimeofday(&tv, NULL)){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get time");
		return;
	}
	time_milliseconds = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	appdata->metronome.prev_time = appdata->metronome.tap_time;
	appdata->metronome.tap_time = time_milliseconds;
	elapsed_time = appdata->metronome.tap_time - appdata->metronome.prev_time;
	if(elapsed_time > 200 && elapsed_time < 1500){ //Make sure it is between 40 and 300 bpm
		appdata->metronome.tempo = floor(1000 / elapsed_time * 60 / 4) * 4;
		snprintf(buffer, sizeof buffer, "%g", appdata->metronome.tempo);
		elm_object_part_text_set(appdata->layout, "elm.text.slider", buffer);
		eext_circle_object_value_set(appdata->tempo_slider, appdata->metronome.tempo);
	}
}

//Creates GUI, starts metronome thread
static bool
app_create(void *data)
{
	appdata_s *appdata = data;

	get_preferences(appdata);

	if(!create_base_gui(appdata)){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create base GUI.");
		return false;
	}
	appdata->thread = ecore_thread_run(run_metronome, NULL, NULL, appdata);
	if (appdata->thread == NULL){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create metronome thread.");
		return false;
	}
	appdata->metronome.state = GNOME_STATE_ON;

	return true;
}

//Launch request
static void
app_control(app_control_h app_control, void *data)
{

}

//Application became invisible
static void
app_pause(void *data)
{

}

//Application became visible
static void
app_resume(void *data)
{

}

//Terminate all resources
static void
app_terminate(void *data)
{

}

//Handles language change
//TODO: Translations
static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

//Device orientation changed
static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	return;
}

//Region format changed
static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{

}

//Low battery
static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{

}

//Low memory
static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{

}

//Initializes app data, sets up callbacks for app events, runs main app loop
int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
	ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
