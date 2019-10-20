#include <pebble.h>
#include "main.h"
#include "timer.h"
#include "edit_timer.h"
#include "timer_complete.h"

static WakeupId wakeup_id = -1;
static time_t timestamp;
  
#define NUM_MINUTES_PKEY 1
#define NUM_SECONDS_PKEY 2
#define TIMER_ACTIVE_PKEY 3
  
#define NUM_MINUTES_DEFAULT 0
#define NUM_SECONDS_DEFAULT 0

Window *window;

AppTimer *action_bar_timeout_timer, *finish_time_state_timer;

TextLayer *title1, *timer_text, *finish_time;

GBitmap *clear_icon, *complete_icon, *edit_icon, *pause_icon, *play_icon, *reset_icon;

ActionBarLayer *main_action_bar;

char start_time_text[] = "Elapsed: 00:00";

bool action_bar_visible = true;
bool timer_active = false;

int mem_minutes;
int mem_seconds;

int minutes;
int seconds;

bool finish_time_visible = false;

int elapsed_minutes = 0;
int elapsed_seconds = 0;

int startx = 10;
int starty = 168;
int endx = 10;
int endy = 120;

enum {
  PERSIST_WAKEUP // Persistent storage key for wakeup_id
};

static void animate_layer(Layer *layer, GRect start, GRect finish, int duration, int delay);
static TextLayer* cl_init_text_layer(GRect location, GColor colour, GColor background, GTextAlignment alignment);
// Wakeup Section

static void wakeup_handler(WakeupId id, int32_t reason) {
  //Delete persistent storage value
  persist_delete(PERSIST_WAKEUP);
  
  window_stack_push(window, true);
  
  seconds = 1;
  minutes = 0;
  
  start_timer();
}

// Animations
void show_action_bar() {
  action_bar_visible = true;
  animate_layer(text_layer_get_layer(title1), GRect(10, 35, 124, 50), GRect(0, 35, 124, 50), 400, 0);
  animate_layer(text_layer_get_layer(timer_text), GRect(10, 50, 124, 50), GRect(0, 50, 124, 50), 400, 0);
  animate_layer(action_bar_layer_get_layer(main_action_bar), GRect(144, 3, 20, 146), GRect(124, 3, 20, 146), 400, 0);
  if(finish_time_visible == true) {
    animate_layer(text_layer_get_layer(finish_time), GRect(10,120,124,50), GRect(0,120,124,50), 400, 0);
  } else {
    animate_layer(text_layer_get_layer(finish_time), GRect(10,168,124,50), GRect(0,120,124,50), 400, 0);
  }
  startx = 0;
  starty = 120;
  endx = 0;
  endy = 120;
}

void hide_action_bar() {
  action_bar_visible = false;
  animate_layer(text_layer_get_layer(title1), GRect(0,35,124,50), GRect(10, 35, 124, 50), 400, 0);
  animate_layer(text_layer_get_layer(timer_text), GRect(0,50,124, 50), GRect(10,50,124, 50), 400, 0);
  animate_layer(action_bar_layer_get_layer(main_action_bar), GRect(124, 3, 20, 146), GRect(144, 3, 20, 146), 400, 0);
  animate_layer(text_layer_get_layer(finish_time), GRect(0,120,124,50), GRect(10,168,124,50), 400, 0);
  finish_time_visible = false;
  startx = 10;
  starty = 168;
  endx = 10;
  endy = 168;
}

void stop_actionbar_timeout() {
  app_timer_cancel(action_bar_timeout_timer);
}

void elapsed_time_timeout() {
  animate_layer(text_layer_get_layer(finish_time), GRect(endx,endy,124,50), GRect(startx,starty,124,50), 400, 0);
  finish_time_visible = false;
}

// Animates time on shake
void tap_handler(AccelAxisType axis, int32_t direction) {
  if(timer_active == true) {
    if(action_bar_visible != true) {
      startx = 10;
      starty = 168;
      endx = 10;
      endy = 120;
      animate_layer(text_layer_get_layer(finish_time), GRect(startx,starty,124,50), GRect(endx,endy,124,50), 400, 0);
      finish_time_visible = true;
      finish_time_state_timer = app_timer_register(3000, elapsed_time_timeout, NULL);
    }
  }
}

//Hides action bar after 4 seconds
void action_bar_timeout() {
  hide_action_bar();
  action_bar_visible = false;
}

//Puts everything in its starting places
void reset_elements() {
  show_action_bar();
  elapsed_seconds = 0;
  elapsed_minutes = 0;
  snprintf(start_time_text, sizeof(start_time_text), "Elapsed: 0:00");
  update_timer_text();
  action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, clear_icon);
  action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, play_icon);
  action_bar_visible = true;
  timer_active = false;
}

void restart_timer() {
  action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, reset_icon);
  action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, pause_icon);
  action_bar_timeout_timer = app_timer_register(4000, action_bar_timeout, NULL);
  action_bar_visible = true;
  
  //Wakeup Stuff
        
  int future_minutes = minutes * 60;
        
  timestamp = time(NULL) + future_minutes + seconds;
        
  wakeup_id = wakeup_schedule(timestamp, 1, true);
  persist_write_int(PERSIST_WAKEUP, wakeup_id);
  
  start_timer();
  timer_active = true;
}

void load_bitmaps() {
  clear_icon = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_ICON);
  complete_icon = gbitmap_create_with_resource(RESOURCE_ID_COMPLETE_ICON);
  edit_icon = gbitmap_create_with_resource(RESOURCE_ID_EDIT_ICON);
  pause_icon = gbitmap_create_with_resource(RESOURCE_ID_PAUSE_ICON);
  play_icon = gbitmap_create_with_resource(RESOURCE_ID_PLAY_ICON);
  reset_icon = gbitmap_create_with_resource(RESOURCE_ID_RESET_ICON);
}

// Takes timer text from timer.c
void update_timer_text()
{
  text_layer_set_text(timer_text, timer_char);
  layer_mark_dirty(text_layer_get_layer(timer_text));
  
  text_layer_set_text(finish_time, elapsed_char);
  layer_mark_dirty(text_layer_get_layer(finish_time));
}

// Click Handlers
void main_up_handler() {
  if(action_bar_visible == true) {
    timer_active = false;
    pause_timer();
    action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, play_icon);
    if(minutes == mem_minutes && seconds == mem_seconds) {
      mem_minutes = 0;
      minutes = 0;
      mem_seconds = 0;
      seconds = 0;
      elapsed_seconds = 0;
      elapsed_minutes = 0;
      snprintf(timer_char, sizeof(timer_char), "0:00");
      snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: 0:00");
      update_timer_text();
    }
    else {
      wakeup_cancel_all();
      minutes = mem_minutes;
      seconds = mem_seconds;
      elapsed_seconds = 0;
      elapsed_minutes = 0;
      snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: 0:00");
      if(seconds < 10) {
        snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
      }
      else {
        snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
      }
      text_layer_set_text(timer_text, timer_char);
      action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, clear_icon);
    }
    app_timer_cancel(action_bar_timeout_timer);
  }
}

void main_select_handler()
{
  if(action_bar_visible == true)
  {
    if(seconds == 0 && minutes == 0) {
      show_edit_timer();
    }
    else {
      timer_active = !timer_active;
      if(timer_active == true) {
        start_timer();
        hide_action_bar();
        action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, reset_icon);
        action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, pause_icon);
        action_bar_visible = false;
        app_timer_cancel(action_bar_timeout_timer);
        finish_time_visible = false;
        
        //Wakeup Stuff
        
        int future_minutes = minutes * 60;
        
        timestamp = time(NULL) + future_minutes + seconds;
        
        wakeup_id = wakeup_schedule(timestamp, 1, true);
        persist_write_int(PERSIST_WAKEUP, wakeup_id);
      }
      else {
        wakeup_cancel_all();
        persist_delete(PERSIST_WAKEUP);
        wakeup_id = -1;
        pause_timer();
        action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, play_icon);
        stop_actionbar_timeout();
        finish_time_visible = true;
      }
    }
  }
  else {
    action_bar_visible = true;
    show_action_bar();
    action_bar_timeout_timer = app_timer_register(4000, action_bar_timeout, NULL);
  }
}

void main_down_handler() {
  if(action_bar_visible == true) {
    show_edit_timer();
  }
  app_timer_cancel(action_bar_timeout_timer);
}

void main_back_handler() {
  if(timer_active == true) {
    if(action_bar_visible == true) {
      action_bar_visible = false;
      hide_action_bar();
      stop_actionbar_timeout();
    }
    else {
      window_stack_pop_all(true);
    }
  } else {
    window_stack_pop_all(true);
  }
}

// Configures the click handlers
void main_click_handler()
{
  window_single_click_subscribe(BUTTON_ID_UP, main_up_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, main_select_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, main_down_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, main_back_handler);
}

// Initialize the window
void handle_init(void) {
  bool wakeup_scheduled = false;
  
  window = window_create();
  window_set_fullscreen(window, false);
  window_set_background_color(window, GColorBlack);

  mem_minutes = persist_exists(NUM_MINUTES_PKEY) ? persist_read_int(NUM_MINUTES_PKEY) : NUM_MINUTES_DEFAULT;
  mem_seconds = persist_exists(NUM_SECONDS_PKEY) ? persist_read_int(NUM_SECONDS_PKEY) : NUM_SECONDS_DEFAULT;
  
  if (persist_exists(PERSIST_WAKEUP) == true) {
    wakeup_id = persist_read_int(PERSIST_WAKEUP);
    // query if event is still valid, otherwise delete
    if (wakeup_query(wakeup_id, NULL)) {
      wakeup_scheduled = true;
    } else {
      persist_delete(PERSIST_WAKEUP);
      wakeup_id = -1;
      wakeup_scheduled = false;
    }
  } else {    

  }
  
  load_bitmaps();
  
  snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: 0:00");
  
  
  main_action_bar = action_bar_layer_create();  
  
  action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, play_icon);
  action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, edit_icon);
  action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, clear_icon);
  action_bar_layer_set_background_color(main_action_bar, GColorWhite);
  
  title1 = cl_init_text_layer(GRect(0,35,124,50), GColorWhite, GColorClear, GTextAlignmentCenter);
  text_layer_set_font(title1, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text(title1, "Countdown");
  timer_text = cl_init_text_layer(GRect(0,50,124, 50), GColorWhite, GColorClear, GTextAlignmentCenter);
  text_layer_set_font(timer_text, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text(timer_text, timer_char);
  finish_time = cl_init_text_layer(GRect(0,120,124,50), GColorWhite, GColorClear, GTextAlignmentCenter);
  text_layer_set_font(finish_time, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  
  update_timer_text();
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(finish_time));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(title1));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(timer_text));
  action_bar_layer_add_to_window(main_action_bar, window);
  
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    WakeupId id = 0;
    int32_t reason = 0;
    if (wakeup_get_launch_event(&id, &reason)) {
      wakeup_handler(id, reason);
    }
  } else if (wakeup_scheduled) {
    wakeup_id = persist_read_int(PERSIST_WAKEUP);
    if (wakeup_id > 0) {
      // There is a wakeup scheduled soon
      time_t timestamp = 0;
      wakeup_query(wakeup_id, &timestamp);
      int seconds_remaining = timestamp - time(NULL);
      int raw_seconds = seconds_remaining;
      int minutes_remaining = 0;
      
      while(seconds_remaining >= 0) {
        seconds_remaining -= 60;
        minutes_remaining += 1;
      }
      
      minutes_remaining -= 1;
      seconds_remaining += 60;
      
      int x = raw_seconds;
      int y = (mem_minutes*60) + mem_seconds;
      
      for(x = raw_seconds; x <= y; x++) {
        elapsed_seconds += 1;
        if(elapsed_seconds == 60) {
          elapsed_seconds = 0;
          elapsed_minutes += 1;
        }
      }
      
      elapsed_seconds -= 1;
      
      minutes = minutes_remaining;
      seconds = seconds_remaining;
      
      if(elapsed_seconds < 10) {
        snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: %d:0%d", elapsed_minutes, elapsed_seconds);
      } else {
        snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: %d:%d", elapsed_minutes, elapsed_seconds);
      }
      
      if(seconds < 10) {
        snprintf(timer_char, sizeof(timer_char), "%d:0%d", minutes, seconds);
      } else {
        snprintf(timer_char, sizeof(timer_char), "%d:%d", minutes, seconds);
      }
      
      action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, reset_icon);
      action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, pause_icon);
      
      action_bar_timeout_timer = app_timer_register(4000, action_bar_timeout, NULL);
      
      action_bar_visible = true;
      
      timer_active = true;
      
      update_timer_text();
      
      start_timer();
    }
  } else {
    minutes = mem_minutes;
    seconds = mem_seconds;
    if(seconds < 10) {
      snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
    }
    else {
      snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
    }
  }
  accel_tap_service_subscribe(&tap_handler);
  
  action_bar_layer_set_click_config_provider(main_action_bar, main_click_handler);
  
  window_stack_push(window, true);
}

void handle_deinit(void) {
  window_destroy(window);
  persist_write_int(NUM_MINUTES_PKEY, mem_minutes);
  persist_write_int(NUM_SECONDS_PKEY, mem_seconds);
  text_layer_destroy(title1);
  text_layer_destroy(timer_text);
  text_layer_destroy(finish_time);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

static void on_animation_stopped(Animation *anim, bool finished, void *context)
{
	//Free the memory used by the Animation
	property_animation_destroy((PropertyAnimation*) anim);
}

static void animate_layer(Layer *layer, GRect start, GRect finish, int duration, int delay)
{
	//Declare animation
	PropertyAnimation *anim = property_animation_create_layer_frame(layer, &start, &finish);

	//Set characteristics
	animation_set_duration((Animation*) anim, duration);
	animation_set_delay((Animation*) anim, delay);
	animation_set_curve((Animation*) anim, AnimationCurveEaseInOut);

	//Set stopped handler to free memory
	AnimationHandlers handlers = {
		//The reference to the stopped handler is the only one in the array
		.stopped = (AnimationStoppedHandler) on_animation_stopped
	};
	animation_set_handlers((Animation*) anim, handlers, NULL);

	//Start animation!
	animation_schedule((Animation*) anim);
}

static TextLayer* cl_init_text_layer(GRect location, GColor colour, GColor background, GTextAlignment alignment)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_text_alignment(layer, alignment);

	return layer;
}