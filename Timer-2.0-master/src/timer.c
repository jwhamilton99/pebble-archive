#include <pebble.h>
#include "timer.h"
#include "main.h"
#include "timer_complete.h"
  
char timer_char[6] = "00:00";
char elapsed_char[15] = "Elapsed: 00:00";

void pause_timer() {
  tick_timer_service_unsubscribe();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  seconds -= 1;
  elapsed_seconds += 1;
  
  if(elapsed_seconds == 60) {
    elapsed_minutes += 1;
    elapsed_seconds = 0;
  }
  
  if(seconds < 0)
  {
    minutes -= 1;
    seconds = 59;
  }
  if(seconds < 10)
  {
    snprintf(timer_char, sizeof(timer_char), "%i:0%i", minutes, seconds);
  }
  else
  {
    snprintf(timer_char, sizeof(timer_char), "%i:%i", minutes, seconds);
  }
  
  if(elapsed_seconds < 10) {
    snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: %i:0%i", elapsed_minutes, elapsed_seconds);
  } else {
    snprintf(elapsed_char, sizeof(elapsed_char), "Elapsed: %i:%i", elapsed_minutes, elapsed_seconds);
  }
  
  if(minutes == 0 && seconds == 0) {
    stop_actionbar_timeout();
    show_action_bar();
    show_timer_complete();
  }
  update_timer_text();
}

void start_timer()
{
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}