/*
 * Gnome - A metronome for the Samsung Gear S2
 * Copyright 2016 Aaron Lehrian
 */

#include "view.h"

//Changes the screen color to display a metronome tick
void *
tick(void *data)
{
	appdata_s *appdata = data;
	elm_bg_color_set(appdata->background, appdata->metronome.color.r, appdata->metronome.color.g, appdata->metronome.color.b);
	return NULL;
}

//Returns screen back to neutral
void *
tock(void *data)
{
	appdata_s *appdata = data;
	elm_bg_color_set(appdata->background, 0, 0, 0);
	return NULL;
}

/* Reactivate when options are implemented
//Called when options are closed - reactivates the circle slider
static void
more_option_closed_cb(void *data, Evas_Object *sender, void *event_info)
{
	appdata_s *appdata = data;
	eext_rotary_object_event_activated_set(appdata->tempo_slider, EINA_TRUE);
}*/

//Sets up all of the GUI elements
bool
create_base_gui(void *data)
{
	appdata_s *appdata = data;
	//Window
	appdata->window = elm_win_util_standard_add(PACKAGE, PACKAGE);
	if (appdata->window == NULL){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create window");
		return false;
	}
	elm_win_conformant_set(appdata->window, EINA_TRUE);
	elm_win_autodel_set(appdata->window, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(appdata->window)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(appdata->window, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(appdata->window, "delete,request", win_delete_request_cb, NULL);

	//Conformant
	appdata->conformant = elm_conformant_add(appdata->window);
	if (appdata->conformant == NULL){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create conformant");
		return false;
	}
	evas_object_size_hint_weight_set(appdata->conformant, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(appdata->window, appdata->conformant);
	evas_object_show(appdata->conformant);

	//Circle Surface
	appdata->circle_surface = eext_circle_surface_conformant_add(appdata->conformant);
	if (appdata->circle_surface == NULL){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create circle surface");
		return false;
	}

	//Layout
	appdata->layout = elm_layout_add(appdata->conformant);
	if (appdata->layout == NULL){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create layout");
		return false;
	}
	if(!elm_layout_file_set(appdata->layout, GNOME_EDJ, "slider_layout")){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to set layout file");
		return false;
	}
	char buffer[6];
	snprintf(buffer, sizeof buffer, "%g", appdata->metronome.tempo);
	elm_object_part_text_set(appdata->layout, "elm.text.slider", buffer);
	evas_object_show(appdata->layout);

	//Circle Surface
	appdata->tempo_slider = eext_circle_object_slider_add(appdata->layout, appdata->circle_surface);
	if (appdata->tempo_slider == NULL){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create tempo slider");
		return false;
	}
	eext_circle_object_slider_step_set(appdata->tempo_slider, TEMPO_STEP);
	eext_circle_object_value_min_max_set(appdata->tempo_slider, TEMPO_MIN, TEMPO_MAX);
	eext_circle_object_value_set(appdata->tempo_slider, appdata->metronome.tempo);

	//Activate the slider
	eext_rotary_object_event_activated_set(appdata->tempo_slider, EINA_TRUE);
	evas_object_smart_callback_add(appdata->tempo_slider, "value,changed", value_changed_cb, appdata);

	/* TODO: Add user options: color, sound?, presets?, accents?
	//More option
	appdata->more_option = eext_more_option_add(appdata->layout);
	if (appdata->more_option == NULL){
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create more_option");
		return false;
	}
	eext_more_option_direction_set(appdata->more_option, EEXT_MORE_OPTION_DIRECTION_TOP);
	//eext_more_option_item_part_text_set(appdata->more_option, "selector,main_text", "Options");
	evas_object_smart_callback_add(appdata->more_option, "more,option,closed", more_option_closed_cb, appdata);
	elm_object_part_content_set(appdata->layout, "elm.swallow.more", appdata->more_option);*/

	elm_object_content_set(appdata->conformant, appdata->layout);

	//Background color
	appdata->background = elm_bg_add(appdata->window);
	if (appdata->background == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to add background");
		return false;
	}
	elm_bg_color_set(appdata->background, 0, 0, 0);
	elm_object_part_content_set(appdata->layout, "elm.swallow.bg", appdata->background);
	evas_object_show(appdata->background);

	//Register tap for tempo callback
	evas_object_event_callback_add(appdata->layout, EVAS_CALLBACK_MOUSE_DOWN, screen_pressed_cb, appdata);

	//Show the window after the GUI is set
	evas_object_show(appdata->window);

	return true;
}
