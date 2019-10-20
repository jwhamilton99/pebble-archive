#include "timer_complete.h"
#include "timer.h"
#include "main.h"
#include "edit_timer.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_timer_complete;
static GBitmap *s_res_reset_icon;
static GBitmap *s_res_complete_icon;
static GBitmap *s_res_edit_icon;
static GFont s_res_gothic_18_bold;
static BitmapLayer *s_bitmaplayer_1;
static ActionBarLayer *s_actionbarlayer_1;
static TextLayer *s_textlayer_1;

void vibe_timer_callback();

AppTimer *vibe_timer;

int vibration_active = true;

void complete_up_handler() {
  seconds = mem_seconds;
  minutes = mem_minutes;
  if(seconds < 10) {
    snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
  } else {
    snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
  }
  app_timer_cancel(vibe_timer);
  stop_actionbar_timeout();
  update_timer_text();
  pause_timer();
  hide_timer_complete();
  snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: 0:00");
  reset_elements();
  restart_timer();
}

void complete_select_handler() {
  seconds = mem_seconds;
  minutes = mem_minutes;
  pause_timer();
  if(seconds < 10) {
    snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
  } else {
    snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
  }
  app_timer_cancel(vibe_timer);
  stop_actionbar_timeout();
  update_timer_text();
  snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: 0:00");
  reset_elements();
  hide_timer_complete();
}

void complete_down_handler() {
  seconds = mem_seconds;
  minutes = mem_minutes;
  if(seconds < 10) {
    snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
  } else {
    snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
  }
  app_timer_cancel(vibe_timer);
  stop_actionbar_timeout();
  update_timer_text();
  hide_timer_complete();
  show_edit_timer();
}

void complete_back_handler() {
  seconds = mem_seconds;
  minutes = mem_minutes;
  elapsed_minutes = 0;
  elapsed_seconds = 0;
  snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: 0:00");
  reset_elements();
  pause_timer();
  if(seconds < 10) {
    snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
  } else {
    snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
  }
  app_timer_cancel(vibe_timer);
  stop_actionbar_timeout();
  update_timer_text();
  hide_timer_complete();
}

void complete_click_handler() {
  window_single_click_subscribe(BUTTON_ID_UP, complete_up_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, complete_select_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, complete_down_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, complete_back_handler);
}

//Thanks to Small Stone Apps for this vibration code
void do_vibrate() {
  vibes_long_pulse();
  vibe_timer = app_timer_register(1000, vibe_timer_callback, NULL);
}

void vibe_timer_callback() {
  vibe_timer = NULL;
  do_vibrate();
}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_timer_complete = gbitmap_create_with_resource(RESOURCE_ID_TIMER_COMPLETE);
  s_res_reset_icon = gbitmap_create_with_resource(RESOURCE_ID_RESET_ICON);
  s_res_complete_icon = gbitmap_create_with_resource(RESOURCE_ID_COMPLETE_ICON);
  s_res_edit_icon = gbitmap_create_with_resource(RESOURCE_ID_EDIT_ICON);
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_timer_complete);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  
  // s_actionbarlayer_1
  s_actionbarlayer_1 = action_bar_layer_create();
  action_bar_layer_add_to_window(s_actionbarlayer_1, s_window);
  action_bar_layer_set_background_color(s_actionbarlayer_1, GColorWhite);
  action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_UP, s_res_reset_icon);
  action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, s_res_complete_icon);
  action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_DOWN, s_res_edit_icon);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_actionbarlayer_1);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(0, 97, 124, 20));
  text_layer_set_background_color(s_textlayer_1, GColorBlack);
  text_layer_set_text_color(s_textlayer_1, GColorWhite);
  text_layer_set_text(s_textlayer_1, "Timer Complete");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  action_bar_layer_set_click_config_provider(s_actionbarlayer_1, complete_click_handler);

  reset_elements();
  do_vibrate();
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer_1);
  action_bar_layer_destroy(s_actionbarlayer_1);
  text_layer_destroy(s_textlayer_1);
  gbitmap_destroy(s_res_timer_complete);
  gbitmap_destroy(s_res_reset_icon);
  gbitmap_destroy(s_res_complete_icon);
  gbitmap_destroy(s_res_edit_icon);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_timer_complete() {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_timer_complete() {
  window_stack_remove(s_window, true);
}