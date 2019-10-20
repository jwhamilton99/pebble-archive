#include "edit_timer.h"
#include "main.h"
#include "timer.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_edit_seconds;
static GBitmap *s_res_edit_minutes;
static BitmapLayer *s_bitmaplayer_1;
static BitmapLayer *s_bitmaplayer_2;

TextLayer *edit_seconds_layer, *edit_minutes_layer;

char edit_minutes_text[3] = "00";
char edit_seconds_text[3] = "00";

int section = 1;

static TextLayer* cl_init_text_layer(GRect location, GColor colour, GColor background, GTextAlignment alignment);

void edit_up_handler() {
  switch(section) {
    case 1:
      minutes++;
      if(minutes > 59) {
        minutes = 0;
      }
      break;
    case 2:
      seconds++;
      if(seconds > 59) {
        seconds = 0;
      }
      break;
  }
  snprintf(edit_minutes_text, sizeof(edit_minutes_text), "%i", minutes);
  if(seconds < 10) {
    snprintf(edit_seconds_text, sizeof(edit_seconds_text), "0%i", seconds);
  } else {
    snprintf(edit_seconds_text, sizeof(edit_seconds_text), "%i", seconds);
  }
  text_layer_set_text(edit_seconds_layer, edit_seconds_text);
  text_layer_set_text(edit_minutes_layer, edit_minutes_text);
}

void edit_select_handler() {
  switch(section) {
    case 1:
      bitmap_layer_destroy(s_bitmaplayer_2);
      gbitmap_destroy(s_res_edit_minutes);
      text_layer_set_text_color(edit_seconds_layer, GColorBlack);
      text_layer_set_text_color(edit_minutes_layer, GColorWhite);
      break;
    case 2:
      mem_seconds = seconds;
      mem_minutes = minutes;
      if(seconds < 10) {
        snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
      } else {
        snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
      }
      section = 0;
      hide_edit_timer();
      elapsed_minutes = 0;
      elapsed_seconds = 0;
      snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: 0:00");
      update_timer_text();
      break;
  }
  section++;
}

void edit_down_handler() {
  switch(section) {
    case 1:
      minutes--;
      if(minutes < 0) {
        minutes = 59;
      }
      break;
    case 2:
      seconds--;
      if(seconds < 0) {
        seconds = 59;
      }
      break;
  }
  snprintf(edit_minutes_text, sizeof(edit_minutes_text), "%i", minutes);
  if(seconds < 10) {
    snprintf(edit_seconds_text, sizeof(edit_seconds_text), "0%i", seconds);
  } else {
    snprintf(edit_seconds_text, sizeof(edit_seconds_text), "%i", seconds);
  }
  text_layer_set_text(edit_seconds_layer, edit_seconds_text);
  text_layer_set_text(edit_minutes_layer, edit_minutes_text);
}

void edit_back_handler() {
  minutes = mem_minutes;
  seconds = mem_seconds;
  if(seconds < 10) {
    snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
  } else {
    snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
  }
  section = 1;
  hide_edit_timer();
  update_timer_text();
}

void edit_click_handler() {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, edit_up_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, edit_select_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, edit_down_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, edit_back_handler);
}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  wakeup_cancel_all();
  
  pause_timer();
  
  seconds = mem_seconds;
  minutes = mem_minutes;
  
  snprintf(edit_minutes_text, sizeof(edit_minutes_text), "%i", minutes);
  if(seconds < 10)
  {
    snprintf(edit_seconds_text, sizeof(edit_seconds_text), "0%i", seconds);
  }
  else
  {
    snprintf(edit_seconds_text, sizeof(edit_seconds_text), "%i", seconds);
  }
  
  s_res_edit_seconds = gbitmap_create_with_resource(RESOURCE_ID_EDIT_SECONDS);
  s_res_edit_minutes = gbitmap_create_with_resource(RESOURCE_ID_EDIT_MINUTES);
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(0, -16, 144, 168));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_edit_seconds);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  
  // s_bitmaplayer_2
  s_bitmaplayer_2 = bitmap_layer_create(GRect(0, -16, 144, 168));
  bitmap_layer_set_bitmap(s_bitmaplayer_2, s_res_edit_minutes);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_2);
  
  edit_minutes_layer = cl_init_text_layer(GRect(15,52,50,34), GColorBlack, GColorClear, GTextAlignmentCenter);
  text_layer_set_font(edit_minutes_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text(edit_minutes_layer, edit_minutes_text);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(edit_minutes_layer));
  
  edit_seconds_layer = cl_init_text_layer(GRect(78,52,50,34), GColorWhite, GColorClear, GTextAlignmentCenter);
  text_layer_set_font(edit_seconds_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text(edit_seconds_layer, edit_seconds_text);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(edit_seconds_layer));
  
  window_set_click_config_provider(s_window, edit_click_handler);
  
  reset_elements();
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer_1);
  gbitmap_destroy(s_res_edit_seconds);
  text_layer_destroy(edit_seconds_layer);
  text_layer_destroy(edit_minutes_layer);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_edit_timer(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_edit_timer(void) {
  window_stack_remove(s_window, true);
}

static TextLayer* cl_init_text_layer(GRect location, GColor colour, GColor background, GTextAlignment alignment)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_text_alignment(layer, alignment);

	return layer;
}